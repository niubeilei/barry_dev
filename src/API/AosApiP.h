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
// 08/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_API_ApiP_h
#define AOS_API_ApiP_h

#include "SEInterfaces/CounterCltObj.h"
#include "SEInterfaces/Ptrs.h"
#include "JQLStatement/Ptrs.h"

#include "UtilTime/TimeGran.h"
#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "Util/String.h"

inline bool AosProcCounter(
			const OmnString &cname, 
			const int64_t &value, 
			const AosTimeGran::E time_type,
			const OmnString &timeStr,
			const OmnString &timeFormat,
			const OmnString &statType,
			const OmnString &operation,
			const AosRundataPtr &rdata)
{
	AosCounterCltObjPtr theobj = AosCounterCltObj::getCounterClt();
	aos_assert_r(theobj, false);
	return theobj->procCounter(cname, value, time_type, 
			timeStr, timeFormat, statType, operation, rdata);
}
	

inline OmnString AosParseRowDelimiter(const OmnString &row_delimiter)
{
	OmnString tmp = row_delimiter;
	tmp.toLower();
	if (tmp == "lf") return "\n";
	if (tmp == "cr") return "\r";
	if (tmp == "crlf") return "\r\n";
	return row_delimiter;
}


inline OmnString AosParseTextQualifier(const OmnString &text_qualifier)
{
	OmnString tmp = text_qualifier;
	tmp.toLower();
	if (tmp == "dqm") return "\"";
	if (tmp == "sqm") return "\'";
	if (tmp == "null") return "\1";
	return text_qualifier;
}

// Chen Ding, 2015/01/31
// extern AosExprObjPtr AosParseJQL(
// 			const OmnString &jql,
// 			const bool flag,
// 			AosRundata *rdata);
extern AosExprObjPtr AosParseExpr(
			const OmnString &jql,
			OmnString &errmsg,
			AosRundata *rdata);

// Chen Ding, 2015/01/31
// extern AosJqlStatementPtr AosParseJQL(
// 			const OmnString &jql,
// 			AosRundata *rdata);
extern AosJqlStatementPtr AosParseStatement(
			const OmnString &jql,
			OmnString &errmsg,
			AosRundata *rdata);

extern vector<AosJqlStatementPtr> AosParseJQLStatements(
			const OmnString &jql_stmts,
			OmnString &errmsg, 
			AosRundata *rdata);

inline OmnString AosParseFieldDelimiter(const OmnString &field_delimiter)
{
	OmnString tmp = field_delimiter;
	tmp.toLower();
	if (tmp == "com") return ",";
	if (tmp == "sem") return ";";
	if (tmp == "tab") return "\t";
	return field_delimiter;
}

#endif

