////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//      
// Description: just for test
//
// Modification History:
// 2013/09/26 Created by Kerong.Li
////////////////////////////////////////////////////////////////////////////
#include "JQLExpr/ExprBinary.h"



AosExprBinary::AosExprBinary()
:
mLHS(0),
mRHS(0)
{
}

AosExprBinary::AosExprBinary(AosExprObjPtr lhs, AosExprObjPtr rhs)
:
mLHSPtr(lhs),
mLHS(mLHSPtr.getPtr()),
mRHSPtr(rhs),
mRHS(mRHSPtr.getPtr())
{
}


int
AosExprBinary::getNumFieldTerms() const
{
	aos_assert_r(mLHS, -1);
	aos_assert_r(mRHS, -1);

	return mLHS->getNumFieldTerms() + mRHS->getNumFieldTerms();
}

bool
AosExprBinary::fieldExprWithAggr() const 
{
	if (mLHS->fieldExprWithAggr() && mRHS->fieldExprWithAggr())
		return true;
	return false;
}


bool
AosExprBinary::getFields(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{
	if (mLHS->getFields(rdata, fields) && mRHS->getFields(rdata, fields))
		return true;
	return false;
}


bool 
AosExprBinary::getFieldsByStat(
		const AosRundataPtr &rdata,
		vector<AosJqlSelectFieldPtr> &fields)
{

	if (mLHS->getFieldsByStat(rdata, fields) && mRHS->getFieldsByStat(rdata, fields))
		return true;
	return false;
}

bool
AosExprBinary::isExprCond()const
{
	if (mLHS->isExprCond() || mRHS->isExprCond())
		return true;
	return false;
}

bool
AosExprBinary::isJoin()const
{
	if (mLHS->isJoin() || mRHS->isJoin())
		return true;
	return false;
}


bool
AosExprBinary::isAlwaysFalse()
{
	if (mLHS->isAlwaysFalse() || mRHS->isAlwaysFalse())
		return true;
	return false;
}


bool 
AosExprBinary::getValue(
		AosRundata *rdata,
		vector<AosDataRecordObj *> records,
		const AosValueRslt &key)
{

	return true;
}

