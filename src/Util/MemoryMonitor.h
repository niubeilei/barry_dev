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
// 10/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_Util_MemoryMonitor_h
#define AOS_Util_MemoryMonitor_h

#include "aosUtil/Types.h"

class AosMemoryMonitor
{

public:
	enum
	{
		eNumBuckets = 100000,
		eBucketSize = 50,
		eMetaSize = 24,
		eNumCounters = 100000,
		eMinSleepTime = 10,
		eDftSleepTime = 600,
		eMaxCounters = 1000,
		eFileMaxLen = 50
	};

private:
	static u32		mCounterIds[eNumBuckets][eBucketSize];
	static char 	mFnames[eNumCounters][eFileMaxLen+1];
	static int		mLines[eNumCounters];
	static int64_t	mCounters[eNumCounters];
	static int64_t	mAllocNumCalls[eNumCounters];
	static int64_t	mNowNumCalls[eNumCounters];

public:
	AosMemoryMonitor(const int sleepTime);
	~AosMemoryMonitor();

	static void setMemLoc(const char *fname, const int line);
	static void *memAllocated(void *mem, u32 size, const char *fname, const int line);
	static void *memDeleted(void *mem);
	static u32  createCounter(const char *fname, const int line);
	static void init();
	static void report(const u64 report_size);
};
#endif

