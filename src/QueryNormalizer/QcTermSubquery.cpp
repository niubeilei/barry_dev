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
// Used as Table in the From-Clause
// --------------------------------
// 		select * from
// 		(
// 			select f1, f2 from t1
//			where f1 = 2
// 		)
// 		where xxx;
//
// This example uses a subquery to temporarily construct a table
// from which the outer selects records. For this type of subqueries,
// we can actually build the table and maintains it. This is called
// derived table. After the derived table is built, the original 
// statement can be re-written as:
// 		select * 
// 		from derived_table_001
// 		where xxx;
//
// Used as Value Sets in Query Conditions
// --------------------------------------
// Below is an example:
// 		select * from t1
// 		where 
// 			f1 in 
// 			(
// 				select t2.f2 from t2
// 				where t2.f3 = 4
// 			)
// When a subquery is used in a query condition, the 
// subquery normally joins the query's table, which is
// handled by QcTermJoin. Otherwise, the subquery should 
// select only one field or multiple fields contained
// in one expression. Any subquery that selects only one 
// field is a Queried Value Set. 
//
// Queried value sets can be created beforehand or on the
// fly. When the value set is available, the original
// query can be normalized by using that value set:
//
// 		select * from t1
// 		where f1 in valueset_001
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
#include "QueryNormalizer/QcTermSubquery.h"

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosQcTermSubquery_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQcTermSubquery(version);
		aos_assert_r(jimo, 0);
		return jimo;
	}

	catch (...)
	{
		AosSetErrorU(rdata, "Failed creating jimo") << enderr;
		return 0;
	}

	OmnShouldNeverComeHere;
	return 0;
}
}



AosQcTermSubquery::AosQcTermSubquery(const int version)
:
AosDataField(version),
mNormalized(false)
{
}


AosQcTermSubquery::~AosQcTermSubquery()
{
}


bool
AosQcTermSubquery::normalizeSubquery(
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
	mSubquery = getSubquery(rdata);
	if (!mSubquery)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

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
AosQcTermSubquery::isNormalizeFinished() const
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
AosQcTermSubquery::normalizeFinished(AosRundata *rdata)
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
AosQcTermSubquery::normalizeFailed(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;
	aos_assert_rr(mCaller, data, false);
	mNormalizeSubqueryResponded = true;
	mCaller->normalizeSubqueryFailed(rdata);
	return true;
}


bool
AosQcTermSubquery::reduce(AosRundata *rdata)
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

