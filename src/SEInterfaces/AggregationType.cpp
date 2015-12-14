////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 2014/04/05	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/AggregationType.h"

#include "Thread/Mutex.h"
#include <map>
using namespace std;

AosAggregationType::E
AosAggregationType::toEnum(const OmnString &id)
{
	if(id == AOS_AGR_SUM)	return eSum;
	if(id == AOS_AGR_AGGR_CONCAT)	return eAggr_concat;
	if(id == AOS_AGR_COUNT)	return eCount;
	if(id == AOS_AGR_MAX)	return eMax;
	if(id == AOS_AGR_MIN)	return eMin;
	if(id == AOS_AGR_ACCU_COUNT)	return eAccuCount;
	if(id == AOS_AGR_ACCU_SUM)	return eAccuSum;
	if(id == AOS_AGR_ACCU_MAX)	return eAccuMax;
	if(id == AOS_AGR_ACCU_MIN)	return eAccuMin;
	if(id == AOS_AGR_DIST_COUNT)	return eDistCount;
	if(id == AOS_AGR_DIST_COUNT_QUERY)	return eDistCountQuery;
	if(id == AOS_AGR_DIST_COUNT_CREATE)	return eDistCountCreate;
	
	return eInvalid;
}

OmnString
AosAggregationType::toStr(const E cond)
{
	if(cond == eSum)	return AOS_AGR_SUM; 
	if(cond == eAggr_concat)	return AOS_AGR_AGGR_CONCAT; 
	if(cond == eCount)	return AOS_AGR_COUNT; 
	if(cond == eMax)	return AOS_AGR_MAX; 
	if(cond == eMin)	return AOS_AGR_MIN; 
	if(cond == eAccuCount)	return AOS_AGR_ACCU_COUNT; 
	if(cond == eAccuSum)	return AOS_AGR_ACCU_SUM; 
	if(cond == eAccuMax)	return AOS_AGR_ACCU_MAX; 
	if(cond == eAccuMin)	return AOS_AGR_ACCU_MIN; 
	if(cond == eDistCount)	return AOS_AGR_DIST_COUNT; 
	if(cond == eDistCountQuery)	return AOS_AGR_DIST_COUNT_QUERY; 
	if(cond == eDistCountCreate) return AOS_AGR_DIST_COUNT_CREATE; 

	return "";
}

