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
// 2014/12/02	Created by Levi Wang
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtError_H
#define AOS_JQLStatement_JqlStmtError_H

#include "JQLStatement/JqlStatement.h"
#include "JQLStatement/JqlStmtExpr.h"
#include "Util/String.h"
#include "SQLDatabase/SqlTable.h"

class AosJqlStmtError : public AosJqlStatement
{
private:
	OmnString 		mErrmsg;

public:
	AosJqlStmtError()
	{
	}
	AosJqlStmtError(const OmnString &errmsg);
	~AosJqlStmtError();

	//member functions
	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();
};

#endif
