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
#ifndef AOS_JQLParserWrapper_JQLParserWrapper_h
#define AOS_JQLParserWrapper_JQLParserWrapper_h

#include "SEInterfaces/JQLParserObj.h"
#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/Ptrs.h"


class AosJQLParserWrapper : public AosJQLParserObj
{
	OmnDefineRCObject;

public:
	AosJQLParserWrapper(const int version);
	~AosJQLParserWrapper();

	virtual int saveStatements();
	virtual int recoverStatements();

	virtual AosJimoPtr cloneJimo() const;
	virtual int parseJQL(char *data, const bool flag);

	virtual AosExprObjPtr parseExpr(
				const OmnString &jql,
				OmnString &errmsg,
				AosRundata *rdata);

	virtual AosJqlStatementPtr parseStatement(
				const OmnString &jql,
				OmnString &errmsg,
				AosRundata *rdata);

	virtual vector<AosJqlStatementPtr> parseStatements(
				const OmnString &jql,
				OmnString &errmsg,  
				AosRundata *rdata);

	virtual AosExprObjPtr parseJQL(
							const OmnString &jql,
					    	const bool flag,
							AosRundata *rdata);

	virtual AosJqlStatementPtr parseJQL(
							const OmnString &jql,
							AosRundata *rdata);


};
#endif

