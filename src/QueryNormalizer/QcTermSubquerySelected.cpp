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
// A (pure) subquery is one that does not have any relation
// with the outer query. Such a sub-query is equivalent to 
// a table that can be built beforehand. 
//
// Used as a Selected Field
// ------------------------
// 	select t.order_no,
//   	(
//      	select se.s_name
//      	from se
//      	where se.order_no = '1'
//   	) order_name,
//   	t.unit_id
// 	from t;
//
// The subquery is used as a selected field. Note that if a 
// subquery is used as a selected field, it should normally
// join with the query's tables. Otherwise, the selected is
// independent of the selection, which means that it always
// returns the same value for every record the query selects. 
// In this case, the sub-query can be replaced by a constant 
// field.
//
// Another important thing we need to pay attention is that
// this subquery should select only one value (normally).
// Otherwise, (need to check MySQL and Oracle).
//
// When a subquery is used as a selected field and it is not
// a join subquery, it is called Queried Value. Queried values
// can be created beforehand or created on the fly. When 
// the queried value is available, the original query can be
// re-written as:
//
// 		select t.order_no, 
// 			queried_value_001 order_name 
// 			t.unit_id
// 		from t;
//
// Modification History:
// 2015/02/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QcTermSubquerySelected.h"


AosQcTermSubquerySelected::AosQcTermSubquerySelected()
{
}


AosQcTermSubquerySelected::~AosQcTermSubquerySelected()
{
}


bool
AosQcTermSubquerySelected::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	// The parent statement is in the form:
	// 	select field, ... 
	// 	from ...
	// 	where ...
	// This is one of the 'field' being selected and this selected
	// 'field' is a sub-query with joins.
	// Below is an example:
	// 	select t.order_no,
	//   	(
	//      	select se.s_name
	//      	from se
	//      	where se.order_no = t.order_no
	//   	) order_name,
	//   	t.unit_id
	// from t;
	//
	// In this case, the subquery is used as a selected field. The 
	// subquery is a join.
	//
	// What is does is to get the subquery. It then treats the subquery
	// as a query and uses AosQueryNormalizer to analyze the subquery.
	if (!mSubquery)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	vector<AosQueryCondPtr> join_conds = mSubquery->getJoinConds(rdata, mParentQuery);
	if (join
	// Since this is a subquery used as a selected field, we 
	// need to built a virtual field for it. 
	if (mTable->isVirtualFieldAvailable(rdata, this))
	{
		mNormalizeSubqueryStatus = eFinished;
		return true;
	}

	// The virtual field is not available yet. It needs
	// to build the virtual field for this sub-query.
	mQueryNormalizer = Jimo::createQueryNormalizer(rdata, mSubquery);
	if (!mQueryNormalizer)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		mNormalizeSubqueryStatus = eFailed;
		return false;
	}

	// Note that we do not need the table to index all the 
	// conditioned fields since all we need is to create a 
	// virtual field.
	mQueryNormalizer->needIndexCondFields(false);
	mQueryNormalizer->setQuery(rdata, mOrigQuery, mRootQuery, 
			mCrtQuery, mSubquery);

	// The normalize(...) function will build a map and a
	// virtual field for this field. When finish successfully, 
	// the virtual field should have been created successfully.
	// This sub-query can then be replaced by that virtual field.
	AosQueryCallerPtr thisptr(this, false);
	if (!mQueryNormalizer->normalize(rdata, thisptr))
	{
		mNormalizeSubqueryStatus = eFailed;
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	if (mQueryNormalizer->normalizeFinished())
	{
		mNormalizeSubqueryStatus = eFinished;
		return reduce(rdata);
	}

	// Need to wait
	mNormalizeSubqueryStatus = eActive;
	return true;
}


bool
AosQcTermSubquerySelected::isNormalizeFinished() const
{
	switch (mNormalizeSubqueryStatus)
	{
	case eFinished:
	case eFailed:
		 return true;

	case eActive:
		 return false;

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermSubquerySelected::normalizeFinished(AosRundata *rdata)
{
	aos_assert_rr(!mNormalizeSubqueryResponded, rdata, false);
	if (!reduce(rdata)) 
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		mStatus = eFailed;
		return false;
	}
	aos_assert_rr(mCaller, rdata, false);
	mNormalizeSubqueryResponded = true;
	return mCaller->normalizeSubqueryFinished(rdata);
}


bool
AosQcTermSubquerySelected::normalizeFailed(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;
	aos_assert_rr(mCaller, data, false);
	mNormalizeSubqueryResponded = true;
	mCaller->normalizeSubqueryFailed(rdata);
	return true;
}


bool
AosQcTermSubquerySelected::reduce(AosRundata *rdata)
{
	// The normalization is finished. This field as a subquery
	// can be replaced by a virtual field. 
	// It needs to modify its parent query to replace
	// the selected field with a simple (virtual) field.
	mFieldName = getFieldName(rdata);
	if (mFieldName == "")
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}
	mNormalized = true;
	mStatus = eFinished;
	aos_assert_rr(mCaller, rdata, false);
	mCaller->normalizeFieldFinished(rdata, mFieldIdx);
	return true;
}

