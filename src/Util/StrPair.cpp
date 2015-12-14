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
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/StrPair.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "Util/StrSplit.h"


AosStrPair::AosStrPair(
		const OmnString &contents, 
		const char field_sep,
		const char range_sep)
:
mCrtIdx(0),
mRangeSep(range_sep)
{
	bool finished;
	AosStrSplit::splitStrByChar(contents.data(), field_sep, mPairs, eMaxRanges, finished);
}


AosStrPair::~AosStrPair()
{
}


bool 
AosStrPair::nextPair(OmnString &left, OmnString &right, AosRundata *rdata)
{
	if (mCrtIdx >= mPairs.size()) return false;

	vector<OmnString> pair;	
	bool finished;
	AosStrSplit::splitStrByChar(mPairs[mCrtIdx].data(), mRangeSep, pair, 2, finished);
	if (pair.size() != 2 || pair[0] == "" || pair[1] == "" || !finished) 
	{
		AosSetErrorU(rdata, "invalid_range") << ": " << mPairs[mCrtIdx];
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}
		
	mCrtIdx++;
	left = pair[0];
	right = pair[1];
	return true;
}

