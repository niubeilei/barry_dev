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
#if 0
#include "JimoLogicNew/JimoLogicDataProcJoin.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcJoin_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcJoin(version);
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


AosJimoLogicDataProcJoin::AosJimoLogicDataProcJoin(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcJoin::~AosJimoLogicDataProcJoin()
{
}


bool 
AosJimoLogicDataProcJoin::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		bool &parsed, 
		AosJqlStatementPtr &statement)
{
	// The statement is in the form:
	// 	data proc join <name>
	// 	(
	// 		inputs:[input1,input2..],
	//		left_condition_fields:[dp_t_owner_select_output.OWNER_ID],
	//		right_condition_fields:[union_output.OWNERNO]
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "join", rdata, false);

	// parse dataset name
	OmnString join_name = jimo_parser->nextObjName(rdata);
	if (join_name == "")
	{
		// This is not what it expects.
		return true;
	}
	// parse name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		return true;
	}

	AosJimoLogicDataProcJoinPtr stmt = OmnNew AosJimoLogicDataProcJoin(*this);
	stmt->mDataProcName = join_name;
	stmt->mNameValueList = name_value_list;
	// parse inputs
	rslt = jimo_parser->getParmArrayStr( rdata, "inputs", name_value_list, stmt->mInputs);
	if (!rslt || stmt->mInputs.size() <= 0)
	{
		AosLogError(rdata, true, "missing_inputs") << enderr;
		return false;
	}

	//parse left_condition_fields
	rslt = jimo_parser->getParmArrayStr( rdata, "left_condition_fields", name_value_list, stmt->mLeftCondFields);
	if (!rslt || stmt->mLeftCondFields.size() <= 0)
	{
		AosLogError(rdata, true, "missing_left_condition_fields") << enderr;
		return false;
	}

	//parse right_condition_fields
	rslt = jimo_parser->getParmArrayStr( rdata, "right_condition_fields", name_value_list, stmt->mRightCondFields);
	if (!rslt || stmt->mRightCondFields.size() <= 0)
	{
		AosLogError(rdata, true, "missing_right_condition_fields") << enderr;
		return false;
	}

	statement = stmt;
	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcJoin::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str) 
{
	bool rslt = createJoinDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addJoinTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcJoin::createJoinDataProc(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	OmnString join_str = "create dataprocjoin ";
	join_str << mJoin << " { "
			<< "\"type\":\"join\","
			<< "\"left_condition_fields\":[" ;
	for(size_t i = 0;i < mLeftCondFields.size();i++)
	{
		if(i > 0)
			join_str << ",";
		join_str << "\"" << mLeftCondFields[i] << "\"";
	}
	join_str <<"],\"right_condition_fields\":[";
	for(size_t i = 0;i < mRightCondFields.size();i++)
	{
		if(i > 0)
			join_str << ",";
		join_str << "\"" << mRightCondFields[i] << "\"";
	}
	join_str << "]};";

	OmnScreen << join_str << endl;
	statement_str << "\n" << join_str;

	bool rslt = parseRun( rdata, join_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJimoLogicDataProcJoin::addJoinTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString join_str;
	OmnString taskname;
	vector<OmnString> outputNames;	
	bool rslt = getOutputsFromDataProc(rdata, prog, mInputs, outputNames);
	aos_assert_rr(rslt ,rdata ,false);

	taskname << "task_join_"<< mDataProcName;
	join_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_join2\","
			<< "\"dataset_splitter\":{\"type\":\"file_join\"},"
			<< "\"join_type\":\"left\","
			<< "\"model_type\":\"simple\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mJoin << "\","
			<< "\"inputs\":" << "[";
	for(size_t i = 0;i < outputNames.size();i++)
	{
		if(i > 0)
			join_str << ",";
		join_str << "\"" << outputNames[i] << "\"";
	}
	join_str << "],";
	join_str << "\"outputs\":[\"" << mJoin << "_output\"";
	join_str << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks", join_str);
	OmnScreen << join_str << endl;
	return true;
}


bool
AosJimoLogicDataProcJoin::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcJoin*>(this);
	mJoin = "_dp_"; 
	mJoin << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	OmnString output_name;
	output_name << mJoin << "_output";
	mOutputNames.push_back(output_name);
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcJoin::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcJoin(*this);
}
#endif
