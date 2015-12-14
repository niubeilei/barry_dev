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
// 2015/05/29 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcMerge.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcMerge_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcMerge(version);
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


AosJimoLogicDataProcMerge::AosJimoLogicDataProcMerge(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcMerge::~AosJimoLogicDataProcMerge()
{
}


bool 
AosJimoLogicDataProcMerge::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc merge <name>
	// 	(
	// 		inputs:[<name>,[name]..]	//two
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "merge", rdata, false);
	mErrmsg = "dataproc merge ";

	// parse dataset name
	OmnString dp_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata ,dp_name, mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dp_name;

	// parse name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata, eGenericError, "",mErrmsg);
		return false;
	}

	mDataProcName = dp_name;
	mNameValueList = name_value_list;

	// parse inputs
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() != 2)
	{
		setErrMsg(rdata, eMissingParm, "inputs",mErrmsg);
		return false;
	}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcMerge::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	bool rslt = createDataProcMerge(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addMergeTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool
AosJimoLogicDataProcMerge::createDataProcMerge(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	mNewDataProcName << "_dp_" << mDataProcName;

	OmnString merge_str = "create dataprocmerge ";
	merge_str << mNewDataProcName << " { "
			<< "\"type\":\"" << mKeywords[1] << "\""
			<< "};";
	OmnScreen << merge_str << endl;
	statement_str << "\n" << merge_str;

	bool rslt = parseRun( rdata, merge_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJimoLogicDataProcMerge::addMergeTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString taskname;
	taskname << "task_merge"<< mNewDataProcName;

	OmnString merge_str;
	merge_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_join2\","
			<< "\"model_type\":" << "\"simple\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mNewDataProcName << "\","
			<< "\"inputs\":" << "[";
	for(size_t i = 0;i < mInputs.size();i++)
	{
		if(i > 0)
			merge_str << ",";
		merge_str << "\"" << mInputs[i] << "\"";
	}
	merge_str << "],";
	merge_str << "\"outputs\":[\"" << mNewDataProcName  << "_output\"";
	merge_str << "]}]}";

	if(prog)
		prog->appendStatement(rdata, "tasks", merge_str);
	OmnScreen << merge_str << endl;

	return true;
}


bool
AosJimoLogicDataProcMerge::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcMerge*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


AosJimoPtr 
AosJimoLogicDataProcMerge::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcMerge(*this);
}

