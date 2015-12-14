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
#include "QueryNormalizer/QueryCondOR.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQueryCondOR::AosQueryCondOR()
{
}


AosQueryCondOR::~AosQueryCondOR()
{
}


bool 
AosQueryCondOR::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	if (mTerms.size() <= 0) return true;

	mCaller = caller;
	AosQueryCallerPtr thisptr(this, false);
	mNormalizeSubqueryResponded = false;
	for (u32 i=0; i<mTerms.size(); i++)
	{
		if (!mTerms[i]->normalizeSubquery(rdata, thisptr)) return false;
	}
	return true;
}


bool
AosQueryCondOr::isNormalizeSubqueryFinished()
{
	for (u32 i=0; i<mTerms.size(); i++)
	{
		if (!mTerms[i]->isNormalizeSubqueryFinished()) return false;
	}
	return true;
}


bool
AosQueryCondOr::normalizeSubqueryFinished(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;
	for (u32 i=0; i<mTerms.size(); i++)
	{
		if (!mTerms[i]->isNormalizeSubqueryFinished()) return true;
	}

	mNormalizeSubqueryResponded = true;
	if (mCaller) mCaller->normalizeSubqueryFinished(rdata);
	return true;
}


bool
AosQueryCondOr::normalizeSubqueryFailed(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;
	mNormalizeSubqueryResponded = true;
	if (mCaller) mCaller->normalizeSubqueryFailed(rdata);
	return true;
}

