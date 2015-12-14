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
#include "JimoLogicNew/JimoLogicIf.h"

#include "API/AosApi.h"
#include "JimoAPI/JimoProgAPI.h"
#include "JimoAPI/JimoParserAPI.h"

extern "C"
{
	AosJimoPtr AosCreateJimoFunc_AosJimoLogicIf_1(const AosRundataPtr &rdata, const int version)
	{
		try
		{
			OmnScreen << "To create Jimo: " << endl;
			AosJimoPtr jimo = OmnNew AosJimoLogicIf(version);
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


AosJimoLogicIf::AosJimoLogicIf(const int version)
:
AosJimo(AosJimoType::eJimoLogicNew,version), 
AosJimoLogicNew(version)
{
	mJimoType = AosJimoType::eJimoLogicNew;
	mJimoVersion = version;
}


AosJimoLogicIf::~AosJimoLogicIf()
{
}


bool 
AosJimoLogicIf::parseJQL(
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
	aos_assert_rr(mKeywords.size() == 1, rdata, false);
	aos_assert_rr(mKeywords[0] == "if", rdata, false);
	// parse expr_str
	OmnString expr_str = jimo_parser->nextExpr(rdata);
	if(expr_str == "")
	{
		mErrmsg << "miss parameter expr \"" << expr_str <<"\"";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}

//	AosExprObjPtr expr = AosParseExpr(expr_str, mErrmsg, rdata);
//	if(!expr)
//	{
//		mErrmsg << "miss parameter expr \"" << expr_str <<"\"";
//		rdata->setJqlMsg(mErrmsg);
//		return false;
//	}
	//parse keyword then
	OmnString keyword = jimo_parser->nextKeyword(rdata);
	if(keyword.toLower() != "then")
	{
		mErrmsg << " miss keyword \"then\"";
		rdata->setJqlMsg(mErrmsg);
		return false;
	}
	
	bool rslt;
	int guard = 100000;
	while(guard--)
	{
		AosJqlStatementPtr ss;
		rslt  = jimo_parser->nextStatement(rdata, prog, expr_str,ss);
		if(!rslt)
		{
			mErrmsg << "please check the statment format!";
			rdata->setJqlMsg(mErrmsg);
			return false;
		}
		if(!ss && rslt)
		{
			parsed = true;
			return true;
		}
		appendStatement(rdata, expr_str, ss);
	}
	OmnShouldNeverComeHere;
	return true;
}


bool 
AosJimoLogicIf::run(
		AosRundata *rdata, 
		AosJimoProgObj *prog,
		OmnString &statement_str,
		bool inparser) 
{
	size_t i = 0;
	for(; i < mExprStmt.size(); i++ )
	{
		bool rslt;
		AosValueRslt vv;
		OmnString expr_str;
		expr_str << mExprStmt[i].first << ";";
		AosExprObjPtr expr = AosParseExpr(expr_str, mErrmsg, rdata);
		rslt = expr->getValue(rdata,NULL,vv);
		aos_assert_r(rslt, false);
		if(vv.getBool())
		{	
			runStatements(rdata,prog,statement_str,mExprStmt[i].second);
			break;
		}
	}
	return true;
}

bool 
AosJimoLogicIf::runStatements(
		AosRundata *rdata,
		AosJimoProgObj *prog,
		OmnString &statement_str,
		vector<AosJqlStatementPtr> &stmts)
{
	OmnString msg = "";
	for (u32 i=0; i<stmts.size(); i++)
	{
		bool rslt = stmts[i]->run(rdata, prog, statement_str, true);
		msg << rdata->getJqlMsg() << "\n";
		aos_assert_r(rslt, false);
	}
	rdata->setJqlMsg(msg);
	return true;
}

bool 
AosJimoLogicIf::appendStatement(
		AosRundata *rdata, 
		OmnString &expr_str, 
		AosJqlStatementPtr &ss)
{
	if(!ss)
	{
		AosLogError(rdata, true, "miss_the_statement!")<<enderr;
		return false;
	}
	itr = mExprStmt.begin();
	for(;itr != mExprStmt.end(); itr++)
	{
		if(itr->first == expr_str)
			break;
	}
	if(itr == mExprStmt.end())
	{
		vector<AosJqlStatementPtr> stmts;
		stmts.push_back(ss);
		pair<OmnString,vector<AosJqlStatementPtr> > pair1(expr_str,stmts);
		mExprStmt.push_back(pair1);
	}
	else
	{
		itr->second.push_back(ss);	
	}
	return true;
}

bool
AosJimoLogicIf::compileJQL(AosRundata *rdata, AosJimoProgObj *prog)
{
	return true;
}


AosJimoPtr 
AosJimoLogicIf::cloneJimo() const
{
	return OmnNew AosJimoLogicIf(*this);
}
