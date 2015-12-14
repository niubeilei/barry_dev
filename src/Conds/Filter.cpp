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
// This is a utility to select docs.
//
// Modification History:
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Conds/Filter.h"

#include "Conds/Condition.h"
#include "SEInterfaces/DataRecordObj.h"
#include "SEInterfaces/ConditionObj.h"


AosFilter::AosFilter()
{
}


AosFilter::AosFilter(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	if (!config(conf, rdata))
	{
		OmnThrowException(rdata->getErrmsg());
		return;
	}
}


AosFilter::~AosFilter()
{
}


bool
AosFilter::config(const AosXmlTagPtr &conf, const AosRundataPtr &rdata)
{
	aos_assert_r(conf, false);

	AosXmlTagPtr tag = conf->getFirstChild(AOSTAG_CONDITION);
	if (!tag)
	{
		AosSetErrorU(rdata, "missing_condition:") << conf->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
	mCondition = AosConditionObj::getConditionStatic(tag, rdata);
	aos_assert_rr(mCondition, rdata, false);
	mIsBlackList = conf->getAttrBool(AOSTAG_IS_BLACKLIST, true);
	return true;
}


bool 
AosFilter::filterData(
		const char *record, 
		const int len, 
		const bool dft,
		const AosRundataPtr &rdata)
{
	aos_assert_rr(mCondition, rdata, dft);
	aos_assert_rr(record, rdata, dft);
	bool rslt = mCondition->evalCond(record, len, rdata);
	if (rslt)
	{
		if (mIsBlackList) return true;
		return false;
	}

	if (mIsBlackList) return false;
	return true;
}


bool 
AosFilter::filterData(const AosDataRecordObjPtr &record, const AosRundataPtr &rdata)
{
	aos_assert_rr(mCondition, rdata, true);
	aos_assert_rr(record, rdata, true);
	bool rslt = mCondition->evalCond(record, rdata);
	if (rslt) return mIsBlackList;
	return !mIsBlackList;
}

