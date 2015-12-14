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
#include "JimoLogicNew/JimoLogicDataProcSwitchCase.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicDataProcSwitchCase_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicDataProcSwitchCase(version);
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


AosJimoLogicDataProcSwitchCase::AosJimoLogicDataProcSwitchCase(const int version)
:
AosJimoLogicNew(version),
AosJimoLogicDataProc(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicDataProcSwitchCase::~AosJimoLogicDataProcSwitchCase()
{
}


bool 
AosJimoLogicDataProcSwitchCase::parseJQL(
		AosRundata *rdata, 
		AosJimoParserObj *jimo_parser, 
		AosJimoProgObj *prog,
		bool &parsed, 
		bool dft)
{
	// The statement is in the form:
	// 	data proc str2str <name>
	// 	(
	// 		inputs    	 :[<name>,<name>..],
	// 		switch_value :"key_field1*2",	
	// 		switch_cases  :[
	// 						(name:value,name:value,...),
	// 						(name:value,name:value,...),
	// 						...
	// 					  ]
	// 	);
	
	parsed = false;
	aos_assert_rr(mKeywords.size() >= 2, rdata, false);
	aos_assert_rr(mKeywords[0] == "dataproc", rdata, false);
	aos_assert_rr(mKeywords[1] == "switchcase", rdata, false);
	mErrmsg << "dataproc switchcase";
	// parse dataset name
	OmnString switchcase_name = jimo_parser->nextObjName(rdata);
	if (!isLegalName(rdata,switchcase_name,mErrmsg))
	{
		// This is not what it expects.
		return false;
	}
	mErrmsg << switchcase_name;
	// parser name_value_list
	vector<AosExprObjPtr> name_value_list;
	bool rslt = jimo_parser->getNameValueList(rdata, name_value_list);
	if (!rslt)
	{
		setErrMsg(rdata,eGenericError,"",mErrmsg);
		return true;
	}

	mDataProcName = switchcase_name;
	mNameValueList = name_value_list;
	//parse inputs and outputs
	rslt = jimo_parser->getParmArrayStr(rdata, "inputs", name_value_list, mInputs);
	if (!rslt || mInputs.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"inputs",mErrmsg);
		return false;
	}

	// parse switch_value
	mSwitchValue = jimo_parser->getParmExpr(rdata, "switch_value", name_value_list);
	if(!mSwitchValue)
	{
		setErrMsg(rdata, eMissingParm, "switch_value", mErrmsg);
		return false;
	}

	// parse switch_case
	rslt = jimo_parser->getParmArray(rdata, "switch_cases", name_value_list, mSwitchCase);
	if(!rslt || mSwitchCase.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"switch_cases",mErrmsg);
		return false;
	}

	//collect dataprocs
	rslt = collectDataProcs(rdata, mSwitchCase, mDataProcs, mCase);
	if(!rslt || mDataProcs.size() <= 0|| mCase.size() <= 0)
	{
		setErrMsg(rdata,eMissingParm,"dataproc",mErrmsg);
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
AosJimoLogicDataProcSwitchCase::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	mOutputNames.clear();
	vector<OmnString> dataProcNames;
	bool rslt = getDataprocNames(rdata,mDataProcs,dataProcNames);
	aos_assert_rr(rslt , rdata, false);
	
	rslt = getOutputsFromDataProc(rdata, prog, dataProcNames,mOutputNames);
	aos_assert_rr(rslt && mOutputNames.size(), rdata, false);

	rslt = createSwitchCaseDataProc(rdata, prog, statement_str);
	aos_assert_rr(rslt, rdata, false);

	rslt = addSwitchCaseTask(rdata, prog);
	aos_assert_rr(rslt, rdata, false);

	//set delete procName
	if (prog)
	{
		for (size_t i = 0; i < dataProcNames.size(); i++)
		{
			OmnString dpname = "_dp_";
			dpname << dataProcNames[i];
			prog->setDeleteProcName(rdata, dpname);
		}
	}
	return true;
}


bool 
AosJimoLogicDataProcSwitchCase::getDataprocNames(
		AosRundata *rdata,
		vector<vector<AosExprObjPtr> > &dataprocs,
		vector<OmnString> &dataproc_names)
{
	aos_assert_rr(dataprocs.size()>0, rdata, false);
	for(size_t i = 0; i<dataprocs.size();i++)
	{
		for(size_t j = 0; j<dataprocs[i].size(); j++)
		{
			OmnString dataproc_name = dataprocs[i][j]->dumpByNoQuote();
			dataproc_names.push_back(dataproc_name);
		}
	}
	return true;
}


bool
AosJimoLogicDataProcSwitchCase::collectDataProcs(
		AosRundata *rdata,
		const vector<AosExprObjPtr> &switchCase,
		vector<vector<AosExprObjPtr> >  &dataprocs,
		vector<vector<AosExprObjPtr> > &cases)
{
	if(switchCase.size() <= 0)return false;
	for(size_t i = 0; i < switchCase.size();i++)
	{
		vector<AosExprObjPtr> exprV = switchCase[i]->getExprList();
		for(size_t j = 0; j < exprV.size(); j++)
		{
			OmnString name = exprV[j]->getName();
			if(name.toLower() == "dataprocs")
			{
				AosExprObjPtr expr = exprV[j]->getValueAsExpr();
				dataprocs.push_back(expr->getExprList());
			}
			if(name.toLower() == "case")
			{
				AosExprObjPtr expr = exprV[j]->getValueAsExpr();
				cases.push_back(expr->getExprList());
			}
		}
	}
	return true;
}

bool
AosJimoLogicDataProcSwitchCase::createSwitchCaseDataProc(
							AosRundata *rdata,
							AosJimoProgObj *prog,
							OmnString &statement_str)
{
	OmnString switchcase_str = "create dataprocswitchcase ";
	switchcase_str << mSwitchCaseName << " { "
			<< "\"type\":\"switch_case\","
			<< "\"switch_value\":\"" << mSwitchValue->dumpByNoQuote() << "\",";
	switchcase_str << "\"switch_cases\":[";
	for(size_t i =0; i< mCase.size();i++)
	{
		if(i>0)
			switchcase_str << ",";
		switchcase_str << "{\"case\":[";
		for(size_t j = 0; j < mCase[i].size();j++ )
		{
			if(j>0)
				switchcase_str << ",";
			switchcase_str << "\"" << mCase[i][j]->dumpByNoQuote() << "\""; 
		}
		switchcase_str << "],\"dataprocs\":[";
		for(size_t m = 0; m < mDataProcs[i].size();m++ )
		{
			if(m > 0)
				switchcase_str << ",";
			switchcase_str << "\"_dp_" << mDataProcs[i][m]->dumpByNoQuote() << "\"";
		}
		switchcase_str << "]}";

	}
	switchcase_str << "]};";
	OmnScreen << switchcase_str << endl;
	statement_str << "\n" << switchcase_str;

	bool rslt = parseRun( rdata, switchcase_str, prog);
	aos_assert_rr(rslt,rdata,false);
	
	return true;
}

bool
AosJimoLogicDataProcSwitchCase::addSwitchCaseTask(
					AosRundata *rdata, 
					AosJimoProgObj *prog)
{
	OmnString add_switchcase_str;
	OmnString taskname;
	vector<OmnString> outputNames;	
	if (!mInputs.empty())
	{
		bool rslt = getOutputsFromDataProc(rdata, prog, mInputs, outputNames);
		aos_assert_rr(rslt ,rdata ,false);
	}

	taskname << "task_switchcase_"<< mDataProcName;
	add_switchcase_str << " {"
			<< "\"name\":" << "\"" << taskname << "\","
			<< "\"dataengine_type\":" << "\"dataengine_scan2\","
			<< "\"dataprocs\":" << "[{"
			<< "\"dataproc\":" << "\"" << mSwitchCaseName << "\","
			<< "\"inputs\":" << "[";
	if (outputNames.empty())
	{
		outputNames = mInputs;
	}
	for(size_t i = 0;i < outputNames.size();i++)
	{
		if(i > 0)
			add_switchcase_str << ",";
		add_switchcase_str << "\"" << outputNames[i] << "\"";
	}
	add_switchcase_str << "],";
	add_switchcase_str << "\"outputs\":[";
	for(size_t i = 0;i < mOutputNames.size();i++)
	{
		if(i > 0)
			add_switchcase_str << ",";
		add_switchcase_str << "\"" << mOutputNames[i] << "\"";	
	}
	add_switchcase_str << "]}]}";
	if(prog)
		prog->appendStatement(rdata, "tasks",add_switchcase_str);
	OmnScreen << add_switchcase_str << endl;
	return true;
}


bool
AosJimoLogicDataProcSwitchCase::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	// 1. Check whether the input dataset exists. In the current implementations,
	//    the input dataset should exist in 'prog'. 
	aos_assert_rr(prog, rdata, false);
	mSwitchCaseName << "_dp_" << mDataProcName;
	AosJimoLogicObjNewPtr jimologic = dynamic_cast<AosJimoLogicDataProcSwitchCase*>(this);
	bool rslt = prog->addJimoLogicNew(rdata, mSwitchCaseName, jimologic);
	aos_assert_rr(rslt, rdata, false);
	return true;
}

AosJimoPtr 
AosJimoLogicDataProcSwitchCase::cloneJimo() const
{
	return OmnNew AosJimoLogicDataProcSwitchCase(*this);
}



