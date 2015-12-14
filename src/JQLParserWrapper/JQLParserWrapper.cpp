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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "JQLParserWrapper/JQLParserWrapper.h"

#include "API/AosApi.h"
#include "JQLParser/Ptrs.h"
#include "JQLParser/JQLParser.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/JimoLogicObj.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"

extern AosJQLParser gAosJQLParser;                  

extern "C"
{

AosJimoPtr AosCreateJimoFunc_AosJQLParserWrapper_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosJQLParserWrapper(version);
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



AosJQLParserWrapper::AosJQLParserWrapper(const int version)
:
AosJQLParserObj(version)
{
}


AosJQLParserWrapper::~AosJQLParserWrapper()
{
}


AosJimoPtr 
AosJQLParserWrapper::cloneJimo() const
{
	return OmnNew AosJQLParserWrapper(*this);
}


extern int AosParseJQL(char *data, const bool debug_flag);

int
AosJQLParserWrapper::parseJQL(char *data, const bool flag)
{
	return AosParseJQL(data, flag);
}


AosExprObjPtr 
AosJQLParserWrapper::parseExpr(
		const OmnString &jql,
		OmnString &errmsg,
		AosRundata *rdata)
{
	gAosJQLParser.lock();
	if (parseJQL(jql.getBuffer(), false) != 0)
	{ 
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;                                                  
	}                                                                  

	AosExprObjPtr expr = gAosJQLParser.getExpr();                        
	if (!expr)
	{
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;
	}

	AosExprObjPtr e2 = expr->cloneExpr();
	gAosJQLParser.unlock();

	return e2;
}

AosJqlStatementPtr
AosJQLParserWrapper::parseStatement(
		const OmnString &jql,
		OmnString &errmsg,
		AosRundata *rdata)
{
	gAosJQLParser.lock();
	if (parseJQL(jql.getBuffer(), true) != 0)
	{ 
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;                                                  
	}                                                                  

	AosJqlStatementPtr stmt = gAosJQLParser.getStatement();                        
	if (!stmt)
	{
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;
	}

	gAosJQLParser.unlock();
	return stmt;
}

vector<AosJqlStatementPtr>
AosJQLParserWrapper::parseStatements(
		const OmnString &jql,
		OmnString &errmsg,
		AosRundata *rdata)
{
	vector<AosJqlStatementPtr> stmts;
	gAosJQLParser.lock();
	if (parseJQL(jql.getBuffer(), true) != 0)
	{ 
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return stmts;                                                  
	}                                                                  

	stmts = gAosJQLParser.getStatements();
	gAosJQLParser.unlock();
	return stmts;
}


int
AosJQLParserWrapper::saveStatements()
{
	gAosJQLParser.lock();
	gAosJQLParser.saveStatements();
	gAosJQLParser.unlock();
	return 0;
}


int
AosJQLParserWrapper::recoverStatements()
{
	gAosJQLParser.lock();
	gAosJQLParser.recoverStatements();
	gAosJQLParser.unlock();
	return 0;
}

AosExprObjPtr 
AosJQLParserWrapper::parseJQL(
							const OmnString &jql,
					    	const bool flag,
							AosRundata *rdata)
{
	gAosJQLParser.lock();
	if (parseJQL(jql.getBuffer(), flag) != 0)
	{ 
		gAosJQLParser.unlock();
	//	AosSetErrorUser(rdata, "syntax_error")
	//		<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;                                                  
	}                                                                  

	AosExprObjPtr expr = gAosJQLParser.getExpr();                        
	if (!expr)
	{
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;
	}

	// Chen Ding, 2015/04/13
	// AosExprObjPtr e2 = expr->cloneExpr();
	// gAosJQLParser.unlock();
	// 
	// return e2;
	gAosJQLParser.unlock();
	return expr;
}

AosJqlStatementPtr 
AosJQLParserWrapper::parseJQL(
							const OmnString &jql,
							AosRundata *rdata)
{
	gAosJQLParser.lock();
	if (parseJQL(jql.getBuffer(), true) != 0)
	{ 
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;                                                  
	}                                                                  

	AosJqlStatementPtr stmt = gAosJQLParser.getStatement();                        
	if (!stmt)
	{
		gAosJQLParser.unlock();
		AosSetErrorUser(rdata, "syntax_error")
			<< "CondExpr AosParseJQL ERROR:: " << jql << enderr;
		return 0;
	}

	gAosJQLParser.unlock();
	return stmt;

}


