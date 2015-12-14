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
// It uses a time plan to name IILs. 
// 1. There are a number of levels
// 2. Each level has a maximum number of periods. When the maximum
//    number is reached, the oldest one is merged to its higher level
//    period.
// 3. A period has a number of days
// 4. When the time comes to merge a period, it is called Trigger Time.
// 5. There is a Merge Wait Time. It will not start merging until
//    the merge wait time passes. Merge wait time is defined at 
//    the time plan.
// 6. There is a maximum merge time. Before the maximum merge time 
//    reaches, the merge is considered not finished, regardless of
//    whether it actually finished or not. 
//    Maximum merge time is defined per level.
//
// 2014/01/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IndexMgr/ParalIIL.h"

#include "API/AosApi.h"
#include "IndexMgr/Ptrs.h"
#include "IndexMgr/IndexMgr.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/QueryTermObj.h"
#include "UtilTime/TimeUtil.h"


AosParalIIL::AosParalIIL()
:
mLock(OmnNew OmnMutex())
{
	mWaitSec = eDftWaitSec;
}


AosParalIIL::~AosParalIIL()
{
}


bool
AosParalIIL::config(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &defs)
{
	// 	<defs 
	// 		AOSTAG_TIME_FIELDNAME="xxx"
	// 		AOSTAG_IILNAME="xxx"
	// 		AOSTAG_NAME="xxx">
	// 		<AOSTAG_TIME_PLAN ...>
	// 			<level level="ddd" num_days="ddd" num_periods="ddd" merge_time="ddd"/>
	// 			<level level="ddd" num_days="ddd" num_periods="ddd" merge_time="ddd"/>
	// 			...
	// 		</AOSTAG_TIME_PLAN>
	// 	<defs>
	aos_assert_r(rdata && defs, false);
	AosXmlTagPtr timeplan_xml = defs->getFirstChild(AOSTAG_TIME_PLAN);
	aos_assert_r(timeplan_xml, false);
	mName = defs->getAttrStr(AOSTAG_NAME);
	mIILName = defs->getAttrStr(AOSTAG_IILNAME);
	if (mIILName == "")
	{
		AosSetEntityError(rdata, "paraliil_missing_iilname", "IndexDoc", mName)
			<< defs << enderr;
		return false;
	}

	mTimeFieldname = defs->getAttrStr(AOSTAG_TIME_FIELDNAME);
	if (mTimeFieldname == "")
	{
		AosSetEntityError(rdata, "paraliil_missing_time_field", "IndexDoc", mName)
			<< defs << enderr;
		return false;
	}

	mWaitSec = timeplan_xml->getAttrInt("wait_time", -1);
	if (mWaitSec <= 0)
	{
		mWaitSec = eDftWaitSec;
	}

	AosXmlTagPtr level_xml = timeplan_xml->getFirstChild();
	while (level_xml)
	{
		AosParalLevel level(mName, mWaitSec);
		if (!level.create(rdata, mName, level_xml)) return false;

		int level_num = level.getLevel();
		if (level_num < 0 || level_num > AosIndexMgr::eMaxLevels)
		{
			AosSetEntityError(rdata, "paralleliil_invalid_level", "IndexDoc", mName) 
				<< defs << enderr;
		}

		if (!level.isValid())
		{
			AosSetEntityError(rdata, "paraliil_invalid_level", "IndexDoc", mName)
				<< defs << enderr;
			return false;
		}

		if (mLevels.size() <= (u32)level_num)
		{
			mLevels.resize(level_num+1);
		}
		if (mLevels[level_num].isValid())
		{
			AosSetEntityError(rdata, "paraliil_duplicate_level", "IndexDoc", mName)
				<< defs << enderr;
			return false;
		}

		mLevels[level_num] = level;
		level_xml = timeplan_xml->getNextChild();
	}

	// Check whether all levels are defined.
	for (u32 i=0; i<mLevels.size(); i++)
	{
		if (!mLevels[i].isValid())
		{
			AosSetEntityError(rdata, "paralleliil_missing_level", "IndexDoc", mName) 
				<< "Level: " << i << defs << enderr;
			return false;
		}
	}

	if (mLevels.size() <= 0)
	{
		AosSetEntityError(rdata, "paralleliil_missing_time_plan", "IndexDoc", mName) 
			<< defs << enderr;
		return false;
	}

	for (u32 i=0; i<mLevels.size()-1; i++)
	{
		if (mLevels[i].getMaxPeriods() <= 0)
		{
			AosSetEntityError(rdata, "paraliil_invalid_max_periods", "IndexDoc", mName)
				<< "Level: " << i << defs << enderr;
		}

		if (i+1 < mLevels.size()-1)
		{
			if (mLevels[i].getLevelSize() >= mLevels[i+1].getLevelSize())
			{
				AosSetEntityError(rdata, "paralleliil_invalid_levels", "IndexDoc", mName) 
					<< defs << enderr;
				return false;
			}
		}
	}

	return true;
}


bool
AosParalIIL::getIILName(
		const AosRundataPtr &rdata, 
		const OmnString &base_name,
		const int epoch_day, 
		OmnString &iilname, 
		int &level,
		int &period, 
		bool &need_create)
{
	// This function is normally called from a data proc. 
	// It uses 'time' to determine the name of the IIL to 
	// which the contents are inserted.
	
	bool found;
	mLock->lock();
	for (u32 i=0; i<mLevels.size(); i++)
	{
		if (!mLevels[i].constructIILName(rdata, base_name, 
					epoch_day, found, period, need_create, iilname)) 
		{
			mLock->unlock();
			return false;
		}
		
		if (found) 
		{
			level = i;
			mLock->unlock();
			return true;
		}
	}
	
	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}


bool 
AosParalIIL::getIILNames(
		const AosRundataPtr &rdata, 
		const AosQueryReqObjPtr &query_req, 
		vector<OmnString> &iilnames)
{
	// This function retrieves the name of the time field 
	// from 'index_doc'. It then checks whether there is 
	// a condition on the time field in 'query_req'. If no, 
	// it returns all the IIL names for the parallel IIL.
	// Otherwise, it returns the names of the member IILs
	// that covers the condition.
	iilnames.clear();
	aos_assert_rr(query_req, rdata, false);

	int start_days[2], end_days[2];
	int num_matched;
	bool rslt = query_req->getTimeCond(rdata, mTimeFieldname, num_matched, start_days, end_days);
	if (!rslt) return false;

	mLock->lock();
	for (u32 i=0; i<mLevels.size(); i++)
	{
		mLevels[i].getIILNames(rdata, num_matched, start_days, end_days, mIILName, iilnames);
	}
	mLock->unlock();
	return true;
}


bool
AosParalIIL::removePeriod(
		const AosRundataPtr &rdata, 
		const int level,
		const int period)
{
	if (level < 0 || (u32)level>= mLevels.size())
	{
		AosSetError(rdata, "paraliil_invalid_level") << level << enderr;
		return false;
	}

	mLock->lock();
	bool rslt = mLevels[level].removePeriod(rdata, period);
	mLock->unlock();
	return rslt;
}

