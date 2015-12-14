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
// 		DATASET mydataset
// 		(
// 		    name: value,
// 		    name: value,
// 		    ...
// 		    name: value,
//		);
//
// Modification History:
// 2015/05/25 Created by Arvin
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcWordCount.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcWordCount_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcWordCount(version);
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


AosJimoLogicDataProcWordCount::AosJimoLogicDataProcWordCount(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcWordCount::~AosJimoLogicDataProcWordCount()
{
}


bool 
AosJimoLogicDataProcWordCount::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc wordcount <name>
	// 	(
	// 		inputs:[name],
	// 		keys:[key_field1,key_field2,...]
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "wordcount", rdata, false);
	mErrmsg << "dataproc wordcount ";
	// parse dataset name
	OmnString wordcount_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,wordcount_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << wordcount_name;
	// parse name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = wordcount_name;
	mNameValueList = name_value_list;
	// parse inputs
	rslt = jimo_parser->getParmArrayStr( rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	//parse keys
	rslt = jimo_parser->getParmArrayStr( rdata, "keys", name_value_list, mKeys);
	if(!rslt || mKeys.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"keys",mErrmsg);
		return false;
	}

	if(!checkNameValueList(rdata,mErrmsg,name_value_list))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcWordCount::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	bool rslt = createWordCountDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addWordCountTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool
AosJimoLogicDataProcWordCount::createWordCountDataProc(
							AosRundata *rdata,
							AosJimoProgObj *prog,
							OmnString &statement_str)
{
	OmnString wordcount_str = "create dataprocwordcount ";
	wordcount_str << mWordCount << " { "
			<< "\"type\":\"wordcount\","
			<< "\"keys\":[";
	for(size_t i = 0; i < mKeys.size(); i++)
	{
		if(i > 0)
			wordcount_str << ",";
		wordcount_str << "\""<< mKeys[i] << "\"";
	}
	wordcount_str << "]};";
 
	OmnScreen << wordcount_str << endl;
	statement_str << "\n" << wordcount_str;
	
	bool rslt = parseRun( rdata, wordcount_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJimoLogicDataProcWordCount::addWordCountTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString add_wordcount_str;
	OmnString taskname;
	taskname << "task_wordcount_"<< mDataProcName;
	add_wordcount_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_scan2\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mWordCount << "\","
			<< "\"inputs\":[";
	for(size_t i = 0; i < mInputs.size(); i++)
	{
		if(i > 0)
			add_wordcount_str << ",";
		add_wordcount_str << "\""<< mInputs[i] << "\"";
	}

	add_wordcount_str << "],\"outputs\":[\"" << mWordCount << "_output\""
					 << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks", add_wordcount_str);
	OmnScreen << add_wordcount_str << endl;
	return true;
}


bool
AosJimoLogicDataProcWordCount::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcWordCount*>(this);
	mWordCount << "_dp_" << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	OmnString output_name;
	output_name << mWordCount << "_output";
	mOutputNames.push_back(output_name);
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcWordCount::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcWordCount(*this);
}


