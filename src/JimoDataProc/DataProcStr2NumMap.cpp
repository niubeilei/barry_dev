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
// 2013/12/23 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcStr2NumMap.h"
#include "DataProc/DataProc.h"

#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcStr2NumMap_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcStr2NumMap(version);
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


AosDataProcStr2NumMap::AosDataProcStr2NumMap(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcStr2NumMap)
{
}


AosDataProcStr2NumMap::AosDataProcStr2NumMap(const AosDataProcStr2NumMap &proc)
:
AosStreamDataProc(0, AosJimoType::eDataProcStr2NumMap)
{
	if (proc.mOutputRecord)
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	for (size_t i = 0; i < (proc.mInputKeys).size(); ++i)
	{
		AosExprObjPtr expr = (proc.mInputKeys)[i]->cloneExpr();
		mInputKeys.push_back(expr);
	}
	if (proc.mInputValue)
		mInputValue = (proc.mInputValue)->cloneExpr();
	if (proc.mCondition)
		mCondition = (proc.mCondition)->cloneExpr();

	mIsStream = proc.mIsStream;
	mName = proc.mName;
}

AosDataProcStr2NumMap::~AosDataProcStr2NumMap()
{
}


bool
AosDataProcStr2NumMap::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1">
	//	<![CDATA[
	//			{
	//			  "type": "str2nummap",
	//			  "key_fields": ["key_field2", "substr(key_field3, 0, 5)"],
	//			  "max_length" :30,
	//			  "value_field" : "key_field1",
	//			  "data_type": "int|double|u64",
	//			  "aggr_opr":"sum|max|min|set",
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

	
	mIsStream = mIsStreaming;
	if(!mIsStream)
	{
		OmnString type = json["type"].asString();
		aos_assert_r(type == "str2nummap", false);
	}
	else
	{
		OmnString type = json["type"].asString();
		aos_assert_r(type == "dataprocstr2nummap", false);
	}

	//input keys
	AosExprObjPtr expr;
	OmnString key_expr;
	JSONValue key_exprs = json["key_fields"];
	OmnString msg;
	for (size_t i = 0; i < key_exprs.size(); i++)
	{
		key_expr = key_exprs[i].asString();
		aos_assert_r(key_expr != "", false);

		key_expr << ";";
		expr = AosParseExpr(key_expr, msg, rdata.getPtr());
		if(!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosPaserJQL error:: " 
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
			<< "CondExpr AosPaserJQL error:: " 
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

	//create output records
	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataProcStr2NumMap::createOutput(
		OmnString &dp_name,
		JSONValue &json,
		const AosRundataPtr &rdata)
{
	//Jozhi 2015-03-30 implement to BuffArrayVar
	OmnString name = "";
	name << dp_name << "_output";

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


	string aggr_opr = json["aggr_opr"].asString();
	string data_type = json["data_type"].asString();
	AosDataFieldType::E ftype = convertToDataFieldType(rdata.getPtr(), data_type);
	aos_assert_r(ftype != AosDataFieldType::eInvalid, false);
	output->setField("value", ftype, sizeof(u64));

	//string data_type = json["data_type"].asString();
	//if (data_type == "") data_type = "bin_int64";
	//if (data_type == "u64")	data_type = "bin_u64";
	//if (data_type == "int" || data_type == "int64") 		
	//{
	//	data_type = "bin_int64";
	//}
	//if (data_type == "double") 	data_type = "bin_double";
	//string aggr_opr = json["aggr_opr"].asString();
	//if(aggr_opr == "count") data_type = "bin_int64";
	//output->setField("value", AosDataFieldType::toEnum(data_type.data()), sizeof(u64));

	output->setCmpFun("custom");
	output->setCmpField("key");

	if ((aggr_opr != "sum" && aggr_opr != "max" && aggr_opr != "min" && aggr_opr != "set" && aggr_opr != "count") || aggr_opr == "") 
	{
		aggr_opr = "set";
	}
	if (aggr_opr == "set") 
	{
		output->setCmpField("value");
	}
	if(aggr_opr == "count")
	{
		OmnString value_expr = "1;";
		OmnString msg;
		mInputValue = AosParseExpr(value_expr, msg, rdata.getPtr());
		aggr_opr = "sum";
	}
	output->setAggrField("value", aggr_opr);

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
AosDataProcStr2NumMap::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;
	
	bool rslt;
	AosDataRecordObj *input_record, *output_record;
	bool outofmem = false;

	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	//get keys
	OmnString key_values = "";
	AosValueRslt key_rslt;
	for (u32 i = 0; i < mInputKeys.size(); i++)
	{
		rslt = mInputKeys[i]->getValue(rdata_raw, input_record, key_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (key_rslt.isNull() || (key_rslt.getStr() == ""))
		{
			return AosDataProcStatus::eContinue;
		}
		if (i > 0) key_values << char(0x01);
		key_values << key_rslt.getStr();   //key1\0x01key2\0x01key3
	}

	//get value
	AosValueRslt value_rslt;
	rslt = mInputValue->getValue(rdata_raw, input_record, value_rslt);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	if (value_rslt.isNull() || key_values == "")
	{
		return AosDataProcStatus::eContinue;
	}

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

	//output record
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	
	
	//set outputkey field
	AosValueRslt k_v(key_values);
	rslt = output_record->setFieldValue(0, k_v, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	//set outputvalue field
	rslt = output_record->setFieldValue(1, value_rslt, outofmem, rdata_raw);
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
AosDataProcStr2NumMap::cloneJimo() const
{
	return OmnNew AosDataProcStr2NumMap(*this);
}


AosDataProcObjPtr                                        
AosDataProcStr2NumMap::cloneProc()
{
	return OmnNew AosDataProcStr2NumMap(*this);
}


bool 
AosDataProcStr2NumMap::createByJql(
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
	dp_str += dp_name + "\" jimo_objid=\"dataprocstr2nummap_jimodoc_v0\">";
	dp_str += "<![CDATA[" + string(jsonstr) + "]]></dataproc>";
	dp_str += "</jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);
	//
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, objid);
	//return AosCreateDoc(dp_xml->toString(), true, rdata);

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}


bool
AosDataProcStr2NumMap::finish(
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
	OmnScreen << "DataProcStr2NumMap " << "(" << mName << ")" << " finished:" 
				<< " procDataCount:" << procDataCount 
				<< ", OutputCount:" << procOutputCount << endl;
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcStr2NumMap");
	return true;
}
