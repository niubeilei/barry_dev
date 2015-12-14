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
// A Value Set Subquery is a subquery that selects only one field 
// and does not have any relation with the outer query. Such a 
// sub-query is equivalent to a value set that is built from a
// table, possibly with conditions.
//
// Valueset used as a selected field, called Valueset as Selected, 
// shall have no more than one value. Note that Valueset as Selected
// may have conditions and/or joins. 
//
// Normal Value Set
// ----------------
// 	select f1, 
// 		(
// 			select t2.f1 
// 			from t2
// 			where <conds>;
// 		),
// 		f3
// 	from t
// 	where <conds>;
//
// Aggregation Value Set
// ---------------------
// 	select f1, 
// 		(
// 			select sum(t2.f2)
// 			from t2
// 			where <conds>
// 			group by <field-list>
// 		),
// 		f3
// 	from t
// 	where <conds>;
//
// This subquery can be replaced by a value set:
//  select f1, valueset_001, f3
//  from t
//  where <conds>;
//
// These are processed the same. 
//
// Valueset Identification
// -----------------------
// Valueset is identified by Valueset Name, which is 
// constructed as:
// 		databasename +
// 		tablename +
// 		fieldname;
//
// Modification History:
// 2015/02/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QcTermValuesetSelected.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQcTermValuesetSelected::AosQcTermValuesetSelected()
{
}


AosQcTermValuesetSelected::~AosQcTermValuesetSelected()
{
}


bool
AosQcTermValuesetSelected::normalize(AosRundata *rdata)
{
	// It is in the form:
	// 		select
	//      	t.order_no,
	//          (
	//          	select t2.f2_
	//              from t2
	//              where <conds>
	//          ) order_name,
	//      from t
	//      where <conds>;
	//
	// This function assumes the valueset does not exists. 
	// That is, the caller should have checked it before 
	// calling this function.
	mValuesetName = determineValuesetName(rdata);
	if (mValuesetName == "")
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (valuesetExists(rdata))
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	// Need to normalize the query to be a simple query.
	AosQueryCallerPtr thisptr(this, false);
	AosStreamCallerPtr stream_caller(this, false);
	if (!mSubquery->normalizeToSimpleQuery(rdata, 
				thisptr, AosStreamCall(stream_caller)))
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (!mSubquery->isSimpleQuery())
	{
		// Need to wait.
		return true;
	}

	// It is already a simple query. It is the time to build 
	// the valueset.
	AosQueryObjPtr thisptr(this, false);
	if (!Jimo::buildValueset(rdata, thisptr))
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return falsae;
	}

	// When it finishes, it will call back.
	return true;
}


bool
AosQcTermValuesetSelected::streamJobFinished(
		AosRundata *rdata, 
		const AosBSONPtr &call_data)
{
	// The valueset has been successfully built. The
	// query can be replaced with the value from
	// the valueset.
	aos_assert_rr(call_data, rdata, false);
	bool exist = false;
	call_data->getValue("valueset_value", mValue, exist);
	if (!exist)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return callFailed(rdata);
	}

	// Now, this term can be replaced with the calculated
	// value: mValue.
	return callSuccessful(rdata);
}


bool
AosQcTermValuesetSelected::streamJobFailed(
		AosRundata *rdata, 
		const AosCallData &call_data)
{
	aos_assert_rr(mCaller, rdata, false);
	return mCaller->callFailed(rdata, mCallData);
}

