////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 2015-09-10 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "DatasetConverters/DatasetConvertMatrix.h"
#include "SEInterfaces/RecordsetObj.h"
#include "API/AosApi.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDatasetConvertMatrix_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			//OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDatasetConvertMatrix(version);
			aos_assert_r(jimo, 0);
			return jimo;
		}

		catch (...)
		{
			AosSetErrorU(rdata, "Failed creating jimo") << enderr;
			return 0;
		}

		OmnShouldNeverComeHere;
		return 0;
	}
}


AosDatasetConvertMatrix::AosDatasetConvertMatrix(const int version)
:
AosDatasetConverterObj(version)
{
}


AosDatasetConvertMatrix::AosDatasetConvertMatrix(const AosDatasetConvertMatrix &jimo)
:
AosDatasetConverterObj(jimo)
{
}

AosDatasetConvertMatrix::~AosDatasetConvertMatrix()
{
}


bool
AosDatasetConvertMatrix::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//1. create matrix data by the def(json)
	//<convert name="matrix">
	//	<arg name="x"><![CDATA[key_field1]]></arg>
	//	<arg name="y"><![CDATA[key_field2]]></arg>
	//	<arg name="cell"><![CDATA[key_field3]]></arg>
	//	<arg name="xstart_time"><![CDATA[20150901]]></arg>
	//	<arg name="ystart_time"><![CDATA[20150901]]></arg>
	//	<arg name="xrange"><![CDATA[10]]></arg>
	//	<arg name="yrange"><![CDATA[10]]></arg>
	//</convert>
	//2. def format:
	//<dataset xrange="xxx" yrange="xxx"...>
	//		<schema ...>
	//			<datarecord>
	//				<datafield type="bin_u32"/>
	//			</datarecord>
	//		</schema>
	//</dataset>
	mXName = def->getChildTextByAttr("name", "x");
	mYName = def->getChildTextByAttr("name", "y");
	mCellName = def->getChildTextByAttr("name", "cell");
	string xstart_time = def->getChildTextByAttr("name", "xstart_time");
	AosDateTime x_dt(xstart_time, "%Y%m%d");
	if (x_dt.isNotADateTime()) 
	{   
		OmnAlarm << "Current DateTime Object is invalid" << enderr;
		return false;
	}   
	mXStartTime = x_dt;
	string ystart_time = def->getChildTextByAttr("name", "ystart_time");
	AosDateTime y_dt(ystart_time, "%Y%m%d");
	if (y_dt.isNotADateTime())
	{
		OmnAlarm << "Current DateTime Object is invalid" << enderr;
		return false;
	}
	mYStartTime = y_dt;
	int xrange = atoi(def->getChildTextByAttr("name", "xrange").data());
	int yrange = atoi(def->getChildTextByAttr("name", "yrange").data());
	OmnString str;
	str << "<dataset jimo_objid=\"dataset_matrix_jimodoc_v0\" zky_name=\"output\" xrange=\"" << xrange << "\" yrange=\"" << yrange << "\">"
		<< "<dataschema jimo_objid=\"dataschema_recordbylen_jimodoc_v0\" zky_name=\"output\">"
		<< "<datarecord type=\"buff\" zky_name=\"output\">"
		<< "<datafields>";
	for (int i=0; i<xrange; i++)
	{
		str << "<datafield type=\"bin_int64\" zky_name=\"x" << i << "\"/>";
	}
	str << "</datafields>"
		<< "</datarecord>"
		<< "</dataschema>"
		<< "</dataset>";
	AosXmlTagPtr dataset = AosXmlParser::parse(str AosMemoryCheckerArgs);
	aos_assert_r(dataset, false);

	mMatrixDataset = AosCreateDataset(rdata.getPtr(), dataset);
	aos_assert_r(mMatrixDataset, false);

	bool rslt = mMatrixDataset->config(rdata.getPtr(), dataset);
	aos_assert_r(rslt, false);
	
	return true;
}


bool
AosDatasetConvertMatrix::procData(
		AosRundata *rdata_raw,
		AosDatasetObj* input_dataset)
{
	AosRecordsetObjPtr input_recordset;
	bool rslt = input_dataset->nextRecordset(rdata_raw, input_recordset);
	aos_assert_r(rslt, false);
	if (AosRecordsetObj::checkEmpty(input_recordset)) return true;
	AosDataRecordObj* input_record;
	AosValueRslt x_value, y_value, cell_value;
	int idx = -1;
	while(1)
	{
		rslt = input_recordset->nextRecord(rdata_raw, input_record);
		aos_assert_r(rslt, false);
		if (!input_record)
		{
			break;
		}
		idx = input_record->getFieldIdx(mXName, rdata_raw);
		aos_assert_r(idx != -1, false);
		input_record->getFieldValue(idx, x_value, false, rdata_raw);

		idx = input_record->getFieldIdx(mYName, rdata_raw);
		aos_assert_r(idx != -1, false);
		input_record->getFieldValue(idx, y_value, false, rdata_raw);

		idx = input_record->getFieldIdx(mCellName, rdata_raw);
		aos_assert_r(idx != -1, false);
		input_record->getFieldValue(idx, cell_value, false, rdata_raw);

		AosDateTime x_dt = x_value.getDateTime();
		AosDateTime y_dt = y_value.getDateTime();

		AosTimeDuration x_du(x_dt - mXStartTime);
		AosTimeDuration y_du(y_dt - mYStartTime);

		int x_idx = x_du.hours()/(24 * 3600);
		int y_idx = y_du.hours()/(24 * 3600);
		/*
		AosDateTime x_dt(x_value.getValueStr1(), "%Y%m%d");
		AosTimeDuration x_du(x_dt - mXStartTime);
		int x_idx = x_du.hours()/(24 * 3600);

		AosDateTime y_dt(y_value.getStr(), "%Y%m%d");
		AosTimeDuration y_du(y_dt - mYStartTime);
		int y_idx = y_du.hours()/(24 * 3600);
		*/

		mMatrixDataset->setCellValue(rdata_raw, x_idx, y_idx, cell_value);
	}
	return true;
}


AosDatasetObjPtr 
AosDatasetConvertMatrix::getOutput() const
{
	aos_assert_r(mMatrixDataset, 0);
	return mMatrixDataset;
}


AosJimoPtr 		
AosDatasetConvertMatrix::cloneJimo() const
{
	try
	{
		return  OmnNew AosDatasetConvertMatrix(*this);
	}

	catch (...)
	{
		    OmnAlarm << "Failed creating dataset" << enderr;
	}

	OmnShouldNeverComeHere;
	return 0;
}
