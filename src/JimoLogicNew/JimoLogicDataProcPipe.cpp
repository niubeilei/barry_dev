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
#include "JimoLogicNew/JimoLogicDataProcPipe.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcPipe_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcPipe(version);
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


AosJimoLogicDataProcPipe::AosJimoLogicDataProcPipe(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcPipe::~AosJimoLogicDataProcPipe()
{
}


bool 
AosJimoLogicDataProcPipe::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc pipe <name>
	// 	(
	// 		inputs:[<name>,<name>..],	//one or two 
	// 		outputs:[<name>,<name>..],	//option
	// 		parent_dataproc:"dp1",
	// 		child_dataprocs:[
	// 					(
	// 						dataprocs:[<name>, <name>...]
	// 					),
	// 					.......
	// 					(
	// 						dataprocs:[<name>, <name>...]
	// 					)
	// 					]
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "pipe", rdata, false);
	mErrmsg = "dataproc pipe ";

	// parse dataproc name
	OmnString dp_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,dp_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << dp_name;
	//parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		// Not for this statement
		setErrMsg(rdata, eGenericError, "",mErrmsg);
		return false;
	}

	//parse parent dataproc
	OmnString parent_proc = jimo_parser->getParmStr(rdata, "parent_dataproc", name_value_list);
	if (parent_proc == "")
	{
		setErrMsg(rdata, eMissingParm, "parent_dataproc",mErrmsg);
		return true;
	}

	mDataProcName = dp_name;
	mParentProcName = parent_proc;

	//parse inputs 
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0 || mInputs.size() > 2)
	{
		setErrMsg(rdata, eMissingParm, "inputs",mErrmsg);
		return false;
	}

	//parse outputs
	rslt = jimo_parser->getParmArrayStr(rdata, "outputs", name_value_list, mOutputs);

	//parse child procs
	vector<AosExprObjPtr> childProcLists;
	rslt = jimo_parser->getParmArray(rdata, "child_dataprocs", name_value_list, childProcLists);
	if (rslt && childProcLists.size() > 0)
	{
		//get child dataprocs
		rslt = getChildDataProcs(rdata, jimo_parser, childProcLists);
		if (!rslt)
		{
			setErrMsg(rdata, eMissingParm, "child_dataprocs",mErrmsg);
			return false;
		}
		mChildProcs = mChildProcs;
	}
	if(!checkNameValueList(rdata, mErrmsg, name_value_list))
	{
		return false;
	}

	parsed = true;
	return true;
}


bool
AosJimoLogicDataProcPipe::getChildDataProcs(
		AosRundata *rdata,
		AosJimoParserObj *jimo_parser, 
		const vector<AosExprObjPtr> &procLists)
{
	aos_assert_r(procLists.size() > 0, false);

	for (size_t i = 0; i < procLists.size(); i++)
	{
		vector<AosExprObjPtr> dpLists = procLists[i]->getExprList();
		vector<OmnString> dpNames;

		bool rslt = jimo_parser->getParmArrayStr(rdata, "dataprocs", dpLists, dpNames);
		if (!rslt && dpNames.size() <= 0)
			return false;
		mChildProcs.push_back(dpNames);
	}

	return true;
}


bool 
AosJimoLogicDataProcPipe::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	//proc inputs
	OmnString input_name = "";
	for (size_t i = 0; i < mInputs.size(); i++)
	{
		bool rslt = AosJimoLogicNew::procInput(rdata, mInputs[i], prog, input_name,
				statement_str);
		if (!rslt)
		{
			OmnString msg = "dataproc pipe ";
			msg << mDataProcName << " missing input : " << mInputs[i] << ".";
			rdata->setJqlMsg(msg);
			return false;
		}
		if (input_name == "")
		{
			AosLogError(rdata, true, "failed_retrieving_output_name")
				<< AosFN("Input Name") << mInputs[i] << enderr;
			return false;
		}
		mInputNames.push_back(input_name);
	}
	bool rslt = createDataProcPipe(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addPipeTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	setDeleteTask(rdata, prog);
	return true;
}


bool
AosJimoLogicDataProcPipe::createDataProcPipe(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str)
{
	mDataProcPipeName << "_dp_" << mDataProcName;
	OmnString proc_str = "create dataprocpipe ";
	proc_str << mDataProcPipeName << " { "
			<< "\"type\":\"" << mKeywords[1] << "\","
			<< "\"dataprocs\":[";

	if (mNewChildProcs.size() == 0)
	{
		proc_str << "{\"dataprocs\":[\"" << mNewParentProcName << "\"]}";
	}
	else
	{
		for (size_t i = 0; i < mNewChildProcs.size(); i++)
		{
			if (i>0) 
				proc_str << ", ";
			proc_str << "{\"dataprocs\":[\"" << mNewParentProcName << "\"";
			for (size_t j = 0; j < mNewChildProcs[i].size(); j++)
			{
				OmnString dp_name = mNewChildProcs[i][j];
				proc_str << ", \""
					<< dp_name
					<< "\"";
			}
			proc_str <<  "]}";
		}
	}
	proc_str << "]};";

	OmnScreen << proc_str << endl;
	statement_str << "\n" << proc_str;

	bool rslt = parseRun(rdata, proc_str, prog);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcPipe::addPipeTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString taskname = "";
	taskname << "task_pipe_"<< mDataProcName;

	OmnString str = "";
	str << " {"
		<< "\"name\":" << "\"" << taskname << "\","
		<< "\"dataengine_type\":" << "\"";
	if (mInputNames.size() == 1)
	{
		str << "dataengine_scan2\", ";
	}
	else if (mInputNames.size() == 2)
	{
		str << "dataengine_join2\", "
			<< "\"dataset_splitter\": {\"type\": \"file_join\"}, "
			<< "\"join_type\": \"left\", "
			<< "\"model_type\":\"simple\", ";
	}
	else
	{
		//error input size
		aos_assert_r(false, rdata);
	}
	str << "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDataProcPipeName << "\","
		<< "\"inputs\":" << "[";
	for(size_t i = 0;i < mInputNames.size();i++)
	{
		if(i > 0)
			str << ",";
		str << "\"" << mInputNames[i] << "\"";
	}
	str << "],";

	str << "\"outputs\":" << "[";
	if (mOutputs.size() > 0)
	{
		for (size_t i = 0; i < mOutputs.size(); i++)
		{
			if (i > 0)
				str << ",";
			str << "\"" << mOutputs[i] << "\"";
		}
	}
	else if (mNewChildProcs.size() <= 0)
	{
		aos_assert_r(mNewParentProcName!= "", false);
		str << "\"" << mNewParentProcName << "_output\"";
	}
	else
	{
		aos_assert_r(mNewChildProcs.size() > 0, false);

		for (size_t i = 0; i < mNewChildProcs.size(); i++)
		{
			if (i>0)
				str << ", ";
			OmnString output_name = mNewChildProcs[i][mNewChildProcs[i].size() - 1];
			str << "\"" << output_name << "_output\"";
		}
	}
	str << "]}]}";

	if(prog)
		prog->appendStatement(rdata, "tasks", str);
	OmnScreen << str << endl;

	return true;
}


void
AosJimoLogicDataProcPipe::setDeleteTask(
		AosRundata *rdata, 
		AosJimoProgObj *prog)
{
	if (prog)
	{
		prog->setDeleteProcName(rdata, mNewParentProcName);
		for (size_t i = 0; i < mNewChildProcs.size(); i++)
		{
			for (size_t j = 0; j < mNewChildProcs[i].size(); j++)
			{
					prog->setDeleteProcName(rdata, mNewChildProcs[i][j]);
			}
		}
	}
}


bool
AosJimoLogicDataProcPipe::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcPipe*>(this);
	bool rslt_t = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt_t, rdata, false);

	OmnString statement_str = "";
	if (mOutputs.size() > 0)
	{
		mOutputNames = mOutputs;
		//reset output
		if (mChildProcs.size() <= 0)
		{
			aos_assert_r(mOutputs.size() == 1, false);
			AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mParentProcName);
			if (!jimo_logic)
			{
				AosLogError(rdata, true, "pipe_input_not_found")
					<< AosFN("Input Name") << mParentProcName << enderr;
				return false;
			}
			jimo_logic->setOutputName(rdata, prog, mOutputs[0]);
		}
		else
		{
			aos_assert_r(mOutputs.size() == mChildProcs.size(), false);
			for (size_t i = 0; i<mOutputs.size(); i++)
			{
				OmnString name = mChildProcs[i][mChildProcs[i].size() - 1];
				AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, name);
				if (!jimo_logic)
				{
					AosLogError(rdata, true, "pipe_input_not_found")
						<< AosFN("Input Name") << name << enderr;
					return false;
				}
				jimo_logic->setOutputName(rdata, prog, mOutputs[i]);
			}
		}
	}
	else if (mChildProcs.size() <= 0)
	{
		aos_assert_r(mParentProcName != "", false);

		OmnString output = "";
		bool rslt = AosJimoLogicNew::procInput(rdata, mParentProcName, prog, output, statement_str);
		if (!rslt)
		{
			OmnString msg = "dataproc pipe ";
			msg << mDataProcName << " missing input : " << mInput << ".";
			rdata->setJqlMsg(msg);
			return false;
		}


		mOutputNames.push_back(output);
	}
	else
	{
		aos_assert_r(mChildProcs.size() > 0, false);

		for (size_t i = 0; i < mChildProcs.size(); i++)
		{
			OmnString output = "";
			OmnString name = mChildProcs[i][mChildProcs[i].size() - 1];
			bool rslt = AosJimoLogicNew::procInput(rdata, name, prog, output, statement_str);
			if (!rslt)
			{
				OmnString msg = "dataproc pipe ";
				msg << mDataProcName << " missing input : " << mInput << ".";
				rdata->setJqlMsg(msg);
				return false;
			}
			mOutputNames.push_back(output);
		}
	}

	bool rslt = changeProcName(rdata,prog);
	aos_assert_r(rslt, false);

	return true;
}


bool
AosJimoLogicDataProcPipe::changeProcName(
		AosRundata *rdata, 
		AosJimoProgObj *prog)
{
	aos_assert_r(prog, false);
	AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mParentProcName);
	if (!jimo_logic)
	{
		return false;
	}
	OmnString name = jimo_logic->getNewDataProcName();
	mNewParentProcName = name;
	for (size_t i = 0; i < mChildProcs.size(); i++)
	{
		vector<OmnString> vv;
		for (size_t j = 0; j < mChildProcs[i].size(); j++)
		{
			AosJimoLogicObjNew * jimo_logic = prog->getJimoLogic(rdata, mChildProcs[i][j]);
			if (!jimo_logic)
			{
				return false;
			}
			OmnString name = jimo_logic->getNewDataProcName();
			vv.push_back(name);
		}
		mNewChildProcs.push_back(vv);
	}
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcPipe::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcPipe(*this);
}

