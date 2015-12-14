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
// 2015/02/11 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcStr2StrMap.h"
#include "DataProc/DataProc.h"

#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcStr2StrMap_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcStr2StrMap(version);
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


AosDataProcStr2StrMap::AosDataProcStr2StrMap(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcStr2StrMap)
{
}


AosDataProcStr2StrMap::AosDataProcStr2StrMap(const AosDataProcStr2StrMap &proc)
:
AosStreamDataProc(0, AosJimoType::eDataProcStr2StrMap)
{
	if (proc.mOutputRecord)
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	for (size_t i = 0; i < proc.mInputKeys.size(); i++)
	{
		AosExprObjPtr expr = proc.mInputKeys[i]->cloneExpr();
		mInputKeys.push_back(expr);
	}
	if (proc.mInputValue)
		mInputValue = proc.mInputValue->cloneExpr();
	if (proc.mCondition)
		mCondition = proc.mCondition->cloneExpr();

	mName = proc.mName;
	mIsStream = proc.mIsStream;

}


AosDataProcStr2StrMap::~AosDataProcStr2StrMap()
{
}


bool
AosDataProcStr2StrMap::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1">
	//	<![CDATA[
	//			{
	//			  "type": "str2strmap",
	//			  "key_fields": ["key_field2", "substr(key_field3, 1,5)"],
	//			  "value_field" : "key_field3",
	//			  "max_length" :35,
	//			  "condition": "key_field1+2>4",
	//			  "record_type":"fixbin|buff"
	//			}
	//			]]>
	//</data_prco>
	aos_assert_r(def, false);

	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);

	OmnString dp_json = def->getNodeText();
	if (dp_json == "") return false;

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_json, json);
	aos_assert_r(rslt, false);
	mJson = json;

	mIsStream = mIsStreaming;
	if(!mIsStream)
	{
		OmnString type = json["type"].asString();
		aos_assert_r(type == "str2strmap", false);
	}
	else
	{
		OmnString type = json["type"].asString();
		aos_assert_r(type == "dataprocstr2strmap", false);
	}

	AosExprObjPtr expr;
	OmnString key_expr;
	//input keys
	OmnString msg;
	JSONValue key_exprs = json["key_fields"];
	for (size_t i = 0; i < key_exprs.size(); i++)
	{
		key_expr = key_exprs[i].asString();
		aos_assert_r(key_expr != "", false);

		key_expr << ";";
		expr = AosParseExpr(key_expr, msg, rdata.getPtr());
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosPaserJQL ERROR:: " 
				<< "" << enderr;
			OmnAlarm << key_expr << enderr;
			return false;
		}
		mInputKeys.push_back(expr);
	}

	//input value
	OmnString value_expr = json["value_field"].asString();
	aos_assert_r(value_expr != "", false);

	value_expr << ";";
	mInputValue = AosParseExpr(value_expr, msg, rdata.getPtr());
	if (!mInputValue)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosPaserJQL ERROR:: " 
			<< "" << enderr;
		OmnAlarm << value_expr << enderr;
		return false;
	}

	//condition
	OmnString condition = json["condition"].asString();
	if (condition != "")
	{
		condition << ";";
		mCondition = AosParseExpr(condition, msg, rdata.getPtr());
		if (!mCondition)            
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			OmnAlarm << condition << enderr;
			return false;
		}
	}

	//create output record
	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosDataProcStr2StrMap::createOutput(
		OmnString &dpname,
		const JSONValue &json,
		const AosRundataPtr &rdata)
{
	//Jozhi 2015-03-25 implement to BuffArrayVar
	OmnString name = "";
	name << dpname << "_output";

	int len = json["max_length"].asInt();
	if (len <= 0) len = 50;

	OmnString type_str = json["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	output->setField("key", AosDataFieldType::eStr, len);
	output->setField("docid", AosDataFieldType::eBinU64, sizeof(u64));

	output->setCmpFun("custom");
	output->setCmpField("key");
	output->setAggrField("docid", "set");

	if(mIsStream)
	{
		AosXmlTagPtr datasetXml = output->init2(0, rdata);
		aos_assert_r(datasetXml, false);
		datasetXml = datasetXml->getFirstChild();
		AosXmlTagPtr collectXml = datasetXml->getFirstChild();
		aos_assert_r(collectXml, false);
		AosXmlTagPtr asmXml = collectXml->getFirstChild();
		aos_assert_r(asmXml, false);
		AosXmlTagPtr compDoc = asmXml->getFirstChild("CompareFun");
		aos_assert_r(compDoc, false);
		
		addComp("output", compDoc);

		AosXmlTagPtr rcd_xml = asmXml->getFirstChild("datarecord");
		aos_assert_r(rcd_xml, false);

		mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata.getPtr() AosMemoryCheckerArgs);
		aos_assert_r(mOutputRecord, false);

		mOutputs.push_back(output);
	}
	else
	{
		output->init(mTaskDocid, rdata);
		mOutputRecord = output->getRecord();
		mOutputs.push_back(output);
	}
	return true;
}


AosDataProcStatus::E
AosDataProcStr2StrMap::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;

	bool rslt;
	bool outofmem = false;
	AosValueRslt key_rslt, value_rslt, docid_rslt;
	AosDataRecordObj *input_record, *output_record;

	//input_record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	//get keys
	OmnString key_values = "";
	for (u32 i = 0; i < mInputKeys.size(); i++)
	{
		rslt = mInputKeys[i]->getValue(rdata_raw, input_record, key_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (key_rslt.isNull())
		{
			return AosDataProcStatus::eContinue;
		}
		if (i > 0) key_values << char(0x01);
		key_values << key_rslt.getStr();   //key1\0x01key2\0x01key3
	}

	//get value
	rslt = mInputValue->getValue(rdata_raw, input_record, value_rslt);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	if (value_rslt.isNull())
	{
		return AosDataProcStatus::eContinue;
	}
	OmnString value = value_rslt.getStr();
	//outputkey format is:[key1\0x01key2\0x01key3\0x02value]
	
	if (key_values == "" || value == "") 
	{
		return AosDataProcStatus::eContinue;
	}
	key_values << char(0x02) << value;

	//condition
	if (mCondition)
	{
		AosValueRslt value_rslt;
		mCondition->getValue(rdata_raw, input_record, value_rslt);
		rslt = value_rslt.getBool();
		if (!rslt) 
		{
			return AosDataProcStatus::eContinue;
		}
	}

	//output_record
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	

	//set outputkey field
	AosValueRslt k_v(key_values);
	rslt = output_record->setFieldValue(0, k_v, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	//set outputdocid field
	docid_rslt.setU64(0);
	rslt = output_record->setFieldValue(1, docid_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	if(!mIsStream)
	{
		rslt = flushRecord(output_records, mOutputRecord.getPtr(), rdata_raw);
	}
	else
	{
		addOutput(rdata_raw, "output", mOutputRecord);
	}
	aos_assert_r(rslt, AosDataProcStatus::eError);

	//mOutputRecord->flushRecord(rdata_raw);
	mOutputCount++;

	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcStr2StrMap::cloneJimo() const
{
	return OmnNew AosDataProcStr2StrMap(*this);
}


AosDataProcObjPtr                                        
AosDataProcStr2StrMap::cloneProc()
{
	return OmnNew AosDataProcStr2StrMap(*this);
}


bool 
AosDataProcStr2StrMap::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	string objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	string dp_name = dpname.data();
	string dp_str;
	dp_str = "<jimodataproc " + string(AOSTAG_CTNR_PUBLIC) + "=\"true\" ";
	dp_str +=  string(AOSTAG_PUBLIC_DOC) + "=\"true\" " + string(AOSTAG_OBJID) + "=\"" + objid + "\" ";
	dp_str += "><dataproc zky_name=\"";
	dp_str += dp_name + "\" jimo_objid=\"dataprocstr2strmap_jimodoc_v0\">";
	dp_str += "<![CDATA[" + string(jsonstr) + "]]></dataproc>";
	dp_str += "</jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);

	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, objid);
	//return AosCreateDoc(dp_xml->toString(), true, rdata);

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


bool
AosDataProcStr2StrMap::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcStr2StrMap " << "(" << mName << ")" << " finished:" 
				<< " procDataCount:" << procDataCount 
				<< ", OutputCount:" << procOutputCount << endl;
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcStr2StrMap");
	return true;
}
