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
// 12/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigSQLDriver/BigSQLParser.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BigSQLDriver/Ptrs.h"
#include "BigSQLDriver/StmtParser.h"
#include "MultiLang/LangDictMgr.h"
#include "Rundata/Rundata.h"
#include "Util/StrParser.h"
#include "XmlUtil/XmlTag.h"


AosBigSQLParser::AosBigSQLParser()
{
}


AosBigSQLParser::~AosBigSQLParser()
{
}


bool
AosBigSQLParser::parseScripts(
	vector<AosXmlTagPtr> &stmts, 
	const OmnString &statement, 
	const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosBigSQLParser::parseStatement(
	const OmnString &statement, 
	OmnString &stmt,
	const AosRundataPtr &rdata)
{
	OmnStrParser1 parser(statement);

	OmnString word = parser.nextWord("");
	if (word.length() <= 0)
	{
		rdata->setErrmsg(AOSDICTERM("statement_null", rdata));
		return false;
	}

	word.toLower();

	AosSqlStmtParserPtr stmt_parser = AosSqlStmtParser::getParser(word, rdata);
	if (!stmt_parser)
	{
		rdata->setErrmsg(AOSDICTERM("invalid_statement", rdata));
		return false;
	}

	return stmt_parser->parse(statement, parser, stmt, this, rdata);
}

