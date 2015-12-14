////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (u64, docid) and is sorted
// based on the u64 value. It has a companion IIL that keeps the
// same set of Docids but sorted based on docids. 
//
// Modification History:
// 03/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/U64U64Array.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Util1/MemMgr.h"

#include <stdlib.h>

static OmnMutex sgLock;

AosU64U64Array::AosU64U64Array()
:
mLock(OmnNew OmnMutex()),
mEntries(0),
mData(0),
mNumEntries(0),
mMemCap(0),
mIsDirty(false),
mCrtIdx(0)
{
}


AosU64U64Array::AosU64U64Array(const AosBuffPtr &buff)
:
mLock(OmnNew OmnMutex()),
mEntries((AosIILU64Entry*)buff->data()),
mData(buff),
mNumEntries(buff->dataLen()/sizeof(AosIILU64Entry)),
mMemCap(buff->dataLen()/sizeof(AosIILU64Entry)),
mIsDirty(false),
mCrtIdx(0)
{
}

AosU64U64Array::~AosU64U64Array()
{
	mEntries = NULL;
}


bool
AosU64U64Array::addValue(
		const u64 &name, 
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
	mLock->lock();
	if (mNumEntries == 0 || mNumEntries >= mMemCap)
	{
		bool rslt = expandMemoryPriv();
		aos_assert_rl(rslt, mLock, false);
	}
	mIsDirty = true;
	mEntries[mNumEntries].name = name;
	mEntries[mNumEntries].value = value;
	mNumEntries ++;
	mLock->unlock();
	return true;
}


bool
AosU64U64Array::expandMemoryPriv()
{
	// This function expands the space for the in-memory IIL. 
	// Note that it does not check whether the list is too big to expand. 
	u32 newsize = mMemCap + mMemCap;
	if (newsize == 0) newsize = 1000;
	//AosIILU64Entry * p = OmnNew AosIILU64Entry[newsize];
	AosBuffPtr buff = OmnNew AosBuff(newsize*sizeof(AosIILU64Entry), 0 AosMemoryCheckerArgs);
	if (mNumEntries > 0)
	{
		aos_assert_r(mMemCap > 0, false);
		memcpy(buff->data(), mEntries, sizeof(AosIILU64Entry) * mMemCap);
	}

	mEntries = (AosIILU64Entry*)buff->data();
	mMemCap = newsize;
	mData = buff;
	return true;
}


bool 
AosU64U64Array::sort()
{
	mLock->lock();
	qsort(mEntries, mNumEntries, sizeof(AosIILU64Entry), Comp);
	mLock->unlock();
	return true;
}

void
AosU64U64Array::clear()
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
AosU64U64Array::nextValue(u64 &name, u64 &value, bool &finished)
{
	mLock->lock();
	finished = false;
	if (mCrtIdx < 0 || (u64)mCrtIdx >= mNumEntries)
	{
		finished = true;
		mLock->unlock();
		return ;
	}

	name = mEntries[mCrtIdx].name;
	value = mEntries[mCrtIdx].value;
	mCrtIdx ++;
	mLock->unlock();
}

