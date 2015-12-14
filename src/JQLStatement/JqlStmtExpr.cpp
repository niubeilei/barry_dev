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
#include "JQLStatement/JqlStmtExpr.h"

#include "Debug/Debug.h"
#include "JQLExpr/Expr.h"
#include "Rundata/Rundata.h"
#include "Util/OmnNew.h"


AosJqlStmtExpr::AosJqlStmtExpr(const AosExprObjPtr expr)
:
mExpr(expr)
{
}


AosJqlStmtExpr::AosJqlStmtExpr(const AosJqlStmtExpr &rhs)
:
mExpr(rhs.mExpr)
{
}


AosJqlStmtExpr::~AosJqlStmtExpr()
{
}


bool
AosJqlStmtExpr::run(const AosRundataPtr &rdata, const AosJimoProgObjPtr &prog)
{
	mExpr->getValue(rdata.getPtrNoLock());
	return true;
}


AosJqlStatement *
AosJqlStmtExpr::clone()
{
	return OmnNew AosJqlStmtExpr(*this);
}


void 
AosJqlStmtExpr::dump()
{
}

