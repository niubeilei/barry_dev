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
// 2015/12/07 Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcTransTorturer.h"
#include "JQLStatement/JqlStatement.h"
#include "SEUtil/JqlTypes.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcTransTorturer_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcTransTorturer(version);
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


AosJimoLogicDataProcTransTorturer::AosJimoLogicDataProcTransTorturer(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
	mIsService = false;
}


AosJimoLogicDataProcTransTorturer::~AosJimoLogicDataProcTransTorturer()
{
}


bool 
AosJimoLogicDataProcTransTorturer::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc trans_torturer name
	// 	(
	//		inputs: input_name, ddd, ddd, ddd,
	//		hours: xxx,
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "trans_torturer", rdata, false);
	mErrmsg = "dataproc trans_torturer";
	mIsService = false;

	// parse dataset name
	mDPName = jimo_parser->nextObjName(rdata);
	if( !isLegalName(rdata, mDPName, mErrmsg) ) return false;
	mErrmsg << mDPName;

	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	// parse hours 
	mHours = jimo_parser->getParmStr(rdata, "hours", name_value_list);
	if (mHours == "")
	{
		setErrMsg(rdata, eMissingParm, "hours", mErrmsg);
		return false;
	}

	// parse inputs
	// hardcode, Now only 1 input
	mInputs = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	if (mInputs == "")
	{
		setErrMsg(rdata, eMissingParm, "inputs", mErrmsg);
		return false;
	}

	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcTransTorturer::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	aos_assert_rr(prog, rdata, false);
	//proc inputs
	
	aos_assert_rr(prog, rdata, false);
	if (mInputs != "")
	{
		bool rslt = AosJimoLogicNew::procInput(rdata, mInputs, prog, mInputName, statement_str);
		if (!rslt)
		{
			OmnString msg = "dataproc trans_torturer";
			msg << mDataProcName << " missing input : " << mInputs << ".";
			rdata->setJqlMsg(msg);
			return false;
		}

		if (mInputName == "")
		{
			AosLogError(rdata, true, "failed_retrieving_output_name")
				<< AosFN("Input Name") << mInputs << enderr;
			return false;
		}
	}

	bool rslt = createJSON(rdata, prog, statement_str);
	aos_assert_r(rslt, false);
	rslt = createTask(rdata, prog);
	aos_assert_rr(rslt, rdata, prog);
	return true;
}


bool
AosJimoLogicDataProcTransTorturer::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcTransTorturer*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcTransTorturer::createJSON(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	// It constructs the JSON statement:
	// 	create dataprocsyncher
	// 	{
	// 		"tablename":xxx,
	// 		"dbname":xxx,
	// 		"operation":xxx
	// 	};
	OmnString conf = "create dataproc_trans_torturer ";
	conf << mDPName << " ";

	JSONValue json;
	json["hours"] = mHours.data();
	conf << json.toStyledString() << ";";

	statement_str << "\n" << conf;
    
	bool rslt = parseRun(rdata, conf, jimo_prog);
    aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcTransTorturer::createTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	OmnString str;
	OmnString taskname;
	taskname << "task_proc_trans_torturer_" << mDPName;
	str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDPName << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"]";
	str <<"}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks", str);
		jimo_prog->addTask(rdata, str, "");
	}

	OmnScreen << str << endl;
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcTransTorturer::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcTransTorturer(*this);
}


OmnString 
AosJimoLogicDataProcTransTorturer::getInput() const
{
	return mInputs;
}

