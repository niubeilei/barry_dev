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
#include "JimoLogicNew/JimoLogicDataProcRankOver.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcRankOver_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcRankOver(version);
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


AosJimoLogicDataProcRankOver::AosJimoLogicDataProcRankOver(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcRankOver::~AosJimoLogicDataProcRankOver()
{
}


bool 
AosJimoLogicDataProcRankOver::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc rankover <name>
	// 	(
	// 		inputs:[name,name..],
	//		record:buff,					//or fixbin
	// 		order_by:[key_field1],
	// 		name:aa
	// 		partition_by:[key_field1,key_field2,...]
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "rankover", rdata, false);
	mErrmsg = "dataproc rankover ";

	// parse dataset name
	OmnString rankover_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,rankover_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << rankover_name;

	// parse name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata, eGenericError, "",mErrmsg);
		return false;
	}

	mDataProcName = rankover_name;
	mNameValueList = name_value_list;
	// parse inputs
	rslt = jimo_parser->getParmArrayStr( rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "inputs",mErrmsg);
		return false;
	}

	//parse order_by
	rslt = jimo_parser->getParmArrayStr( rdata, "order_by", name_value_list, mOrderBy);
	if(!rslt || mOrderBy.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "order_by",mErrmsg);
		return false;
	}

	//parse name
	mName = jimo_parser->getParmStr( rdata, "name", name_value_list);
	if(mName == "")
	{
		setErrMsg(rdata, eMissingParm, "name",mErrmsg);
		return false;
	}

	//parse partition_by
	rslt = jimo_parser->getParmArrayStr( rdata, "partition_by",name_value_list, mPartitionBy);
	if(!rslt || mPartitionBy.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "partition_by",mErrmsg);
		return false;
	}

	//parse record_type	,if has null,it will take buff
	mRecordType = jimo_parser->getParmStr( rdata, "record_type", name_value_list);
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcRankOver::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser) 
{
	bool rslt = createRankOverDataProc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addRankOverTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcRankOver::createRankOverDataProc(
		AosRundata *rdata,
		AosJimoProgObj *prog, 
		OmnString &statements_str)
{
	OmnString rankover_str = "create dataprocrankover ";
	rankover_str << mRankOver << " { "
			<< "\"type\":\"rankover\","
			<< "\"name\":\"" << mName << "\","
			<< "\"record_type\":"; 
	if(mRecordType == "")
		rankover_str << "\"buff\",";
	else
		rankover_str << "\"" << mRecordType<<"\",";
	rankover_str << "\"order_by\":[";
	for(size_t i = 0; i < mOrderBy.size(); i++)
	{
		if(i > 0)
			rankover_str << ",";
		rankover_str << "\""<< mOrderBy[i] << "\"";
	}
	rankover_str << "],\"partition_by\":[";
	for(size_t i = 0; i < mPartitionBy.size(); i++)
	{
		if(i > 0)
			rankover_str << ",";
		rankover_str << "\""<< mPartitionBy[i] << "\"";
	}
	rankover_str << "]};";

	OmnScreen << rankover_str << endl;
	statements_str << "\n" << rankover_str;

	bool rslt = parseRun( rdata, rankover_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJimoLogicDataProcRankOver::addRankOverTask(
		AosRundata *rdata, 
		AosJimoProgObj *prog)
{
	OmnString add_rankover_str;
	OmnString taskname;
	taskname << "task_rankover_"<< mDataProcName;
	add_rankover_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_scan2\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mRankOver << "\","
			<< "\"inputs\":[\"" << mInputs[0] << "\"],";
	add_rankover_str << "\"outputs\":[\"" << mRankOver << "_output\""
					 << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks", add_rankover_str);
	OmnScreen << add_rankover_str << endl;
	return true;
}


bool
AosJimoLogicDataProcRankOver::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcRankOver*>(this);
	mRankOver = "_dp_";
	mRankOver << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mRankOver, jimologic);
	aos_assert_rr(rslt, rdata, false);
	OmnString output_name;
	output_name ="";
	output_name << mRankOver << "_output";
	mOutputNames.push_back(output_name);
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcRankOver::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcRankOver(*this);
}

