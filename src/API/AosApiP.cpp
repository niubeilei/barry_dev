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
// handle the SEServer send request to MsgServer 
//
// Modification History:
// 11/17/2014	Created by Andy
////////////////////////////////////////////////////////////////////////////
#include "API/AosApiP.h"

#include "SEInterfaces/JQLParserObj.h"
#include "JQLParser/JQLParser.h"
#include "alarm_c/alarm.h"
#include "XmlUtil/XmlTag.h"

// AosExprObjPtr AosParseJQL(
// 		const OmnString &jql,
// 		const bool flag,
// 		AosRundata *rdata)
AosExprObjPtr AosParseExpr(
		const OmnString &jql,
		OmnString &errmsg,
		AosRundata *rdata)
{
	AosExprObjPtr expr;
	AosJQLParserObj* parser = AosJQLParserObj::getJQLParserStatic(rdata);
	aos_assert_rr(parser, rdata, 0);

	parser->saveStatements();
	expr = parser->parseExpr(jql, errmsg, rdata);
	parser->recoverStatements();
	return expr;
}

// AosJqlStatementPtr AosParseJQL(
// 		const OmnString &jql,
// 		AosRundata *rdata)
AosJqlStatementPtr AosParseStatement(
		const OmnString &jql,
		OmnString &errmsg,
		AosRundata *rdata)
{
	AosJQLParserObj* parser = AosJQLParserObj::getJQLParserStatic(rdata);
	aos_assert_rr(parser, rdata, 0);
	return parser->parseStatement(jql, errmsg, rdata);
}

vector<AosJqlStatementPtr> AosParseJQLStatements(
		const OmnString &jql,
		OmnString &errmsg,
		AosRundata *rdata)
{
	vector<AosJqlStatementPtr> stmts;
	AosJQLParserObj* parser = AosJQLParserObj::getJQLParserStatic(rdata);
	aos_assert_rr(parser, rdata, stmts);
	return parser->parseStatements(jql, errmsg, rdata);
}

