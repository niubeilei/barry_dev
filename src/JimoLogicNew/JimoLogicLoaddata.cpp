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
// Statement Syntax:
// create job job5
// {
// 	"tablename": "unicom",
// 		"subtablenames":["xxx","xxx","xxxx","xxx"],
// 	    "type": "loaddata",
// 	    "input": "unicom_dataset"
//  };
//  run job job5 {"threads": 1};
//
// Modification History:
// 2015/05/18 Created by Chen Ding
// 2015/05/19 Worked on by Levi
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicLoaddata.h"
#include "JQLStatement/JqlStmtTable.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"
#include "SEUtil/JqlTypes.h" 
#include "JQLStatement/JqlStatement.h"
#include "XmlUtil/XmlTag.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicLoaddata_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicLoaddata(version);
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


AosJimoLogicLoaddata::AosJimoLogicLoaddata(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicLoaddata::~AosJimoLogicLoaddata()
{
}


bool 
AosJimoLogicLoaddata::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	//load data <input dataset name> into <table name>;
	// create job job5
	// {
	// 	"tablename": "unicom",
	// 		"subtablenames":["xxx","xxx","xxxx","xxx"],
	// 	    "type": "loaddata",
	// 	    "input": "unicom_dataset"
	//  };
	//  run job job5 {"threads": 1};

	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "load", rdata, false);
	aos_assert_rr(keywords[1] == "data", rdata, false);
	mErrmsg = "load data ";
	OmnString dataset_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dataset_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataset_name;

	// parser name_value_list

	mDatasetName = dataset_name;
	OmnString key = jimo_parser->nextKeyword(rdata);
	if(key == "into" || key == "INTO")
	{
		//parse table name 
		OmnString tableName = jimo_parser->nextObjName(rdata);
		mTableName = tableName;
	}


	key = jimo_parser->nextKeyword(rdata);
	vector<OmnString> tableList;
	if(key == "subtablenames" || key == "SUBTABLENAMES")
	{
		jimo_parser->parseProcedureParms(rdata, tableList);
		mSubTableList = tableList;
	}

	key = jimo_parser->nextKeyword(rdata);
	if(key == "thread" || key == "THREAD")
	{
		OmnString thread = jimo_parser->nextObjName(rdata);
		mThread = thread;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicLoaddata::run(
		AosRundata *rdata, 
		AosJimoProgObj* prog,
		//const OmnString &verb_name,
		OmnString &statements_str,
		bool inparser) 
{
	bool rslt = createLoaddataDataproc(rdata, prog, statements_str);
	aos_assert_r(rslt, false);

	return rslt;
}


bool
AosJimoLogicLoaddata::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	aos_assert_rr(prog, rdata, false);
   //AosJimoLogicLoaddataPtr jimologic = dynamic_cast<AosJqlStatementPtr>(this);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicLoaddata*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDatasetName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicLoaddata::createLoaddataDataproc(
		AosRundata *rdata,
		AosJimoProgObj* jimo_prog,
		OmnString &statements_str)
{
	OmnString jobCreate_str = "create job ";
	jobCreate_str<< "job_" << mDatasetName << " { "
		<< "\"type\":" << "\"" << "loaddata" << "\","
		<< "\"tablename\":" << "\"" << mTableName << "\","
		<< "\"input\":" << "\"" << mDatasetName << "\"";

	if(mSubTableList.size()>0)
	{
		jobCreate_str << "," << "\"subtablenames\" : [";
		for(u32 i=0; i<mSubTableList.size(); i++)
		{
			if(i == (mSubTableList.size()-1))
				jobCreate_str << mSubTableList[i] << "]";
			else
				jobCreate_str << mSubTableList[i] << ",";
		}
	}
	jobCreate_str << "} ;";

    OmnString jobRun_str = "run job ";
	jobRun_str << "job_" << mDatasetName;

	if(mThread != "")
	{
		jobRun_str << " ( threads :" << mThread;
	}
	else
	{
		jobRun_str << " ( threads : 1";
	}
	jobRun_str << ") ;";

	bool rslt = parseRun(rdata, jobCreate_str, jimo_prog);
	aos_assert_rr(rslt, rdata, false);
	rslt = parseRun(rdata, jobRun_str, jimo_prog);
	aos_assert_rr(rslt, rdata, false);
	
	return true;
}


AosJimoPtr 
AosJimoLogicLoaddata::cloneJimo() const
{
	return OmnNew AosJimoLogicLoaddata(*this);
}


//this is for JimoLogicNew::getDatasetOutput
vector<OmnString> 
AosJimoLogicLoaddata::getOutputNames()
{
	mOutputNames.push_back(mDatasetName);
	return mOutputNames;
}

bool 
AosJimoLogicLoaddata::getSchemaName(
				AosRundata *rdata, 
				AosJimoProgObj *prog,
				vector<OmnString> &schema_name)
{
	schema_name.push_back(mSchemaName);
	return true;
}
