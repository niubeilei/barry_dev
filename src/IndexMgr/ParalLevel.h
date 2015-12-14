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
// 2014/01/11 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_IndexMgr_ParalLevel_h
#define Aos_IndexMgr_ParalLevel_h

#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;

class AosParalLevel 
{
private:
	int				mLevel;
	int				mNumDays;
	int				mMaxPeriods;
	int				mWaitSec;
	vector<int>		mPeriods;
	int				mNumPeriods;
	OmnString		mName;

public:
	AosParalLevel()
	{
		mLevel = -1;
		mNumDays = -1;
		mMaxPeriods = -1;
		mWaitSec = -1;
		mNumPeriods = 0;
	}

	AosParalLevel(const OmnString &name, const int wait_sec)
	{
		mLevel = -1;
		mNumDays = -1;
		mMaxPeriods = -1;
		mWaitSec = wait_sec;
		mNumPeriods = 0;
		mName = name;
	}

	int getLevel() const {return mLevel;}
	int getMaxPeriods() const {return mMaxPeriods;}
	bool isValid() const
	{
		return (mLevel>=0 && mNumDays>0);
	}

	int getLevelSize()
	{
		return mNumDays * mMaxPeriods;
	}

	bool create(const AosRundataPtr &rdata, 
				const OmnString &name, 
				const AosXmlTagPtr &def);

	bool getIILNames(
				const AosRundataPtr &rdata, 
				const int num_matched,
				const int *start_days,
				const int *end_days, 
				const OmnString &base_name,
				vector<OmnString> &iilnames);

	bool getIILNames(
				const vector<int> &periods,
				const OmnString &base_name,
				vector<OmnString> &iilnames);

	bool constructIILName(
				const AosRundataPtr &rdata,
				const OmnString &base_name, 
				const int epoch_day, 
				bool &found,
				int &period,
				bool &need_create,
				OmnString &iilname);

	bool removePeriod(
				const AosRundataPtr &rdata, 
				const int period);

	inline OmnString constructIILName(
				const OmnString &base_name, 
				const int period)
	{
		OmnString iilname = base_name;
		iilname << "_" << mLevel << "_" << period;
		return iilname;
	}
};
#endif

