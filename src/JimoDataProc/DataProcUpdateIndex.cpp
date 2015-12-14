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
#include "JimoDataProc/DataProcUpdateIndex.h"
#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcUpdateIndex_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcUpdateIndex(version);
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


AosDataProcUpdateIndex::AosDataProcUpdateIndex(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcUpdateIndex)
{
}


AosDataProcUpdateIndex::AosDataProcUpdateIndex(const AosDataProcUpdateIndex &proc)
:
AosJimoDataProc(proc)
{
	if (proc.mNewValue)                            
	{                                                
		mNewValue = proc.mNewValue->cloneExpr(); 
	}                                                

	if (proc.mOldValue)                            
	{                                                
		mOldValue = proc.mOldValue->cloneExpr(); 
	}                                                

	if (proc.mInputDocid)
	{
		mInputDocid = proc.mInputDocid->cloneExpr();
	}

	if (proc.mOutputRecord1)
	{
		mOutputRecord1 = proc.mOutputRecord1->clone(0 AosMemoryCheckerArgs);
	}

	if (proc.mOutputRecord2)                                               
	{                                                                     
		mOutputRecord2 = proc.mOutputRecord2->clone(0 AosMemoryCheckerArgs);  
	}                                                                     
}


AosDataProcUpdateIndex::~AosDataProcUpdateIndex()
{
}


bool
AosDataProcUpdateIndex::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	//	"type": "updateindex",
	//	"record_type":"fixbin|buff",
	//  "new_value": ["new_value"], 
	//  "old_value": ["old_value"],
	//  "max_keylen":15,
	//  "shuffle_type":"cube",
	//  "shuffle_field":"xxx",
	//  "docid": "docid"
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

	//input keys
	OmnString new_value = json["new_value"].asString();
	AosExprObjPtr expr;
	OmnString msg;
	aos_assert_r(new_value != "", false);

	new_value << ";";
	expr = AosParseExpr(new_value, msg, rdata.getPtr());
	if (!expr)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;
		OmnAlarm << new_value << enderr;
		return false;
	}
	mNewValue= expr;


	OmnString old_value = json["old_value"].asString();
	aos_assert_r(old_value != "", false);

	old_value << ";";
	expr = AosParseExpr(old_value, msg, rdata.getPtr());
	if (!expr)
	{
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " 
			<< "" << enderr;
		OmnAlarm << old_value << enderr;
		return false;
	}
	mOldValue = expr;

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

	//create output record
	rslt = createOutput(mName, json, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosDataProcUpdateIndex::createOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	//Jozhi 2015-03-25 implement to BuffArrayVar
	OmnString name1 = "";
	OmnString name2 = "";
	name1 << dpname << "_output_add";
	name2 << dpname << "_output_del";

	int max_keylen = json_conf["max_keylen"].asInt();
	if (max_keylen <= 0) max_keylen = 50;

	OmnString shuffle_type = json_conf["shuffle_type"].asString();
	OmnString shuffle_field = json_conf["shuffle_field"].asString();

	OmnString type_str = json_conf["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> new_value = boost::make_shared<Output>(name1, type);
	new_value->setField("key", AosDataFieldType::eStr, max_keylen);
	new_value->setField("docid", AosDataFieldType::eBinU64, sizeof(u64));

	new_value->setCmpFun("custom");
	new_value->setCmpField("key");
	new_value->setCmpField("docid");

	boost::shared_ptr<Output> old_value = boost::make_shared<Output>(name2, type);
	old_value->setField("key", AosDataFieldType::eStr, max_keylen);
	old_value->setField("docid", AosDataFieldType::eBinU64, sizeof(u64));

	old_value->setCmpFun("custom");
	old_value->setCmpField("key");
	old_value->setCmpField("docid");


	if (shuffle_type != "")
	{
		new_value->setShuffAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		new_value->setShuffAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
//		new_value->setAssemblerAttr("shuffle_field", shuffle_field);
//		new_value->setAssemblerAttr("shuffle_type", shuffle_type);
//		new_value->setDataColAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
//		new_value->setDataColAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		new_value->setDataColAttr("zky_type", "group");
		new_value->setDataColAttr("zky_child_type", "sorted_file");
	}

	if (shuffle_type != "")
	{
		old_value->setShuffAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		old_value->setShuffAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
//		old_value->setAssemblerAttr("shuffle_field", shuffle_field);
//		old_value->setAssemblerAttr("shuffle_type", shuffle_type);
//
//		old_value->setDataColAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
//		old_value->setDataColAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		old_value->setDataColAttr("zky_type", "group");
		old_value->setDataColAttr("zky_child_type", "sorted_file");
	}
	new_value->init(mTaskDocid, rdata);
	old_value->init(mTaskDocid, rdata);
	mOutputRecord1 = new_value->getRecord();
	mOutputRecord2 = old_value->getRecord();
	mOutputs.push_back(new_value);
	mOutputs.push_back(old_value);
	return true;
}
			

AosDataProcStatus::E
AosDataProcUpdateIndex::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;

	bool rslt;
	bool outofmem = false;
	AosValueRslt key_rslt, docid_rslt;
	AosDataRecordObj * input_record;
	AosDataRecordObj * output_record;

	//input_record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	

	//key_values
	OmnString key_values = "";
	rslt = mNewValue->getValue(rdata_raw, input_record, key_rslt);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	if (key_rslt.isNull())
	{
		return AosDataProcStatus::eContinue;
	}

	rslt = mInputDocid->getValue(rdata_raw, input_record, docid_rslt); 
	if (!rslt) return AosDataProcStatus::eError;                       

	mOutputRecord1->clear();                               
	output_record = mOutputRecord1.getPtr();               
	aos_assert_r(output_record, AosDataProcStatus::eError);

	AosValueRslt k_v(key_rslt);                                        
	rslt = output_record->setFieldValue(0, k_v, outofmem, rdata_raw);  
	if (!rslt) return AosDataProcStatus::eError;                       

	AosValueRslt d_v(docid_rslt);                                      
	rslt = output_record->setFieldValue(1, d_v, outofmem, rdata_raw);  
	if (!rslt) return AosDataProcStatus::eError;                       

	key_values = "";
	rslt = mOldValue->getValue(rdata_raw, input_record, key_rslt);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	if (key_rslt.isNull())
	{
		return AosDataProcStatus::eContinue;
	}

	//get docid
	rslt = mInputDocid->getValue(rdata_raw, input_record, docid_rslt);
	if (!rslt) return AosDataProcStatus::eError;

	//output record
	mOutputRecord2->clear();
	output_record = mOutputRecord2.getPtr();
	aos_assert_r(output_record, AosDataProcStatus::eError);	

	AosValueRslt k_s(key_rslt);                                        
	rslt = output_record->setFieldValue(0, k_s, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;
	
	AosValueRslt d_s(docid_rslt);
	rslt = output_record->setFieldValue(1, d_s, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	mOutputRecord1->flushRecord(rdata_raw);
	mOutputRecord2->flushRecord(rdata_raw);
	mOutputCount++;

	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcUpdateIndex::cloneJimo() const
{
	return OmnNew AosDataProcUpdateIndex(*this);
}


AosDataProcObjPtr
AosDataProcUpdateIndex::cloneProc() 
{
	return OmnNew AosDataProcUpdateIndex(*this);
}


bool 
AosDataProcUpdateIndex::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocupdateindex_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

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
AosDataProcUpdateIndex::finish(
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
	OmnScreen << "DataProcIndex " << "(" << mName << ")" << "finished:" 
				<< ", procDataCount:" << procDataCount
				<< ", OutputCount:" << procOutputCount << endl;
	return true;
}
