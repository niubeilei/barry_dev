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
#include "JimoDataProc/DataProcIndex.h"
#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include "Util/DataTypes.h"
#include <boost/make_shared.hpp>

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcIndex_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcIndex(version);
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


AosDataProcIndex::AosDataProcIndex(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcIndex),
mRawInputDocid(0),
mRawOutputRecord(0),
mRawCondition(0)
{
}


AosDataProcIndex::AosDataProcIndex(const AosDataProcIndex &proc)
:
//AosStreamDataProc(1, AosJimoType::eDataProcIndex)
AosStreamDataProc(proc),
mRawInputDocid(0),
mRawOutputRecord(0),
mRawCondition(0)
{
	for (size_t i = 0; i < proc.mInputKeys.size(); i++)
	{
		mInputKeys.push_back(proc.mInputKeys[i]->cloneExpr());
		mRawInputKeys.push_back(mInputKeys[i].getPtr());
	}

	if (proc.mInputDocid)
	{
		mInputDocid = proc.mInputDocid->cloneExpr();
		mRawInputDocid = mInputDocid.getPtr();
	}
	
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
		mRawOutputRecord = mOutputRecord.getPtr();
	}
	
	if (proc.mCondition)
	{
		mCondition = proc.mCondition->cloneExpr();
		mRawCondition = mCondition.getPtr();
	}

	mNumFields = proc.mNumFields;
	mName = proc.mName;
}


AosDataProcIndex::~AosDataProcIndex()
{
}


bool
AosDataProcIndex::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp1"><![CDATA[
	//{
	//	"type": "index",
	//	"record_type":"fixbin|buff",
	//	"keys": ["key_field2"], 
	//	"max_keylen":15,
	//	"docid": "docid",
	//	"condition":"key_field1>2",
	//	"shuffle_type":"cube",
	//	"shuffle_field":"key"
	//}
	//]]></data_proc>
	try
	{
		aos_assert_r(def, false);

		OmnString dp_jsonstr = def->getNodeText();
		mName = def->getAttrStr(AOSTAG_NAME, "");
		aos_assert_r(mName != "", false);

		JSONValue json;
		JSONReader reader;
		bool rslt = reader.parse(dp_jsonstr, json);
		aos_assert_r(rslt, false);
		mJson = json;


		//input keys
		JSONValue key_exprs = json["keys"];
		OmnString key_expr;
		AosExprObjPtr expr;
		OmnString msg;
		for (size_t i = 0; i < key_exprs.size(); i++)
		{
			key_expr = key_exprs[i].asString();
			aos_assert_r(key_expr != "", false);
			key_expr << ";";
OmnScreen << "###1111111 expr : " << key_expr << endl;
			expr = AosParseExpr(key_expr, msg, rdata.getPtr());
			if (!expr)
			{
				AosSetErrorUser(rdata, "syntax_error")
					<< "CondExpr AosParseJQL ERROR:: " 
					<< "" << enderr;
				OmnAlarm << key_expr << enderr;
				return false;
			}
			mInputKeys.push_back(expr);
			mRawInputKeys.push_back(expr.getPtr());
		}
		mNumFields = mInputKeys.size();

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
		mRawInputDocid = mInputDocid.getPtr();

		//condition
		OmnString cond_str = json["condition"].asString();
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
			mRawCondition = mCondition.getPtr();
		}

		//create output record
		rslt = createOutput(mName, json, rdata);
		aos_assert_r(rslt, false);

		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}


bool
AosDataProcIndex::createOutput(
		const OmnString &dpname,
		const JSONValue &json_conf,
		const AosRundataPtr &rdata)
{
	//Jozhi 2015-03-25 implement to BuffArrayVar
	OmnString name = "";
	name << dpname << "_output";

	int max_keylen = json_conf["max_keylen"].asInt();
	if (max_keylen <= 0) max_keylen = 50;

	OmnString shuffle_type = json_conf["shuffle_type"].asString();
	OmnString shuffle_field = json_conf["shuffle_field"].asString();

	OmnString type_str = json_conf["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}

	
	// 2015/07/22 JIMODB-55
	// get keys' type and convert
	//AosDataType::E data_type;
	//OmnString typeStr;
	/*
	if (mInputKeys.size()>1)
	{
		type_str = "string";
	}
	else
	{
		AosDataRecordObjPtr input_record = mInputRecords[0];
		data_type = mInputKeys[0]->getDataType(rdata,input_record.getPtr());
		if (!(AosDataType::isValid(data_type)))
		{
			OmnString msg = "invalid type";
			rdata->setJqlMsg(msg);
			return false;
		}
		typeStr = AosDataType::getTypeStr(data_type);
	}
	*/
	AosDataFieldType::E datafield_type;
	if (mNumFields > 1)
		datafield_type = AosDataFieldType::eStr;
	else
	{
		if(!mIsStreaming)
		{
			AosDataRecordObjPtr input_record = mInputRecords[0];
			AosExprObjPtr input_key = mInputKeys[0];
			datafield_type = convertToDataFieldType(rdata.getPtr(), input_key ,input_record);
			if (!(AosDataFieldType::isValid(datafield_type)))
			{
				OmnString msg = "invalid type";
				rdata->setJqlMsg(msg);
				return false;
			}
		}
		else
		{
			AosXmlTagPtr doc;
			OmnString schema_name = mJson["format"].asString();
			if(!schema_name.isNull())
			{
				doc = AosJqlStatement::getDoc(
						rdata, JQLTypes::eDataRecordDoc, schema_name);
			}
			aos_assert_r(doc, false);

			AosXmlTagPtr recordXml = doc;
			aos_assert_r(recordXml, false);
			AosXmlTagPtr dataFieldsXml = recordXml->getFirstChild("datafields");
			aos_assert_r(dataFieldsXml, false);
			AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();
			aos_assert_r(dataFieldXml, false);

			AosDataFieldType::E type;
			OmnString str_type, value_str;
			OmnString name;
			AosExprObjPtr expr;
			AosExprObjPtr input_key = mInputKeys[0];     
			while (dataFieldXml)
			{
				name = dataFieldXml->getAttrStr("zky_name");
				if(name == input_key->dumpByNoEscape())
				{
					str_type = dataFieldXml->getAttrStr("data_type");
					if(str_type == "varchar")
						str_type = "str";
					if(str_type == "")
						str_type = dataFieldXml->getAttrStr("type");
					type = convertToDataFieldType(rdata.getPtr(), str_type);
					
					datafield_type = type;
					break;
				}
				else if((input_key->dumpByNoEscape()).substr(0, 7) == "getDocid")
				{
					type = convertToDataFieldType(rdata.getPtr(), "u64");
					datafield_type = type;
					break;
				}

				dataFieldXml = dataFieldsXml->getNextChild();
			}
		}
	}
	
	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	output->setField("key", datafield_type, max_keylen);
	//output->setField("key", AosDataFieldType::eStr, max_keylen);
	output->setField("docid", AosDataFieldType::eBinU64, sizeof(u64));

	if(!mIsStreaming)
	{
		output->setCmpFun("custom");
		output->setCmpField("key");
		output->setCmpField("docid");
	}
	else
	{
		//datarecord
		boost::shared_ptr<Output> Compoutput = boost::make_shared<Output>(name, type);

		Compoutput->setField("key", datafield_type, max_keylen);
		Compoutput->setField("docid", AosDataFieldType::eBinU64, sizeof(u64));

		Compoutput->setCmpFun("custom");
		Compoutput->setCmpField("key");
		Compoutput->setCmpField("docid");
		AosXmlTagPtr datasetXml = Compoutput->init2(0, rdata);
		aos_assert_r(datasetXml, false);
		datasetXml = datasetXml->getFirstChild();
		AosXmlTagPtr collectXml = datasetXml->getFirstChild();
		aos_assert_r(collectXml, false);
		AosXmlTagPtr asmXml = collectXml->getFirstChild();
		aos_assert_r(asmXml, false);
		AosXmlTagPtr compDoc = asmXml->getFirstChild("CompareFun");
		aos_assert_r(compDoc, false);
		
		addComp("output", compDoc);
	}

	if (shuffle_type != "")
	{
		output->setAssemblerAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		output->setAssemblerAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setAssemblerAttr("type", "group");

		output->setDataColAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setDataColAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		output->setDataColAttr("zky_type", "group");
		output->setDataColAttr("zky_child_type", "sorted_file");
	}

	output->init(mTaskDocid, rdata);
	mOutputRecord = output->getRecord();
	mRawOutputRecord = mOutputRecord.getPtr();
	mOutputs.push_back(output);
	return true;
}
			

AosDataProcStatus::E
AosDataProcIndex::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	//return AosDataProcStatus::eContinue;
	mProcDataCount++;
	if (mProcDataCount % eShowProcNum == 0)
	{
		updateCounter(mName, eShowProcNum);
	}
	bool rslt;
	bool outofmem = false;
	AosValueRslt key_rslt, docid_rslt;
	AosDataRecordObj * input_record;
	AosDataRecordObj * output_record;

	//input_record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);	
	DumpRecord(input_record,"Index input is: ");

	//condition
	if (mRawCondition)
	{
		AosValueRslt value_rslt;
		mRawCondition->getValue(rdata_raw, input_record, value_rslt);
		rslt = value_rslt.getBool();
		if (!rslt) 
		{
			return AosDataProcStatus::eContinue;
		}
	}

	//key_values
	OmnString key_values = "";
	for (u32 i = 0; i < mNumFields; i++)
	{
		mInputKeys[i]->setIsParsedFieldIdx(false);
		rslt = mRawInputKeys[i]->getValue(rdata_raw, input_record, key_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		if ( mRawInputKeys.size() == 1)
		{
			break;
		}
		if (i > 0) key_values << char(0x01);

		if (key_rslt.isNull())
		{
			//return AosDataProcStatus::eContinue;
			key_values << '\b';		//key1\0x01\0x08
		}
		else 
		{
			key_values << key_rslt.getStr();		//key1\0x01key2
		}
	}

	//get docid
	mInputDocid->setIsParsedFieldIdx(false);
	rslt = mInputDocid->getValue(rdata_raw, input_record, docid_rslt);
	if (!rslt) return AosDataProcStatus::eError;

	//output record
	mRawOutputRecord->clear();
	output_record = mRawOutputRecord;
	if (mNumFields == 1)
	{
		rslt = output_record->setFieldValue(0, key_rslt, outofmem, rdata_raw);
	}
	else
	{
		AosValueRslt k_v(key_values);
		rslt = output_record->setFieldValue(0, k_v, outofmem, rdata_raw);
	}
	if (!rslt) return AosDataProcStatus::eError;

	rslt = output_record->setFieldValue(1, docid_rslt, outofmem, rdata_raw);
	if (!rslt) return AosDataProcStatus::eError;

	DumpRecord(mOutputRecord,"Index output is: ");

	if(!mIsStreaming)
	{
		rslt = flushRecord(output_records, mRawOutputRecord, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}
	else
	{
		OmnString outputName = "output";
		addOutput(rdata_raw, outputName, mOutputRecord);
	}
	//mOutputRecord->flushRecord(rdata_raw);
	mOutputCount++;

	return AosDataProcStatus::eContinue;
}

vector<AosDataRecordObjPtr> 
AosDataProcIndex::getOutputRecords()
{
	vector<AosDataRecordObjPtr> v;
	v.push_back(mOutputRecord);
	return v;
}


AosJimoPtr 
AosDataProcIndex::cloneJimo() const
{
	return OmnNew AosDataProcIndex(*this);
}

AosDataProcObjPtr
AosDataProcIndex::cloneProc() 
{
	return OmnNew AosDataProcIndex(*this);
}


bool 
AosDataProcIndex::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr, 
		const AosJimoProgObjPtr &prog)
{
	AosXmlTagPtr dp_xml;
	
	JSONValue json;
	JSONReader reader;
	bool rslt = reader.parse(jsonstr, json);
	aos_assert_r(rslt, false);
	mJson = json;
	int max_keylen = json["max_keylen"].asInt();
	if (max_keylen <= 0) max_keylen = 50;
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	if(mIsStreaming)
	{
		OmnString shuffle_type;
		OmnString shuffle_field;

		try
		{
			shuffle_type = json["shuffle_type"].asString();
			shuffle_field = json["shuffle_field"].asString();
		}
		catch (...)
		{
			OmnScreen << "JSONException..." << endl;
			return false;
		}

		dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
		dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" <<objid << "\" ";
		dp_str << "><dataproc zky_name=\"";
		dp_str << dpname << "\" jimo_objid=\"dataprocindex_jimodoc_v0\">";
		dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
		dp_str << "</jimodataproc>";

		dp_xml = AosStr2Xml(rdata, dp_str AosMemoryCheckerArgs);
		aos_assert_r(dp_xml, false);

		dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
		dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
		dp_xml->setAttr(AOSTAG_OBJID, objid);
	}
	else
	{
		dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
		dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
			<< AOSTAG_OBJID << "=\"" << objid << "\" ";
		dp_str << "><dataproc zky_name=\"";
		dp_str << dpname << "\" jimo_objid=\"dataprocindex_jimodoc_v0\">";
		dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
		dp_str << " </jimodataproc>";
	}
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


bool
AosDataProcIndex::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	i64 counter = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
		counter += procs[i]->getProcDataCount() % eShowProcNum;
	}
	OmnScreen << "DataProcIndex " << "(" << mName << ")" << " finished:" 
				<< " procDataCount:" << procDataCount
				<< ", OutputCount:" << procOutputCount << endl;

	updateCounter(mName, counter);
	showDataProcInfo(__FILE__, __LINE__, procDataCount, procOutputCount);
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcIndex");
	return true;
}

// JIMODB-55
void
AosDataProcIndex::setInputDataRecords(vector<AosDataRecordObjPtr> &records)
{
	mInputRecords = records;
}
	

bool 
AosDataProcIndex::start(const AosRundataPtr &rdata)
{
	showDataProcInfo(__FILE__, __LINE__, "start", "DataProcIndex");
	return true;
}

