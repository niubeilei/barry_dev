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
// Modification History:
// 2015/02/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QcTermValueSet.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQcTermValueSet::AosQcTermValueSet()
{
}


AosQcTermValueSet::~AosQcTermValueSet()
{
}


bool
AosQcTermValueSet::normalize(AosRundata *rdata)
{
	// A simple query field is a field that is not a
	// sub-query. A field needs to normalize if:
	// 1. If the field is in a sub-query and its table is 
	//    different from the outer table. Below is an example:
	// 		select
	//      	t.order_no,
	//          (
	//          	select se.s_name
	//              from se
	//              where se.order_no = t.order_no
	//          ) order_name,
	//      from t
	//      where xxx;
	//    In this example, the outer table is 't' and inner
	//    table is 'se'. The selected is 'se', not 't'. 
	//    As a matter of fact, inner sub-query shall not select
	//    outer table's field.
	// 2. If the current query has multiple tables and this 
	//    selected field is not on the master table, the 
	//    field needs to be normalized. Below is an example:
	//    	select t1.f1, t2.f2 
	//    	from t1, t2
	//    	where t1.f3 = t2.f4;
	//    If t1 is the master table, t2.f2 shall be normalized.
	// 3. The join is used as a filter, or:
	// 		a. The field is used in a join condition, 
	// 		b. The field is not selected, 
	// 		c. The the join cannot be reduce by other join fields, 
	// 	  This means that the join shall be reduced by an exist virtual
	//    field.
	bool is_master_field = mMasterTable->doesTableOwnField(rdata, this);
	if (is_master_field)
	{
		mNeedNormalize = false;
		return true;
	}

	if (isSubquery())
	{
		// Case 1: it is a sub-query and its field is not in 
		// the master table. This field needs to be normalized
		// to a virtual field.
		if (!normalizeSubquery(rdata))
		{
			AosLogError(rdata, true, "failed_normalize_subquery") 
				<< AosFN("Query") << mOrigQuery->toString() << enderr;
			return false;
		}

		if (isFinished())
		{
			mCaller->normalizeFinished(rdata);
		}
		return true;
	}

	if (fieldNotInMasterTable(rdata))
	{
		// Case 2
		if (!normalizeJoin(rdata))
		{
			AosLogError(rdata, true, "failed_normalize_join")
				<< AosFN("Query") << mOrigQuery->toString() << enderr;
			return false;
		}

		if (isFinished())
		{
			mCaller->normalizeFinished(rdata);
		}
		return true;
	}

	if (canBeReplacedByExistVirtualField(rdata))
	{
		// Case 3
		if (!normalizeFilterJoin(rdata))
		{
			AosLogError(rdata, true, "failed_normalize_filter_join")
				<< AosFN("Query") << mOrigQuery->toString() << enderr;
			return false;
		}

		if (isFinished())
		{
			mCaller->normalizeFinished(rdata);
		}
		return true;
	}

	AosLogError(rdata, false, "unrecognized_query")
		<< AosFN("Query") << mOrigQuery->toString()
		<< AosFN("Field") << toString() << enderr;
	return false;
}


bool
AosQcTermValueSet::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	// Simple term does not need to do anything with
	// normalizing subqueries.
	return true;	
}


bool 
AosQcTermValueSet::isNormalizeSubqueryFinished()
{
	return true;
}


bool 
AosQcTermValueSet::normalizeSubqueryFinished()
{
	OmnShouldNevetComeHere;
	return false;
}


bool 
AosQcTermValueSet::normalizeSubqueryFailed()
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermValueSet::normalizeJoin(AosRundata *rdata)
{
	// This field is in a sub-query and it is not in 
	// the master table. It needs to create a map and
	// a virtual field.

	if (isSimpleJoin(rdata))
	{
		if (!normalizeSimpleJoin(rdata, mMaster, mTables[0]))
		{
			AosLogError(rdata, true, "failed_normalize_simple_join")
				<< AosFN("Query") << mOrigQuery->toString() << enderr;
			return false;
		}
		return true;
	}
	else if (isCompoundJoin(rdata))
	{
		if (!normalizeCompoundJoin(rdata, mMaster, mTables[0]))
		{
			AosLogError(rdata, true, "failed_normalize_compound_join")
				<< AosFN("Query") << mOrigQuery->toString() << enderr;
			return false;
		}
		return true;
	}

	AosLogError(rdata, false, "internal_error") 
		<< AosFN("Query") << mOrigQuery->toString() << enderr;
	return false;
}


bool
AosQcTermValueSet::normalizeCompoundJoin(AosRundata *rdata)
{
}


bool
AosQcTermValueSet::normalizeSimpleJoin(
		AosRundata *rdata, 
		const AosQueryTable &master_table, 
		const AosQueryTable &joined_table)
{
	mJoinedFields = getJoinedFields(master_table, joined_table);
	if (mJoinedFields.size() <= 0)
	{
		AosLogError(rdata, true, "missing_joined_fields") << enderr;
		return false;
	}

	// 'joined_fields' are the key fields for the map.
	mFieldNames = mSelectExpr->getFieldNames(rdata);
	if (mFieldNames.size() <= 0)
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return false;
	}

	AosQueryCallerPtr thisptr(this, false);
	if (!Jimo::createJoinMap(rdata, mJoinedFields, mFieldNames, thisptr))
	{
		AosLogError(rdata, false, "failed_create_join_map") << enderr;
		return false;
	}
	return true;
}


bool
AosQcTermValueSet::callFinished(
		AosRundata *rdata, 
		const bool status,
		const int call_id, 
		const OmnString &errmsg)
{
	if (call_id == eCreateJoinMap)
	{
		// Creating join map finished.
		if (!createVirtualField(rdata))
		{
			AosLogError(rdata, false, "create_virtual_field_failed") << enderr;
			return false;
		}
	}

	if (call_id == eCreateVirtualFieldMap)
	{
		// Creating virtual fields finished. It is the time
		// to replace the original fields with the virtual 
		// field names.
		if (mVirtualFieldNames.size() != mFieldNames.size())
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}

		if (!mSelectExpr->replaceFieldNames(rdata, mFieldNames, mVirtualFieldNames))
		{
			AosLogError(rdata, true, "failed_replace_names") << enderr;
			return false;
		}

		// This finishes the normalization.
		mCaller->normalizeFinished(rdata);
		return true;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermValueSet::createVirtualField(AosRundata *rdata)
{
	// This function assumes mJoinedFields are the fields being 
	// joined, and mFieldNames are the field names in mSelectExpr.
	// These fields need to be replaced with the virtual fields
	// to be created by this function. The needed map should
	// have built successfully.
	AosQueryCallerPtr thisptr(this, false);
	if (!Jimo::createVirtualField(rdata, mMasterTable, 
				mMasterJoinedFields, mFieldNames, thisptr))
	{
		AosLogError(rdata, true, "failed_creating_join_virtual_fields") << enderr;
		return false;
	}

	return true;
}


bool
AosQcTermValueSet::isNormalizeFinished() const
{
	return true;
}


