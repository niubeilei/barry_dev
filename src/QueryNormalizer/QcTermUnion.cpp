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
// A union is in the following format:
// 		Subquery (QcTerm)
// 		union [all]
// 		Subquery (QcTerm)
// 		union [all]
// 		...
//
// In other word, it is a number of tables, which can be persisted tables
// or temporary tables. 
//
// Persisted Tables
// ----------------
//
//
// Modification History:
// 2015/02/05 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryNormalizer/QcTermUnion.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosQcTermUnion::AosQcTermUnion()
{
}


AosQcTermUnion::~AosQcTermUnion()
{
}


bool
AosQcTermUnion::normalize(AosRundata *rdata)
{
	return false;
}


bool
AosQcTermUnion::normalizeSubquery(
		AosRundata *rdata, 
		const AosQueryCallerPtr &caller)
{
	// Simple term does not need to do anything with
	// normalizing subqueries.
	return true;	
}


bool 
AosQcTermUnion::isNormalizeSubqueryFinished()
{
	return true;
}


bool 
AosQcTermUnion::normalizeSubqueryFinished()
{
	OmnShouldNevetComeHere;
	return false;
}


bool 
AosQcTermUnion::normalizeSubqueryFailed()
{
	OmnShouldNeverComeHere;
	return false;
}


bool
AosQcTermUnion::normalizeJoin(AosRundata *rdata)
{
	return false;
}


bool
AosQcTermUnion::normalizeCompoundJoin(AosRundata *rdata)
{
}


bool
AosQcTermUnion::normalizeSimpleJoin(
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
AosQcTermUnion::callFinished(
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
AosQcTermUnion::createVirtualField(AosRundata *rdata)
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
AosQcTermUnion::isNormalizeFinished() const
{
	return true;
}


