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
// Each memory allocation [filename, line] is treated as a counter. 
// The class supports eNumCounters counters. If there are more than
// eNumCounters counters, the class will not track them. Counter ID 0
// is not used.
//
// For each counter, it tracks the memory activity. 
//
// Modification History:
// 10/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/MemoryMonitor.h"

#include "Debug/Debug.h"
#include "Porting/Mutex.h"
#include "Porting/Sleep.h"


static bool sgInited = false;
static OmnMutexType sgLock;
static int sgCounterId = 1;
const u64 sgInvalidThreadId = 0xffffffffffffffffULL;
const u32 sgPoison1 = 0x637da46e;
const u32 sgPoison2 = 0x0376837f;

u32		AosMemoryMonitor::mCounterIds[AosMemoryMonitor::eNumBuckets][AosMemoryMonitor::eBucketSize];
char	AosMemoryMonitor::mFnames[AosMemoryMonitor::eNumCounters][AosMemoryMonitor::eFileMaxLen+1];
int 	AosMemoryMonitor::mLines[AosMemoryMonitor::eNumCounters];
int64_t AosMemoryMonitor::mCounters[AosMemoryMonitor::eNumCounters];
int64_t AosMemoryMonitor::mAllocNumCalls[AosMemoryMonitor::eNumCounters];
int64_t AosMemoryMonitor::mNowNumCalls[AosMemoryMonitor::eNumCounters];


void
AosMemoryMonitor::init()
{
	for (int i=0; i<eNumBuckets; i++)
	{
		for (int j=0; j<eBucketSize; j++)
		{
			mCounterIds[i][j] = 0;
		}
		mFnames[i][0] = 0;
		mLines[i] = 0;
		mCounters[i] = 0;
		mAllocNumCalls[i] = 0;
		mNowNumCalls[i] = 0;
	}
	sgInited = true;
}


static char sgNames[AosMemoryMonitor::eMaxCounters][AosMemoryMonitor::eFileMaxLen+1];
static int sgLines[AosMemoryMonitor::eMaxCounters];
static int64_t sgSizes[AosMemoryMonitor::eMaxCounters];
static int64_t sgTotalCalls[AosMemoryMonitor::eMaxCounters];
static int64_t sgNowCalls[AosMemoryMonitor::eMaxCounters];


void
AosMemoryMonitor::report(const u64 report_size)
{
	if (sgCounterId <= 1) return;

	// OmnMutexLock(sgLock);
	cout << endl 
		 << "============================ Memory Usage Report ======================================" << endl
		 << "                               File Name   Line          Size    Allocates          Now" << endl
		 << "---------------------------------------------------------------------------------------" << endl;

	// 1. Copy the data 
	int idx = 0;
	for (u32 i=1; i<(u32)sgCounterId; i++)
	{
		if (idx >= eMaxCounters) break;

		if (mCounters[i] == 0) continue;

		strcpy(sgNames[idx], mFnames[i]);
		sgLines[idx] =  mLines[i];
		sgSizes[idx] = mCounters[i];
		sgTotalCalls[idx] = mAllocNumCalls[i];
		sgNowCalls[idx] = mNowNumCalls[i];
		idx++;
	}

	// Add OmnString
	if (idx < eMaxCounters)
	{
		strcpy(sgNames[idx], "String.cpp");
		sgLines[idx] = 0;
		sgSizes[idx] = OmnString::getNumInstances() * sizeof(OmnString);
		sgTotalCalls[idx] = OmnString::getTotalInstances();
		sgNowCalls[idx] = OmnString::getNumInstances();
		idx++;
	}

	// 2. Sort the data
	u32 size = (u32)idx;
	if (size > 1)
	{
		for (u32 i=0; i<size-1; i++)
		{
			int64_t min = sgSizes[i];
			u32 idx = i;
			for (u32 j=i+1; j<size; j++)
			{
				if (min > sgSizes[j])
				{
					min = sgSizes[j];
					idx = j;
				}
			}

			if (idx != i)
			{
				char nn[eFileMaxLen+1];
				strcpy(nn, sgNames[i]);
				strcpy(sgNames[i], sgNames[idx]);
				strcpy(sgNames[idx], nn);

				int line = sgLines[i];
				sgLines[i] = sgLines[idx];
				sgLines[idx] = line;

				int64_t ss = sgSizes[i];
				sgSizes[i] = sgSizes[idx];
				sgSizes[idx] = ss;

				ss = sgTotalCalls[i];
				sgTotalCalls[i] = sgTotalCalls[idx];
				sgTotalCalls[idx] = ss;
				
				ss = sgNowCalls[i];
				sgNowCalls[i] = sgNowCalls[idx];
				sgNowCalls[idx] = ss;
			}
		}
	}

	int64_t total = 0;
	for (u32 i=0; i<size; i++)
	{
		total += sgSizes[i];
		if (sgSizes[i] < (int64_t)report_size) continue;
		printf("%40s : %-5d%13ld%13ld%13ld\n", sgNames[i], sgLines[i], sgSizes[i], sgTotalCalls[i], sgNowCalls[i]);
	}
	
	cout << "---------------------------------------------------------------------------------------" << endl
		 << "                                     total : " << total << endl
		 << "=======================================================================================" << endl;
	// OmnMutexUnlock(sgLock);

}


/*
void 
AosMemoryMonitor::setMemLoc(const char *fname, const int line)
{
	OmnMutexLock(sgLock);
	if (!sgInited)
	{
		OmnMutexUnlock(sgLock);
		return;
	}
	
	u64 thread_id = (u64)OmnGetCurrentThreadId();
	int idx = thread_id % eNumBuckets;
	for (int i=0; i<eBucketSize; i++)
	{
		if (mThreadIds[idx][i] == thread_id)
		{
			mFnames[idx][i] = fname;
			mLines[idx][i] = line;
			OmnMutexUnlock(sgLock);
			return;
		}
		if (mThreadIds[idx][i] == sgInvalidThreadId)
		{
			mThreadIds[idx][i] = thread_id;
			mFnames[idx][i] = fname;
			mLines[idx][i] = line;
			OmnMutexUnlock(sgLock);
			return;
		}

	}
	cout << __FILE__ << ":" << __LINE__ << ": ***********" << endl;
	OmnMutexUnlock(sgLock);
}


void 
AosMemoryMonitor::memAllocated(void *mem)
{
	OmnMutexLock(sgLock);
	if (!sgInited)
	{
		OmnMutexUnlock(sgLock);
		return;
	}

	u64 thread_id = (u64)OmnGetCurrentThreadId();
	int idx = thread_id % eNumBuckets;
	for (int i=0; i<eBucketSize; i++)
	{
		if (mThreadIds[idx][i] == thread_id)
		{
			char *memory = (char *)mem;
			*(u64*)memory = sgPoison;
			*(int*)&memory[eLineStart] = mLines[idx][i];
			if (!mFnames[idx][i]) 
			{
				cout << "******** fname is null: " << thread_id << endl;
				OmnMutexUnlock(sgLock);
				return;
			}

			int len = strlen(mFnames[idx][i]);
			if (len > eMaxFnameLen)
			{
				memcpy(&memory[eFnameStart], &mFnames[idx][i][len - eMaxFnameLen], eMaxFnameLen);
				memory[eFnameStart + eMaxFnameLen] = 0;
			}
			else
			{
				memcpy(&memory[eFnameStart], mFnames[idx][i], len);
				memory[eFnameStart + len] = 0;
			}
			cout << "Fname: " << mFnames[idx][i] << ":" << mLines[idx][i] << endl;
			OmnMutexUnlock(sgLock);
			return;
		}
	}
	cout << "*******" << endl;
	OmnMutexUnlock(sgLock);
}
*/


void *
AosMemoryMonitor::memAllocated(
		void *mem, 
		u32 size,
		const char *fname, 
		const int line)
{
	OmnMutexLock(sgLock);
	if (!sgInited)
	{
		init();
	}

	// cout << __FILE__ << ":" << __LINE__ << ": Memory: " << mem << endl;
	// Set the info:
	char *memory = (char *)mem;
	u32 * meta_info = (u32 *)mem;
	meta_info[0] = sgPoison1;
	meta_info[1] = sgPoison2;
	meta_info[2] = size;
	int fname_len = strlen(fname);
	u64 hashkey = AosGetHashKey(fname, fname_len);
	int bkt_key = hashkey % eNumBuckets;
	for (int i=0; i<eBucketSize; i++)
	{
		if (mCounterIds[bkt_key][i] == 0)
		{
			// Not there yet. Add it. 
			u32 counter_id = createCounter(fname, line);

			if (counter_id == 0)
			{
				// Failed allocating a counter ID for it. this should normally
				// not happen. Will mark the counter ID as 0 (not used) and return.
				meta_info[3] = 0;
				OmnMutexUnlock(sgLock);
				return (void *)&memory[eMetaSize];
			}

			// A counter ID has been allocated.
			mCounters[counter_id] = size;
			mAllocNumCalls[counter_id] = 1;
			mNowNumCalls[counter_id] = 1;
			meta_info[3] = counter_id;
			mCounterIds[bkt_key][i] = counter_id;
			OmnMutexUnlock(sgLock);
			return (void *)&memory[eMetaSize];
		}
			
		// The counter is already defined
		u32 counter_id = mCounterIds[bkt_key][i];
		const char *ff = mFnames[counter_id];
		int len = strlen(fname);
		if (((len >= eFileMaxLen && strncmp(ff, fname, eFileMaxLen) == 0) || 
			(len < eFileMaxLen && strcmp(ff, fname) == 0)) && mLines[counter_id] == line)
		{
			mCounters[counter_id] += size;
			mAllocNumCalls[counter_id]++;
			mNowNumCalls[counter_id]++;
			meta_info[3] = counter_id;
			OmnMutexUnlock(sgLock);
			return (void *)&memory[eMetaSize];
		}
	}
	cout << __FILE__ << ":" << __LINE__ << ": ********** Alarm: Run out of bucket size: "
		<< eBucketSize << endl;
	OmnMutexUnlock(sgLock);
	meta_info[3] = 0;
	return (void *)&memory[eMetaSize];
}


u32
AosMemoryMonitor::createCounter(const char *fname, const int line) 
{
	// This function assumes the class is locked. It allocates the 
	// next counter ID and returns it.
	if (sgCounterId >= eNumCounters) 
	{
		cout << __FILE__ << ":" << __LINE__ << ": ********** too many counters: " 
			<< eNumCounters << endl;
		return 0;
	}

	u32 counter_id = sgCounterId++;
	int fname_len = strlen(fname);
	if (fname_len >= eFileMaxLen)
	{
		strncpy(mFnames[counter_id], fname, eFileMaxLen);
	}
	else
	{
		strcpy(mFnames[counter_id], fname);
	}
	mLines[counter_id] = line;
	return counter_id;
}


void *
AosMemoryMonitor::memDeleted(void *mem)
{
	// cout << __FILE__ << ":" << __LINE__ << ": Memory: " << mem << endl;
	OmnMutexLock(sgLock);
	if (!sgInited)
	{
		// free(mem);
		cout << __FILE__ << ":" << __LINE__ << ": **********: internal error!" << endl;
		OmnMutexUnlock(sgLock);
		return 0;
	}

	char *memory = (char *)mem;
	memory -= eMetaSize;
	u32 *meta_info = (u32 *)memory;

	if (meta_info[0] != sgPoison1 || meta_info[1] != sgPoison2)
	{
		// Not allocated by this class.
		// cout << __FILE__ << ":" << __LINE__ << ": **********: internal error!" << endl;
		OmnMutexUnlock(sgLock);
		return mem;
	}

	u32 size = meta_info[2];
	u32 counter_id = meta_info[3];
	if (counter_id == 0)
	{
		// This should normally not happen.
		cout << __FILE__ << ":" << __LINE__ << ": **********: counter is 0!" << endl;
		OmnMutexUnlock(sgLock);
		return memory;
	}

	if (counter_id < 0 || counter_id >= eNumCounters)
	{
		cout << __FILE__ << ":" << __LINE__ << ": **********: internal error!" << endl;
		OmnMutexUnlock(sgLock);
		return memory;
	}

	mCounters[counter_id] -= size;
	mNowNumCalls[counter_id]--;
	// int line = mLines[counter_id];
	// const char *fname = mFnames[counter_id];
	// cout << __FILE__ << ":" << __LINE__ 
	// 		<< ": Delete memory: " << fname << ":" << line << endl;
	OmnMutexUnlock(sgLock);
	return memory;
}

