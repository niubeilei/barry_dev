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
// This data proc filters a record.
//
//
// Modification History:
// 05/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcStatJoin.h"

#include "API/AosApi.h"
#include "IILClient/IILClient.h"

#include "JQLExpr/Expr.h"
#include "JQLExpr/ExprGenFunc.h"

#include "TaskMgr/Task.h"
#include "StatUtil/Jimos/StatModel.h"

static bool mShowLog = false;

////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcStatJoin_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcStatJoin(version);
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


AosDataProcStatJoin::AosDataProcStatJoin(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcStatJoin)
{
	mPrevCreatedStatKey = NULL;
	mPrevCreatedStatKeyLen = 0;
	
	//yang,2015/09/27
	mSdocid = 0;
}


AosDataProcStatJoin::AosDataProcStatJoin(const AosDataProcStatJoin &proc)
:
AosStreamDataProc(proc)
{
	vector<AosDataRecordObjPtr> v;
	hash_map<OmnString, AosDataRecordObjPtr, Omn_Str_hash, compare_str>::iterator itr;
	AosDataRecordObjPtr record;
	if (proc.mOutputRecordMap.size()>0)
	{
		mOutputRecordMap = proc.mOutputRecordMap;
	}
	mJson = proc.mJson;
	mKeyList = proc.mKeyList;
	mMeasureList = proc.mMeasureList;
	mTimeField = proc.mTimeField;
	mMaxKeyLen = proc.mMaxKeyLen;
	mCubeId = proc.mCubeId;
	mStatIdGen = proc.mStatIdGen;
	mRecordType = proc.mRecordType;
	mOutputStatKey = proc.mOutputStatKey;
	mOutputKeyPrefix = proc.mOutputKeyPrefix;
	mOutputVt2d = proc.mOutputVt2d;
	//yang,2015/09/27
	mPrevCreatedStatKey = NULL;//proc.mPrevLeftStatKey;
	mPrevCreatedStatKeyLen = 0;//proc.mPrevLeftStatKeyLen;

	mInfoFields = proc.mInfoFields;
	mHasKey = proc.mHasKey;
	mStatKeyList = proc.mStatKeyList;
	mSdocidList = proc.mSdocidList;
	//mStatKeyValueMap = proc.mStatKeyValueMap;
	mQueryContext = proc.mQueryContext;
	mKeyIdx = proc.mKeyIdx;
	mIILName = proc.mIILName;
	mKeyExprList = proc.mKeyExprList;
	mInfoFields = proc.mInfoFields;
	mRecordsKeyList = proc.mRecordsKeyList;
	mRcdKeyList = proc.mRcdKeyList;
	
	//yang,2015/09/27
	mSdocid = 0;
}

AosDataProcStatJoin::~AosDataProcStatJoin()
{
}

AosJimoPtr
AosDataProcStatJoin::cloneJimo() const
{
	return OmnNew AosDataProcStatJoin(*this);
	//return (AosJimo*)this;
}

AosDataProcObjPtr
AosDataProcStatJoin::clone() 
{
	return OmnNew AosDataProcStatJoin(*this);
}

void
AosDataProcStatJoin::setInputDataRecords(
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
AosDataProcStatJoin::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "statjoin",
	//		"keys": ["k1", "k2", "k3"],
	//		"time": "d1",
	//		"measures": ["sum(v1)", "sum(v2)"],
	//		"cube_id" : 0,
	//	    "outputs" : {
	// 				"statkey_output" : "output_t1_stat_model_statkey",
	//				"key_prefix_output" : "output_t1_stat_model_key_prefix",
	//				"vt2d_output" : "output_t1_stat_model_vt2d"
	//		},
	//		"opr" : "delete"
	//	}
	//]]></dataproc>
	bool rslt;

	try
	{
		aos_assert_r(def, false);
		OmnString dp_jsonstr = def->getNodeText();

		JSONValue json;
		JSONReader reader;
		reader.parse(dp_jsonstr, mJson);
		if(!mIsStreaming)
			aos_assert_r(isVersion1(def), false);

		mName = def->getAttrStr("zky_name");
		aos_assert_r(mName != "", false);
		OmnScreen << "dataproc config json is: " << dp_jsonstr << endl;

		//get JSON parameters
		rslt = getJSONParams(mJson,rdata.getPtr());
		aos_assert_r(rslt, false);

		//yang,2015/08/22
		//initialize stat id generator
		//use stat model name as the key generator name
		OmnString statName = mJson["stat_name"].asString();
		//50000 means reserve 50000 docids once,may waste many docids but
		//can improve performance
		mStatIdGen = OmnNew AosStatIdIDGen(statName, 1000);
		if(mIsStreaming)
		{
			//create output record templates
			aos_assert_r(createOutput(rdata.getPtr()), false);	
			// for iil reecord
			mJson["isIIL"] = false;
		}
		else
		{
			//get cube id which is shuffle Id for statjoin right now
			AosTaskObjPtr task = AosTask::getTaskStatic(mTaskDocid, rdata);
			OmnString value = task->getTaskENV(AOSTAG_SHUFFLE_ID, rdata);
			mCubeId = value.toInt(); 

			//create output record templates
			rslt = createOutput(rdata.getPtr());
			aos_assert_r(rslt, false);
		}

		//get opr
		mOpr = INSERT;
		if (mJson.isMember("opr"))
		{
			OmnString opr = mJson["opr"].asString();
			if(opr == "delete") mOpr = DELETE;
		}

		mRhsRecordFirstEmpty = true;
		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}

bool
AosDataProcStatJoin::createStatkeyOutput(AosRundata* rdata)
{
	OmnString shuffle_type = mJson["shuffle_type"].asString();
	OmnString shuffle_field = mJson["shuffle_field"].asString();

	OmnString name = mOutputStatKey;
	boost::shared_ptr<Output> output = boost::make_shared<Output>(name, mRecordType);
	output->setField("right_statkey", AosDataFieldType::eStr, mMaxKeyLen);
	output->setField("sdocid", AosDataFieldType::eBinU64, sizeof(u64));

	if (shuffle_type != "")
	{
		output->setAssemblerAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setAssemblerAttr("type", "group");
		//output->setAssemblerAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		//output->setAssemblerAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		//output->setShuffAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);

		output->setDataColAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setDataColAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		output->setDataColAttr("zky_type", "group");
		output->setDataColAttr("zky_child_type", "sorted_file");
	} 

	output->setDataColAttr(AOSTAG_NAME, mOutputStatKey);
	output->setCmpFun("custom");
	output->setCmpField("right_statkey");
	output->setCmpField("sdocid");

	if(mIsStreaming)
	{
		AosXmlTagPtr rcdset_xml = output->init2(0, rdata);
		AosXmlTagPtr col_xml = rcdset_xml->getFirstChild("datacollector");
		AosXmlTagPtr asm_xml = col_xml->getFirstChild("asm");
		AosXmlTagPtr comp_xml = asm_xml->getFirstChild("CompareFun");
		aos_assert_r(comp_xml, false);
		addComp(name, comp_xml);
		AosXmlTagPtr rcd_xml = asm_xml->getFirstChild("datarecord");
		aos_assert_r(rcd_xml, false);

		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
		aos_assert_r(record, false);

		//mOutputRecordMap[mOutputStatKey] = record;

		mStatKeyOutputRecord = record;

		mOutputs.push_back(output);
	}
	else
	{
		output->init(mTaskDocid, rdata);
		mStatKeyOutputRecord = output->getRecord();
		mOutputs.push_back(output);
	}
	return true;
}

bool
AosDataProcStatJoin::createPrefixKeyOutput(AosRundata* rdata)
{
	//yang,2015/08/25
	mKeyOutputRecordArray = OmnNew AosDataRecordObjPtr[mKeyList.size()];

	for (u32 i = 0; i < mKeyList.size(); i++)
	{
		OmnString name;
		name = mOutputKeyPrefix;
		name << mKeyList[i];
		boost::shared_ptr<Output> output = boost::make_shared<Output>(name, mRecordType);

		//yang
		OmnString keyIndexOpr = "str";
        if (mKeyIndexOprMap.find(mKeyList[i]) != mKeyIndexOprMap.end())
        {
        	keyIndexOpr = mKeyIndexOprMap[mKeyList[i]];
		}
		//arvin 2015.07.27
		//JIMODB-117:support d64 and i64
#if 0
		OmnString targetKeyIndexOpr;
		AosDataFieldType::E targetOpr;
		targetOpr = convertToDataFieldType(rdata, keyIndexOpr);
		//targetKeyIndexOpr = AosDataFieldType::toString(targetOpr);
#else if 
		if(keyIndexOpr == "u64")
			output->setField("u64", AosDataFieldType::eBinU64, sizeof(u64));
		else if(keyIndexOpr == "i64")
			output->setField("i64", AosDataFieldType::eBinInt64, sizeof(i64));
		else if(keyIndexOpr == "d64")
			output->setField("d64", AosDataFieldType::eBinDouble, sizeof(double));
		else
			output->setField("str", AosDataFieldType::eStr,mMaxKeyLen);
#endif
		//output->setField(keyIndexOpr, targetOpr, mMaxKeyLen);
		output->setField("sdocid", AosDataFieldType::eBinU64, sizeof(u64)); //???

		output->setCmpFun("custom");
		if(keyIndexOpr == "u64")
			output->setCmpField("u64");
		else if(keyIndexOpr == "i64")
			output->setCmpField("i64");
		else if(keyIndexOpr == "d64")
			output->setCmpField("d64");
		else
			output->setCmpField("str");
		output->setCmpField("sdocid");

		if(mIsStreaming)
		{
			AosXmlTagPtr rcdset_xml = output->init2(0, rdata);
			AosXmlTagPtr col_xml = rcdset_xml->getFirstChild("datacollector");
			AosXmlTagPtr asm_xml = col_xml->getFirstChild("asm");
			AosXmlTagPtr compDoc = asm_xml->getFirstChild("CompareFun");
			aos_assert_r(compDoc, false);
			addComp(name, compDoc);
			AosXmlTagPtr rcd_xml = asm_xml->getFirstChild("datarecord");
			aos_assert_r(rcd_xml, false);

			AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
			aos_assert_r(record, false);
			
			//mOutputRecordMap[name] = record;
			//yang,2015/08/27
			mKeyOutputRecordArray[i] = record;

			mOutputs.push_back(output);
		}
		else
		{
			output->init(mTaskDocid, rdata);
//			mOutputRecordMap[name] = output->getRecord();
			mKeyOutputRecordArray[i] = output->getRecord();
			mOutputs.push_back(output);
		}
	}
	return true;
}

bool
AosDataProcStatJoin::createVector2dOutput(AosRundata* rdata)
{
	OmnString shuffle_type = mJson["shuffle_type"].asString();
	OmnString shuffle_field = mJson["shuffle_field"].asString();

	boost::shared_ptr<Output> output = boost::make_shared<Output>(mOutputVt2d, mRecordType);
	output->setField("isnew", AosDataFieldType::eBinChar, sizeof(char));
	output->setField("sdocid", AosDataFieldType::eBinU64, sizeof(u64));
	output->setField("statkey", AosDataFieldType::eStr, mMaxKeyLen);
	output->setField("time", AosDataFieldType::eBinInt64, sizeof(i64));

	if (shuffle_type != "")
	{
		output->setAssemblerAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setAssemblerAttr("type", "group");
	//	output->setAssemblerAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);

		output->setDataColAttr(AOSTAG_SHUFFLE_TYPE, shuffle_type);
		output->setDataColAttr(AOSTAG_SHUFFLE_VALUE, shuffle_field);
		output->setDataColAttr("zky_type", "group");
		output->setDataColAttr("zky_child_type", "sorted_file");
	}

	output->setCmpFun("custom");
	//output->setCmpField("isnew");
	output->setCmpField("sdocid");
	//output->setCmpField("statkey");
	//output->setCmpField("time");
	for (size_t i = 0; i < mMeasureList.size(); i++)
	{
		OmnString val = mMeasureVals[i];
		OmnString aggr_opr = mAggrOprs[i];

		OmnString fType, name;
		AosDataFieldType::E type;
		int fieldidx = 0;
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
				{
					fType = dataFieldXml->getAttrStr("type"); 
					aos_assert_r(fType != "", false);
					break;
				}
				else if(val == "*" || val =="0x2a")
				{
					fType = "u64";
					break;
				}

				dataFieldXml = dataFieldsXml->getNextChild();
			}
			const OmnString cType = fType;
			type = convertToDataFieldType(rdata, cType);
		}
		else
			fieldidx = mInputRecords[0]->getFieldIdx(mMeasureList[i],rdata);

		if(aggr_opr != "count")
		{
			if(!mIsStreaming)
				type = mInputRcdFieldTypes[fieldidx];

			if(type == AosDataFieldType::eNumber)
				type = AosDataFieldType::eBinDouble;

			switch(type)
			{
				case AosDataFieldType::eBinU64:
					output->setField(mMeasureList[i], type, sizeof(u64));
					break;
				case AosDataFieldType::eBinInt64:
					output->setField(mMeasureList[i], type, sizeof(i64));
					break;
				case AosDataFieldType::eBinDouble:
					output->setField(mMeasureList[i], type, sizeof(double));
					break;
				default:
					output->setField(mMeasureList[i], AosDataFieldType::eBinU64, sizeof(u64));
					break;
			}
		}
		else
		{
			aggr_opr = "sum";
			output->setField(mMeasureList[i], AosDataFieldType::eBinU64, sizeof(u64));
		}

	}

	if(mIsStreaming)
	{
		AosXmlTagPtr rcdset_xml = output->init2(0, rdata);
		AosXmlTagPtr col_xml = rcdset_xml->getFirstChild("datacollector");
		AosXmlTagPtr asm_xml = col_xml->getFirstChild("asm");
		
		AosXmlTagPtr compDoc = asm_xml->getFirstChild("CompareFun");
		aos_assert_r(compDoc, false);
		addComp(mOutputVt2d, compDoc);

		AosXmlTagPtr rcd_xml = asm_xml->getFirstChild("datarecord");
		aos_assert_r(rcd_xml, false);

		AosDataRecordObjPtr record = AosDataRecordObj::createDataRecordStatic(rcd_xml, 0, rdata AosMemoryCheckerArgs);
		aos_assert_r(record, false);
		
		//yang,2015/08/27
		//mOutputRecordMap[mOutputVt2d] = record;
		mVt2dOutputRecord = record;

		mOutputs.push_back(output);
	}
	else
	{
		output->init(mTaskDocid, rdata);
//		mOutputRecordMap[mOutputVt2d] = output->getRecord();
		mVt2dOutputRecord = output->getRecord();
		mOutputs.push_back(output);
	}

	return true;
}

//
// read json parameters into member variables
//
bool
AosDataProcStatJoin::getJSONParams(JSONValue json,AosRundata* rdata)
{
	AosExprObjPtr expr;
	//get key fields
	JSONValue keyFields = json["keys"];
	OmnString key,errmsg;
	
	//arvin 2015.10.23
	//JIMODB-1014,if keys just have one key and key_field_name is equal to "const_statkey",
	//is means the statmodel have no group by key
	mHasKey = true;
	if(keyFields.size()==1)
	{
		key = keyFields[(u32)0].asString();
		if(key == AOS_EMPTY_KEY)
			mHasKey =  false;
	}
	for (size_t i = 0; i < keyFields.size(); i++)
	{
		key = keyFields[i].asString();
		mKeyList.push_back(key);
		key << ";";
		expr = AosParseExpr(key, errmsg, rdata);
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< errmsg << enderr;
			return false;
		}
		mKeyExprList.push_back(expr);
	}
	//get info_field
	JSONValue info_fields = json["info_fields"];
	for(size_t  i = 0; i< info_fields.size();i++)	
	{
		OmnString field = info_fields[i]["field"].asString();
		OmnString alias = info_fields[i]["alias"].asString();
		alias << ";";
		expr = AosParseExpr(alias, errmsg, rdata);
		if (!expr)
		{
			AosSetErrorUser(rdata, "syntax_error")
				<< "CondExpr AosParseJQL ERROR:: " 
				<< errmsg << enderr;
			return false;
		}
		mInfoFields[field] = expr;
	}

	//get time field
	//levi, 2015/08/21
	if(!json["time"].isNull())
		mTimeField = JSONValue(json["time"]).asString();

	//get measure fields
	JSONValue measures = json["measures"];
	OmnString measure, expr_str, aggr_opr;
	AosExprGenFunc* funcExpr = NULL;

	if(mIsStreaming)
	{
		//levi 2015/11/23
		mMeasureList.clear();
		mAggrOprs.clear();
		mOutputRecordMap.clear();
	}

	for (size_t i = 0; i < measures.size(); i++)
	{
		measure = measures[i].asString();
		mMeasureList.push_back(measure);

		//get function name(sum, max, min.....)
		expr_str = measure;
		expr_str << ";";
		expr = AosParseExpr(expr_str, errmsg, rdata);
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

		OmnString val = (*funcExpr->getParmList())[0]->getValue(rdata);
		mMeasureVals.push_back(val);

		if(aggr_opr == "count") aggr_opr = "sum";
		mAggrOprs.push_back(aggr_opr);
	}

	//get max len
	JSONValue max_keylen = json["max_keylen"];
	mMaxKeyLen = max_keylen.asInt();
	if (mMaxKeyLen <= 0) 
		mMaxKeyLen = 50;

	//set record type
	OmnString type_str = json["record_type"].asString();
	if (type_str == "")
	{
		type_str = AOSRECORDTYPE_FIXBIN;
	}
	mRecordType = AosDataRecordType::toEnum(type_str);

	//get output dataset name
	string outputStr = "outputs";
	if (json.isMember(outputStr))
	{
		OmnScreen << "Found outputs configuration" << endl;
		//get output dataset names
		JSONValue outputJson = json["outputs"];
		mOutputStatKey = JSONValue(outputJson["statkey_output"]).asString();
		mOutputKeyPrefix = JSONValue(outputJson["key_output_prefix"]).asString();
		mOutputVt2d = JSONValue(outputJson["vt2d_output"]).asString();
	}
	else
	{
		//use default output names
		OmnScreen << "Use default outputs" << endl;
		if (mIsStreaming)
		{
			mOutputStatKey = "output_statkey";
		}
		else
		{
			mOutputStatKey = mName;
			mOutputStatKey << "_output_statkey";
		}

		if(mIsStreaming)
		{
			mOutputKeyPrefix = "output_key_";
		}
		else
		{
			mOutputKeyPrefix = mName;
			mOutputKeyPrefix << "_output_key_";
		}

		if(mIsStreaming)
		{
			mOutputVt2d = "output_vt2d";
		}
		else
		{
			mOutputVt2d = mName;
			mOutputVt2d << "_output_vt2d";
		}
	}

	//get cube id
	mCubeId = JSONValue(json["cube_id"]).asInt();

	if(mIsStreaming)
		mIILName = json["iilname"].asString();
	else
	{
		//get key opr settings
		mKeyIndexOprMap.clear();
		if (json.isMember("key_index_opr"))
		{
			JSONValue  jsonKeyOpr = json["key_index_opr"];
			Value::Members members = jsonKeyOpr.getMemberNames();
			OmnString name;

			for (u32 i = 0; i < members.size(); i++)
			{
				name = members[i];
				mKeyIndexOprMap[name] = jsonKeyOpr[name].asString();
			}
		}
	}

	return true;
}


//
//create data record templates
//
bool 
AosDataProcStatJoin::createOutput(AosRundata *rdata)
{
	//Jozhi 2015-03-28 implement to BuffArrayVar
	bool rslt = createStatkeyOutput(rdata);
	aos_assert_r(rslt, false);

	rslt = createPrefixKeyOutput(rdata);
	aos_assert_r(rslt, false);

	rslt = createVector2dOutput(rdata);
	aos_assert_r(rslt, false);
	return true;
}

////////////////////////////////////////////////////////////////
// JQL methods
////////////////////////////////////////////////////////////////

//
//For dataproc with downstreams, we need to generate output 
//dataset, data collector, data assemblers, data records
//for now
//
bool
AosDataProcStatJoin::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//json:
	//
	//create dataprocstatjoin dp_stat
	//{
	//  "type" : "statjoin",
	//	"keys" :["key_field1", "key_field2", "key_field3"], 
	//	"time" : "epoch_time",
	//	"measures" : ["m1", "m2"],
	//	"cube_id": 123,
	//  "outputs" : {
	//		"statkey_output" : "output_t1_stat_model_statkey",
	//		"key_prefix_output" : "output_t1_stat_model_key_prefix",
	//		"vt2d_output" : "output_t1_stat_model_vt2d"
	//	}
	//};
	
	//<jimodataproc >
	//  <dataproc zky_name="dp_stat" jimo_objid="dataprocstatjoin_jimodoc_v0">
	//    <![CDATA[{key_fields:["k1", "k2", "k3"], time_field:"xxx", measure_fields:["m1", "m2"], cube_id:"xxx"}]]>
	//  </dataproc>
	//</jimodataproc>
	
	//get parameters from the json string
	JSONReader reader;
	reader.parse(jsonstr, mJson);

	mName = dpname;
	bool rslt = getJSONParams(mJson,rdata);
	aos_assert_r(rslt, false);

	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, mName);
	//generate xml configuration string for jimo dataproc
	OmnString dp_str = "";
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " 
		<< AOSTAG_OBJID << "=\"" << objid << "\">";

	//add json string into the dataproc node text
	dp_str << "<dataproc zky_name=\"" << mName << "\"";
	dp_str << "jimo_objid=\"dataprocstatjoin_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]>";
	dp_str << "</dataproc>";
	dp_str << "</jimodataproc>";

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}

bool
AosDataProcStatJoin::fillOneNewKey(
		AosRundata*& rdata,
		const char* key,
		const int len)
{
	bool rslt;

	if (len == mPrevCreatedStatKeyLen &&
		strncmp(mPrevCreatedStatKey, key, len) == 0)
	{
		//the key has already created
		return true;
	}

	//new key
	mSdocid = mStatIdGen->nextDocid(rdata, mCubeId);
	mIsStatKeyNew = true;
	if(mPrevCreatedStatKey)
	{
		delete mPrevCreatedStatKey;
		mPrevCreatedStatKey = NULL;
	}
	mPrevCreatedStatKey = new char[len+1];
	memcpy(mPrevCreatedStatKey,key,len);
	mPrevCreatedStatKeyLen = len;

	//yang,2015/09/27
	aos_assert_r(mSdocid > 0, AosDataProcStatus::eError);

	//allocate a sdocid,create statkey index and key indexes
	rslt = fillRecordStatKey(rdata, key,len, mSdocid);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	rslt = fillRecordKey(rdata, key,len, mSdocid);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	return true;
}


////////////////////////////////////////////////////////////////
// procData/input/output methods
////////////////////////////////////////////////////////////////

//
//rhs_recordset: Statkey IIL records
//lhs_recordset: Statkey, time and value records
//
//This method assume that:
//1. lhs_recordset's records have the measure values for
//   the same statKey but different epoch time. It needs
//   to have the following fields:
//   [key1, key2, ..., timefield, measure1, measure2, ...]
//
//2. rhs_recordset has 0 or 1 statkey iil records, if
//   it has no record, a new statkey table entry and sdocid
//   needs to be created. Otherwise an existing one will be
//   used. It needs to have the following fields:
//   [sdocid]
//
//3. output_records has only one record with the following
//	 fields:
//   [statKey, keyIndex, statValue]
//
AosDataProcStatus::E
AosDataProcStatJoin::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	return AosDataProcStatus::eContinue;
}


AosDataProcStatus::E
AosDataProcStatJoin::procData(
		AosRundata *rdata,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	bool rslt;	
	AosDataRecordObj *lhs_record = input_records[0];
	AosDataRecordObj *rhs_record = input_records[1];

	//init flags
	mIsStatKeyNew = false;
	if (!lhs_record)
	{
		if(mPrevCreatedStatKey)
		{
			delete mPrevCreatedStatKey;
			mPrevCreatedStatKey = NULL;
		}
		return AosDataProcStatus::eExit;
	}

	char * rhsdata;
	u64 sdocid = 0;

	rslt = getStatKey(rdata, lhs_record);
	aos_assert_r(rslt, AosDataProcStatus::eError);

	char *lKey = mStatKeyValue.getBuffer();
	int lKeyLen = mStatKeyValue.length();

	
	//yang
	if(rhs_record)
	{
		//for debugging
		DumpRecord(rhs_record,"StatJoin input is: (lhs)");
		//this code only support buff array var,
		//if want to support fixbin,should modify this code
		//look up the index table
		rhsdata = rhs_record->getData(rdata);
		char* rKey = &rhsdata[4];
		int rKeyLen = *(int*)&rhsdata[0];

		int cmprslt;//= strcmp(lKey,rKey);
		if(lKeyLen != rKeyLen || (cmprslt = strncmp(lKey,rKey,lKeyLen)))
		{
			// lkey != rkey
			if(cmprslt < 0)
			{
				//rkey > lkey
				//key not exist
				if(mOpr == INSERT)
				{
					//insert new statkey based on last new statkey info
					fillOneNewKey(rdata,lKey,lKeyLen);
					fillRecordVt2d(rdata, lhs_record, mSdocid, mIsStatKeyNew);
				}
				else
				{
					//delete the non-existing key, ignore
				}

				return AosDataProcStatus::eLT;
			}
			else
			{
				//rkey < lkey
				return AosDataProcStatus::eGT;
			}
		}
		else
		{
			//get sdocid from rhs_record
			AosValueRslt value;
			bool rslt = rhs_record->getFieldValue(1, value, false, rdata);
			aos_assert_r(rslt, AosDataProcStatus::eError);

			//this sdocid is from statkey iil which can be either
			//local or global, right now, it is global
			sdocid = value.getU64();

			//confirm that sdocid's cube_id part is the same as current
			//cube_id or shuffle_id
			u32 cubeId = sdocid >> 32;
			aos_assert_r(cubeId == mCubeId, AosDataProcStatus::eError);

			//for debugging
			DumpRecord(lhs_record,"StatJoin input is: (lhs)");

			//t1 = clock();
			rslt = fillRecordVt2d(rdata, lhs_record, sdocid, mIsStatKeyNew);
			//t2 = clock();
			//vt2d_t += (double)( (t2 - t1)/(double)CLOCKS_PER_SEC );
			aos_assert_r(rslt, AosDataProcStatus::eError);

			//We don't want right side move on. Instead, we
			//would like all the left keys == current right
			//keys processed
			return AosDataProcStatus::eLT;
		}
	}

	DumpRecord(lhs_record,"StatJoin input is: (lhs)");
	//this code handles the case of prev rhs_record is not null and prev lKey > prev rKey
	if(mOpr == INSERT)
	{
		//key has not been filled
		fillOneNewKey(rdata,lKey,lKeyLen);
		fillRecordVt2d(rdata, lhs_record, mSdocid, mIsStatKeyNew);
	}
	else
	{
		//delete the non-existing key, ignore

	}

	//move left side
	return AosDataProcStatus::eLT;
}

//
//set statkey record value
//
bool
AosDataProcStatJoin::fillRecordStatKey(
		AosRundata *rdata,
		const char* key,
		const int len,
		const u64 sdocid)
{
	aos_assert_r(key, AosDataProcStatus::eContinue);
	bool rslt;

	//input record has:  statkey  timefield sum(v1) sum(v2) count(v3)
	//statKey index is: |statkey |  sdocid |
	bool outofmem = false;

	//OmnString name = mOutputStatKey;
	AosDataRecordObjPtr& output_record = mStatKeyOutputRecord;
	output_record->clear();
	aos_assert_r(output_record, AosDataProcStatus::eError);

	AosValueRslt k_v(OmnString(const_cast<char*>(key),len));
	rslt = output_record->setFieldValue(0, k_v, outofmem, rdata);
	if (!rslt) return AosDataProcStatus::eError;              

	AosValueRslt d_v(sdocid);
	rslt = output_record->setFieldValue(1, d_v, outofmem, rdata);
	if (!rslt) return AosDataProcStatus::eError;                 

	//for debugging
	DumpRecord(output_record,"StatJoin output is: (statkey) ");
	if (!mIsStreaming) 
	{
		//this is batch mode
		output_record->flushRecord(rdata);
	}
	else
	{
		//in streaming mode
		//insert the record into output recordset
		addOutput(rdata, mOutputStatKey, output_record);
		output_record->reset();
	}
	return true;
}

//
//set individual key record value
//
bool
AosDataProcStatJoin::fillRecordKey(
		AosRundata *rdata,
		const char* key,
		const int len,
		const u64 sdocid)
{
	bool rslt;
	bool outofmem = false;
	
	//key index format is:  | key | sdocid |
	OmnString dsKeyName;
	AosDataRecordObjPtr output_record;
	for (u32 i = 0; i < mKeyList.size(); i++)
	{	
		dsKeyName = mOutputKeyPrefix;
		dsKeyName << mKeyList[i];
		//yang,2015/08/25
		output_record = mKeyOutputRecordArray[i];
		output_record->clear();

		aos_assert_r(output_record, AosDataProcStatus::eError);   

		if(mIsStreaming)
			rslt = output_record->setFieldValue(0, mRcdKeyList[i], outofmem, rdata);       
		else
			rslt = output_record->setFieldValue(0, mKeyValueList[i], outofmem, rdata);       
		if (!rslt) return AosDataProcStatus::eError;            

		AosValueRslt d_v(sdocid);
		rslt = output_record->setFieldValue(1, d_v, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;                

		//for debugging
		DumpRecord(output_record,"StatJoin output is: (key) ");
		if (!mIsStreaming) 
		{
			//this is batch mode
			output_record->flushRecord(rdata);
		}
		else
		{
			//in streaming mode
			//insert the record into output recordset
			addOutput(rdata, dsKeyName, output_record);
			output_record->reset();
		}
	}

	return true;
}

//
// input format is : (from dataprocgroupby)
//     statkey | time |  measure1 |  measure2 ...
//
// output format is: (to statdoc)
//     isnew | local sdocid  |  statkey |  time | measure1 |  measure2 ...
//
bool
AosDataProcStatJoin::fillRecordVt2d(
		AosRundata *rdata,
		AosDataRecordObj *input_record,
		const u64 sdocid,
		const char isnew)
{
	//format: isnew|sdocid|statKey|epoch_time|mearsure1|mearsure2|
	//AosDataRecordObjPtr output_record = mOutputRecordMap[mOutputVt2d];
	//yang,2015/08/25
	AosDataRecordObjPtr& output_record = mVt2dOutputRecord;
	aos_assert_r(output_record, AosDataProcStatus::eError);
	output_record->clear();

	bool outofmem = false;
	bool rslt;

	//set isnew
	AosValueRslt k_v(isnew);
	rslt = output_record->setFieldValue(0, k_v, outofmem, rdata);
	if (!rslt) return AosDataProcStatus::eError;      

	//set sdocid
	//translate sdocid to local sdocid and send to the cube.
	//Assume local statjoin data will be directed to local
	//statdoc, then local cube process to handle
	//
	//So far, the first 4 bytes are cube_id which is local.
	//The rest 4 bytes are localdocid
	if(mIsStreaming)
	{
		//set sdocid
		AosValueRslt s_d(sdocid);
		rslt = output_record->setFieldValue(1, s_d, outofmem, rdata);
	}
	else
	{
		u64 localDocid = sdocid & 0x00000000ffffffff;
		//arvin,2015.12.08
		//JIMODB-1366
		if(localDocid <= 0)
		{
			OmnScreen << "fillRecordVt2d::" << mStatKeyValue << "unallocated sdocid " << localDocid  << endl;					
			return false;
		}
		AosValueRslt d_v(localDocid);
		rslt = output_record->setFieldValue(1, d_v, outofmem, rdata);
	}
	if (!rslt) return AosDataProcStatus::eError;      

	//set statKey
	AosValueRslt s_v(mStatKeyValue);
	rslt = output_record->setFieldValue(2, s_v, outofmem, rdata);
	aos_assert_r(rslt, AosDataProcStatus::eError);      

	//set epoch_time
	AosValueRslt time_rslt;
	u32 idx = mKeyExprList.size();
	if(0 == idx)  idx =1;
	rslt = input_record->getFieldValue(idx++, time_rslt, false, rdata);
	aos_assert_r(rslt, AosDataProcStatus::eError);  

	rslt = output_record->setFieldValue(3, time_rslt, outofmem, rdata);
	if (!rslt) return AosDataProcStatus::eError;      

	//set measure_values
	AosValueRslt measure_rslt;
	OmnString measure_value;
	//u32 inMeasurePos = 2;
	u32 outMeasurePos = 4;
	for (u32 i = 0; i < mMeasureList.size(); i++)
	{
		rslt = input_record->getFieldValue(i + idx, measure_rslt, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError); 
		
		rslt = output_record->setFieldValue(i + outMeasurePos, measure_rslt, outofmem, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);      
	} 

	//for debugging
	DumpRecord(output_record,"StatJoin output is: (vt2d) ");
	if (!mIsStreaming) 
	{
		//this is batch mode
		output_record->flushRecord(rdata);
	}
	else
	{
		//in streaming mode
		//insert the record into output recordset
		addOutput(rdata, mOutputVt2d, output_record);
		output_record->reset();
	}
	return true;
}

///////////////////////////////////////////////////////////
//  Helper methods
///////////////////////////////////////////////////////////


//
//generate a data field and insert into the data record
//
bool
AosDataProcStatJoin::addDataField(
								 boost::shared_ptr<AosConf::DataRecordFixbin> &dr,
								 const OmnString &name,
								 const OmnString &type,
								 const OmnString &shortplc,
								 const u32 offset,
								 const u32 len
								 )
{
	boost::shared_ptr<AosConf::DataField> df = boost::make_shared<AosConf::DataField>();

	df = boost::make_shared<AosConf::DataField>();
	df->setAttribute("zky_name", name);
	df->setAttribute("type", type);

	if (shortplc != "")
		df->setAttribute("zky_datatooshortplc", shortplc);
	df->setAttribute("zky_offset", offset);
	df->setAttribute("zky_length", len);

	dr->setField(df);
	return true;
}

//
//if return empty statkey, continue this record
//JIMODB-1040
//arvin 2015.20.28
//mKeyValueList will keep all the value_rslt
bool
AosDataProcStatJoin::getStatKey(
						AosRundata *rdata,
						AosDataRecordObj* &lhs_record)
{
	mStatKeyValue = "";
	mKeyValueList.clear();
	AosValueRslt key_rslt;
	bool rslt;
	if(!mHasKey)
	{
	//	rslt = lhs_record->getFieldValue(0, key_rslt, false, rdata);
		mStatKeyValue = "true";
		key_rslt.setStr(mStatKeyValue);
		mKeyValueList.push_back(key_rslt);
	}
	else
	{
		for(size_t i = 0;i < mKeyExprList.size();i++ )
		{
			if(i>0)
				mStatKeyValue << (char)0x01;
			OmnString key = mKeyExprList[i]->dumpByNoEscape();
			AosExprObjPtr iilmap = mInfoFields[key];
			if(iilmap)
			{
				rslt = iilmap->getValue(rdata,lhs_record, key_rslt);
			}
			else
			{
				rslt = mKeyExprList[i]->getValue(rdata,lhs_record, key_rslt);
			}
			aos_assert_r(rslt,false);
			if(key_rslt.getType()==AosDataType::eDateTime)
			{
				i64 vv = key_rslt.getI64();
				key_rslt.setI64(vv);
			}
		
			mKeyValueList.push_back(key_rslt);
			if (key_rslt.isNull())
			{
				mStatKeyValue << char(0x08);
			}
			else
			{
				mStatKeyValue << key_rslt.getStr();
			}
		}
		return true;
	}
	return true;
}

//////////////////////////////////////////////////////////////
//For Streaming dataproc. Modified by Levi and Felicia
//////////////////////////////////////////////////////////////
//
// for dataproc with multiple input recordset. The dataproc need
// to implement its procData method with rsMap argument
//
//
bool 
AosDataProcStatJoin::procData(AosRundata *rdata, 
		const AosRDDPtr &rdd)
			  //RecordsetMap &rsMap)
{
	AosRecordsetObjPtr rs;
	//xuqi 2015/9/8
	//mPrevLeftStatKeyLen = 0;
	mPrevCreatedStatKeyLen = 0;

	//call dataproc's own start method
	start(rdata);

	//get leftdata and right data
	rs = rdd->getData();
	
	//collect all the distinct keys from input record set
	//input format is:
	//   |statkey | time | measure1 | measure2 |
	//u64 tStart,tEnd;
	OmnString prevKey, curKey;
	AosDataRecordObj* rcd;
	vector<AosDataRecordObjPtr> rcdList;
	vector<OmnString> keyList;
	vector<u64> sizeList;
	rcdList.clear();
	keyList.clear();
	sizeList.clear();
	mRecordsKeyList.clear();
	bool rslt = false;
	rs->resetReadIdx();
	u64 idx = 0;
	u64 same_idx = 0;
	mStatKeyValue = "";
	AosDataRecordObjPtr record;
	AosDataRecordObj *raw_rcd;
	while (1)
	{
		idx++;
		rslt = rs->nextRecord(rdata, rcd);
		if(idx <= eDefRcdSize && rcd)
		{	
			rslt = false;
			record = rcd->clone(rdata AosMemoryCheckerArgs);
			rcdList.push_back(record);
			
			raw_rcd = record.getPtr();
			rslt = getStatKey(rdata, raw_rcd);
			curKey = mStatKeyValue;

			if(prevKey == "")
			{
				prevKey = curKey;
				same_idx++;
			}
			else if(prevKey != curKey)
			{
				//set record key list to list
				//getRecordKey 's record is not the current record

				raw_rcd = rcdList[rcdList.size()-same_idx-1].getPtr();
				mRecordsKeyList.push_back(getRecordKeyList(rdata, raw_rcd));
				keyList.push_back(prevKey);
				sizeList.push_back(same_idx);

				prevKey = curKey;
				same_idx = 1;
			}
			else if(prevKey == curKey)
			{
				same_idx++;
			}
		}
		if(!rcd && prevKey != "")
		{
			if(same_idx > 1)
			{
				sizeList[keyList.size()-1] = same_idx;
			}
			else
			{
				raw_rcd = rcdList[rcdList.size()-1].getPtr();
				mRecordsKeyList.push_back(getRecordKeyList(rdata, raw_rcd));

				keyList.push_back(prevKey);
				sizeList.push_back(same_idx);
			}
		}
		mHasKeyValue = false;
		if(rslt && !rcd)
		{
			//get docids by statkeys
			bool needDfValue = true;
			const u64 dfValue = 0;
			AosBuffPtr buff = OmnNew AosBuff(50000000 AosMemoryCheckerArgs);
			AosIILClientObjPtr iilClient = AosIILClientObj::getIILClient();
			iilClient->getDocidsByKeys(mIILName, keyList, needDfValue, dfValue, buff, rdata);
			
			//fill value
			getDocidAndFillValue(rdata, rcdList, sizeList, keyList, buff);
			
			keyList.clear();
			sizeList.clear();
			rcdList.clear();
			idx = 0;
	
			break;
		}

		if(idx == eDefRcdSize)
		{
			//get docids by statkeys
			bool needDfValue = true;
			const u64 dfValue = 0;
			AosBuffPtr buff = OmnNew AosBuff(50000000 AosMemoryCheckerArgs);
			AosIILClientObjPtr iilClient = AosIILClientObj::getIILClient();
			iilClient->getDocidsByKeys(mIILName, keyList, needDfValue, dfValue, buff, rdata);

			//fill value
			getDocidAndFillValue(rdata, rcdList, sizeList, keyList, buff);
			
			keyList.clear();
			sizeList.clear();
			rcdList.clear();
			idx = 0;
		}
	}

	//call dataproc's own finish method
	finish(rdata);
	//tEnd = OmnGetTimestamp();
	//mStatJoinTime += tEnd - tStart;

	return true;
}


bool
AosDataProcStatJoin::getIILEntries(
		const AosRundataPtr &rdata,           
		const OmnString &keyValue,            
		vector<AosDataRecordObj *> &recordList)
{
	//u64 tStart = OmnGetTimestamp();
	AosQueryRsltObjPtr query_rslt = AosQueryRsltObj::getQueryRsltStatic();
	query_rslt->setWithValues(true);
	AosQueryContextObjPtr query_context = AosQueryContextObj::createQueryContextStatic();

	//add filter condition 
	AosXmlTagPtr query_filters;
	AosQueryFilterObjPtr filter;
	//query_context->addFilter(filter);

	query_context->setOpr(AosOpr_toEnum("pf")); //opr = "="???
	query_context->setStrValue(keyValue); 
	query_context->setBlockSize(0);
	query_context->setPageSize(100);

	bool rslt = AosQueryColumn(mIILName, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, false);

	bool finished = false;
	OmnString vv;
	u64 sdocid;
	vector<u64> docidList;
	OmnString str ,key, val, key_val;
	AosBuffDataPtr Data;
	vector<OmnString> values;
	vector<OmnString> fields;

	while (query_rslt->nextDocidValue(sdocid, vv, finished, rdata))
	{
		//for the data isnull the leftjoin and the rightjoin are the same
		if(finished) break;
		//aos_assert_r(vv != "", false);
		values.push_back(vv.getBuffer());
		mSdocid = sdocid;
	}

	if(values.size()>0)
	{
		mIsStatKeyNew = false;
		mHasKeyValue = false;
		for(u32 i=0; i<recordList.size(); i++)
		{
			rslt = fillRecordVt2d(rdata.getPtr(), recordList[i], mSdocid, mIsStatKeyNew);
		}
	}
	else
	{
		if(mOpr == INSERT)
		{
			AosRundata * rundata = rdata.getPtr();
			//mHasKeyValue = false;
			for(u32 i=0; i<recordList.size(); i++)
			{
				mIsStatKeyNew = false;
				mHasKeyValue = false;
				fillOneNewKey(rundata, keyValue.data(), keyValue.length());
				rslt = fillRecordVt2d(rdata.getPtr(), recordList[i], mSdocid, mIsStatKeyNew);
			}
		}
		else
		{
			//delete the non-existing key, ignore
		}
	}

	return true;
}

	
bool
AosDataProcStatJoin::getDocidAndFillValue(
		const AosRundataPtr &rdata,
		vector<AosDataRecordObjPtr> &recordList,
		vector<u64> &sizeList,
		vector<OmnString> &keyList,
		AosBuffPtr &buffPtr)
{
	u64 docid = 0;
	AosDataRecordObj *rcd;
	for(u64 i=0; i<keyList.size(); i++)
	{
		docid = buffPtr->getU64(0);
		for(u64 j=0; j<sizeList[i]; j++)
		{
			if(mOpr == INSERT)
			{
				if(docid != 0)
				{
					mSdocid = docid;
					mIsStatKeyNew = false;
					mHasKeyValue = false;

					mStatKeyValue = keyList[i];
					rcd = recordList[i+j].getPtr();
					bool rslt = fillRecordVt2d(rdata.getPtr(), rcd, mSdocid, mIsStatKeyNew);
					aos_assert_r(rslt, false);
				}
				else
				{
					bool rslt = false;
					AosRundata * rundata = rdata.getPtr();
					//record key
					mRcdKeyList = mRecordsKeyList[i];
					mStatKeyValue = keyList[i];
					fillOneNewKey(rundata, keyList[i].data(), keyList[i].length());
					mIsStatKeyNew = true;

					//mStatKeyValue = keyList[i];
					rcd = recordList[i+j].getPtr();
					rslt = fillRecordVt2d(rdata.getPtr(), rcd, mSdocid, mIsStatKeyNew);
					aos_assert_r(rslt, false);
				}
			}
			else
			{
				//delete the non-existing key, ignore
			}
		}
	}

	return true;
}

bool
AosDataProcStatJoin::fillValue(
		const AosRundataPtr &rdata,
		const OmnString &keyValue,
		vector<AosDataRecordObj *> &recordList)
{
	//mLock->lock();
	if(mOpr == INSERT)
	{
		bool rslt = false;
		AosRundata * rundata = rdata.getPtr();
		fillOneNewKey(rundata, keyValue.data(), keyValue.length());
		mIsStatKeyNew = true;
		for(u32 i=0; i<recordList.size(); i++)
		{
			rslt = fillRecordVt2d(rdata.getPtr(), recordList[i], mSdocid, mIsStatKeyNew);
			aos_assert_r(rslt, false);
		}
	}
	else
	{
		//delete the non-existing key, ignore
	}
	//mLock->unlock();
	return true;
}

vector<AosValueRslt>
AosDataProcStatJoin::getRecordKeyList(
						AosRundata *rdata,
						AosDataRecordObj* &lhs_record)
{
	vector<AosValueRslt> recordKeyList;
	recordKeyList.clear();
	AosValueRslt key_rslt;
	bool rslt;
	for(size_t i = 0;i < mKeyExprList.size();i++ )
	{
		rslt = mKeyExprList[i]->getValue(rdata,lhs_record, key_rslt);
		aos_assert_r(rslt, recordKeyList);

		if(key_rslt.getType()==AosDataType::eDateTime)
		{
			i64 vv = key_rslt.getI64();
			key_rslt.setI64(vv);
		}
		recordKeyList.push_back(key_rslt);
	}
	return recordKeyList;
}


