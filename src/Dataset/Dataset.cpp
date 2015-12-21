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
// 2013/11/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Dataset/Dataset.h"

#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/DatasetConverterObj.h"


AosDataset::AosDataset(
		const OmnString &type,
		const u32 version)
:
AosDatasetObj(version),
mJimoSubType(type)
{
}


AosDataset::~AosDataset()
{
}


bool 
AosDataset::reset(const AosRundataPtr &rdata)
{
	OmnShouldNeverComeHere;
	return false;
}


int64_t 
AosDataset::getNumEntries()
{
	OmnShouldNeverComeHere;
	return -1;
}


bool 
AosDataset::nextRecordset(
		const AosRundataPtr &rdata, 
		AosRecordsetObjPtr &recordset)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataset::stop (
		const AosRundataPtr &rdata, 
		const AosRecordsetObjPtr &recordset)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataset::addDataConnector(
	const AosRundataPtr &rdata, 
	const AosDataConnectorObjPtr &data_cube)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool 
AosDataset::addUnrecogContents(
	const AosRundataPtr &rdata,
	const AosBuffDataPtr &buff_data,
	const char *data,
	const int64_t &start_pos,
	const int64_t &length)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataset::addInvalidContents(
	const AosRundataPtr &rdata,
	AosDataRecordObj * record)
{
	OmnShouldNeverComeHere;
	return false;
}

	
bool 
AosDataset::addFilterContents(
	const AosRundataPtr &rdata,
	AosDataRecordObj * record)
{
	OmnShouldNeverComeHere;
	return false;
}


AosJimoPtr 
AosDataset::cloneJimo()  const
{
	OmnShouldNeverComeHere;
	return NULL;
}


// Chen Ding, 2014/08/13
bool 
AosDataset::addContents(AosRundata *rdata, AosBuff *data)
{
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosDataset::addContents(AosRundata *rdata, AosBuffArrayVar *data)
{
	OmnShouldNeverComeHere;
	return false;
}


// Chen Ding, 2015/01/28
vector<AosDatasetObjPtr>
AosDataset::split(AosRundata *rdata)
{
	OmnShouldNeverComeHere;
	vector<AosDatasetObjPtr> vec;
	return vec;
}

//helper method: translate dataset data into
//string
bool
AosDataset::data2Str(
		const AosRundataPtr &rdata,
		OmnString &contents)
{
	bool rslt;
	AosRecordsetObjPtr recordset;
	rslt = nextRecordset(rdata, recordset);
	aos_assert_r(rslt, false);

	AosXmlTagPtr infor;
	rslt = getRsltInfor(infor, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(infor, false);

	if (recordset)
	{
		OmnString rslt_str;
		AosDataRecordObj * rcd;

		AosRundata *rdata_raw = rdata.getPtr();
		while (1)
		{
			rslt = recordset->nextRecord(rdata.getPtr(), rcd);
			aos_assert_r(rslt, false);
			if (!rcd) break;
			rslt_str.append(rcd->getData(rdata_raw), rcd->getRecordLen());
		}

		rslt = infor->setText(rslt_str, false);
		aos_assert_r(rslt, false);
	}

	contents = infor->toString();
	return true;
}


//convert one dataset to another dataset
bool
AosDataset::convert(
		AosRundata* rdata_raw,
		AosDatasetObjPtr &output_dataset,
		const AosXmlTagPtr &def,
		const AosDatasetObjPtr &dataset)
{
	//<convert name="matrix">
	//	<arg name="x"><![CDATA[key_field1]]></arg>
	//	<arg name="y"><![CDATA[key_field2]]></arg>
	//	<arg name="cell"><![CDATA[key_field3]]></arg>
	//	<arg name="xrange"><![CDATA[20150901, day, 30]]></arg>
	//	<arg name="yrange"><![CDATA[20150901, day, 30]]></arg>
	//</convert>
	AosJimoPtr jimo = AosCreateJimoByName(rdata_raw, "dataset_converter", "matrix", 1);
	aos_assert_r(jimo, false);
	AosDatasetConverterObj* convert = dynamic_cast<AosDatasetConverterObj*>(jimo.getPtr());
	aos_assert_r(convert, false);
	bool rslt = convert->config(def, rdata_raw);
	aos_assert_r(rslt, false);
	rslt = convert->procData(rdata_raw, dataset.getPtr());
	aos_assert_r(rslt, false);
	output_dataset = convert->getOutput();
	return true;
}

