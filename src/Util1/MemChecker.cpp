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
// This class is used to monitor the use of memory. Here is how to use it.
// 1. Whenver a memory is created, call "addMem(mem, size)"
// 2. Whenever a memory is removed, call "removeMem(mem, size)"
// 3. Whenever a memory is used, call "checkMem(mem, size)"
//
// Modification History:
// 2011/01/22	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util1/MemChecker.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"



AosMemChecker::AosMemChecker()
	:
mLock(OmnNew OmnMutex()),
mCheckFlag(true)
{
//	mMemory = OmnNew u32[eInitSize];
//	mSize = OmnNew u32[eInitSize];
	mBlocks = OmnNew AosMemBlk[eInitSize];
	mCrtSize = eInitSize;
	mNumEntries = 0;
}


AosMemChecker::~AosMemChecker()
{
}


bool	
AosMemChecker::addMemPriv(void *mem, const int size,const OmnString &file,const int &line)
{
	// Check whether it overlaps in any existing one
	aos_assert_r(size > 0, false);
	mLock->lock();
	unsigned long addr = (unsigned long)mem;
	
	// expand memory if needed
	if(mNumEntries >= (int)mCrtSize)
	{
		mCrtSize += eInitSize;
		OmnDelete []mBlocks;
		mBlocks = OmnNew AosMemBlk[mCrtSize];
	} 

	AosMemBlk block(mem,size,file,line);
	// If it is the first one, just add it.
	if (mNumEntries == 0)
	{
		mBlocks[0] = block;
		mNumEntries ++;
		mLock->unlock();
		return true;
	}

	// Check whether to prepend
	int pos = 0;

	for (int i=mNumEntries -1; i>= 0; i--)
	{
		if (addr >= (unsigned long)mBlocks[i].mem + mBlocks[i].size)
		{
			if(i < mNumEntries)
			{
				// assert (addr + size < (u32)mBlocks[i+1].mem) 
				if(addr + size >= (unsigned long)mBlocks[i+1].mem)
				{
					OmnAlarm << "Error happened in adding.:" << 
					"mem is: "<< (u64)mem << "  size:" << size << 
					"  FILE: " << file << ":" << line << 
					"\n . The existing block 1:mem:"  << (unsigned long )mBlocks[i].mem << " size:" << mBlocks[i].size << 
					"  FILE: " << mBlocks[i].filename << ":" << mBlocks[i].line << 
					"\n  block 2:mem:" << (unsigned long )mBlocks[i+1].mem << " size:" << mBlocks[i+1].size << 
					"  FILE: " << mBlocks[i+1].filename << ":" << mBlocks[i+1].line << 
					"." << enderr;
					mLock->unlock();
					return true;
				}
			}
			pos = i+1;
		}
	}

	if(pos == 0 && mNumEntries > 0)
	{
		if(addr + size >= (unsigned long )mBlocks[0].mem)
		{
			OmnAlarm << "Error happened in adding.:" << 
			"mem is: "<< (u64)mem << "  size:" << size << 
			"  FILE: " << file << ":" << line << 
			"\n . The existing block(first one):mem:"  << (unsigned long )mBlocks[0].mem << " size:" << mBlocks[0].size << 
			"  FILE: " << mBlocks[0].filename << ":" << mBlocks[0].line << 
			"." << enderr;
		}
	}
	
	bool rslt = insertAt(pos,block);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}


bool	
AosMemChecker::removeMemPriv(void *mem, const int size,const OmnString &file,const int &line)
{
	aos_assert_r(size > 0, false);
	mLock->lock();
	for (int i=0; i<mNumEntries; i++)	
	{
		if (mBlocks[i].mem == mem)
		{
			if(mBlocks[i].size != (u32)size)
			{
				OmnAlarm << "Error happened in removing.:" << 
				"mem is: "<< (u64)mem << "  size:" << size << 
				"  FILE: " << file << ":" << line << 
				"\n . The original block(first one):mem:"  << (unsigned long )mBlocks[i].mem << " size:" << mBlocks[i].size << 
				"  FILE: " << mBlocks[i].filename << ":" << mBlocks[i].line << 
				"." << enderr;

				mLock->unlock();
				return true;
			}
			bool rslt = removeEntry(i);
			aos_assert_rl(rslt, mLock, false);
			mLock->unlock();
			return true;
		}
	}

	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}


bool	
AosMemChecker::checkMemPriv(void *mem, const int size,const OmnString &file,const int &line)
{
	// The memory must be in an entry
	aos_assert_r(size > 0, false);
	mLock->lock();
	u64 addr = (unsigned long )mem;
	for (int i = mNumEntries -1; i>= 0 ; i--)	
	{
		if (addr >= (unsigned long )mBlocks[i].mem)
		{
			if(addr + (u32)size > (unsigned long )mBlocks[i].mem + mBlocks[i].size)
			{
				OmnAlarm << "Error happened in checking.:" << 
				"mem is: "<< (u64)mem << "  size:" << size << 
				"  FILE: " << file << ":" << line << 
				"\n . The original block :mem:"  << (unsigned long )mBlocks[i].mem << " size:" << mBlocks[i].size << 
				"  FILE: " << mBlocks[i].filename << ":" << mBlocks[i].line << 
				"." << enderr;

				mLock->unlock();
				return false;
			}
			mLock->unlock();
			return true;
		}
	}

	OmnAlarm << "Error happened in checking.:mem is: "<< (u64)mem << 
	"  size:" << size << "  . File: " << file << ":" << line << enderr;
	mLock->unlock();
	OmnShouldNeverComeHere;
	return false;
}




bool	
AosMemChecker::removeEntry(const int idx)
{
	for(int i = idx ;i < mNumEntries -2;i ++)
	{
		mBlocks[i] = mBlocks[i+1];
	}
	mBlocks[mNumEntries -1].clear();
	mNumEntries --;
	return false;
}


bool	
AosMemChecker::insertAt(const int idx, const AosMemBlk &block)
{
	//1. check memory
	//2. insert block into blocks
	for(int i = mNumEntries;i > idx;i --)
	{
		mBlocks[i] = mBlocks[i-1];
	}
	
	mBlocks[idx] = block;

	mNumEntries ++;
	return false;
}

