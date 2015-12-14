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
#include "JimoDataProc/DataProcIILJoin.h"

#include "API/AosApi.h"
#include "API/AosApiS.h"
#include "Util/SPtr.h"
//#include "API/AosApiQ.h"
#include "Util/String.h"
#include "IILClient/IILClient.h"


////////////////////////////////////////////////////////////////
// Constructors/Destructors/Config
////////////////////////////////////////////////////////////////
extern "C"
{

	AosJimoPtr AosCreateJimoFunc_AosDataProcIILJoin_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcIILJoin(version);
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


AosDataProcIILJoin::AosDataProcIILJoin(const int version)
:
AosStreamDataProc(version, AosJimoType::eDataProcIILJoin)
{
}


AosDataProcIILJoin::AosDataProcIILJoin(const AosDataProcIILJoin &proc)
:
AosStreamDataProc(0, AosJimoType::eDataProcIILJoin)
{
}


AosDataProcIILJoin::~AosDataProcIILJoin()
{
}


AosJimoPtr
AosDataProcIILJoin::cloneJimo() const
{
	return OmnNew AosDataProcIILJoin(*this);
}

bool
AosDataProcIILJoin::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc><![CDATA[
	//	{
	//		"type": "IILJoin",
	//		"left_keys": ["k1", "k2", "k3"],
	//		"right_keys": ["k1", "k2", "k3"],
	//		"left_vals": ["k1", "k2", "k3"],
	//		"right_vals": ["k1", "k2", "k3"],
	//	}
	//]]></dataproc>
	try
	{
		mLock = OmnNew OmnMutex();
		aos_assert_r(def, false);
		//aos_assert_r(isVersion1(def), false);

		JSONValue json;
		JSONReader reader;
		bool rslt = false;
		OmnString dp_jsonstr = def->getNodeText();
		reader.parse(dp_jsonstr, mJson);
		
		mName = def->getAttrStr("zky_name");
		aos_assert_r(mName != "", false);
		OmnScreen << "dataproc config json is: " << dp_jsonstr << endl;
		
		//get JSON parameters
		rslt = getJSONParams(mJson);
		aos_assert_r(rslt, false);

		//create output record templates
		rslt = createOutputRecords(rdata.getPtr());
		aos_assert_r(rslt, false);

		mJson["flag"] = false;
		return true;
	}
	catch (...)
	{
		OmnScreen << "JSONException..." << endl;
		return false;
	}
}

//
//create data record templates
//
bool 
AosDataProcIILJoin::createOutputRecords(AosRundata *rdata)
{
	AosDataRecordObjPtr record;
	AosXmlTagPtr recordXml;
	OmnString xmlStr;

	mOutput= "output";

	//create statkey output record
			
	OmnString rcd_type = "buff";
	OmnString str = "";
	AosXmlTagPtr xmlRecord;
	if(rcd_type == "buff")
	{
		OmnString record_type = AOSRECORDTYPE_BUFF;
		AosDataRecordType::E type = AosDataRecordType::toEnum(record_type);
		boost::shared_ptr<Output> output = boost::make_shared<Output>(mOutput, type);
		
		AosXmlTagPtr doc;
		OmnString schema_name = mJson["schema"].asString("");
		if (schema_name != "")
		{
			doc = AosJqlStatement::getDoc(
					rdata, JQLTypes::eDataRecordDoc, schema_name);
		}
		aos_assert_r(doc, false);
		AosXmlTagPtr dataFieldsXml = doc->getFirstChild("datafields"); 
		aos_assert_r(dataFieldsXml, false);                                  

		bool rslt = false;
		for(u32 i=0; i<mLeftKeyList.size(); i++)
		{
			rslt = setOutputField(rdata, dataFieldsXml, mLeftKeyList[i], output);
			aos_assert_r(rslt, false);
		}

		for(u32 i=0; i<mLeftValList.size(); i++)
		{
			rslt = setOutputField(rdata, dataFieldsXml, mLeftValList[i], output);
			aos_assert_r(rslt, false);
		}
		
		for(u32 i=0; i<mRightKeyList.size(); i++)
		{
			rslt = setOutputField(rdata, dataFieldsXml, mRightKeyList[i], output);
			aos_assert_r(rslt, false);
		}

		for(u32 i=0; i<mRightValList.size(); i++)
		{
			rslt = setOutputField(rdata, dataFieldsXml, mRightValList[i], output);
			aos_assert_r(rslt, false);
		}
		
		for(u32 i=0; i<mLeftKeyList.size(); i++)
		{
			mLeftList.push_back(mLeftKeyList[i]);
		}
		
		for(u32 i=0; i<mLeftValList.size(); i++)
		{
			mLeftList.push_back(mLeftValList[i]);
		}

		for(u32 i=0; i<mRightKeyList.size(); i++)
		{
			mRightList.push_back(mRightKeyList[i]);
		}

		for(u32 i=0; i<mRightValList.size(); i++)
		{
			mRightList.push_back(mRightValList[i]);
		}

		/*
		while(dataFieldXml)
		{
			name = dataFieldXml->getAttrStr("zky_name");
			fType = dataFieldXml->getAttrStr("type"); 
			const OmnString cType = fType;
			fieldType = convertToDataFieldType(rdata, cType); 
	
			strLen = dataFieldXml->getAttrStr("zky_length");
			output->setField(name, fieldType, strLen.toInt64(0));

			dataFieldXml = dataFieldsXml->getNextChild();
		}
		*/	
		output->init(mTaskDocid, rdata);
		AosDataRecordObjPtr dataRecord = output->getRecord();
		xmlRecord = dataRecord->getRecordDoc();
	}

	record = AosDataRecordObj::createDataRecordStatic(xmlRecord, mTaskDocid, rdata AosMemoryCheckerArgs);
	aos_assert_r(record, false);
	mOutputRecordMap[mOutput] = record;

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
AosDataProcIILJoin::createByJql(
		AosRundata *rdata,
		const OmnString &dpname,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	//get parameters from the json string
	JSONReader reader;
	reader.parse(jsonstr, mJson);

	//aos_assert_r(getJSONParams(mJson), false);

	//generate xml configuration string for jimo dataproc
	OmnString dp_str = "";
	OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, dpname);
	dp_str << "<jimodataproc " << AOSTAG_CTNR_PUBLIC << "=\"true\" ";
	dp_str << AOSTAG_PUBLIC_DOC << "=\"true\" " << AOSTAG_OBJID << "=\"" << objid << "\">";

	//add json string into the dataproc node text
	dp_str << "<dataproc zky_name=\"" << dpname << "\" ";
	dp_str << "jimo_objid=\"dataprociiljoin_jimodoc_v0\">";
	dp_str << "<![CDATA[" << jsonstr << "]]>";
	dp_str << "</dataproc>";

	//generate output dataset
	dp_str << "<datasets>";
	//AosConf::DataRecordFixbin drStatKey;
	dp_str << "<dataset zky_type=\"datacol\" zky_name=\"" << mOutput << "\">";
	dp_str << "<datacollector zky_type=\"iil\" type=\"reduce\" zky_name=\"" << mOutput << "\">";
	dp_str << "<asm type=\"stradd\">";  
	dp_str << defineRecord(rdata);
	dp_str << "</asm> </datacollector> </dataset> </datasets> </jimodataproc> ";

	AosXmlTagPtr dp_xml = AosStr2Xml(rdata, dp_str AosMemoryCheckerArgs);
	dp_xml->setAttr(AOSTAG_CTNR_PUBLIC, "true");
	dp_xml->setAttr(AOSTAG_PUBLIC_DOC, "true");
	dp_xml->setAttr(AOSTAG_OBJID, objid);

	prog->saveLogicDoc(rdata, objid, dp_str);
	return true;
}

////////////////////////////////////////////////////////////////
// procData/input/output methods
////////////////////////////////////////////////////////////////

//
//This is inner join method. 
//Assumption:
// 	both right and left dataset have the same key values
//
//Output recordset:
//  A production of left and right dataset. E.g:
//	Left dataset:
//	  f1     v1
//	  ---------
//	  a      10
//	  a      20
//
// Right dataset:
//    f2     v2
//    ---------
//    a      30
//    a      40
//    a      50
//
// The result is:
//   f1   f2   v1   v2
//   a    a    10   30
//   a    a    10   40
//   a    a    10   50
//   a    a    20   30
//   a    a    20   40
//   a    a    20   50
//
AosDataProcStatus::E
AosDataProcIILJoin::procData(
		AosRundata *rdata,
		const AosRecordsetObjPtr &lhs_recordset,
		const AosRecordsetObjPtr &rhs_recordset,
		AosDataRecordObj **output_records)
{
	AosDataRecordObj *lhs_record = NULL;
	AosDataRecordObj *rhs_record = NULL;

	bool rslt;
	lhs_recordset->resetReadIdx();
	for (u32 i = 0; i < lhs_recordset->size(); i++)
	{
		rslt = lhs_recordset->nextRecord(rdata, lhs_record);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		DumpRecord(lhs_record,"IILJoin input is: (lhs)");
		rhs_recordset->resetReadIdx();
		for (u32 j = 0; j < rhs_recordset->size(); j++)
		{
			rslt = rhs_recordset->nextRecord(rdata, rhs_record);
			aos_assert_r(rslt, AosDataProcStatus::eError);

			DumpRecord(rhs_record,"IILJoin input is: (rhs)");
			rslt = fillRecord(rdata, lhs_record, rhs_record);
			aos_assert_r(rslt, AosDataProcStatus::eError);
		}
	}
	//while (lhs_record);

	return AosDataProcStatus::eContinue;
}

//
//generate xml string for output record
//
OmnString
AosDataProcIILJoin::defineRecord(AosRundata *rdata)
{
	//get output dataset name
	mOutput= "output";

	OmnString rcd_type = "buff";
	OmnString str = "";
	if(rcd_type == "buff")
	{
		OmnString record_type = AOSRECORDTYPE_BUFF;
		AosDataRecordType::E type = AosDataRecordType::toEnum(record_type);
		boost::shared_ptr<Output> output = boost::make_shared<Output>(mOutput, type);
		
		OmnString field_type = "str";
		const OmnString cFieldType = field_type;
		AosDataFieldType::E fieldType = convertToDataFieldType(rdata, cFieldType);
		//aos_assert_r(fieldType != AosDataFieldType::eInvalid, false);
		int len = 20;
		for(u32 i = 0; i < mLeftKeyList.size(); i++)
			output->setField(mLeftKeyList[i], fieldType, len);
	
		for (u32 i = 0; i < mLeftValList.size(); i++)
			output->setField(mLeftValList[i], fieldType, len);
	
		/*
		for(u32 i = 0; i < mRightKeyList.size(); i++)
			output->setField(mRightKeyList[i], fieldType, len);
		
		for (u32 i = 0; i < mRightValList.size(); i++)
			output->setField(mRightValList[i], fieldType, len);
		*/

		output->init(mTaskDocid, rdata);
		AosDataRecordObjPtr dataRecord = output->getRecord();
		AosXmlTagPtr xmlRecord = dataRecord->getRecordDoc();
		str = xmlRecord->toString();
	}
	else
	{
		//Jozhi may be problem
		OmnNotImplementedYet;
		/*
		//for fixbin
		AosConf::DataRecordFixbin dr;
		str << "<datarecord type=\"ctnr\" zky_name=\"" << mOutput << "\">";

		dr.setAttribute("zky_name", mOutput);
		dr.setAttribute("type", "fixbin");

		//add data fields to dr
		for (u32 i = 0; i < mLeftKeyList.size(); i++)
		{
			addDataField(dr, mLeftKeyList[i], "str", "cstr", 0, mMaxKeyLen); 
		}

		for (u32 i = 0; i < mRightKeyList.size(); i++)
		{
			addDataField(dr, mRightKeyList[i], "str", "cstr", 0, mMaxKeyLen); 
		}

		for (u32 i = 0; i < mLeftValList.size(); i++)
		{
			addDataField(dr, mLeftValList[i], "str", "cstr", 0, mMaxKeyLen); 
		}

		for (u32 i = 0; i < mRightValList.size(); i++)
		{
			addDataField(dr, mRightValList[i], "str", "cstr", 0, mMaxKeyLen); 
		}

		int total = 0;
		total += mLeftKeyList.size();
		total += mRightKeyList.size();
		total += mLeftValList.size();
		total += mRightValList.size();
		dr.setAttribute("zky_length", mMaxKeyLen * total);
		str << dr.getConfig();
		str << "</datarecord>";
		*/
	}
	return str;
}

//
//set output record value
//
bool
AosDataProcIILJoin::fillRecord(
		AosRundata *rdata,
		AosDataRecordObj *lhs_record,
		AosDataRecordObj *rhs_record)
{
	bool rslt;
	bool outofmem = false;
	OmnString key;
	vector<OmnString> lhsNameList;
	vector<OmnString> rhsNameList;

	mLock->lock();
	AosDataRecordObjPtr output_record = mOutputRecordMap[mOutput];
	aos_assert_r(output_record, AosDataProcStatus::eError);   
	output_record->clear();

	//get lhs fields
	AosXmlTagPtr lhsXml = lhs_record->getRecordDoc();
	AosXmlTagPtr lhsDataFields = lhsXml->getFirstChild("datafields");
	AosXmlTagPtr lhsDataField = lhsDataFields->getFirstChild();
	while(lhsDataField)
	{
		OmnString name = lhsDataField->getAttrStr("zky_name");
		lhsNameList.push_back(name);

		lhsDataField = lhsDataFields->getNextChild();
	}
	
	//get rhs fields
	AosXmlTagPtr rhsXml = rhs_record->getRecordDoc();
	AosXmlTagPtr rhsDataFields = rhsXml->getFirstChild("datafields");
	AosXmlTagPtr rhsDataField = rhsDataFields->getFirstChild();
	while(rhsDataField)
	{
		OmnString name = rhsDataField->getAttrStr("zky_name");
		rhsNameList.push_back(name);

		rhsDataField = rhsDataFields->getNextChild();
	}

	//get left key from lhs_record
	AosValueRslt value;
	int idx = 0;
	for(u32 i=0; i<lhsNameList.size(); i++)
	{
		rslt = lhs_record->getFieldValue(i, value, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		key = value.getStr();

		AosValueRslt l_v(key);
		rslt = output_record->setFieldValue(i, l_v, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError; 
		idx++;
	}

	for(u32 i=0; i<rhsNameList.size(); i++)
	{
		//get right key from rhs_record
		rslt = rhs_record->getFieldValue(i, value, false, rdata);
		aos_assert_r(rslt, AosDataProcStatus::eError);
		key = value.getStr();

		AosValueRslt r_v(key);
		rslt = output_record->setFieldValue(i+idx, r_v, outofmem, rdata);
		if (!rslt) return AosDataProcStatus::eError;              
	}
	
	output_record->flush(false);
	mLock->unlock();
	//for debugging
	DumpRecord(output_record,"IILJoin output is: ");
	
	if (!mIsStreaming) 
	{
		//this is batch mode
		output_record->flushRecord(rdata);
	}
	else
	{
		//in streaming mode
		//insert the record into output recordset
		addOutput(rdata, mOutput, output_record);
	}

	return true;
}

///////////////////////////////////////////////////////////
//  Helper methods
///////////////////////////////////////////////////////////

//
// read json parameters into member variables
//
bool
AosDataProcIILJoin::getJSONParams(JSONValue json)
{
	//get left key fields
	JSONValue keyFields = json["left_keys"];
	OmnString key;
	for (size_t i = 0; i < keyFields.size(); i++)
	{
		key = JSONValue(keyFields[i]).asString();
		mLeftKeyList.push_back(key);
	}

	//get right key fields
	keyFields = json["right_keys"];
	for (size_t i = 0; i < keyFields.size(); i++)
	{
		key = JSONValue(keyFields[i]).asString();
		mRightKeyList.push_back(key);
	}

	//get left val fields
	JSONValue valFields = json["left_values"];
	OmnString value;
	for (size_t i = 0; i < valFields.size(); i++)
	{
		value = JSONValue(valFields[i]).asString();
		mLeftValList.push_back(value);
	}

	//get right val fields
	valFields = json["right_values"];
	for (size_t i = 0; i < valFields.size(); i++)
	{
		value = JSONValue(valFields[i]).asString();
		mRightValList.push_back(value);
	}
	
	//get Max key length
	JSONValue max_keylen = json["max_keylen"];
	mMaxKeyLen = max_keylen.asInt();
	if(mMaxKeyLen <= 0)
		mMaxKeyLen = 50;

	

	//get iil name
	mIILName = json["iilname"].asString();
	return true;
}

vector<AosDataRecordObjPtr> 
AosDataProcIILJoin::getOutputRecords()
{
	vector<AosDataRecordObjPtr> v;
	hash_map<OmnString, AosDataRecordObjPtr, Omn_Str_hash, compare_str>::iterator itr;
	AosDataRecordObjPtr record;

	itr =  mOutputRecordMap.begin();
	while (itr != mOutputRecordMap.end())
	{
		record = itr->second;
		v.push_back(record);

		itr++;
	}

	return v;
}

//
// This method join dataset with an IIL. It is the
// outer procData method
//
// Assumption:
//   The dataset is sorted already
//
// Result:
//   1. Find dataset's key group in the IIL. A key
//      group contains records with the same key
//   2. If not found, go to next key group in the 
//     dataset
//   3. otherwise, find all the IIL entries with 
//      with the same key
//   4. The key group is the lhs_recordset. The IIL
//      entry group with the same key will be used
//      as rhs_recordset. Both lhs_recordset and 
//      rhs_recordset are provided to the inner
//      procData method
//
bool 
AosDataProcIILJoin::procData(AosRundata *rdata, 
				const AosRDDPtr &rdd)
//			  RecordsetMap &rsMap)
{
	AosRecordsetObjPtr rs;

	//this dataproc has two inputs
	//aos_assert_r(rsMap.size() == 1, false);

	//call dataproc's own start method
	start(rdata);

	//get leftdata and right data
	//rs = rsMap["input"];
	rs = rdd->getData();
	
	//AosRecordsetObjPtr lhs_recordset;
	//OmnString curKey, prevKey;
	//OmnString strContainer;
	//AosQueryRsltObjPtr query_rslt;
	//AosQueryContextObjPtr query_context;
	//vector<OmnString> rhs_valueList;
	vector<AosDataRecordObj *> rcdList;
	AosDataRecordObj* rcd;
	//AosDataRecordObjPtr rcdContainer;
	//AosBuffDataPtr Data;
	//AosTaskObjPtr task = NULL;
	//AosXmlParser parser;

	/*
	//create recordcontainer
	strContainer = defineRecord(rdata);
	//for right data proc
	AosXmlTagPtr xmlContainer = parser.parse(strContainer.data(), "" AosMemoryCheckerArgs);

	rcdContainer = AosDataRecordObj::createDataRecordStatic(xmlContainer, task, rdata AosMemoryCheckerArgs);
	lhs_recordset = AosRecordsetObj::createRecordsetStatic(rdata, rcdContainer);
	aos_assert_r(lhs_recordset, false);
	//collect all the distinct keys from input record set
	*/
	
	OmnString prevKey = "";
	OmnString curKey = "";
	bool rslt = false;
	if(mIsStreaming)
	{
		rs->resetReadIdx();
	}
	while (1)
	{
		mLock->lock();
		rslt = rs->nextRecord(rdata, rcd);
		mLock->unlock();
		if(rslt && !rcd)
			break;
		//else
		//{
		//	OmnString strData = rcd->getData(rdata);
		//	if(strData.isNull())
		//		break;
		//}

		aos_assert_r(rcd, false);
		//get Record type
		//AosXmlTagPtr xmlRcd = rcd->getRecordDoc();
		//mRecordType = xmlRcd->getAttrStr("type");

		//get one key group
		curKey = getCombinedKeyValues(rcd, mLeftKeyList);
		AosDataRecordObjPtr rcdPtr = rcd;
		
		if (prevKey == "" || curKey == prevKey)
		{
			//lhs_recordset->appendRecord(rdata, rcdPtr, Data);
			rcdList.push_back(rcd);

			//set the prevKey value
			if (prevKey == "")
			{
				prevKey = curKey;
			}
		}
		else
		{
			//till here, we get a key group
			//need to join with the IIL
			//1. get the iil entries with the same key value

			getIILEntries(rdata, prevKey, rcdList);
			//clear the joined reecord from record list.
			rcdList.clear();
			
			//push the curKey's rcd to record list.
			rcdList.push_back(rcd);

			//set prevKey for next key group
			prevKey = curKey;

			//this approach will cause many recordset creation, not efficient.
			//Needs to be improved later
			//lhs_recordset = 0;
		}
	}

	if (prevKey != "")
	{
		//handle the last group
		//mRecordset = lhs_recordset;
		getIILEntries(rdata, prevKey, rcdList);

		//lhs_recordset = 0;
	}

	return true;
}

bool
AosDataProcIILJoin::getIILEntries(
		const AosRundataPtr &rdata,
		const OmnString &keyValue,
		vector<AosDataRecordObj *> &recordList)
{
	mLock->lock();
	
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
	query_context->setPageSize(1000);

	bool rslt = AosQueryColumn(mIILName, query_rslt, 0, query_context, rdata);
	aos_assert_r(rslt, false);

	bool finished = false;
	//bool outofmem = false;
	//int numDocs = query_context->getTotalDocInRslt();
	//int tmp = 1;
	OmnString vv;
	//u64 u64_vv = 0;
	u64 docid;
	OmnString str ,key, val, key_val;
	AosRecordsetObjPtr rhs_recordset;
	AosRecordsetObjPtr lhs_recordset;
	AosBuffDataPtr Data;
	vector<OmnString> values;

	while (query_rslt->nextDocidValue(docid, vv, finished, rdata))
	{
		//for the data isnull the leftjoin and the rightjoin are the same
		if(finished) break;
		//aos_assert_r(vv != "", false);
		values.push_back(vv.getBuffer());
	}

	vector<OmnString> fields;
	OmnString sep = "0x01";
	AosConvertAsciiBinary(sep);
	//const char *sep = "_";
	OmnString keys = "";
	//2. translate rhs_valueList to rhs_recordset
	
	//for create left and right record
	for (u32 i = 0; i < values.size(); i++)
	{
		//str = values[i];
		int num = AosSplitStr(values[i], sep.data(), fields, 20);
		aos_assert_r(num >=0, false);
	
		//if(!mJson["flag"].asBool())
		//{
			mJson["flag"] = true;
			//for left record
			//rslt = leftOutputRecord(rdata.getPtr(), recordList);
			//aos_assert_r(rslt, false);

			//for right record
			//rslt = rightOutputRecord(rdata.getPtr());
			//aos_assert_r(rslt, false);
			//OmnScreen << "this test create right left record  one time" << endl;
		//}
		fillRecordValue(rdata.getPtr(), recordList, fields);
	}
	mLock->unlock();

	return true;
}


OmnString
AosDataProcIILJoin::getCombinedKeyValues(
		AosDataRecordObj* &rcd,
		vector<OmnString> &keyList)
{
	OmnString str_key, key;
	mLock->lock();
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	for(u32 i=0; i<keyList.size(); i++)
	{
		key << rcd->getFieldValue(rdata.getPtr(), keyList[i]);

		if(i == (keyList.size()-1))
			str_key << key;
		else
			str_key << key << '\01';
	}
	mLock->unlock();
	return str_key;
}


bool
AosDataProcIILJoin::setOutputField(AosRundata* rdata, 
							AosXmlTagPtr xml, 
							OmnString &field_name, 
							boost::shared_ptr<Output> out_put)
{
	aos_assert_r(xml, false);
	AosXmlTagPtr dataFieldXml = xml->getFirstChild();          
	aos_assert_r(dataFieldXml, false);

	AosDataFieldType::E fieldType;
	OmnString name = "";
	OmnString fType = "";
	OmnString strLen = "";

	while(dataFieldXml)
	{
		name = dataFieldXml->getAttrStr("zky_name");
		if(field_name == name)
		{
			fType = dataFieldXml->getAttrStr("type"); 
			const OmnString cType = fType;
			fieldType = convertToDataFieldType(rdata, cType); 

			strLen = dataFieldXml->getAttrStr("zky_length");
			out_put->setField(name, fieldType, strLen.toInt64(0));
			break;
		}
		
		dataFieldXml = xml->getNextChild();
	}

	return true;
}

	
bool
AosDataProcIILJoin::fillRecordValue(
					AosRundata* rdata, 
					vector<AosDataRecordObj *> &record_list,
					vector<OmnString> &field_list)
{
	AosDataRecordObjPtr record = mOutputRecordMap[mOutput];
	record->clear();

	AosValueRslt value;
	bool outofmem = false;
	size_t i ,j;
	for(u32 k=0; k<record_list.size(); k++)
	{
		for(i=0; i<mLeftList.size(); i++)
		{
			record_list[k]->getFieldValue(mLeftList[i], value, outofmem, rdata);
			record->setFieldValue(i, value, outofmem, rdata);
		}

		for(j=0; j<mRightList.size(); j++)
		{
			int idx = record->getFieldIdx(mRightList[j], rdata);
			AosDataFieldObj *data_field = record->getFieldByIdx1(idx);
			AosDataType::E type = data_field->getDataType(rdata, record.getPtr());
			if(type == AosDataType::eDateTime)
			{
				AosDateTime date(field_list[j], "");
				if(date.isNotADateTime())
				{
					OmnAlarm << "Current DateTime Object is invalid" << enderr;
					return false;
				}	
				i64 time = date.to_time_t();
				value.setI64(time);
			}
			else
			{
				value.setStr(field_list[j]);
			}

			record->setFieldValue(j+i, value, outofmem, rdata);
		}
		addOutput(rdata, mOutput, record);
	}
	return true;
}

