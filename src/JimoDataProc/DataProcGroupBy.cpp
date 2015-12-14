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
#include "JimoDataProc/DataProcGroupBy.h"

#include "DataProc/DataProc.h"
#include "API/AosApi.h"
#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h"
#include "UtilTime/TimeUtil.h"    
#include "StatUtil/StatTimeUnit.h"
#include "StatUtil/Jimos/StatModel.h"
#include <boost/make_shared.hpp>
using boost::shared_ptr;

#define COUNTER	true;

extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcGroupBy_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcGroupBy(version);
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


AosDataProcGroupBy::AosDataProcGroupBy(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcGroupBy)
{
	mConfigCalled = 0;
	mDistCountIdx = 0;
	mTimeField = "";
	mTimeFormat = "";
	mIsDistCount = false;
}


AosDataProcGroupBy::AosDataProcGroupBy(const AosDataProcGroupBy &proc)
:
//AosStreamDataProc(0, AosJimoType::eDataProcGroupBy)
AosStreamDataProc(proc)
{
	if (proc.mOutputRecord)
	{
		mOutputRecord = proc.mOutputRecord->clone(0 AosMemoryCheckerArgs);
		mOutputRecordRaw = mOutputRecord.getPtr();
	}
	mStatKeys = proc.mStatKeys;
	mStatKeysSize = proc.mStatKeysSize;
	mInputMeasures = proc.mInputMeasures;
	mInputMeasuresSize = proc.mInputMeasuresSize;
	mStatInputTime = proc.mStatInputTime;
	mMeasures = proc.mMeasures;
 	if (proc.mConditionPtr)
	          mConditionPtr = proc.mConditionPtr->cloneExpr();
	mCondition = mConditionPtr.getPtr();
	mTimeField = proc.mTimeField;
	mTimeFormat = proc.mTimeFormat;
	mTimeUnit = proc.mTimeUnit;
	mCountValue = proc.mCountValue;

	//mOutputs = proc.mOutputs;
	mConfigCalled = 0;
	
	//arvin		
	mInfoFields = proc.mInfoFields;
	mInfoFieldsSize = proc.mInfoFieldsSize;
	mInputRecords = proc.mInputRecords;
	mIsGenFieldIdx = false;
	mHasKey = proc.mHasKey;	
	//for test
	mIsDistCount = proc.mIsDistCount;
	mDistCountIILEntryMap = proc.mDistCountIILEntryMap;
	mDistCountMeasureKeys.clear();
	mDistCountMeasureValues.clear();
	mDistCountIdx = 0;	
	mName = proc.mName;
	mTmpBuff = OmnNew AosBuff(DEFAULT_BUFF_SIZE AosMemoryCheckerArgs);
	initCounters();
}


AosDataProcGroupBy::~AosDataProcGroupBy()
{
}

bool
AosDataProcGroupBy::initCounters()
{
	mTimeTotal = 0;       
	mTimeCondition = 0;   
	mTimeConditionNum = 0;   
	mRecordClear  = 0 ;     
	mTimeStatKey = 0;     
	mTimeStatKeyNum = 0;     
	mTimeInfoKey = 0;     
	mTimeInfoKeyNum = 0;     
	mTimeTrans = 0;       
	mTimeFieldTime = 0;       
	mTimeFieldTimeNum = 0;       
	mTimeGenIdx = 0;      
	mTimeGenIdxNum = 0;      
	mTimeMeasure = 0;     
	mTimeMeasureNum = 0;     
	mTimeFlush = 0;       
	mTimeFlushNum = 0;       
	mTimeConfig = 0;      
	mTimeOutput = 0;      
	return true;
}

bool
AosDataProcGroupBy::outputCounters()
{
	OmnScreen << "groupby count : " << mName << "\n"
		<< "total Time:   " << mTimeTotal << "\n"
		<< "GenFeidlIdx:" << mTimeGenIdx << " "<< mTimeGenIdxNum <<"\n"
		<< "condition :   " << mTimeCondition << " " << mTimeConditionNum <<"\n"
		<< "clear record: " << mRecordClear << "\n"
		<< "statKey :     " << mTimeStatKey << " " << mTimeStatKeyNum <<"\n"
		<< "InfoKey:"		<< mTimeInfoKey << " " << mTimeInfoKeyNum << "\n"
		<< "Time tanslate:" << mTimeTrans << "\n"
		<< "TimeField:    " << mTimeFieldTime <<" " << mTimeFieldTimeNum <<"\n"
		<< "TimeMeasure  :" << mTimeMeasure << " "<< mTimeMeasureNum<< "\n"
		<< "flush record :" << mTimeFlush << " " << mTimeFlushNum<<"\n"
		<< "config       :" << mTimeConfig << "\n"
		<< "create output:" << mTimeOutput << endl;
	return true;
}

bool 
AosDataProcGroupBy::finish(
		const vector<AosDataProcObjPtr> &procs, 
		const AosRundataPtr &rdata)
{
	bool rslt;
	i64 procDataCount = 0;
	i64 procOutputCount = 0;
	for (size_t i = 0; i < procs.size(); i++)
	{
		rslt = procs[i]->finish(rdata);
		aos_assert_r(rslt,false);
		procDataCount += procs[i]->getProcDataCount();
		procOutputCount += procs[i]->getProcOutputCount();
	}
	OmnScreen << "DataProcGroupBy" << "(" << mName << ")" << "finished:" 
		<< ", procDataCount:" << procDataCount 
		<< ", OutputCount:" << procOutputCount << endl;

	showDataProcInfo(__FILE__, __LINE__, procDataCount, procOutputCount);
	showDataProcInfo(__FILE__, __LINE__, "finish", "DataProcGroupBy");

	return true;
}

bool 
AosDataProcGroupBy::finish(const AosRundataPtr &rdata)
{
	if(mIsDistCount)
	{
		bool rslt = flushRecordset(rdata.getPtr());
		aos_assert_r(rslt,false);
	}
//	outputCounters();
	return true;
}


bool
AosDataProcGroupBy::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	// Example:
	// 	select city, town, sum(population) from t
	// 	group by city, town;
	//
	//<dataproc type="groupby"><![CDATA[
	//	{
	//	create dataprocgroupby dp4
	//	{
	//	  "type":"groupby",
	//	  "keys":["k1","k2", "k3", "k4", "k5", "k6"],
	//	  "measure_inputs":["sum(v1)", "sum(v2)"],
	//	  "max_keylen":50,
	//	  "time": "d1",
	//	  "append_non_existing_field" : "false",
	//	  "count_value" : 0 | 1
	//	          };
	//	}
	//]]></dataproc>
	aos_assert_r(!mConfigCalled, false);
	try
	{
		OmnString dp_jsonstr = def->getNodeText();
		mConfigCalled++;

		aos_assert_r(def, false);
		mName = def->getAttrStr("zky_name", "");
		aos_assert_r(mName != "", false);

		JSONValue json;
		JSONReader reader;
		reader.parse(dp_jsonstr, json);
		mJson = json;

		if(mIsStreaming)
		{
			string type = json["type"].asString();
			aos_assert_r(type == "dataprocgroupby", false);
		}
		else
		{
			string type = json["type"].asString();
			aos_assert_r(type == "groupby", false);
		}

		OmnString key,errmsg;
		AosExprObjPtr expr;
		//parse info_fields
		JSONValue info_fields = json["info_fields"];
		for(size_t  i = 0;i < info_fields.size();i++)
		{
			key = info_fields[i].asString();
			mInfoFields.push_back(key);
			key << ";";
			expr = AosParseExpr(key, errmsg, rdata.getPtr());
			if (!expr)
			{
				AosSetErrorUser(rdata, "syntax_error")
					<< "CondExpr AosParseJQL ERROR:: " 
					<< errmsg << enderr;
				return false;
			}
			mInfoFieldExprList.push_back(expr);
		}
		mInfoFieldsSize = mInfoFields.size();
		//input keys
		JSONValue key_fields = json["keys"];
		//arvin 2015.10.23
		//JIMODB-1014,if keys just have one key and key_field_name is equal to "const_statkey",
		//is means the statmodel have no group by key
		mHasKey = true;
		if(key_fields.size()==1)
		{
			key = key_fields[(u32)0].asString("");
			if(key == AOS_EMPTY_KEY)
				mHasKey = false;
		}
		for (size_t i = 0; i < key_fields.size(); i++)
		{
			key = key_fields[i].asString("");
			aos_assert_r(key != "", false);
			if(count(mInfoFields.begin(),mInfoFields.end(),key))
				continue;
			key << ";";
			OmnString errmsg;
			expr = AosParseExpr(key, errmsg, rdata.getPtr());
			if (!expr)
			{
				AosSetErrorUser(rdata, "syntax_error")
					<< "CondExpr AosParseJQL ERROR:: " 
					<< errmsg << enderr;
				return false;
			}
			mStatKeys.push_back(expr);
		}
		mStatKeysSize = mStatKeys.size();

		if(!mIsStreaming)
		{
			//condition
			OmnString cond_str = json["condition"].asString();
			if (cond_str != "")
			{
				OmnString condstr = cond_str.append(";",1);
				OmnString errmsg;
				mConditionPtr = AosParseExpr(condstr, errmsg, rdata.getPtr());
				if (!mConditionPtr)
				{
					AosSetErrorUser(rdata, "syntax_error")
						<< "CondExpr AosParseJQL ERROR:: " << "" << enderr;
					return false;
				}
				mCondition = mConditionPtr.getPtr();
			}
		}

		//input measures
		JSONValue measures = json["measures"];
		OmnString measure;
		for (size_t i = 0; i < measures.size(); i++)
		{
			measure = measures[i].asString("");
			aos_assert_r(measure != "", false);
			measure << ";";
			OmnString errmsg;
			expr = AosParseExpr(measure, errmsg, rdata.getPtr());
			if (!expr)
			{
				AosSetErrorUser(rdata, "syntax_error")
					<< "CondExpr AosParseJQL ERROR:: " 
					<< errmsg << enderr;
				return false;
			}
			aos_assert_r(AosExpr::checkIsAgrFunc(expr), false);

			AosExprGenFunc* funcExpr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());
			aos_assert_r(funcExpr->getParmList()->size() > 0, false);
			mMeasures.push_back(expr);
			mInputMeasures.push_back((*(funcExpr->getParmList()))[0]);
		}
		mInputMeasuresSize = mInputMeasures.size();

		JSONValue time = json["time"];
		mTimeField = JSONValue(time["field"]).asString("");
		mTimeFormat = JSONValue(time["format"]).asString("");
		OmnString timeUnit = JSONValue(time["unit"]).asString("");
		if(timeUnit != "")
		{	
			mTimeUnit = AosStatTimeUnit::getTimeUnit(timeUnit);
		}	
		//input time
		OmnString input_time = mTimeField;
		if (input_time != "") 
		{
			input_time << ";";
			OmnString errmsg;
			mStatInputTime = AosParseExpr(input_time, errmsg, rdata.getPtr());
			if (!mStatInputTime)
			{
				AosSetErrorUser(rdata, "syntax_error")
					<< "CondExpr AosParseJQL ERROR:: " 
					<< errmsg << enderr;
				return false;
			}
		}

		//count_value
		if (json.isMember("count_value"))
		{
			mCountValue = json["count_value"].asInt(0);
		}
		else
		{
			mCountValue = 1;
		}

		bool rslt;
		if(mIsStreaming)
			rslt = createOutputDataRecord(mName, json, rdata);
		else
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


void
AosDataProcGroupBy::setInputDataRecords(
		vector<AosDataRecordObjPtr> &records)
{
	OmnString name = "";
	AosDataRecordObjPtr inputrcd;
	vector<AosDataFieldObjPtr> input_fields;
	map<OmnString, AosDataFieldObjPtr>::iterator itr;

	mInputRecords = records;
	inputrcd = records[0];

	input_fields = inputrcd->getFields();
	for(size_t i = 0; i < input_fields.size(); i++)
	{
		name = input_fields[i]->getName();
		mInputRcdFieldTypes.push_back(input_fields[i]->getType());
	}
}

	
bool
AosDataProcGroupBy::createOutputDataRecord(
		OmnString &dp_name,
		JSONValue &json,
		const AosRundataPtr &rdata)
{
	//data record name
	OmnString dr_name = dp_name;
	dr_name << "_output";
	//dp_name << "_output";

	OmnString type_str = json["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	int max_keylen = json["max_keylen"].asInt();
	if (max_keylen <= 0) max_keylen = 50;


	OmnString shuffle_type, shuffle_field, datacol_type, child_type;
	shuffle_type = json["shuffle_type"].asString();
	shuffle_field = json["shuffle_field"].asString();

	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(dr_name, type);
	if(mStatKeysSize)
	{
		for(size_t i = 0;i < mStatKeysSize;i++)
		{
			OmnString key = mStatKeys[i]->dumpByNoEscape();
			OmnString fType, name;
			if(mIsStreaming)
			{
				OmnString schema_name = "";
				AosXmlTagPtr doc;
				schema_name = mJson["format"].asString("");
				if (schema_name != "")
				{
					doc = AosJqlStatement::getDoc(
							rdata, JQLTypes::eDataRecordDoc, schema_name);
				}
				AosXmlTagPtr dataFieldsXml = doc->getFirstChild("datafields"); 
				aos_assert_r(dataFieldsXml, false);                                  
				AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();          
				aos_assert_r(dataFieldXml, false);                                   
				while (dataFieldXml)
				{
					name = dataFieldXml->getAttrStr("zky_name"); 
					if(name == key)
						fType = dataFieldXml->getAttrStr("type"); 

					dataFieldXml = dataFieldsXml->getNextChild();
				}
			}
			const OmnString cType = fType;
			AosDataFieldType::E type = convertToDataFieldType(rdata.getPtr(), cType);
			output->setField(key, type, max_keylen);
			//output->setCmpField(key);
		}
		
		for(size_t i = 0;i < mInfoFieldsSize;i++)
		{
			AosDataFieldType::E type = convertToDataFieldType(rdata.getPtr(), mInfoFieldExprList[i], mInputRecords[0]);
			output->setField(mInfoFields[i], type, max_keylen);
		}	
	}
	else
	{
		output->setField("statkey", AosDataFieldType::eStr, max_keylen);
	}
	output->setField("time", AosDataFieldType::eBinU64, sizeof(u64));


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

	JSONValue measures = json["measures"];
	vector<AosDataProcGroupBy::MeasrueInfo> infos;
	OmnString measure, expr_str, aggr_opr, errmsg;
	AosExprObjPtr expr;
	AosExprGenFunc* funcExpr = NULL;
	for (size_t i = 0; i < measures.size(); i++)
	{
		measure = measures[i].asString();

		//get function name(sum, max, min.....)
		expr_str = measure;
		expr_str << ";";
		expr = AosParseExpr(expr_str, errmsg, rdata.getPtr());
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		aos_assert_r(AosExpr::checkIsAgrFunc(expr), false);

		funcExpr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());       
		aos_assert_r(funcExpr, false);                      

		aggr_opr = funcExpr->getFuctName(); 
		aos_assert_r(aggr_opr != "", false);

		if(aggr_opr != "count")
		{
			OmnString val = (*funcExpr->getParmList())[0]->getValue(rdata.getPtr());
			
			OmnString fType, name;
			if(mIsStreaming)
			{
				OmnString schema_name = "";
				AosXmlTagPtr doc;
				schema_name = mJson["format"].asString("");
				if (schema_name != "")
				{
					doc = AosJqlStatement::getDoc(
							rdata, JQLTypes::eDataRecordDoc, schema_name);
				}
				AosXmlTagPtr dataFieldsXml = doc->getFirstChild("datafields"); 
				aos_assert_r(dataFieldsXml, false);                                  
				AosXmlTagPtr dataFieldXml = dataFieldsXml->getFirstChild();          
				aos_assert_r(dataFieldXml, false);                                   
				while (dataFieldXml)
				{
					name = dataFieldXml->getAttrStr("zky_name"); 
					if(name == val)
						fType = dataFieldXml->getAttrStr("type"); 

					dataFieldXml = dataFieldsXml->getNextChild();
				}
			}
			const OmnString cType = fType;
			AosDataFieldType::E type = convertToDataFieldType(rdata.getPtr(), cType);

			if(type == AosDataFieldType::eNumber)
				type = AosDataFieldType::eBinDouble;
			//arvin 2015.07.28
			//JIMODB-118:add eU64,eInt64,eDouble and eDateTime into switchcase
			switch(type)
			{
				case AosDataFieldType::eU64:
				case AosDataFieldType::eBinU64:
					output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
					break;
				case AosDataFieldType::eInt64:
				case AosDataFieldType::eBinInt64:
				case AosDataFieldType::eDateTime:
				case AosDataFieldType::eBinDateTime:
					output->setField(measure, AosDataFieldType::eBinInt64, sizeof(i64));
					break;
				case AosDataFieldType::eDouble:
				case AosDataFieldType::eBinDouble:
					output->setField(measure, AosDataFieldType::eBinDouble, sizeof(double));
					break;
				default:
					output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
					break;
			}

			output->setAggrField(measure, aggr_opr);
		}
		else
		{
			aggr_opr = "sum";
			//output->setField(measure, AosDataFieldType::eBinInt64, sizeof(i64));
			output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
			output->setAggrField(measure, aggr_opr);
		}
	}

	output->setCmpFun("custom");
	if(mStatKeysSize)
	{
		for(size_t i = 0;i < mStatKeysSize;i++)
		{
			OmnString key = mStatKeys[i]->dumpByNoEscape();
			output->setCmpField(key);
		}
	}
	else
	{
		output->setCmpField("statkey");
	}
	output->setCmpField("time");

	AosXmlTagPtr rcdset_xml = output->init2(0, rdata);
	AosXmlTagPtr col_xml = rcdset_xml->getFirstChild("datacollector");
	AosXmlTagPtr asm_xml = col_xml->getFirstChild("asm");
	
	//for add comp
	AosXmlTagPtr comp_xml = asm_xml->getFirstChild("CompareFun");
	aos_assert_r(comp_xml, false);
	addComp("output", comp_xml);

 	AosXmlTagPtr rcd_xml = asm_xml->getFirstChild("datarecord");
  	aos_assert_r(rcd_xml, false);

	mOutputRecord = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata.getPtr() AosMemoryCheckerArgs);
	aos_assert_r(mOutputRecord, false);
	mOutputRecordRaw = mOutputRecord.getPtr();

	//mOutputRecord = output->getRecord();
	mOutputs.push_back(output);

	return true;



	/*
	//recordctnr
	boost::shared_ptr<AosConf::DataRecordCtnr> recordCtnr = boost::make_shared<AosConf::DataRecordCtnr>();
	recordCtnr->setAttribute("zky_name", dr_name);

	//datarecord
	u32 offset = 0;
	boost::shared_ptr<AosConf::DataRecordFixbin> dr = boost::make_shared<AosConf::DataRecordFixbin>();
	dr->setAttribute("zky_name", dr_name);
	dr->setAttribute("type", "fixbin");

	int max_keylen = json["max_keylen"].asInt();
	if (max_keylen <= 0) max_keylen = 50;

	//fix the bug,should be length of key+time+m1,m2,...
	JSONValue measures = json["measures"];
	boost::shared_ptr<AosConf::CompFun> fun = boost::make_shared<AosConf::CompFun>("custom", max_keylen + sizeof(u64) + sizeof(u64) * measures.size());

	//datafields
	boost::shared_ptr<AosConf::DataField> keyfield = boost::make_shared<AosConf::DataField>();
	boost::shared_ptr<AosConf::DataField> timefield = boost::make_shared<AosConf::DataField>();
	//key field
	keyfield->setAttribute("zky_name", "statkey");
	keyfield->setAttribute("type", "str");
	keyfield->setAttribute("zky_datatooshortplc", "cstr");
	keyfield->setAttribute("zky_offset", "0");
	keyfield->setAttribute("zky_length", max_keylen);

	fun->setCmpField("str", 0);
	dr->setField(keyfield);
	offset = max_keylen;

	//time field
	timefield->setAttribute("zky_name", "time");
	timefield->setAttribute("type", "bin_u64");          //?????
	timefield->setAttribute("zky_offset", offset);
	timefield->setAttribute("zky_length", sizeof(u64));
	fun->setCmpField("u64", offset);
	dr->setField(timefield);

	offset +=sizeof(u64);

	//measure_field
	OmnString measure;
	for (size_t i = 0; i < measures.size(); i++)
	{
		boost::shared_ptr<AosConf::DataField> measure_field = boost::make_shared<AosConf::DataField>();

		measure = measures[i].asString();
		measure_field->setAttribute("zky_name", measure);
		measure_field->setAttribute("type", "bin_u64");    
		measure_field->setAttribute("zky_offset", offset);
		measure_field->setAttribute("zky_length", sizeof(u64));
		fun->setAggrField("u64", offset, "sum");
		dr->setField(measure_field);
		offset += sizeof(u64);
	}

	recordCtnr->setRecord(dr);
	string dr_str = recordCtnr->getConfig();
	AosXmlTagPtr xml = AosStr2Xml(rdata.getPtr(), dr_str);

	//create record
	AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(xml, mTask, rdata.getPtr());
	mOutputRecord = record;
	return true;
	*/
}


bool
AosDataProcGroupBy::createOutput(
		OmnString &dp_name,
		JSONValue &json,
		const AosRundataPtr &rdata)
{
	//Jozhi 2015-03-28 implement to BuffArrayVar
	OmnString name = dp_name;
	name << "_output";

	OmnString type_str = json["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	int max_keylen = json["max_keylen"].asInt();
	if (max_keylen <= 0) max_keylen = 50;


	OmnString shuffle_type, shuffle_field, datacol_type, child_type;
	shuffle_type = json["shuffle_type"].asString();
	shuffle_field = json["shuffle_field"].asString();

	AosDataRecordType::E type = AosDataRecordType::toEnum(type_str);
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, type);
	if(mHasKey)
	{
		for(size_t i = 0;i < mStatKeysSize; i++)
		{
			OmnString key = mStatKeys[i]->dumpByNoEscape();
			AosDataFieldType::E type = convertToDataFieldType(rdata.getPtr(), mStatKeys[i], mInputRecords[0]);
			output->setField(key, type, 100);
		}
		for(size_t i = 0;i < mInfoFieldsSize;i++)
		{
			AosDataFieldType::E type = convertToDataFieldType(rdata.getPtr(), mInfoFieldExprList[i], mInputRecords[0]);
			output->setField(mInfoFields[i], type, max_keylen);
		}	
	}
	else
	{
		output->setField("statkey", AosDataFieldType::eStr, max_keylen);
	}
	output->setField("time", AosDataFieldType::eBinInt64, sizeof(i64));

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

	JSONValue measures = json["measures"];
	vector<AosDataProcGroupBy::MeasrueInfo> infos;
	OmnString measure, expr_str, aggr_opr, errmsg;
	AosExprObjPtr expr;
	AosExprGenFunc* funcExpr = NULL;
	for (size_t i = 0; i < measures.size(); i++)
	{
		measure = measures[i].asString();

		//get function name(sum, max, min.....)
		expr_str = measure;
		expr_str << ";";
		expr = AosParseExpr(expr_str, errmsg, rdata.getPtr());
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< "" << enderr;
			return false;
		}
		aos_assert_r(AosExpr::checkIsAgrFunc(expr), false);

		funcExpr = dynamic_cast<AosExprGenFunc*>(expr.getPtr());       
		aos_assert_r(funcExpr, false);                      

		aggr_opr = funcExpr->getFuctName(); 
		aos_assert_r(aggr_opr != "", false);
		
		if(aggr_opr != "count" && aggr_opr != "dist_count")
		{
			OmnString val = (*funcExpr->getParmList())[0]->getValue(rdata.getPtr());
			int fieldidx = mInputRecords[0]->getFieldIdx(val,rdata.getPtr());

			AosDataFieldType::E type = mInputRcdFieldTypes[fieldidx];
			if(type == AosDataFieldType::eNumber)
				type = AosDataFieldType::eBinDouble;
			//arvin 2015.07.28
			//JIMODB-118:add eU64,eInt64,eDouble and eDateTime into switchcase
			switch(type)
			{
				case AosDataFieldType::eU64:
				case AosDataFieldType::eBinU64:
					output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
					break;
				case AosDataFieldType::eInt64:
				case AosDataFieldType::eBinInt64:
					output->setField(measure, AosDataFieldType::eBinInt64, sizeof(i64));
					break;
				case AosDataFieldType::eDouble:
				case AosDataFieldType::eBinDouble:
					output->setField(measure, AosDataFieldType::eBinDouble, sizeof(double));
					break;
				case AosDataFieldType::eDateTime:
				case AosDataFieldType::eBinDateTime:
					output->setField(measure, AosDataFieldType::eBinDateTime, sizeof(i64));
					break;
				default:
					output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
					break;
			}

			output->setAggrField(measure, aggr_opr);
		}
		else if(aggr_opr == "count")
		{
			aggr_opr = "sum";
			output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
			output->setAggrField(measure, aggr_opr);
		}
		else 
		{
			mIsDistCount = true;	
			output->setField(measure, AosDataFieldType::eBinU64, sizeof(u64));
		}

	}

	output->setCmpFun("custom");
	if(mHasKey)
	{
		for(size_t i = 0;i < mStatKeysSize;i++)
		{
			OmnString key = mStatKeys[i]->dumpByNoEscape();
			output->setCmpField(key);
		}
	}
	else
	{
		output->setCmpField("statkey");
	}
	output->setCmpField("time");
	//yang,2015/08/24
	if(mIsDistCount)
	{
		//arvin 2015.11.25
		//just support one distcount measure
		OmnString dictName = "_zt44_dict_";
		OmnString dbTableName = json["table"].asString();
		dictName << dbTableName << "_dict_"<< mInputMeasures[0]->dumpByNoEscape();
		
		mDistCountIILEntryMap = AosIILEntryMapMgr::retrieveIILEntryMap(dictName,rdata);
		
		for (u32 i =0; i<measures.size(); i++)
	    {
		    output->setCmpField(measures[i].asString());
	    }
	}

	output->init(mTaskDocid, rdata);

	mOutputRecord = output->getRecord();
	mOutputRecordRaw = mOutputRecord.getPtr();
	mOutputs.push_back(output);

	return true;
}

#if 0
void
AosDataProcGroupBy::transformTime(
		AosRundata* rdata,
		AosValueRslt& time,
		const OmnString& tfmt,
		const OmnString& tunit)
{
	/*
	i64 eptime;
	AosTimeUtil::str2Time(rdata,tfmt,time.getValueStr1(),eptime);
	AosStatTimeUnit::init();
	i64 etrslt = AosStatTimeUnit::parseTimeValue(eptime,AosStatTimeUnit::eEpochDay,AosStatTimeUnit::eEpochMonth);
	time.setI64(etrslt);
	*/

	//e.g. from "_day" to E::eEpochDay
	AosStatTimeUnit::E toTimeUnit = AosStatTimeUnit::getTimeUnit(tunit);
	AosTimeFormat::E format;

	AosStatTimeUnit::E fromTimeUnit;
	//arvin 2015.08.11
	//JIMODB-336
	if(time.getType() == AosDataType::eDateTime)
	{
		AosDateTime dateTime = time.getDateTime();
		OmnString timeFormat = convertTimeFormat(tfmt);
		dateTime.setFormat(timeFormat);
		time.setDateTime(dateTime);
	}

	format = AosTimeFormat::toEnum(tfmt);

	bool bNeedStrToEPTTransform = true;// need str to eptime transform if other than following format
	switch (format)
	{
	case AosTimeFormat::eDayOfMonth:
	case AosTimeFormat::eDayOfWeek:
	case AosTimeFormat::eEpochDay:
		fromTimeUnit = AosStatTimeUnit::eEpochDay;
		bNeedStrToEPTTransform = false;
		break;
	case AosTimeFormat::eEpochHour:
		fromTimeUnit = AosStatTimeUnit::eEpochHour;
		bNeedStrToEPTTransform = false;
		break;
	case AosTimeFormat::eEpochMonth:
		fromTimeUnit = AosStatTimeUnit::eEpochMonth;
		bNeedStrToEPTTransform = false;
		break;
	case AosTimeFormat::eEpochTime:
	case AosTimeFormat::eEpochWeek:
	case AosTimeFormat::eEpochYear:
	case AosTimeFormat::eHourOfDay:
		bNeedStrToEPTTransform = false;
		break;
	default:
		break;
	}

	int etrslt;
	if(bNeedStrToEPTTransform)
	{
		switch (toTimeUnit)
		{
			case AosStatTimeUnit::eEpochTime:
				etrslt = AosTimeFormat::strToEpochTime(time.getStr(),format);
				break;
			case AosStatTimeUnit::eEpochHour:
				etrslt = AosTimeFormat::strToEpochHour(time.getStr(),format);
				break;
			case AosStatTimeUnit::eEpochDay:
				etrslt = AosTimeFormat::strToEpochDay(time.getStr(),format);
				break;
			case AosStatTimeUnit::eEpochWeek:
				etrslt = AosTimeFormat::strToEpochWeek(time.getStr(),format);
				break;
			case AosStatTimeUnit::eEpochMonth:
				etrslt = AosTimeFormat::strToEpochMonth(time.getStr(),format);
				break;
			case AosStatTimeUnit::eEpochYear:
				etrslt = AosTimeFormat::strToEpochYear(time.getStr(),format);
				break;
			default:
				OmnNotImplementedYet;
				break;
				//return "";
		}
	}
	else
	{
		i64 eptime = time.getI64();
		etrslt = eptime;
		if(fromTimeUnit != toTimeUnit)
		{
			etrslt = AosStatTimeUnit::parseTimeValue(eptime,fromTimeUnit,toTimeUnit);
		}
	}
	time.setI64(etrslt);

	/*
	AosTimeFormat::E targetFormat = AosTimeFormat::eYYYY_MM_DDHHMMSS;
	i64 eptime;
	AosTimeFormat::convert(time.getCharStr(), time.getStrValueRef().length(), fromFormat,
		AosTimeFormat::eYYYYMMDD, value, mRundata);
	*/
}
#endif

AosDataProcStatus::E
AosDataProcGroupBy::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	mProcDataCount++;
	bool rslt, outofmem = false;
	AosValueRslt statKey_rslt, id_rslt;
	AosDataRecordObj * input_record;

	//input_record
	input_record = input_records[0];
	aos_assert_r(input_record, AosDataProcStatus::eError);

	//get statKeyIdx and measureFieldIdx
	if(!mIsGenFieldIdx)
	{
		rslt = genFieldIdx(rdata_raw,input_record);
		aos_assert_r(rslt,AosDataProcStatus::eError);
	}

	//dump all the input data from upstream 
	DumpRecord(input_record, "GroupBy input is: ");

	if(!mIsStreaming)
	{
		//filter out records not meeting the condition
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
	}

	//output_record
	mOutputRecordRaw->clear();
	aos_assert_r(mOutputRecordRaw, AosDataProcStatus::eError);

	//set statkey field
	// Key fields are concatenated by 0x01.
	OmnString statKey;
	u32 idx;
	if(mHasKey)
	{
		for (size_t i = 0; i < mStatKeysSize; i++)
		{
			rslt = input_record->getFieldValue(mKeyIdxV[i], statKey_rslt,true,rdata_raw);
			aos_assert_r(rslt,AosDataProcStatus::eError);

			rslt = mOutputRecordRaw->setFieldValue(i, statKey_rslt, outofmem, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);
		}

		idx = mStatKeysSize;
		//statKey_rslt.setStr("");
		//arvin 2015.11.02
		//JIMODB-1097
		statKey_rslt.setNull();
		for(size_t i = 0; i < mInfoFieldsSize;i++)
		{
			rslt = mOutputRecordRaw->setFieldValue(idx+i, statKey_rslt, outofmem, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);
		}
		idx += mInfoFieldsSize;
	}
	else
	{
		AosValueRslt statKey_v("");
		rslt = mOutputRecordRaw->setFieldValue(0, statKey_v, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		idx = 1;
	}
	
	//time
	if (mStatInputTime)
	{
		AosValueRslt statTime_rslt;
		rslt = mStatInputTime->getValue(rdata_raw, input_record, statTime_rslt);
		aos_assert_r(rslt, AosDataProcStatus::eError);

		if (statTime_rslt.isNull())
		{
			return AosDataProcStatus::eContinue;
		}
//		transformTime(rdata_raw,statTime_rslt, mTimeFormat, mTimeUnit);

		rslt = AosStatTimeUnit::convertToTimeUnit(statTime_rslt,mTimeUnit);
		if(!rslt)
			return AosDataProcStatus::eContinue;
		
		rslt = mOutputRecordRaw->setFieldValue(idx++, statTime_rslt, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}
	else 
	{
		AosValueRslt statTime_rslt;
		statTime_rslt.setU64(0);
		rslt = mOutputRecordRaw->setFieldValue(idx++, statTime_rslt, outofmem, rdata_raw);
		aos_assert_r(rslt, AosDataProcStatus::eError);
	}
	//measures
	AosValueRslt measure_rslt;

	if(mIsDistCount)
	{
		rslt = procDistCount(rdata_raw,idx,input_record);	
		aos_assert_r(rslt,AosDataProcStatus::eError);
		return AosDataProcStatus::eContinue;
	}
	else
	{
		for (size_t i = 0; i < mInputMeasuresSize; i++)
		{
			if(mMeasureFuncTypeV[i] == AosDataProcGroupBy::eCountAll)	//if count,then polulate 1
			{
				measure_rslt.setU64((u64)mCountValue);
			}
			else
			{
				rslt = input_record->getFieldValue(mMeasureIdxV[i], measure_rslt,true,rdata_raw);
				aos_assert_r(rslt, AosDataProcStatus::eError);
				if (measure_rslt.isNull()) 
				{
					measure_rslt.setU64((u64)0);
				}
				else if(mMeasureFuncTypeV[i] == AosDataProcGroupBy::eCountNormal)
				{
					measure_rslt.setU64((u64)mCountValue);
				}
			}
			rslt = mOutputRecordRaw->setFieldValue(idx+i, measure_rslt, outofmem, rdata_raw);
			aos_assert_r(rslt, AosDataProcStatus::eError);
		}
	}
	//display all the output data
	DumpRecord(mOutputRecord, "GroupBy output is: ");

	if (!mIsStreaming) 
	{
		//this is batch mode
		mOutputRecord->flushRecord(rdata_raw);
	}
	else
	{
		//in streaming mode
		//insert the record into output recordset
		addOutput(rdata_raw, "output", mOutputRecord);
	}
	mOutputCount++;
	return AosDataProcStatus::eContinue;
}

vector<AosDataRecordObjPtr> 
AosDataProcGroupBy::getOutputRecords()
{
	vector<AosDataRecordObjPtr> v;
	v.push_back(mOutputRecord);
	return v;
}



AosJimoPtr 
AosDataProcGroupBy::cloneJimo() const
{
	return OmnNew AosDataProcGroupBy(*this);
}



AosDataProcObjPtr
AosDataProcGroupBy::cloneProc() 
//
//this jimo is stateful which means some data is saved 
//within the object. Therefore cloneJimo() need to return
//the object itself, not to create a new one
//
{
	return OmnNew AosDataProcGroupBy(*this);
}


bool 
AosDataProcGroupBy::createByJql(
		AosRundata *rdata, 
		const OmnString &dpname, 
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//create dataprocidx dp1 {
	//	type:groupby, 
	//	keys: ["city","town",...],
	//	time: "xxx",
	//	measure_inputs: ["sum(f1)","xxx",...],
	//	measure_outputs: ["xxx","xxx",...]
	//};

	//<jimodataproc >
	//  <dataproc zky_name="dp1" jimo_objid="dataprocindex_jimodoc_v0">
	//    <![CDATA[{type:groupby, keys: ["city","town",...], time: "xxx", measure_inputs:["sum(f1)","xxx",...], measure_outputs: ["xxx","xxx",...]"}]]>
	//  </dataproc>
	//  <datasets>
	//    <dataset  zky_type="datacol">
	//      <datacollector zky_type="???">
	//        <asm type="stradd">
	//          <datarecord type="ctnr" zky_name="dp1">
	//            <datarecord type="fixbin" zky_length="???" zky_name="dp1">
	//              <datafields>
	//                <datafield type="str" 
	//                	zky_datatooshortplc="cstr" 
	//                	zky_length="xxx" 
	//                	zky_name="key" 
	//                	zky_offset="0">
	//                </datafield>
	//                <datafield 
	//                	type="bin_u64" 
	//                	zky_length="8" 
	//                	zky_name="time" 
	//                	zky_offset="9">
	//                </datafield>
	//                <datafield
	//                	type="bin_u64"
	//                	zky_length="8"
	//                	zky_name="xxx"
	//                	zky_offset="xxx">
	//                </datafield>
	//                ...
	//              </datafields>
	//            </datarecord>
	//          </datarecord>
	//        </asm>
	//      </datacollector>
	//    </dataset>
	//  </datasets>
	//</jimodataproc>
	
	JSONValue jsonObj;
	JSONReader reader;
	bool rslt =	reader.parse(jsonstr, jsonObj);
	aos_assert_r(rslt, false);
	mJson = jsonObj;

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\" ";
	dp_str << "><dataproc zky_name=\"";
	dp_str << dpname << "\" jimo_objid=\"dataprocgroupby_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]></dataproc>";
	dp_str << " </jimodataproc>";

	//AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str);
	//dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	//dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	//dp_xml->setAttr(AOSTAG_OBJID, AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname));
	//return AosCreateDoc(dp_xml->toString(), true, rdata);

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}

bool 
AosDataProcGroupBy::genFieldIdx(
			AosRundata* rdata,
			AosDataRecordObj *record)
{
	mMeasureFuncTypeV.clear();
	mMeasureIdxV.clear();
	E func_type;
	int idx;
	if(mHasKey)
	{
		for (size_t i = 0; i < mStatKeysSize; i++)
		{
			OmnString field_name = mStatKeys[i]->dumpByNoEscape();
			idx = record->getFieldIdx(field_name,rdata);
			aos_assert_r(idx >=0 ,false);
			mKeyIdxV.push_back(idx);
		}
	}
	for (size_t i = 0; i < mInputMeasuresSize; i++)
	{
		OmnString func_name = mMeasures[i]->getFuctName();
		OmnString field_name = mInputMeasures[i]->dumpByNoEscape();
		if(func_name == "count")	//if count,then polulate 1
		{
			if(field_name == "0x2a")
				func_type = AosDataProcGroupBy::eCountAll;
			else
				func_type = AosDataProcGroupBy::eCountNormal;
		}
		else
		{
			func_type = AosDataProcGroupBy::eOther;
		}
		idx = record->getFieldIdx(field_name,rdata);
		mMeasureFuncTypeV.push_back(func_type);
		mMeasureIdxV.push_back(idx);
	}

	mIsGenFieldIdx = true;
	return true;
}

//arvin 2015.08.12
//JIMODB-369
OmnString 
AosDataProcGroupBy::convertTimeFormat(const OmnString &tfmt)
{
	if(tfmt == "yyyymmdd")
	{
		return "%Y%m%d";	
	}
	if(tfmt == "yymmdd")
	{
		return "%y%m%d";	
	}
	if(tfmt == "yymmddhhmmss")
	{
		return "%y%m%d%H%M%S";	
	}
	if(tfmt == "yyyymmddhhmmss")
	{
		return "%Y%m%d%H%M%S";
	}
	if(tfmt == "yymmddhhmmss")
	{
		return "%y%m%d%H%M%S";
	}
	if(tfmt == "yyyy-mm-dd hh:mm:ss")
	{
		return  "%Y-%m-%d %H:%M:%S";
	}
	OmnShouldNeverComeHere;
	return "";
}
	

bool 
AosDataProcGroupBy::start(const AosRundataPtr &rdata)
{
	showDataProcInfo(__FILE__, __LINE__, "start", "DataProcGroupBy");
	return true;
}

bool 
AosDataProcGroupBy::procDistCount(
			AosRundata *rdata,
			const u32 measure_idx,
			AosDataRecordObj *input_record)
{
	//just support one distcount measure
	bool rslt = false,outofmem;
	AosValueRslt measure_rslt;
	mDistCountIdx = measure_idx;
	for (size_t i = 0; i < mInputMeasuresSize; i++)
	{
		rslt = input_record->getFieldValue(mMeasureIdxV[i], measure_rslt,true,rdata);
		aos_assert_r(rslt,false);
		itr = mDistCountIILEntryMap->find(measure_rslt.getStr());
		if(itr != mDistCountIILEntryMap->end())
		{
			OmnString value  = (*mDistCountIILEntryMap)[measure_rslt.getStr()];
			measure_rslt.setU64(value.toU64());
			rslt = mOutputRecordRaw->setFieldValue(measure_idx + i, measure_rslt, outofmem, rdata);
			aos_assert_r(rslt, AosDataProcStatus::eError);
			mOutputRecordRaw->flushRecord(rdata);
			mOutputCount++;
			return true;
		}
		else
		{
			mDistCountMeasureKeys.push_back(measure_rslt.getStr());
			int len = mOutputRecordRaw->getRecordLen();
			char *data = mOutputRecordRaw->getData(rdata);
			mTmpBuff->setCharStr(data,len);
		}
	}
	if(mTmpBuff->dataLen() > DEFAULT_BUFF_SIZE)
	{
		rslt = flushRecordset(rdata);
		aos_assert_r(rslt,false);
		mTmpBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
	}
	return true;
}

bool
AosDataProcGroupBy::flushRecordset(AosRundata* rdata)
{
	bool outofmem,rslt;
	AosValueRslt measure_rslt;
	AosIILEntryMapMgr::getMapValues(mDistCountIILEntryMap,mDistCountMeasureKeys,mDistCountMeasureValues,"-1",true,rdata);
	aos_assert_r(mDistCountMeasureKeys.size() == mDistCountMeasureValues.size(),false);
		
	u64 crtIdx = 0;
	mTmpBuff->reset();
	u32 i = 0;
	u64 totalLen = mTmpBuff->dataLen();
	while(crtIdx < totalLen)
	{
		mOutputRecordRaw->clear();
		int len = -1;
		char *data = mTmpBuff->getCharStr(len);
		AosBuffPtr buff = OmnNew AosBuff(data,len+9*mInputMeasuresSize,len,true AosMemoryCheckerArgs);
		int offset = len;
		for (size_t j = 0; j < mInputMeasuresSize; j++)
		{
			u64 value = mDistCountMeasureValues[i].toU64();
			aos_assert_r(value > 0,false);
			buff->appendU8(0);
			buff->appendU64(value);
			offset += 9;
		}
		// jimodb-1301
		int status;
		mOutputRecordRaw->setData(buff->data(),offset,NULL,status);
		mOutputRecordRaw->flushRecord(rdata);
		mOutputCount++;
		i++;
		crtIdx = mTmpBuff->getCrtIdx();
	}
	clearDistCountValues();
	return true;
}

void    
AosDataProcGroupBy::clearDistCountValues()
{
	mDistCountMeasureKeys.clear();
	mDistCountMeasureValues.clear();
}
