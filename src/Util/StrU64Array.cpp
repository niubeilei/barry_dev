////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/StrU64Array.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util1/MemMgr.h"

#include <stdlib.h>

static OmnMutex sgLock;



AosStrU64Array::AosStrU64Array(const bool stable)
:
mLock(OmnNew OmnMutex()),
mStable(stable),
mEntries(0),
mData(0),
mNumEntries(0),
mMemCap(0),
mIsDirty(false),
mCrtIdx(0)
{
	mComp = OmnNew AosCompareFun1();
}


AosStrU64Array::AosStrU64Array(const AosCompareFun1Ptr &comp, const bool stable)
:
mLock(OmnNew OmnMutex()),
mComp(comp),
mStable(stable),
mEntries(0),
mData(0),
mNumEntries(0),
mMemCap(0),
mIsDirty(false),
mCrtIdx(0)
{
}


AosStrU64Array::AosStrU64Array(
		const AosBuffPtr &buff,
		const AosCompareFun1Ptr &comp, 
		const bool stable)
:
mLock(OmnNew OmnMutex()),
mComp(comp),
mStable(stable),
mEntries((AosIILStrEntry*)buff->data()),
mData(buff),
mNumEntries(buff->dataLen()/sizeof(AosIILStrEntry)),
mMemCap(buff->dataLen()/sizeof(AosIILStrEntry)),
mIsDirty(false),
mCrtIdx(0)
{
}

AosStrU64Array::~AosStrU64Array()
{
	mEntries = NULL;
}


bool
AosStrU64Array::addValue(
		const OmnString &name, 
		const u64 &value,
		AosRundata *rdata)
{
	// This function does the following:
	//  1. Retrieve the entry [name, value] by 'name'. 
	//  2. If the entry does not exist in the iil, add the entry [name, value]
	//  3. If the entry is found, add 'value' to the entry's value.
	// This function should always success. 
	//
	// After adding, if the entry has too many entries, create an IIL transaction
	// and sends it to the IILMgr to process it. It then clear some of the 
	// old entries from memory.
	int len = name.length();
	aos_assert_rr(len < AosIILStrEntry::eMaxStrLen - 1, rdata, false);
	mLock->lock();
	if (mNumEntries == 0 || mNumEntries >= mMemCap)
	{
		bool rslt = expandMemoryPriv();
		aos_assert_rl(rslt, mLock, false);
	}
	mIsDirty = true;
	strncpy(mEntries[mNumEntries].name, name.data(), len);
	mEntries[mNumEntries].name[len] = 0;
	mEntries[mNumEntries].value = value;
	mNumEntries ++;
	mLock->unlock();
	return true;
}


bool
AosStrU64Array::addValue(
		const char *name, 
		const int len,
		const u64 &value,
		AosRundata *rdata)
{
	// This function does the following:
	//  1. Retrieve the entry [name, value] by 'name'. 
	//  2. If the entry does not exist in the iil, add the entry [name, value]
	//  3. If the entry is found, add 'value' to the entry's value.
	// This function should always success. 
	//
	// After adding, if the entry has too many entries, create an IIL transaction
	// and sends it to the IILMgr to process it. It then clear some of the 
	// old entries from memory.
	aos_assert_rr(len < AosIILStrEntry::eMaxStrLen - 1, rdata, false);
	mLock->lock();
	if (mNumEntries == 0 || mNumEntries >= mMemCap)
	{
		bool rslt = expandMemoryPriv();
		aos_assert_rl(rslt, mLock, false);
	}
	mIsDirty = true;
	strncpy(mEntries[mNumEntries].name, name, len);
	mEntries[mNumEntries].name[len] = 0;
	mEntries[mNumEntries].value = value;
	mNumEntries ++;
	mLock->unlock();
	return true;
}


bool
AosStrU64Array::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = mMemCap + mMemCap;
	if (newsize == 0) newsize = 4000;
	//AosIILStrEntry * p = OmnNew AosIILStrEntry[newsize];
	AosBuffPtr buff = OmnNew AosBuff(newsize*sizeof(AosIILStrEntry), 0 AosMemoryCheckerArgs);
	if (mNumEntries > 0)
	{
		aos_assert_r(mMemCap > 0, false);
		memcpy(buff->data(), mEntries, sizeof(AosIILStrEntry) * mMemCap);
	}

	mEntries = (AosIILStrEntry*)buff->data();
	mMemCap = newsize;
	mData = buff;
	return true;
}


bool 
AosStrU64Array::sort()
{
	mLock->lock();
	//qsort(mEntries, mNumEntries, sizeof(AosIILStrEntry), Comp);
	if (!mStable)
	{
		std::sort<AosIILStrEntry*, const AosCompareFun1 &>
			(mEntries, mEntries+mNumEntries, *mComp);
	}
	else
	{
		stable_sort<AosIILStrEntry*, const AosCompareFun1 &>
			(mEntries, mEntries+mNumEntries, *mComp);
	}
	mLock->unlock();
	return true;
}

void
AosStrU64Array::clear()
{
	mLock->lock();
	mCrtIdx = 0;
	mNumEntries = 0;
	mMemCap = 0,
	mIsDirty = false;
	mEntries = 0;
	mLock->unlock();
}

void
AosStrU64Array::nextValue(OmnString &name, u64 &value, bool &finished)
{
	mLock->lock();
	finished = false;
	if (mCrtIdx < 0 || (u64)mCrtIdx >= mNumEntries)
	{
		finished = true;
		mLock->unlock();
		return ;
	}

	name = OmnString(mEntries[mCrtIdx].name);
	value = mEntries[mCrtIdx].value;
	mCrtIdx ++;
	mLock->unlock();
}

