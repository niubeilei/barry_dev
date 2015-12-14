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
// A UnionAll is in the following format:
// 		Subquery (QcTerm)
// 		UnionAll all
// 		Subquery (QcTerm)
// 		UnionAll all
// 		...
//
// Union all can be used in one of the following was:
//
// 1. Used in a Root Query
// -----------------------
// 	select * from
// 		subquery
// 		unionall
// 		subquery
// 		unionall
// 		...
// 	where <conds>;
//
// 2. Used by a Value Set
// ----------------------
// It can be either in a selected fields or in a condition.
// 	select 
// 		(
// 			subquery
// 			unionall
// 			subquery
// 			unionall
// 			...
// 		), 
// 		f2
// 	from t
// 	where <conds>
//
// Or:
// 	select f1, f2, ...
// 	from t
// 	where
// 		f3 in 
// 		(
// 			subquery
// 			unionall
// 			subquery
// 			unionall
// 			...
// 		) AND
// 		<conds>;
//
// 3. Used in Joins
// ----------------
//  select * from t
//  where
//  	field in 
//  	(
//  		select f 
//  		from
//  		(
//  			subquery 
//  			unionall
//  			subquery
//  			unionall
//  			...
// 			) 
// 			where <conds>
// 		)
// 		<other_conds>;
//
//	Used in From 
//	------------
//	Conditions in each subquery are moved to outside. The original 
//	query can be re-written as:
//
//		select ... 
//		where
//		   <conds1> OR
//		   <conds2> OR
//		   ...;
// 
// In addition, virtual fields will be created on each table as needed
// to translate from outside selected expressions to per-table 
// selected expressions. 
//
// Have Joins
// ----------
// Having joins or not is important. If the table joins with others, 
// it is processed differently.
//
// All subqueries must be normalized. 
//
// Modification History:
// 2015/02/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QcTermUnionAll.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQcTermUnionAll::AosQcTermUnion()
{
}


AosQcTermUnionAll::~AosQcTermUnion()
{
}


bool
AosQcTermUnionAll::normalize(
		AosRundata *rdata, 
		bool &call_finished,
		const AosBSONPtr &call_data,
		const AosQueryCallerPtr &caller)
{
	mCallData = call_data;
	mCaller = caller;
	determineHowBeingUsed(rdata);
	switch (mUsedAs)
	{
	case eValueset:
		 return normalizeInValueset(rdata, call_finished, call_data, caller);

	case eRootQuery:
		 return normalizeAsRootQuery(rdata, call_finished, call_data, caller);

	case eJoin:
		 return normalizeInJoins(rdata, call_finished, call_data, caller);

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermUnionAll::determineHowUsed(AosRundata *rdata)
{
	// This union can be used in one of the following:
	// 		eInValueset,
	//		eRootQuery,
	//		eInJoins
	
	// Check whether it is used as the root query
	mUsedAs = eInvalidUsedAs;
	if (mRootQuery == mCrtQuery)
	{
		// It is the root query. This term must be one of the 
		// from tables.
		vector<AosQcTermPtr> tables = mCrtQuery->getFromTables(rdata);
		if (tables.size() == 1)
		{
			aos_assert_rr(tables[0] == this, rdata, false);
			mUsedAs = eRootQuery;
			return true;
		}

		// This table must be in one of 'tables'. Otherwise, it is an error
		// This is an extra check.
		bool found = false;
		for (u32 i=0; i<tables.size(); i++)
		{
			if (tables[i] == this)
			{
				found = true;
			}
		}

		if (!found)
		{
			// This term is not in the from-clause. This is not
			// possible.
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}

		// Since there are multiple tables, this term nust hae alias. 
		// Otherwise, it is an error.
		if (mAlias == "")
		{
			// It is not used in joins.
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}

		// The query uses more than one table, this table has alias, 
		// and it should be in a join. This table shall be normalized
		// into a single table. This table then joins with other
		// tables.
		mUsedAs = eRootQuery;
		return true;
	}

	// Check whether it is used in a valueset. A valueset is in
	// the form:
	// 	1. It is one of the selected field:
	// 		select ...,
	// 			(select f1 from (select ...) unionall (select ...)),
	// 			...
	// 	2. It is used in a condition:
	// 		select ...
	// 		where
	// 		   expr in (select ... unionall select ...);
	
}


bool
AosQcTermUnionAll::normalizeInValueset(
		AosRundata *rdata, 
		bool &call_finished,
		const AosBSONPtr &call_data,
		const AosQueryCallerPtr &caller)
{
	AosQueryCallerPtr thisptr(this, false);
	for (u32 i=0; i<mSubqueries.size(); i++)
	{
		AosBSONPtr bson = OmnNew AosBSON();
		bson->appendFieldInt(AosFN::eCallType, eNormalize);
		bson->appendFieldInt(AosFN::eIndex, i);
		if (!mSubqueries[i]->normalize(rdata, thisptr), bson)
		{
			mStatus = eFailed;
			mResponded = true;
			call_finished = true;
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}
	}

	return checkCallFinished(rdata);
}


bool
AosQcTermUnionAll::normalizeAsRootQuery(
		AosRundata *rdata, 
		bool &call_finished,
		const AosBSONPtr &call_data,
		const AosQueryCallerPtr &caller)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosQcTermUnionAll::normalizeAsSubquery(
		AosRundata *rdata, 
		bool &call_finished,
		const AosBSONPtr &call_data,
		const AosQueryCallerPtr &caller)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosQcTermUnionAll::normalizeInJoins(
		AosRundata *rdata, 
		bool &call_finished,
		const AosBSONPtr &call_data,
		const AosQueryCallerPtr &caller)
{
	OmnNotImplementedYet;
	return false;
}


bool
AosQcTermUnionAll::callFinished(
		AosRundata *rdata, 
		const AosBSONPtr &call_data)
{
	CallType call_type = (CallType)bson->getInt(AosFN::eCallType, eInvalidCallType);
	int idx;
	switch (call_type)
	{
	case eNormalize:
		 idx = bson->getInt(AosFN::eIndex, -1);
		 if (idx < 0 || idx >= mSubqueries.size())
		 {
			 AosLogError(rdata, false, "internal_error") << enderr;
			 return false;
		 }
		 mCallStatus[idx] = true;
		 return checkNormalizeFinish(rdata);

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermUnionAll::checkNormalizeFinish(AosRundata *rdata)
{
	if (mResponded) return true;

	for (u32 i=0; i<mCallStatus.size(); i++)
	{
		if (!mCallStatus[i]) return true;
	}

	// Call finished
	mResponded = true;
	AosBSONPtr call_data = mCallData;
	mCallData = 0;
	if (!moveConditions(rdata))
	{
		AosLogError(rdata, false, "internal_error") << enderr;
		return mCaller->callFailed(rdata, call_data);
	}

	return mCaller->callFinished(rdata, call_data);
}


bool
AosQcTermUnionAll::moveConditions(AosRundata *rdata)
{
	// This function is called when it finishes all the 
	// subqueries. It needs to move conditions from each
	// subquery to this term. These conditions are ORed.
	
	mConvertedConds = OmnNew AosQueryCondOR();
	for (u32 i=0; i<mSubqueries.size(); i++)
	{
		vector<AosQueryCondPtr> conds = mSubqueries[i]->getConds(rdata);
		if (conds.size() == 0)
		{
			// This is not allowed. If the subquery selects in the global
			// space, it may have no tables but must have conditions. If
			// the subquery has no conditions, it must select from a table, 
			// which needs to add a Table Membership Condition. 
			AosLogError(rdata, true, "internal_error") << enderr;
			return false;
		}

		try
		{
			AosQueryCondAndPtr andCond = OmnNew AosQueryCondAnd(rdata, conds);
			mConvertedConds->addAndCond(rdata, andCond);
		}

		catch (...)
		{
			AosLogError(rdata, false, "internal_error") << enderr;
			return false;
		}
	}

}


bool
AosQcTermUnionAll::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	// Simple term does not need to do anything with
	// normalizing subqueries.
	return true;	
}


bool 
AosQcTermUnionAll::isNormalizeSubqueryFinished()
{
	return true;
}


bool 
AosQcTermUnionAll::normalizeSubqueryFinished()
{
	OmnShouldNevetComeHere;
	return false;
}


bool 
AosQcTermUnionAll::normalizeSubqueryFailed()
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermUnionAll::normalizeJoin(AosRundata *rdata)
{
	return false;
}


bool
AosQcTermUnionAll::normalizeCompoundJoin(AosRundata *rdata)
{
}


bool
AosQcTermUnionAll::normalizeSimpleJoin(
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
AosQcTermUnionAll::callFinished(
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
AosQcTermUnionAll::createVirtualField(AosRundata *rdata)
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
AosQcTermUnionAll::isNormalizeFinished() const
{
	return true;
}


