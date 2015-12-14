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
// 2015/07/06 Created by Barry
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcSelectR1R2.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcSelectR1R2_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcSelectR1R2(version);
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


AosJimoLogicDataProcSelectR1R2::AosJimoLogicDataProcSelectR1R2(const int version)
:
AosJimoLogicDataProc(version)
//AosJimo(AosJimoType::eJimoLogicNew, version),
//AosJimoLogicNew(version)
{
//	mJimoType = AosJimoType::eJimoLogicNew;
//	mJimoVersion = version;
}


AosJimoLogicDataProcSelectR1R2::~AosJimoLogicDataProcSelectR1R2()
{
}


bool 
AosJimoLogicDataProcSelectR1R2::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc select datasetname
	// 	(
	// 		name: value,
	// 		name: value,
	// 		...
	// 		name: value
	// 	);
	parsed = false;
	vector<OmnString> keywords = getKeyword();

	aos_assert_rr(keywords.size() >= 2, rdata, false);
	aos_assert_rr(keywords[0] == "dataproc", rdata, false);
	aos_assert_rr(keywords[1] == "selectr1r2", rdata, false);
	mErrmsg << "dataproc selectr1r2 ";
	// parse dataproc name
	OmnString dpname = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dpname,mErrmsg))
	{
		return false;
	}
	mErrmsg << dpname;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = dpname;
	mNameValueList = name_value_list;

	// parse input
	OmnString input = jimo_parser->getParmStr(rdata, "inputs", name_value_list);
	//if (input == "")
	//{
	//	AosLogError(rdata, true, "missing_inputs") << enderr;
	//	return false;
	//}
	mInput = input;
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}
	parsed = true;
	return true;
}


bool 
AosJimoLogicDataProcSelectR1R2::run(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statements_str,
		bool inparser)
{
	//proc inputs
	aos_assert_rr(prog, rdata, false);
	if (mInput != "")
	{
		bool rslt = AosJimoLogicNew::procInput(rdata, mInput, prog, mInputName, statements_str);
		aos_assert_rr(rslt, rdata, false);
	
		if (mInputName == "")
		{
			AosLogError(rdata, true, "failed_retrieving_output_name")
				<< AosFN("Input Name") << mInput << enderr;
			return false;
		}
	}

	bool rslt = createSelectR1R2Dataproc(rdata, prog, statements_str);
	aos_assert_rr(rslt, rdata, false);

    //task for select
    rslt = addSelectR1R2Task(rdata, prog);
    aos_assert_rr(rslt, rdata, prog);

	return true;
}


bool
AosJimoLogicDataProcSelectR1R2::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	// 2. 
	//
	// mInput format:dataset or dataproc.output
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSelectR1R2*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	mJobName = prog->getJobname();
	mDataprocSelectName << "_dp_" << mJobName << "_" << mDataProcName;

	mOutputName << mDataprocSelectName << "_output";
	mOutputNames.push_back(mOutputName);

	return true;
}


OmnString
AosJimoLogicDataProcSelectR1R2::getNewDataProcName()const
{
	return mDataprocSelectName;
}


bool
AosJimoLogicDataProcSelectR1R2::createSelectR1R2Dataproc(
		AosRundata *rdata,
		AosJimoProgObj *jimo_prog, 
		OmnString &statements_str)
{
	OmnString dataprocSelect_str = "create dataprocselect_r1r2 ";
	dataprocSelect_str << mDataprocSelectName << " { "
		<< "\"type\":" << "\"" << "selectr1r2" << "\""
		<< ",\"output_name\":" << "\"" << mOutputName << "\""
		<< ", \"record_type\":" << "\"buff\""
		<< "};";

OmnScreen <<  dataprocSelect_str << endl;
	statements_str << "\n" << dataprocSelect_str;

    bool rslt = parseRun(rdata,dataprocSelect_str, jimo_prog);
    aos_assert_rr(rslt,rdata,false);
	
	return true;
}


bool
AosJimoLogicDataProcSelectR1R2::addSelectR1R2Task(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	//aos_assert_rr(jimo_prog, rdata, false);
	OmnString addselect_str;
 	OmnString input;
	OmnString output;
	OmnString taskName;
	taskName << "task_select_" << mDataProcName;
    output << mOutputName;
	addselect_str << " {"
		<< "\"name\":" << "\"" << taskName << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataprocSelectName << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"],"
        << "\"outputs\":" << "[\"" << output << "\"]"
        << "}]}";

	if(jimo_prog)
		jimo_prog->appendStatement(rdata, "tasks",addselect_str);

	OmnScreen << addselect_str << endl;
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcSelectR1R2::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcSelectR1R2(*this);
}

bool
AosJimoLogicDataProcSelectR1R2::getInputV(vector<OmnString> &inputs)
{
	inputs.push_back(mInput);
	return true;
}
