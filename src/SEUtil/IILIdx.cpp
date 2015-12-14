////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2012/09/26	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/IILIdx.h"


#define	AOSIILIDX_IDXSEP ','


AosIILIdx::AosIILIdx()
{
	reset();
}
	

AosIILIdx::~AosIILIdx()
{
}


void
AosIILIdx::reset()
{
	for(int i=0; i<eMaxNumLevels; i++)
	{
		mIdx[i] = eStartIndex;
	}
}


i64
AosIILIdx::getIdx(const int level) const 
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, eInvalidIndex);
	aos_assert_r(mIdx[level] == eStartIndex || mIdx[level] == eInvalidIndex
		|| (mIdx[level] >= 0 && mIdx[level] < eMaxEntries), eInvalidIndex);
	return mIdx[level];
}


bool
AosIILIdx::setIdx(const int level, const int64_t &idx)
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	aos_assert_r((idx >= 0 && idx < eMaxEntries) || idx == eInvalidIndex, false);
	mIdx[level] = idx;
	return true;
}


bool
AosIILIdx::isStart(const int level) const
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	return (mIdx[level] == eStartIndex);
}
	

bool
AosIILIdx::isValid(const int level) const
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	return (mIdx[level] >= 0 && mIdx[level] < eMaxEntries);
}
	

bool
AosIILIdx::notFound(const int level) const
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	for (int i=0; i<=level; i++)
	{
		if(mIdx[i] == eInvalidIndex) return true;
	}
	return false;
}


bool
AosIILIdx::setStart(const int level)
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	mIdx[level] = eStartIndex;
	return true;
}


bool
AosIILIdx::setInvalid()
{
	for(int i = 0;i < eMaxNumLevels;i++)
	{
		setInvalid(i);
	}
	return true;
}

bool
AosIILIdx::setInvalid(const int level)
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	mIdx[level] = eInvalidIndex;
	return true;
}
	

bool
AosIILIdx::next(const int level, const bool reverse)
{
	aos_assert_r(level >= 0 && level < eMaxNumLevels, false);
	if (reverse)
	{
		if (mIdx[level] <= 0) 
		{
			mIdx[level] = eInvalidIndex;
		}
		else
		{
			mIdx[level]--;
		}
	}
	else
	{
		if (mIdx[level] >= eMaxEntries)
		{
			mIdx[level] = eInvalidIndex;
		}
		else
		{
			mIdx[level]++;
		}
	}
	return true;
}


OmnString
AosIILIdx::toString() const 
{
	OmnString ss;
	for(int i=0; i<eMaxLevelIndex; i++)
	{
		ss << mIdx[i] << AOSIILIDX_IDXSEP;
	}
	ss << mIdx[eMaxLevelIndex];
	return ss;
}
	

bool
AosIILIdx::setValue(const OmnString &value) 
{
	bool finished = false;
	vector<OmnString> substr;
	int n = AosStrSplit::splitStrByChar(
		value.data(), AOSIILIDX_IDXSEP, substr, eMaxNumLevels, finished);
	aos_assert_r(finished, false);
	aos_assert_r(n == (int)substr.size(), false);
	aos_assert_r(substr.size() == eMaxNumLevels, false);
	int64_t idx;
	for(int i=0; i<eMaxNumLevels; i++)
	{
		idx = substr[i].parseInt64(eStartIndex);
		aos_assert_r(idx == eInvalidIndex || idx == eStartIndex
			|| (idx >= 0 && idx < eMaxEntries), false);
		mIdx[i] = idx;
	}
	return true;
}


int
AosIILIdx::cmpPos(const AosIILIdx &idx1, const AosIILIdx &idx2)
{
	// This function compares idx1 and idx2:
	// 	1. Return negative if idx1 < idx2
	// 	2. Return 0 if idx1 == idx2
	// 	3. Return positive if idx1 > idx2
	for(int level = eMaxLevelIndex; level >= 0; level--)
	{
		if(idx1.mIdx[level] != idx2.mIdx[level])
		{
			return idx1.mIdx[level] - idx2.mIdx[level];
		}
	}
	return 0;
}

int			
AosIILIdx::cmpPos(const AosIILIdx &idx1, const AosIILIdx &idx2, const int cur_level)
{
	for(int level = eMaxLevelIndex; level >= cur_level; level--)
	{
		if(idx1.mIdx[level] != idx2.mIdx[level])
		{
			return idx1.mIdx[level] - idx2.mIdx[level];
		}
	}
	return 0;
}
	

