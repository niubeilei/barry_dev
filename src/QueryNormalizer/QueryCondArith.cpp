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
// A Subquery condition term is in the form:
// 	expr operator expr
// where at least one expression is a subquery. 
//
// Modification History:
// 2015/02/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryAnalyzer/QueryCondArith.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQueryCondArith::AosQueryCondArith()
{
}


AosQueryCondArith::~AosQueryCondArith()
{
}


bool
AosQueryCondArith::setQuery(
		AosRundata *rdata, 
		const AosJqlExprObjPtr &expr)
{
	if (expr->isSubquery())
	{
		mLHS = OmnNew AosQueryCondArith(rdata, expr);
		return true;
	}

	// It is not a subquery. It must be in the form:
	// 	expr operator expr
	AosJqlExprObjPtr lhs = expr->getLHS(rdata);
	AosJqlExprObjPtr rhs = expr->getRHS(rdata);
	if (!lhs)
	{
		AosLogError(rdata, true, "missing_lhs_expr")
			<< AosFN("Query") << mOrigQuery->toString()
			<< AosFN("Expression") << expr->toString() << enderr;
		return false;
	}

	if (!rhs)
	{
		AosLogError(rdata, true, "missing_rhs_expr")
			<< AosFN("Query") << mOrigQuery->toString()
			<< AosFN("Expression") << expr->toString() << enderr;
		return false;
	}

	mLHS = Jimo::createQueryTerm(rdata, lhs);
	mRHS = Jimo::createQueryTerm(rdata, rhs);
	if (!mLHS)
	{
		AosLogError(rdata, true, "failed_creating_lhs_term")
			<< AosFN("Query") << mOrigQuery->toString()
			<< AosFN("Expression") << expr->toString() << enderr;
		return false;
	}

	if (!mRHS)
	{
		AosLogError(rdata, true, "failed_creating_rhs_term")
			<< AosFN("Query") << mOrigQuery->toString()
			<< AosFN("Expression") << expr->toString() << enderr;
		return false;
	}

	return true;
}


bool 
AosQueryCondArith::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	mCaller = caller;	
	AosQcTermPtr thisptr(this, false);
	aos_assert_rr(mLHS, rdata, false);
	aos_assert_rr(mRHS, rdata, false);
	if (!mLHS->normalizeSubquery(rdata, thisptr)) return false;
	if (!mRHS->normalizeSubquery(rdata, thisptr)) return false;
	return true;
}


bool
AosQueryCondArith::isNormalizeSubqueryFinished(AosRundata *rdata) const
{
	if (!mLHS->isNormalizeSubqueryFinished()) return false;
	if (!mRHS->isNormalizeSubqueryFinished()) return false;
	return true;
}


bool
AosQueryCondArith::normalizeSubqueryFinished(AosRundata *rdata) 
{
	if (mNormalizeSubqueryResponded) return true;
	if (!mLHS->isNormalizeSubqueryFinished()) return true;
	if (!mRHS->isNormalizeSubqueryFinished()) return true;
	mNormalizeSubqueryResponded = true;
	aos_assert_rr(mCaller, rdata, false);
	return mCaller->normalizeSubqueryFinished(rdata);
}
	

bool
AosQueryCondArith::normalizeSubqueryFailed(AosRundata *rdata) 
{
	if (mNormalizeSubqueryResponded) return true;
	mNormalizeSubqueryResponded = true;
	aos_assert_rr(mCaller, rdata, false);
	return mCaller->normalizeSubqueryFailed(rdata);
}
