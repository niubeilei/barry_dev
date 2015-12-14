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
// 2014/07/26 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SEInterfaces_JQLParserObj_h
#define AOS_SEInterfaces_JQLParserObj_h

#include "SEInterfaces/Ptrs.h"
#include "JQLStatement/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "XmlUtil/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Jimo/Ptrs.h"
#include "Util/Ptrs.h"


class AosJQLParserObj : public AosJimo
{
protected:
	static AosJQLParserObjPtr	smJQLParserObj;

public:
	AosJQLParserObj(const int version);
	~AosJQLParserObj();

	static AosJQLParserObj* getJQLParserStatic(AosRundata *rdata);
	
	// JQLParserObj Interface
	virtual int parseJQL(char *data, const bool flag) = 0;

	virtual int saveStatements() = 0;
	virtual int recoverStatements() = 0;

	virtual AosExprObjPtr parseExpr(
							const OmnString &jql,
							OmnString &errmsg,
							AosRundata *rdata) = 0;

	virtual AosJqlStatementPtr parseStatement(
							const OmnString &jql,
							OmnString &errmsg,
							AosRundata *rdata) = 0;

	virtual vector<AosJqlStatementPtr> parseStatements(
							const OmnString &jql,
							OmnString &errmsg,
							AosRundata *rdata) = 0;
    //arvin 2015.04.19
	virtual AosExprObjPtr parseJQL(
							const OmnString &jql,
					    	const bool flag,
							AosRundata *rdata) = 0;

	virtual AosJqlStatementPtr parseJQL(
							const OmnString &jql,
							AosRundata *rdata) = 0;


private:
	static bool createJimoDocStatic(AosRundata *rdata);
};
#endif

