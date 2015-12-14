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
// 09/11/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "QueryFormat/QueryFormatCSV.h"

#include "AosConf/DataRecordCSV.h"
#include "AosConf/DataRecordCtnr.h"
#include "API/AosApiP.h"
#include "JQLExpr/Expr.h"
#include "Jimo/GlobalFuncs.h"

#include "SEInterfaces/RecordsetObj.h"
#include "SEInterfaces/TaskObj.h"

#include <boost/make_shared.hpp>


extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosQueryFormatCSV_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosQueryFormatCSV(version);
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


AosQueryFormatCSV::AosQueryFormatCSV()
:
AosJimo(AosJimoType::eQueryFormatCSV, 0)
{
}


AosQueryFormatCSV::AosQueryFormatCSV(const int version)
:
AosJimo(AosJimoType::eQueryFormatCSV, version)
{
}


AosQueryFormatCSV::AosQueryFormatCSV(const AosQueryFormatCSV &rhs)
{
	AosExprObjPtr expr = NULL;
	for (size_t i = 0; i < (rhs.mFields).size(); ++i)       
	{                                                      
		expr = (rhs.mFields)[i]->cloneExpr(); 
		mFields.push_back(expr);                             
	}                                                          
	if (rhs.mOutputRecord)                     
		mOutputRecord = rhs.mOutputRecord->clone(0 AosMemoryCheckerArgs);   
	if (rhs.mQueryIntoFile)       
		mQueryIntoFile = rhs.mQueryIntoFile->clone();
}


AosQueryFormatCSV::~AosQueryFormatCSV()
{
}


bool
AosQueryFormatCSV::config(
		const AosXmlTagPtr &format,
		const AosXmlTagPtr &output_record,
		const AosRundataPtr &rdata)
{
	//format:
	//	<format name=\"csv\">
	//			<arg name=\"field_delimiter\"><![CDATA[|]]></arg>
	//			<arg name=\"row_delimiter\"><![CDATA[LF]]></arg>
	//			<arg name=\"text_qualifier\"><![CDATA[DQM]]></arg>
	//	</format>
	//
	//
	//output_record:
	//<outputrecord type=\"ctnr\" zky_name=\"doc\">
	//	   <datarecord type=\"xml\" zky_name=\"output_datarecord_t1\">
	//			<datafields>
	//				<datafield type=\"xpath\" zky_name=\"LOC_CHNL_CODE\" zky_precision=\"-1\" zky_scale=\"-1\" zky_value_type=\"str\" zky_xpath=\"LOC_CHNL_CODE/_#text\"//>
	//				......
	//			</datafields>
	//	   </datarecord>
	//</outputrecord>
	aos_assert_r(format, false);

	AosXmlTagPtr arg = format->getNextChild("arg");
	OmnString name = "", value = "";
	map<OmnString, OmnString> argMap;
	while(arg)
	{
		name = arg->getAttrStr("name");
		value = arg->getNodeText();
		argMap.insert(make_pair(name, value));
		arg = format->getNextChild("arg");
	}

	OmnString field_delimiter = ",";
	OmnString row_delimiter = "LF";
	OmnString text_qualifier = "NULL";
	map<OmnString, OmnString>::iterator itr_end = argMap.end();
	map<OmnString, OmnString>::iterator itr = argMap.find("field_delimiter");
	if(itr != itr_end)
	{
		field_delimiter = itr->second;
	}
	itr = argMap.find("row_delimiter");
	if(itr != itr_end)
	{
		row_delimiter = itr->second;
	}
	itr = argMap.find("text_qualifier");
	if(itr != itr_end)
	{
		text_qualifier = itr->second;
	}

	//create output record
	aos_assert_r(output_record, false);

	AosXmlTagPtr data_record = output_record->getFirstChild("datarecord");
	aos_assert_r(data_record, false);

	name = data_record->getAttrStr(AOSTAG_NAME);
	OmnString dr_name = name;
	dr_name << "_output";

	boost::shared_ptr<AosConf::DataRecordCtnr> recordCtnr = boost::make_shared<AosConf::DataRecordCtnr>();
	recordCtnr->setAttribute(AOSTAG_NAME, dr_name);
	boost::shared_ptr<AosConf::DataRecordCSV> dr = boost::make_shared<AosConf::DataRecordCSV>();
	dr->setAttribute(AOSTAG_NAME, dr_name);
	dr->setAttribute(AOSTAG_FIELD_DELIMITER, field_delimiter);
	dr->setAttribute(AOSTAG_ROW_DELIMITER, row_delimiter);
	dr->setAttribute(AOSTAG_TEXT_QUALIFIER,text_qualifier);

	AosXmlTagPtr datafields = data_record->getFirstChild("datafields");
	aos_assert_r(datafields, false);

	AosXmlTagPtr field = datafields->getFirstChild("datafield");
	OmnString msg = "";
	AosExprObjPtr expr = NULL;
	while(field)
	{
		name = field->getAttrStr(AOSTAG_NAME);
		dr->setField(name, "str");

		name << ";";
		expr = AosParseExpr(name, msg, rdata.getPtr());
		aos_assert_r(expr, false);

		mFields.push_back(expr);
		field = datafields->getNextChild("datafield");
	}
	recordCtnr->setRecord(dr);

	OmnString output_str = recordCtnr->getConfig();
	AosXmlTagPtr rcd_xml = AosXmlParser::parse(output_str AosMemoryCheckerArgs);
	aos_assert_r(rcd_xml, false);

//OmnScreen << "output_record:" << rcd_xml->toString() << endl;
	mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mOutputRecord, false);
	return true;
}


bool
AosQueryFormatCSV::proc(
		const AosDatasetObjPtr &dataset,
		AosRundata* rdata)
{
	aos_assert_r(dataset, false);

	AosRecordsetObjPtr record_set = NULL;
	bool rslt = dataset->nextRecordset(rdata, record_set);
	aos_assert_r(rslt, false);
	aos_assert_r(record_set, false);

	AosDataRecordObj *input_record = NULL;    
	AosDataRecordObj * output_record = NULL;
	AosValueRslt value_rslt;
	bool outofmem = false;
	while(1)
	{
		rslt = record_set->nextRecord(rdata, input_record);
		aos_assert_r(rslt, false);

		if(!input_record)   break;       

		mOutputRecord->clear();
		for(size_t i = 0; i < mFields.size(); i++)
		{
			rslt = mFields[i]->getValue(rdata, input_record, value_rslt);
			aos_assert_r(rslt, false);

			output_record = mOutputRecord.getPtr();
			aos_assert_r(output_record, false); 

			rslt = output_record->setFieldValue(i, value_rslt, outofmem, rdata);
			aos_assert_r(rslt, false);
		}
		aos_assert_r(mQueryIntoFile, false);
		rslt = mQueryIntoFile->appendEntry(mOutputRecord.getPtr(), rdata);
	}
	
	rslt = mQueryIntoFile->flush(rdata);
	return true;
}


void
AosQueryFormatCSV::setQueryInto(const AosQueryIntoObjPtr &queryInto)
{
	mQueryIntoFile = queryInto;
}


AosJimoPtr
AosQueryFormatCSV::cloneJimo() const                                 
{
	return OmnNew AosQueryFormatCSV(*this);
}


AosQueryFormatCSVPtr
AosQueryFormatCSV::clone() const                                 
{
	return OmnNew AosQueryFormatCSV(*this);
}

