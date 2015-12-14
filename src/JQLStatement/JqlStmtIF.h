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
// 2015/01/21 Created by Andy Zhang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtIF_H
#define AOS_JQLStatement_JqlStmtIF_H

#include "JQLStatement/JqlStatement.h"
#include "Util/String.h"

class AosJqlStmtIF : public AosJqlStatement
{
public:
	typedef vector<AosJqlStatementPtr> AosJqlStatementList;

private:
	AosExprObjPtr 					mCond;
	AosJqlStatementList				mThen;
	AosJqlStatementList				mElse;
	bool							mIsNot;

	OmnString						mErrmsg;

public:
	AosJqlStmtIF(const OmnString &errmsg);
	AosJqlStmtIF();
	~AosJqlStmtIF();

	// interfaces
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();

	void setCond(const AosExprObjPtr &cond);
	void setNot(bool flag);
	void setThen(AosJqlStatementList *then);
	void setElse(AosJqlStatementList *elsex);

private:
	bool execution(const AosRundataPtr &rdata);

};

#endif
