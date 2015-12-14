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
// A QueryRunner is responsible for analyziing and executing a query.
// Any time when a query is received, a QueryRunner class is created. 
// The input to this class is the query being parsed. What this class
// does is:
// 1. Check whether normalization is needed. 
// 2. If yes, whether auto normalization is turned on.
// 3. If yes, do the normalization first. 
//
// Modification History:
// 2015/01/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/QueryRunner.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQueryRunner::AosQueryRunner()
{
}


AosQueryRunner::~AosQueryRunner()
{
}


bool
AosQueryRunner::runQuery(
		AosRundata *rdata, 
		const AosJqlSelectPtr &query,
		const AosQueryCallerPtr &caller)
{
	// This function is called when a new query is received. 
	
	addTimestamp(eTimestampStart);
	mCaller = caller;
	mOrigQuery = query;
	mParentQuery = query;
	mCrtQuery = query;
	mStatus = eStarted;

	if (!AosQueryRunnerUtil::analyzeJoins(rdata, this))
	{
		AosLogError(rdata, true, "failed_analyze_joins") << enderr;
		return false;
	}

	if (!normalize(rdata))
	{
		AosLogError(rdata, true, "failed_normalization") << enderr;
		return false;
	}

	if (mStatus != eReadyToRunQuery) return true;

	if (!postAnalyzeQuery(rdata) || !mQueryProc) 
	{
		AosLogError(rdata, true, "query_not_supported") << enderr;
		return queryFailed(rdata);
	}

	AosQueryRunnerObjPtr thisptr(this, false);
	if (!mQueryProc->runQuery(rdata, thisptr))
	{
		AosLogError(rdata, true, "failed_run_query") << enderr;
		return queryFailed(rdata);
	}

	return true;
}


void
AosQueryRunner::normalize(AosRundata *rdata)
{
	AosQueryRunnerObjPtr thisptr(this, false);
	mNormalizer = Jimo::createQueryNormalizer(rdata, thisptr);
	if (!mNormalizer)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	AosQueryCallerPtr thisptr(this, false);
	mNormalizer->setQuery(rdata, mOrigQuery, thisptr);
	return mNormalizer->normalize(rdata);
}


void
AosQueryRunner::normalizeToSimpleQuery(
		AosRundata *rdata, 
		const bool index_all_fields)
{
	// Normalizing to simple query means to convert the
	// query to a single table query. Conditioned
	// fields may or may not need to be indexed, depending
	// on 'index_all_fields' flag. 
	mIndexCallFields = index_all_fields;

	mNormalizer = Jimo::createQueryNormalizer(rdata, thisptr);
	if (!mNormalizer)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	AosQueryCallerPtr thisptr(this, false);
	mNormalizer->setQuery(rdata, mOrigQuery, mParentQuery, mCrtQuery);
	return mNormalizer->normalizeToSimpleQuery(rdata, 
			index_all_fields, thisptr);
}


bool
AosQueryRunner::postAnalyzeQuery(AosRundata *rdata)
{
	// There are the following to consider:
	// 1. Have conditions
	// 2. Have Tables
	// 3. Have "select *": (0:no, 1:yes)
	// 4. Have selected aggregation functions (0:no, 1:yes)
	// 5. Have group by fields: (0:no, 1:yes)
	// 6. Have order by fields: (0:no, 1:yes)
	// 7. Have having-clause
	// 8. Have joins
	u8 pattern_id = 0;
	if (haveConditions()) 		pattern_id |= BIT_HAVE_CONDITIONS;
	if (haveSelectAggrFunc()) 	pattern_id |= BIT_HAVE_SELECT_AGGR_FUNC;
	if (haveGroubyFields()) 	pattern_id |= BIT_HAVE_GROUPBY_FIELDS;
	if (haveIndexes()) 			pattern_id |= BIT_HAVE_INDEXES;
	if (haveJoins()) 			pattern_id |= BIT_HAVE_JOINS;

	mQueryProc = sgQueryProcs[pattern_id];
	if (!mQueryProc)
	{
		AosLogError(rdata, true, "query_not_supported") << enderr;
		return false;
	}

	return true;
}

