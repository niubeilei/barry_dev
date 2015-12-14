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
// 2013/10/09 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_JQLStatement_JqlStmtExpr_H
#define AOS_JQLStatement_JqlStmtExpr_H

#include "JQLStatement/JqlStatement.h"
#include "SEInterfaces/ExprObj.h"

class AosExpr;

class AosJqlStmtExpr : public AosJqlStatement
{
private:
	AosExprObjPtr		mExpr;

public:
	AosJqlStmtExpr(const AosExprObjPtr expr);
	AosJqlStmtExpr(const AosJqlStmtExpr &rhs);
	~AosJqlStmtExpr();
	
	AosExprObjPtr getExpr(){return mExpr;}

	virtual bool run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog);
	virtual AosJqlStatement *clone();
	virtual void dump();
};

#endif
