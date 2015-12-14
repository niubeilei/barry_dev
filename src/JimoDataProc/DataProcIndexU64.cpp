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
#include "JimoDataProc/DataProcIndexU64.h"
#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

static bool mShowLog = false;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcIndexU64_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcIndexU64(version);
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


AosDataProcIndexU64::AosDataProcIndexU64(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcIndexU64)
{
}


AosDataProcIndexU64::AosDataProcIndexU64(const AosDataProcIndexU64 &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mInputKey)
	{
		mInputKey = proc.mInputKey->cloneExpr();
	}

	if (proc.mInputDocid)
	{
		mInputDocid = proc.mInputDocid->cloneExpr();
	}
	
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
	}
	
	if (proc.mCondition)
	{
		mCondition = proc.mCondition->cloneExpr();
	}

}


AosDataProcIndexU64::~AosDataProcIndexU64()
{
}


bool
AosDataProcIndexU64::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	//	"type": "indexu64",
	//	"key": "key_field2", 
	//	"docid": "docid",
	//	"record_type":"fixbin|buff",
	//	"shuffle_type":"cube",
	//	"shuffle_field":"ddd"
	//}
	//]]></data_proc>
	aos_assert_r(def, false);

	OmnString dp_jsonstr = def->getNodeText();
	mName = def->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mName != "", false);

	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(dp_jsonstr, json);
	aos_assert_r(rslt, false);

	if (mShowLog) cout << "dataproc config json is: " << dp_jsonstr << endl;

	//input key
	OmnString key = json["key"].asString();
	aos_assert_r(key != "", false);

	OmnString msg;
	key << ";";
	AosExprObjPtr expr = AosParseExpr(key, msg, rdata.getPtr());
	if (!expr)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;
		OmnAlarm << key << enderr;
		return false;
	}
	mInputKey = expr;

	//input docid
	OmnString docid_expr = json["docid"].asString();
	aos_assert_r(docid_expr != "", false);

	docid_expr << ";";
	mInputDocid = AosParseExpr(docid_expr, msg, rdata.getPtr());
	if (!mInputDocid)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;
		OmnAlarm << docid_expr << enderr;
		return false;
	}

	//condition
	OmnString cond_str = json["condition"].asString("");
	if (cond_str != "")
	{
		cond_str << ";";
		mCondition = AosParseExpr(cond_str, msg, rdata.getPtr());
		if (!mCondition)                                                       
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " << "" << enderr;
			OmnAlarm << cond_str << enderr;
			return false;
		}
	}

	//create output record
	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataProcIndexU64::createOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	//Jozhi 2015-03-25 implement to BuffArrayVar
	OmnString name = "";
	name << dpname << "_output";

	OmnString shuffle_type = json_conf["shuffle_type"].asString();
	OmnString shuffle_field = json_conf["shuffle_field"].asString();

	OmnString type_str = json_conf["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	output->setField("key", AosDataFieldType::eBinU64, sizeof(u64));
	output->setField("docid", AosDataFieldType::eBinU64, sizeof(u64));

	output->setCmpFun("custom");
	output->setCmpField("key");
	output->setCmpField("docid");

	if (shuffle_type != "")
	{
		output->setAssemblerAttr("shuffle_field", shuffle_field);
		output->setAssemblerAttr("shuffle_type", shuffle_type);
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mOutputs.push_back(output);

	return true;
}
			

AosDataProcStatus::E
AosDataProcIndexU64::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt;
	bool outofmem = false;
	AosValueRslt key_rslt, docid_rslt;
	AosDataRecordObj * input_record;
	AosDataRecordObj * output_record;

	//input_record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	DumpRecord(input_record,"Index input is: ");

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

	//key_value
	rslt = mInputKey->getValue(rdata_raw, input_record, key_rslt);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	
	//get docid
	rslt = mInputDocid->getValue(rdata_raw, input_record, docid_rslt);
	if (!rslt) return AosDataProcStatus::eError;

	//output record
	mOutputRecord->clear();
	output_record = mOutputRecord.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	

	rslt = output_record->setFieldValue(0, key_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	rslt = output_record->setFieldValue(1, docid_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	if (mShowLog) OmnScreen << "key: " << key_rslt.getU64()<< " , docid: " << docid_rslt.getU64() << endl;

	DumpRecord(mOutputRecord,"Index output is: ");
	mOutputRecord->flushRecord(rdata_raw);

	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcIndexU64::cloneJimo() const
{
	return OmnNew AosDataProcIndexU64(*this);
}


AosDataProcObjPtr
AosDataProcIndexU64::cloneProc() 
{
	return OmnNew AosDataProcIndexU64(*this);
}


bool 
AosDataProcIndexU64::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocindexu64_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//aos_assert_r(dp_xml, false);
	//
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);
	
	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}

