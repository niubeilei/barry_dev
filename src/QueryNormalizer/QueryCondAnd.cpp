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
// 2015/02/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QueryCondAnd.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQueryCondAnd::AosQueryCondAnd()
:
mNormalizeSubqueryResponded(false)
{
}


AosQueryCondAnd::~AosQueryCondAnd()
{
}


bool 
AosQueryCondAnd::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	AosQueryCallerPtr thisptr(this, false);
	mNormalizeSubqueryResponded = false;
	for (u32 i=0; i<mConds.size(); i++)
	{
		if (!mConds[i]->normalizeSubquery(rdata, thisptr)) return false;
	}
	mCaller = caller;
	return true;
}


bool
AosQueryCondAnd::isNormalizeSubqueryFinished() const
{
	for (u32 i=0; i<mConds.size(); i++)
	{
		if (!mConds[i]->isNormalizeSubqueryFinished()) return false;
	}
	return true;
}


bool
AosQueryCondAnd::normalizeSubqueryFinished(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;

	// Check whether all conditions are finished.
	for (u32 i=0; i<mConds.size(); i++)
	{
		if (!mConds[i]->isNormalizeSubqueryFinished()) return true;
	}

	// All terms are finished.
	mNormalizeSubqueryResponded = true;
	if (mCaller) mCaller->normalizeSubqueryFinished(rdata);
	return true;
}


bool
AosQueryCondAnd::normalizeSubqueryFailed(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;
	mNormalizeSubqueryResponded = true;
	if (mCaller) mCaller->normalizeSubqueryFailed(rdata);
	return true;
}

