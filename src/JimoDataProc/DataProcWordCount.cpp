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
// 03/04/2015 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoDataProc/DataProcWordCount.h"

#include "API/AosApi.h"
#include "JSON/JSON.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosDataProcWordCount_0(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosDataProcWordCount(version);
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


AosDataProcWordCount::AosDataProcWordCount(const int version)
:
AosJimoDataProc(version, AosJimoType::eDataProcWordCount)
{
}


AosDataProcWordCount::~AosDataProcWordCount()
{
}


bool
AosDataProcWordCount::config(
		const AosXmlTagPtr &def,
		const AosRundataPtr &rdata)
{
	//<dataproc zky_name = "dp2"><![CDATA[
	//{
	//  "type": "wordcount",
	//  "keys": ["abc", "dsf"],
	//};
	
	mName = def->getAttrStr("zky_name", "");
	aos_assert_r(mName != "", false);
	OmnString dp_json = def->getNodeText();
	JSONValue json;
	JSONReader reader;
	reader.parse(dp_json, json);
	OmnScreen << "dataproc config json is: " << dp_json << endl;

	//keys
	JSONValue keys = json["keys"];
	for(size_t i = 0; i < keys.size(); i++)
	{
		OmnString key = keys[i].asString();
		mKeys.push_back(key);
	}
	return true;
}


AosDataProcStatus::E
AosDataProcWordCount::procData(
		AosRundata *rdata_raw,
		AosDataRecordObj **input_records,
		AosDataRecordObj **output_records)
{
	AosValueRslt value_rslt;
	AosWordParser parser;
	OmnString word = "";
	char *doc = NULL;
	int rcd_len = 0;
	int count = 0;
	map<OmnString, int>::iterator itr = mWordMap.end();
	AosDataRecordObj* input_record = input_records[0];

	rcd_len = input_record->getRecordLen();
	doc = input_record->getData(rdata_raw);
	OmnString keywords(doc, rcd_len);

	parser.setSrc(keywords);
	while(parser.nextWord(word))
	{
		itr = mWordMap.find(word);
		if (itr == mWordMap.end())
			mWordMap[word] = 0;
		else
		{
			count = itr->second;
			mWordMap[word] = count + 1;
		}
	}

	return AosDataProcStatus::eContinue;
}


AosJimoPtr 
AosDataProcWordCount::cloneJimo() const
{
	return  OmnNew AosDataProcWordCount(*this);
}


AosDataProcObjPtr
AosDataProcWordCount::cloneProc() 
{
	AosDataProcObjPtr thisptr(this,true);
	return thisptr;
}


bool
AosDataProcWordCount::getRecordFieldInfosRef(vector<RecordFieldInfo *> &rf_infos)
{
	return true;
}


bool
AosDataProcWordCount::finish(
		const vector<AosDataProcObjPtr> &procs,
		const AosRundataPtr &rdata)
{
	map<OmnString, int>::iterator itr;

	for (size_t i = 0; i < mKeys.size(); i++)
	{
		int count = 0;
		itr = mWordMap.find(mKeys[i]);
		if (itr != mWordMap.end()) count = itr->second;
OmnScreen << mKeys[i] << ":" << count << endl;
	}
	return true;
}


bool 
AosDataProcWordCount::createByJql(
		AosRundata *rdata,
		const OmnString &obj_name,
		const OmnString &jsonstr,
		const AosJimoProgObjPtr &prog)
{
	try
	{
		JSONReader reader;
		JSONValue jsonObj;
		reader.parse(jsonstr, jsonObj);

		string cdata = jsonstr;

		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eDataProcDoc, obj_name);
		OmnString dpconf = "";	
		dpconf 
			<< "<jimodataproc " << AOSTAG_CTNR_PUBLIC "=\"true\" " << AOSTAG_PUBLIC_DOC "=\"true\" "
			<< 	AOSTAG_OBJID << "=\"" << objid << "\">"
			<< 		"<dataproc zky_name=\"" << obj_name << "\" jimo_objid=\"DataProcWordCount_jimodoc_v0\">"
			<< 			"<![CDATA[" << cdata << "]]>"
			<< 		"</dataproc>"
			<< "</jimodataproc>";
		//return AosCreateDoc(dpconf, true, rdata);
		
		prog->saveLogicDoc(rdata, objid, dpconf);
		return true;
	}
	catch (std::exception &e)
	{
		OmnScreen << "JSONException: " << e.what() << "\n" << jsonstr << endl;
		return false;
	}

	return false;
}
