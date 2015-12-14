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
// 2015/11/04 Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "JimoLogicNew/JimoLogicDataProcSyncher.h"
#include "JQLStatement/JqlStatement.h"
#include "SEUtil/JqlTypes.h"
#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"


extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcSyncher_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcSyncher(version);
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


AosJimoLogicDataProcSyncher::AosJimoLogicDataProcSyncher(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
	mIsService = false;
}


AosJimoLogicDataProcSyncher::~AosJimoLogicDataProcSyncher()
{
}


bool 
AosJimoLogicDataProcSyncher::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	dataproc syncher name
	// 	(
	//		inputs: input_name, ddd, ddd, ddd,
	//		operator: ['create' | 'delete'],
	//		dbname: xxx,
	//		tablename: xxx
	// 	);
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "syncher", rdata, false);
	mErrmsg = "dataproc syncher";
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

	// parse table
	mTableName = jimo_parser->getParmStr(rdata, "table", name_value_list);
	if (mTableName == "")
	{
		setErrMsg(rdata, eMissingParm, "table", mErrmsg);
		return false;
	}

	// parse database
	mDBName = jimo_parser->getParmStr(rdata, "database", name_value_list);
	if (mDBName == "")
	{
		setErrMsg(rdata, eMissingParm, "database", mErrmsg);
		return false;
	}

	// parse operator
	mOperator = jimo_parser->getParmStr(rdata, "operation", name_value_list);
	if (mOperator == "") mOperator = "add";

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
AosJimoLogicDataProcSyncher::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	aos_assert_rr(prog, rdata, false);
	//proc inputs
	
	aos_assert_rr(prog, rdata, false);
	if (mInputs != "")
	{
		bool rslt = AosJimoLogicNew::procInput(rdata, mInputs, prog, mInputName, statement_str);
		if (!rslt)
		{
			OmnString msg = "dataproc select ";
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
AosJimoLogicDataProcSyncher::compileJQL(
				AosRundata *rdata,
				AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSyncher*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mDataProcName, jimologic);
	aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcSyncher::createJSON(
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
	OmnString conf = "create dataprocsyncher ";
	conf << mDPName << " ";

	JSONValue json;
	json["table"] = mTableName.data();
	json["database"] = mDBName.data();
	json["operator"] = mOperator.data();
	conf << json.toStyledString() << ";";

	statement_str << "\n" << conf;
    
	bool rslt = parseRun(rdata, conf, jimo_prog);
    aos_assert_rr(rslt, rdata, false);

	return true;
}


bool
AosJimoLogicDataProcSyncher::createTask(
        AosRundata *rdata,
        AosJimoProgObj *jimo_prog)
{
	OmnString addsyncher_str;
	OmnString taskname;
	taskname << "task_proc_syncher_" << mDPName;
	addsyncher_str << " { "
		<< "\"name\":" << "\"" << taskname << "\"" <<","
		<< "\"dataengine_type\":" << "\"dataengine_scan2\"" << ","
		<< "\"dataprocs\":" << "[{"
		<< "\"dataproc\":" << "\"" << mDPName << "\","
		<< "\"inputs\":" << "[\"" << mInputName << "\"]";
	addsyncher_str <<"}]}";
	if(jimo_prog)
	{
		jimo_prog->appendStatement(rdata, "tasks",addsyncher_str);
		jimo_prog->addTask(rdata, addsyncher_str, "");
	}

	OmnScreen << addsyncher_str << endl;
	return true;
}


AosJimoPtr 
AosJimoLogicDataProcSyncher::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcSyncher(*this);
}


OmnString 
AosJimoLogicDataProcSyncher::getInput() const
{
	return mInputs;
}


OmnString 
AosJimoLogicDataProcSyncher::getTableName() const
{
	return  mTableName;
}


