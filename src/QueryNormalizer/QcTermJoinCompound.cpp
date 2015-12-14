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
// QcTermJoinCompound is a subquery that is a join statement.
//
// Used as a Selected Field
// ------------------------
// 	select t.order_no,
//   	(
//      	select se.s_name
//      	from se
//      	where se.order_no = t.order_no
//   	) order_name,
//   	t.unit_id
// 	from t;
//
// The subquery joins with one or more outer tables. The selected
// should be (always) one select expression (which may involve
// one or more fields), and the values being selected can be null
// or just one. If multiple values are selected, (need to check
// with MySQL and Oracle).
//
// When a join subquery is used as a selected field, the subquery
// defines a Join Value Set. A Join Value Set functions like a 
// map. It maps Input Tuples to Value Sets. 
//
// 1. Inner Join
//    Given an input tuple, it maps to a value set if and only
//    if the corresponding joined record set is not empty.
// 2. Left Outer Join
//    The map is constructed the same way as the inner join.
// 3. Right Outer Join
//    It is just a matter of on which table to build the map.
// 4. Left Exclusive Join
//
// This subquery is normalized through a Join Map and a Join
// Virtual Field. The join map is built on the joined table
// and the join virtual field is built on the joining table.
//
// If the virtual field is present, the original select is 
// replaced as:
// 		select t.order_no, t._vf_se_s_name, t.unit_id from t;
//
// Used in Query Conditions
// ------------------------
// 	select * from t1
// 	where
// 		t1.f1 in 
// 		(
// 			select t2.f2 
// 			from t2
// 			where
// 				t1.f3 = t2.f4
// 		);
//
// In this example, the subquery is used to dynamically generate
// a value set based on the record being selected. This join
// can be replaced with join virtual fields.
//
// Used in From-Clause
// -------------------
// (Need example).
//
// Modification History:
// 2015/02/01 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QcTermJoinCompound.h"

extern "C"
{
AosJimoPtr AosCreateJimoFunc_AosQcTermJoinCompound_1(const AosRundataPtr &rdata, const int version)
{
	try
	{
		OmnScreen << "To create Jimo: " << endl;
		AosJimoPtr jimo = OmnNew AosQcTermJoinCompound(version);
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



AosQcTermJoinCompound::AosQcTermJoinSimple(const int version)
:
AosDataField(version),
mNormalized(false)
{
}


AosQcTermJoinCompound::~AosQcTermJoinSimple()
{
}


bool
AosQcTermJoinCompound::normalizeSubquery(
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
	if (!selectMasterTable(rdata))
	{
		AosLogError(rdata, false, "failed_selecting_master_table") << enderr;
		return false;
	}

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
AosQcTermJoinCompound::selectMasterQuery(AosRundata *rdata)
{
	// This function selects the master table. 
	//

bool
AosQcTermJoinCompound::isNormalizeFinished() const
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
AosQcTermJoinCompound::normalizeFinished(AosRundata *rdata)
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
AosQcTermJoinCompound::normalizeFailed(AosRundata *rdata)
{
	if (mNormalizeSubqueryResponded) return true;
	aos_assert_rr(mCaller, data, false);
	mNormalizeSubqueryResponded = true;
	mCaller->normalizeSubqueryFailed(rdata);
	return true;
}


bool
AosQcTermJoinCompound::reduce(AosRundata *rdata)
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

