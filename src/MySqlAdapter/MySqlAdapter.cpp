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
// 2013/12/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "MySqlAdapter/MySqlAdapter.h"
#include "JQLParser/JQLParser.h"
#include "JQLParser/Ptrs.h"

using namespace std;
extern int AosParseJQL(char *data, const bool flag);
extern AosJQLParser gAosJQLParser;

AosMySqlAdapter::AosMySqlAdapter()
{
}

AosMySqlAdapter::~AosMySqlAdapter()
{
}


bool
AosMySqlAdapter::procCommand(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff) 
{
	// This function is called when the connection receives
	// a request. The command is in 'buff'. 
	AosParseJQL(buff->data(), true);
	OmnScreen << "Parsing finished" << endl;
	gAosJQLParser.dump();
	OmnScreen << "------------ Run Statements" << endl;
//	gAosJQLParser.run1(rdata);
//	AosJqlStatementPtr statements = gAosJQLParser.getStatement();
//	AosMySqlRecord* record = statements->getMySqlRecord();
//	aos_assert_r(record, false);
	buff->reset();
	//record->encodeHeader(rdata, buff);
//	record->writeDelimiter(rdata, buff);
//	record->encodeRows(rdata, buff);
//	record->writeDelimiter(rdata, buff);
//	gAosJQLParser.clearStatements();
	aos_assert_rr(buff, rdata, false);
	gAosJQLParser.finishParse(); 
	//return procQuery(rdata, buff);
	return true;
}


bool
AosMySqlAdapter::procQuery(
		const AosRundataPtr &rdata, 
		const AosBuffPtr &buff)
{
	//const char *data = buff->data();
	//AosJqlStmtQueryObjPtr statement = parseQuery(rdata, &data[1], buff->dataLen()-1);
	//	if (!statement) return false;
	//
	//	AosBuffPtr results;
	//	if (!statement->runQuery(rdata, results)) return false;
	//
	//	AosMySqlRecordObjPtr record = statement->getResultRecord(rdata);
	//	if (!record) return false;
	////
	//	AosBuffPtr response = OmnNew AosBuff();
	//	if (!record->encodeResponse(rdata, results, response)) return false;
	return true;
}

/*
AosJqlStmtQueryObjPtr
AosMySqlAdapter::parseQuery(
const AosRundataPtr &rdata, 
const char *query_str,
const int len)
{
	// This function parses the query. If successful, it returns
	// the parse object 'AosJqlStmtQueryObjPtr'. Otherwise, 
	//	 errors are reported in 'rdata' and null is returned.
	OmnNotImplementedYet;
	return 0;
}
*/
