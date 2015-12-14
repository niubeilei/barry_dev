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
// 2015/12/13 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcOnNetOffNet.h"
#include "JQLStatement/JqlStatement.h"
#include "SEUtil/JqlTypes.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcOnNetOffNet_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcOnNetOffNet(version);
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


AosJimoLogicDataProcOnNetOffNet::AosJimoLogicDataProcOnNetOffNet(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcOnNetOffNet::~AosJimoLogicDataProcOnNetOffNet()
{
}


bool 
AosJimoLogicDataProcOnNetOffNet::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc onnetoffnet data_proc_name
	// 	(
	//		 inputs:"datasetiil01",
	//		 user_id_field:"msisdn",
	//		 time_field:"start_datetime",
	//		 time_unit:"%Y-%m-%d",
	//		 shreshold:90,
	//		 start_day:"2012-07-03",
	//		 end_day:"2012-07-05"
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "onnetoffnet", rdata, false);
	mErrmsg = "dataproc onnetoffnet ";

	OmnString dataproc_name = jimo_parser->nextObjName(rdata);
	if(!isLegalName(rdata, dataproc_name, mErrmsg))
		return false;
	mErrmsg << dataproc_name;

	// parser mNameValueList
	bool rslt = jimo_parser->getNameValueList(rdata, mNameValueList);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}
	mDataProcName = dataproc_name;

	mInput = jimo_parser->getParmStr(rdata, "inputs", mNameValueList);
	if(mInput == "")
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	mUserIdField = jimo_parser->getParmStr(rdata, "user_id_field", mNameValueList);
	if(mUserIdField == "")
	{
		setErrMsg(rdata,eMissingParm,"user_id_field",mErrmsg);
		return false;
	}

	mTimeField = jimo_parser->getParmStr(rdata, "time_field", mNameValueList);
	if(mTimeField == "")
	{
		setErrMsg(rdata,eMissingParm,"user_id_field",mErrmsg);
		return false;
	}

	mTimeUnit = jimo_parser->getParmStr(rdata, "time_unit", mNameValueList);
	if(mTimeUnit == "")
	{
		mTimeUnit = "%Y-%m-%d";
	}

	mShreshold = jimo_parser->getParmInt(rdata, "shreshold", mNameValueList);
	if(mShreshold <= 0)
	{
		mShreshold = 90;
	}
	
	mStartDay = jimo_parser->getParmStr(rdata, "start_day", mNameValueList);
	if(mStartDay == "")
	{
		setErrMsg(rdata,eMissingParm,"start_day",mErrmsg);
		return false;
	}
	
	mEndDay = jimo_parser->getParmStr(rdata, "end_day", mNameValueList);
	if(mEndDay == "")
	{
		setErrMsg(rdata, eMissingParm, "end_day", mErrmsg);
		return false;
	}

	if(!checkNameValueList(rdata, mErrmsg, mNameValueList))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcOnNetOffNet::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	aos_assert_rr(prog, rdata, false);
	bool rslt = procInput(rdata, mInput, prog, mInputName, statement_str);
	aos_assert_r(rslt, false);

	if (mInputName == "")
	{
		//for load data
		mInputName = mInput;
	}

	rslt = createDataproc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, prog);

	rslt = addTask(rdata, prog);
	aos_assert_rr(rslt, rdata, prog);
	return true;
}


bool
AosJimoLogicDataProcOnNetOffNet::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcOnNetOffNet*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	mJobName = prog->getJobname();
	mNewDataProcName << "_dp_" << mJobName << "_" << mDataProcName;

	OmnString output_name;
	output_name << mNewDataProcName << "_onnet_output";
	mOutputNames.push_back(output_name);
	output_name = "";
	output_name << mNewDataProcName << "_offnet_output";
	mOutputNames.push_back(output_name);
	return true;
}


bool
AosJimoLogicDataProcOnNetOffNet::createDataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog,
		OmnString &statement_str)
{
	OmnString dp_str = "create dataproconnetoffnet ";
	dp_str << mNewDataProcName << " ";

	JSONValue json;
	json["type"]= "onnetoffnet";
	json["record_type"]= "buff";
	json["user_id_field"] = mUserIdField.data();
	json["time_field"] = mTimeField.data();
	json["time_unit"] = mTimeUnit.data();
	json["shreshold"] = (unsigned int)mShreshold;
	json["start_day"] = mStartDay.data();
	json["end_day"] = mEndDay.data();
	dp_str << json.toStyledString();
	dp_str << ";";

	OmnScreen << dp_str << endl;
	statement_str << "\n" << dp_str;
    
	bool rslt = parseRun(rdata, dp_str, jimo_prog);
    aos_assert_rr(rslt, rdata, false);
	return true;
}


bool
AosJimoLogicDataProcOnNetOffNet::addTask(
		AosRundata *rdata, 
		AosJimoProgObj *prog)
{
	OmnString task_str;
	OmnString taskname;
	taskname << "task_onnetoffnet_" << mNewDataProcName;
	task_str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mNewDataProcName << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"], "
		<< "\"outputs\":" << "[\"" << mNewDataProcName << "_onnet_output"<< "\", "
		<< "\"" << mNewDataProcName << "_offnet_output"<< "\"";
	task_str <<"]}]}";

	if(prog)
	{
		prog->appendStatement(rdata, "tasks", task_str);
		prog->addTask(rdata, task_str, "");
	}

	OmnScreen << task_str << endl;
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcOnNetOffNet::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcOnNetOffNet(*this);
}
