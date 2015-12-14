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
#include "JimoLogicNew/JimoLogicDataProcIf.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcIf_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcIf(version);
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


AosJimoLogicDataProcIf::AosJimoLogicDataProcIf(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcIf::~AosJimoLogicDataProcIf()
{
}


bool 
AosJimoLogicDataProcIf::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc if <name>
	// 	(
	// 		inputs:[<name>,<name>..],				//it has only one value
	// 		outputs:[<name>,<name>..],				//option
	// 		condition:a>b,
	// 		true_procs:[<name>,<name>...],
	// 		false_procs:[<name>,<name>...],
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "if", rdata, false);
	mErrmsg = "dataproc if ";

	// parse dataset name
	OmnString dataproc_if_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata, dataproc_if_name, mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dataproc_if_name;
	
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return false;
	}

	mDataProcName = dataproc_if_name;
	mNameValueList = name_value_list;
	
	// parse condition
	mCond = jimo_parser->getParmExpr(rdata, "condition", name_value_list);
	if (!mCond)
	{
		setErrMsg(rdata,eMissingParm,"condition",mErrmsg);
		return false;
	}

	// parse true/false procs
	rslt = jimo_parser->getParmArrayStr(rdata, "true_procs", name_value_list, mTrueProcs);
	if (!rslt || mTrueProcs.size()<0)
	{
		setErrMsg(rdata,eMissingParm,"true_procs",mErrmsg);
		return false;
	}
	
	rslt = jimo_parser->getParmArrayStr(rdata, "false_procs", name_value_list, mFalseProcs);
	if (!rslt || mFalseProcs.size()<0)
	{
		setErrMsg(rdata,eMissingParm,"false_procs",mErrmsg);
		return false;
	}
	
	//parse inputs and outputs
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
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
AosJimoLogicDataProcIf::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	mOutputNames.clear();
	bool rslt = getOutputsFromDataProc(rdata, prog, mTrueProcs, mOutputNames);
	aos_assert_rr(rslt, rdata, false);

	rslt = getOutputsFromDataProc(rdata, prog, mFalseProcs, mOutputNames);
	aos_assert_rr(rslt && mOutputNames.size() > 0, rdata, false);

	rslt = createIfDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addIfTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	setDeleteTask(rdata, prog);
	return true;
}


void
AosJimoLogicDataProcIf::setDeleteTask(
		AosRundata *rdata, 
		AosJimoProgObj *prog)
{
	if (prog)
	{
		for (size_t i = 0; i < mTrueProcs.size(); i++)
		{
			OmnString procName = "_dp_";
			procName << mTrueProcs[i];
			prog->setDeleteProcName(rdata, procName);
		}
		for (size_t i = 0; i < mFalseProcs.size(); i++)
		{
			OmnString procName = "_dp_";
			procName << mFalseProcs[i];
			prog->setDeleteProcName(rdata, procName);
		}
	}
}


bool
AosJimoLogicDataProcIf::createIfDataProc(
							AosRundata *rdata,
							AosJimoProgObj *prog,
							OmnString &statement_str)
{
	OmnString procif_str = "create dataprocif ";
	procif_str << mNewDataProcName << " { "
			<< "\"type\":\"" << mKeywords[1] << "\","
			<< "\"condition\":\"" << mCond->dumpByNoQuote() << "\",";
	procif_str << "\"true_procs\":[";
	if(mTrueProcs.size() <= 0)
	{
		procif_str << "\"\",";
	}
	else
	{
		for(size_t i =0; i<mTrueProcs.size();i++)
		{
			if(i>0)
				procif_str << ",";
			procif_str << "\""<< "_dp_" << mTrueProcs[i] << "\"";

		}
	}
	procif_str << "],";
	procif_str << "\"false_procs\":[";
	if(mFalseProcs.size() <= 0)
	{
		procif_str << "\"\",";
	}
	else
	{
		for(size_t i =0; i<mFalseProcs.size();i++)
		{
			if(i>0)
				procif_str << ",";
			procif_str << "\"" << "_dp_" << mFalseProcs[i] << "\"";

		}
	}
	procif_str << "]};";
	OmnScreen <<procif_str << endl;
	statement_str << "\n" << procif_str ;

	bool rslt = parseRun(rdata,procif_str,prog);
	aos_assert_rr(rslt,rdata,false);

	OmnString msg = "create dataprocif ";
	msg << mDataProcName << "successful!" ;
	rdata->setJqlMsg(msg);

	return true;
}


bool
AosJimoLogicDataProcIf::addIfTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString add_if_str;
	OmnString taskname;
	vector<OmnString> outputNames;
	if (!mInputs.empty())
	{
		bool rslt = getOutputsFromDataProc(rdata, prog, mInputs, outputNames);
		aos_assert_rr(rslt ,rdata ,false);
	}

	taskname << "task_if_"<< mDataProcName;
	add_if_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_scan2\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mNewDataProcName << "\","
			<< "\"inputs\":" << "[";
	if (outputNames.empty())
	{
		outputNames = mInputs;
	}

	for(size_t i = 0;i < outputNames.size();i++)
	{
		if(i > 0)
			add_if_str << ",";
		add_if_str << "\"" << outputNames[i] << "\"";
	}
	add_if_str << "],";
	add_if_str << "\"outputs\":" << "[";
	for(size_t i = 0; i < mOutputNames.size(); i++)
	{
		if(i > 0)
			add_if_str << ",";
		add_if_str << "\"" << mOutputNames[i] <<"\""; 
	}
	add_if_str << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks",add_if_str);
	OmnScreen << add_if_str << endl;
	return true;
}


bool
AosJimoLogicDataProcIf::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcIf*>(this);
	mNewDataProcName = "_dp_";
	mNewDataProcName << mDataProcName;
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcIf::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcIf(*this);
}
