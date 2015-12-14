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
// 2015/06/10 Created by Barry Niu
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcSelectAverage.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcSelectAverage_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcSelectAverage(version);
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


AosJimoLogicDataProcSelectAverage::AosJimoLogicDataProcSelectAverage(const int version)
:
AosJimoLogicDataProc(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDataProcSelectAverage::~AosJimoLogicDataProcSelectAverage()
{
}


bool 
AosJimoLogicDataProcSelectAverage::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc selectaverage datasetname
	// 	(
	// 		inputs:[name, name],
	// 		group_by:[name, name],
	// 		fields:[name,....]
	// 	);
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataproc", rdata, false);
	aos_assert_rr(keywords[1] == "selectaverage", rdata, false);
	mErrmsg = "dataproc selectaverage ";

	// parse dataset name
	OmnString dp_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dp_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dp_name;
	// parser name_value_list
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

	// parse input
	mInput = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	if (mInput == "")
	{
		setErrMsg(rdata, eMissingParm, "inputs",mErrmsg);
		return false;
	}

	//parse groupby
	rslt = jimo_parser->getParmArrayStr(rdata, "group_by", name_value_list, mGroupBys);
	if (!rslt && mGroupBys.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "group_by",mErrmsg);
		return false;
	}

	//parse field
	rslt = jimo_parser->getParmArrayStr(rdata, "fields", name_value_list, mFields);
	if (!rslt && mFields.size() <= 0)
	{
		setErrMsg(rdata, eMissingParm, "fields",mErrmsg);
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
AosJimoLogicDataProcSelectAverage::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	//01:
	//dataproc select xxxx
	//(
	//	inputs:"xxx",
	//	fields:[
	//			(
	//			fname:field1
	//			)
	//			....
	//			(
	//			alias:_dp_select01_count,
	//			type:u64,
	//			fname:1
	//			)
	//			]
	//);
	//
	//02:
	//dataproc select xxxx
	//(
	//	inputs:select01,
	//	fields:[
	//			(
	//			alias:sum_field1
	//			fname:sum(field1)
	//			),
	//			....
	//			(
	//			alias:sum__dp_select01_count
	//			fname:sum(_dp_select01_count)
	//			)
	//			],
	//	group_by:[f1,.....]
	//);
	//
	//03:
	//dataproc select xxxxx
	//(
	//	inputs:select02,
	//	fields:[
	//			(
	//			alias:_dp_select_avg
	//			fname:_dp_select02_sum/_dp_select02_count
	//			),
	//			....
	//			)
	//			]
	//);

	OmnString dp_name = "";
	bool rslt = createSelectDataprocAddCount(rdata, prog, dp_name, statements_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = createSelectDataprocSumCount(rdata, prog, dp_name, statements_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = createSelectDataprocAverage(rdata, prog, dp_name, statements_str);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcSelectAverage::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	aos_assert_rr(prog, rdata, false);
   //AosJimoLogicDataProcSelectAveragePtr jimologic = dynamic_cast<AosJqlStatementPtr>(this);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSelectAverage*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

//	OmnString outputName = mDataProcName;
//	outputName << "_output";
//	mOutputNames.push_back(outputName);
	return true;
}


bool
AosJimoLogicDataProcSelectAverage::createSelectDataprocAddCount(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &dp_name,
		OmnString &statements_str)
{
	//dataproc select xxxxx
	//(
	//	inputs:xxx,
	//	fields:[
	//			(
	//			fname:field1
	//			)
	//			....
	//			(
	//			alias:_dp_select01_count,
	//			type:u64,
	//			fname:1
	//			)
	//			]
	//);

	OmnString str = "dataproc select ";
	dp_name = "_dp_";
	dp_name << mDataProcName << "_addcount";
	str << dp_name;
	str << " ( inputs:\"" << mInput << "\"";
	str << ", fields:[";
	for (size_t i = 0; i < mFields.size(); i++)
	{
		if (i>0)
			str << ", ";
		str << " ( fname:" << mFields[i] << " )";
	}
	for (size_t i = 0; i < mGroupBys.size(); i++)
	{
		str << ", ( fname:" << mGroupBys[i] << " )";
	}
	mNumName = "_dp_";
	mNumName << mDataProcName << "_num";
	str << ", ( alias:" << mNumName << ", type:u64, fname: 1" << " ) ] ) ";

	OmnScreen << str << endl;
	statements_str << "\n" << str;

    bool rslt = parseRun(rdata, str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


bool
AosJimoLogicDataProcSelectAverage::parseRun(
		AosRundata* rdata,
		OmnString &stmt,
		AosJimoProgObj *jimo_prog)
{
	AosJimoParserObjPtr jimo_parser = Jimo::jimoCreateJimoParser(rdata);
	aos_assert_rr(jimo_parser, rdata, false);

	vector<AosJqlStatementPtr> statements;
	bool rslt = jimo_parser->parse(rdata, jimo_prog, stmt, statements);
	aos_assert_rr(rslt, rdata, false);
	
	if (statements.size() <= 0)
	{
		AosLogError(rdata, true, "failed_parsing_statement")
			<< AosFN("Statement") << stmt << enderr;
		return false;
	}

	OmnString ss;
	for(size_t i = 0;i < statements.size();i++)
	{
		statements[i]->compileJQL(rdata, jimo_prog);
		statements[i]->setContentFormat(OmnString("print"));
		statements[i]->run(rdata, jimo_prog, ss, true);
	}
	return true;
}


bool
AosJimoLogicDataProcSelectAverage::createSelectDataprocSumCount(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &dp_name,
		OmnString &statements_str)
{
	//data proc select02
	//(
	//	inputs:select01,
	//	fields:[
	//			(
	//			alias:_dp_select02_sum
	//			fname:sum(field1)
	//			),
	//			....
	//			(
	//			alias:_dp_select02_count
	//			fname:sum(_dp_select02_sum)
	//			)
	//			],
	//	group_by:[f1,.....]
	//);

	OmnString input = dp_name;

	OmnString str = "dataproc select ";
	dp_name = "_dp_";
	dp_name << mDataProcName << "_sum_count";
	str << dp_name;

	str << " ( inputs:" << input << ", fields:[";
	for (size_t i = 0; i < mFields.size(); i++)
	{
		OmnString fname = "\"sum(";
		fname << mFields[i] << ")\"";
		OmnString alias = "sum_";
		alias << mFields[i];
		mSumNames.push_back(alias);
		if (i>0)
			str << ", ";
		str << " ( fname:" << fname << ", alias:" << alias << " )";
	}
	OmnString alias = "sum_";
	alias << mNumName;
	mSumNames.push_back(alias);
	str << ", ( fname:\"sum(" << mNumName << ")\", alias:" << alias << " ) ]";
	str << ", group_by:[";
	for (size_t i = 0; i < mGroupBys.size(); i++)
	{
		if (i>0)
			str << ", ";
		str << mGroupBys[i];
	}
	str << "])";

	OmnScreen << str << endl;
	statements_str << "\n" << str;

    bool rslt = parseRun(rdata, str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


bool
AosJimoLogicDataProcSelectAverage::createSelectDataprocAverage(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &dp_name,
		OmnString &statements_str)
{
	//data proc select03
	//(
	//	inputs:select02,
	//	fields:[
	//			(
	//			alias:avg_field1,
	//			fname:sum_field1/sum__dp_select01_count,
	//			type:u64
	//			),
	//			....
	//			]
	//);
	aos_assert_r(mFields.size() == mSumNames.size() -1, false);
	int n = mSumNames.size() - 1;

	OmnString str = "dataproc select ";
	str << mDataProcName;
	str << " ( inputs:" << dp_name << ", fields:[";
	for (size_t i = 0; i < mFields.size(); i++)
	{
		OmnString alias = "avg_";
		alias << mFields[i];

		OmnString fname = mSumNames[i];
		fname << "/" << mSumNames[n];

		if (i>0)
			str << ", ";
		str << " ( fname:" << fname << ", alias:" << alias << ", type:u64 )";
	}
	for (size_t i = 0; i < mGroupBys.size(); i++)
	{
		str << ", (fname:" << mGroupBys[i] << ")";
	}
	str << "])";

	OmnScreen << str << endl;
	statements_str << "\n" << str;

    bool rslt = parseRun(rdata, str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcSelectAverage::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcSelectAverage(*this);
}
