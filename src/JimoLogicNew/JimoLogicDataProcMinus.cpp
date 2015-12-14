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
#include "JimoLogicNew/JimoLogicDataProcMinus.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcMinus_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcMinus(version);
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


AosJimoLogicDataProcMinus::AosJimoLogicDataProcMinus(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcMinus::~AosJimoLogicDataProcMinus()
{
}


bool 
AosJimoLogicDataProcMinus::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc minus <name>
	// 	(
	// 		inputs:[<name>,[name]..]
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "minus", rdata, false);
	mErrmsg = "dataproc minus ";

	// parse dataset name
	OmnString minus_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata, minus_name, mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << minus_name;
	// parse name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = minus_name;
	mNameValueList = name_value_list;
	
	// parse inputs
	rslt = jimo_parser->getParmArrayStr( rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
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
AosJimoLogicDataProcMinus::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{


	bool rslt = createMinusDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);
	
	rslt = addMinusTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}

bool
AosJimoLogicDataProcMinus::createMinusDataProc(
							AosRundata *rdata,
							AosJimoProgObj *prog,
							OmnString &statement_str)
{
	OmnString minus_str = "create dataprocminus ";
	minus_str << mMinus << " { "
			<< "\"type\":\"" << mKeywords[1] << "\""
			<< "};";
	OmnScreen << minus_str << endl;
	statement_str << "\n" << minus_str;

	bool rslt = parseRun( rdata, minus_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJimoLogicDataProcMinus::addMinusTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{

	OmnString minus_str;
	OmnString taskname;
	vector<OmnString> outputNames;	
	bool rslt = getOutputsFromDataProc(rdata, prog, mInputs, outputNames);
	aos_assert_rr(rslt ,rdata ,false);

	taskname << "task_minus_"<< mDataProcName;
	minus_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_join2\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mMinus << "\","
			<< "\"inputs\":" << "[";
	for(size_t i = 0;i < outputNames.size();i++)
	{
		if(i > 0)
			minus_str << ",";
		minus_str << "\"" << outputNames[i] << "\"";
	}
	minus_str << "],";
	minus_str << "\"outputs\":[\"" << mMinus << "_output\"";
	minus_str << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks", minus_str);
	OmnScreen << minus_str << endl;
	return true;
}


bool
AosJimoLogicDataProcMinus::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcMinus*>(this);
	mMinus = "_dp_"; 
	mMinus << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	OmnString output_name;
	output_name ="";
	output_name << mMinus << "_output";
	mOutputNames.push_back(output_name);
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcMinus::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcMinus(*this);
}



