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
// This is the semantic object for "struct aos_hashtable".  
//
// Modification History:
// 12/10/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SemanticObj/SOAosHashtab.h"

#include "alarm/Alarm.h"
#include "aosUtil/HashTable.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"


AosHashtabSO::AosHashtabSO(struct aos_hashtable *table)
:
AosHashtabInterface("AosHashtabSO", AosSOType::eAosHashtab, table),
mTable(table),
mLock(OmnNew OmnMutex()),
mTableSize(table->size)
{
	aos_assert(mTable);
	mBucketLens = OmnNew u16[mTableSize];
	aos_assert(mBucketLens);
	memset(mBucketLens, 0, sizeof(u16) * mTableSize);
}


AosHashtabSO::~AosHashtabSO()
{
	OmnDelete mBucketLens;
	mBucketLens = 0;
}


u32 	
AosHashtabSO::getNoe() const
{
	aos_assert_r(mTable, 0);
	return aos_hashtab_get_noe(mTable);
}


u32		
AosHashtabSO::getTableSize() const
{
	return mTable->size;
}


u32		
AosHashtabSO::maxAllowedElem() const
{
	return mTable->max;
}


u32		
AosHashtabSO::averageBucketLen() const
{
	u32 total = 0;
	mLock->lock();
	for (u32 i=0; i<mTableSize; i++)
	{
		total += mBucketLens[i];
	}
	mLock->unlock();
	return total/mTableSize;
}


u32		
AosHashtabSO::bucketUsagePct() const
{
	u32 used = 0;
	mLock->lock();
	for (u32 i=0; i<mTableSize; i++)
	{
		if (mBucketLens[i]) used++;
	}
	mLock->unlock();
	return used * 100 / mTableSize;
}


u32		
AosHashtabSO::maxBucketLen() const
{
	u32 length = 0;
	mLock->lock();
	for (u32 i=0; i<mTableSize; i++)
	{
		if (length < mBucketLens[i]) length = mBucketLens[i];
	}
	mLock->unlock();
	return length;
}


u32	
AosHashtabSO::averageQueryTime() const
{
	return mQueryTime/mNumQueries;
}


u32 	
AosHashtabSO::maxQueryTime() const
{
	return mMaxQueryTime;
}


u32		
AosHashtabSO::averageAddTime() const
{
	return mAddTime/mNumAdds;
}


u32		
AosHashtabSO::maxAddTime() const
{
	return mMaxAddTime;
}


u32		
AosHashtabSO::averageDeleteTime() const
{
	return mDeleteTime/mNumDeletes;
}


u32		
AosHashtabSO::maxDeleteTime() const
{
	return mMaxDeleteTime;
}


u32		
AosHashtabSO::totalQueries() const
{
	return mNumQueries;
}


u32		
AosHashtabSO::totalAdditions() const
{
	return mNumAdds;
}


u32		
AosHashtabSO::totalDeletions() const
{
	return mNumDeletes;
}



