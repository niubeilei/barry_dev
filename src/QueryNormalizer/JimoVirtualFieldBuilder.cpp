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
// 2015/02/03 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Query/JimoVirtualFieldBuilder.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"


AosJimoVirtualFieldBuilder::AosJimoVirtualFieldBuilder()
{
}


AosJimoVirtualFieldBuilder::~AosJimoVirtualFieldBuilder()
{
}


bool
AosJimoVirtualFieldBuilder::buildVirtualField(
		AosRundata *rdata, 
		const OmnString &virtual_field_name,
		const OmnString &map_name,
		const vector<OmnString> &keys,
		const AosJoinType::E join_type,
		const vector<AosQueryCondPtr> &conds,
		const AosQueryCallerPtr &caller)
{
	// This function starts the process of building a
	// join virtual field. A join virtual field is implemented
	// through a join map and then a join field.
	mVirtualFieldName = virtual_field_name;
	mMapName = map_name;
	mKeys = keys;
	mJoinType = join_type;
	mConds = conds;
	mCaller = caller;
	if (isJoinMapAvailable(rdata))
	{
		// The join map is available. The 
		// join virtual field shall not be available.
		aos_assert_rr(!isJoinVirtualFieldAvailable(rdata), rdata, false);
		return buildJoinVirtualFieldPrivate(rdata);
	}

	return buildJoinMap(rdata);
}


bool
AosJimoVirtualFieldBuilder::buildJoinMap(AosRundata *rdata)
{
	// A join map has one or more key fields. It
	// reads in all the records. For each record, it
	// runs through the conditions, if any, and then
	// retrieves the values. The values are then 
	// reduced to an IIL (currently).
	// This is done by a job. 
	AosStreamEngineCallerPtr thisptr;
	mStatus = eBuildingMap;
	return Jimo::buildJoinMap(rdata, mMapName, 
				mKeyFields, mJoinType, mConds, thisptr);
}


bool
AosJimoVirtualFieldBuilder::buildVirtualField(AosRundata *rdata)
{
	AosStreamEngineCallerPtr thisptr;
	mStatus = eBuildingVirtualField;
	return Jimo::buildVirtualField(rdata, mVirtualFieldName, 
				mKeyFields, mJoinType, mConds, thisptr);
}


bool
AosJimoVirtualFieldBuilder::jobFinished(AosRundata *rdata)
{
	switch (mStatus)
	{
	case eBuildingMap:
		 if (!buildVirtualField(rdata))
		 {
			aos_assert_rr(mCaller, rdata, false);
			mCaller->callFailed(rdata);
			return true;
		 }
		 return true;

	case eBuildingVirtualField:
		 aos_assert_rr(mCaller, rdata, false);
		 mCaller->callFinished(rdata);
		 return true;

	default:
		 break;
	}

	OmnShouldNeverComeHere;
	return false;
}

