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
// 2014/01/16 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IndexMgr/ParalLevel.h"

#include "API/AosApi.h"
#include "IndexMgr/Ptrs.h"
#include "Jimo/Jimo.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "SEInterfaces/QueryReqObj.h"
#include "SEInterfaces/QueryTermObj.h"
#include "UtilTime/TimeUtil.h"

static int sgMaxPeriods = 1000;


bool
AosParalLevel::create(
		const AosRundataPtr &rdata, 
		const OmnString &name,
		const AosXmlTagPtr &def)
{
	// The XML format is:
	// 	<level AOSTAG_NUM_DAYS="ddd" AOSTAG_MAX_PERIODS="ddd" AOSTAG_PERIODS="ddd,ddd,..."/>
	
	mLevel = def->getAttrInt("level", -1);
	if (mLevel < 0)
	{
		AosSetEntityError(rdata, "paralleliil_invalid_level", "Parallel IIL", name) 
			<< def << enderr;
		return false;
	}

	mNumDays = def->getAttrInt(AOSTAG_NUM_DAYS, -1);
	if (mNumDays <= 0)
	{
		AosSetEntityError(rdata, "paralleliil_invalid_num_days", "Parallel IIL", name) 
			<< def << enderr;
		return false;
	}

	// for (u32 i=0; i<mLevels.size(); i++) aos_assert_r(mLevels[i] < level_size, false);
	mMaxPeriods = def->getAttrInt(AOSTAG_MAX_PERIODS, -1);

	OmnString periods = def->getAttrStr(AOSTAG_PERIODS);
	OmnScreen << "Level: " << mLevel << ", NumDays: " 
		<< mNumDays << ", MaPeriods: " 
		<< mMaxPeriods << ", Periods: " << periods << endl;
	if (periods != "")
	{
		vector<OmnString> pds;
		int nn = AosSplitStr(periods, ",", pds, sgMaxPeriods);
		aos_assert_rr(nn < sgMaxPeriods, rdata, false);
		for (u32 i=0; i<pds.size(); i++)
		{
			mPeriods.push_back(atoi(pds[i].data()));
			mNumPeriods++;
		}
	}

	return true;
}


bool
AosParalLevel::constructIILName(
		const AosRundataPtr &rdata,
		const OmnString &base_name, 
		const int epoch_day, 
		bool &found,
		int &period,
		bool &need_create,
		OmnString &iilname)
{
	period = AosTimeUtil::eMinDay;
	if (mMaxPeriods == -1)
	{
		// It is the top level
		period = (epoch_day / mNumDays) * mNumDays;
	}
	else
	{
		aos_assert_rr(mMaxPeriods > 0, rdata, false);

		// It is not a top level. Its period must be in the range:
		// 	[start_epochday, start_epochday + num_days * max_period]
		int nn = (AosGetCrtDay() / mNumDays) * mNumDays;
		int end_epochday = nn + mNumDays - 1;
		int start_epochday = nn - mNumDays * (mMaxPeriods-1);
		if (epoch_day < start_epochday || epoch_day > end_epochday)
		{
			found = false;
			return true;
		}

		aos_assert_rr(mPeriods.size() <= (u32)mMaxPeriods, rdata, false);
		period = (epoch_day / mNumDays) * mNumDays;
	}

	for (u32 i=0; i<mPeriods.size(); i++)
	{
		if (mPeriods[i] == period)
		{
			// The period exists. 
			iilname = base_name;
			iilname << "_" << mLevel << "_" << period;
			found = true;
			need_create = false;
			return true;
		}
	}

	// The IIL does not exist yet. Need to create it.
	found = true;
	need_create = true;
OmnScreen << "NNNNNNNNN: " << base_name << ":" << mLevel << ":" << mPeriods.size() << endl;
	if (mPeriods.size() == 0)
	{
		mPeriods.push_back(period);
		mNumPeriods = 1;
		iilname = constructIILName(base_name, period);
		return true;
	}

	for (u32 i=0; i<mPeriods.size(); i++)
	{
		if (period > mPeriods[i])
		{
			mPeriods.push_back(0);
			mNumPeriods++;
			for (u32 k=mPeriods.size()-1; k>i; k--)
			{
				mPeriods[k] = mPeriods[k-1];
			}
			mPeriods[i] = period;

			if (mMaxPeriods > 0)
			{
				aos_assert_rr(mPeriods.size() <= (u32)mMaxPeriods, rdata, false);
			}
			iilname = constructIILName(base_name, period);
			return true;
		}
	}

	mPeriods.push_back(period);
	mNumPeriods++;
	if (mMaxPeriods > 0)
	{
		aos_assert_rr(mPeriods.size() <= (u32)mMaxPeriods, rdata, false);
	}
	iilname = constructIILName(base_name, period);
	return true;
}


bool
AosParalLevel::getIILNames(
		const AosRundataPtr &rdata, 
		const int num_matched,
		const int *start_days,
		const int *end_days, 
		const OmnString &base_name,
		vector<OmnString> &iilnames)
{
	// This is to get all the IIL names for the time period:
	// 		[start_day, end_day]
	// for queries, where start_day and/or end_day can be
	// unbound.
	
	bool rslt;
	for (int m=0; m<num_matched; m++)
	{
		vector<int> selected_periods;
		if (start_days[m] == AosTimeUtil::eMinDay && end_days[m] == AosTimeUtil::eMaxDay)
		{
			// It is to select all the periods.
			rslt = getIILNames(mPeriods, base_name, iilnames);
			aos_assert_rr(rslt, rdata, false);
			continue;
		}
	
		if (start_days[m] == AosTimeUtil::eMinDay && end_days[m] != AosTimeUtil::eMaxDay)
		{
			// This is to look for values before a given day 'end_days[m]'. 
			for (u32 i=0; i<mPeriods.size(); i++)
			{
				if (mPeriods[i] <= end_days[m])
				{
					selected_periods.push_back(mPeriods[i]);
				}
			}
			rslt = getIILNames(selected_periods, base_name, iilnames);
			aos_assert_r(rslt, false);
			continue;
		}

		if (start_days[m] != AosTimeUtil::eMinDay && end_days[m] == AosTimeUtil::eMaxDay)
		{
			// This is to look for values after a given day,
			for (u32 i=0; i<mPeriods.size(); i++)
			{
				if (mPeriods[i] >= start_days[m])
				{
					selected_periods.push_back(mPeriods[i]);
				}
			}
			rslt = getIILNames(selected_periods, base_name, iilnames);
			aos_assert_r(rslt, false);
			continue;
		}
	
		for (u32 i=0; i<mPeriods.size(); i++)
		{
			if (end_days[m] >= mPeriods[i] && start_days[m] <= mPeriods[i])
			{
				selected_periods.push_back(mPeriods[i]);
			}
		}
	
		rslt = getIILNames(selected_periods, base_name, iilnames);
		aos_assert_r(rslt, false);
		continue;
	}
	return true;
}


bool
AosParalLevel::getIILNames(
		const vector<int> &selected_periods,
		const OmnString &base_name,
		vector<OmnString> &iilnames)
{
	for (u32 i=0; i<selected_periods.size(); i++)
	{
		OmnString nn = constructIILName(base_name, selected_periods[i]); 
		iilnames.push_back(nn);
	}

	return true;
}


bool
AosParalLevel::removePeriod(
		const AosRundataPtr &rdata, 
		const int period)
{
	if (mPeriods.size() <= 0)
	{
		AosSetError(rdata, "paraliil_internal_error") << enderr;
		return false;
	}

	for (u32 i=0; i<mPeriods.size(); i++)
	{
		if (period == mPeriods[i])
		{
			if (i == mPeriods.size()-1)
			{
				mPeriods.pop_back();
				return true;
			}

			for (u32 k=i; k<mPeriods.size()-2; k++)
			{
				mPeriods[k] = mPeriods[k+1];
			}
			mPeriods.pop_back();
			return true;
		}
	}

	AosSetError(rdata, "paraliil_internal_error") << enderr;
	return false;
}


