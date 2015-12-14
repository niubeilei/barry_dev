////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoAgentUtil_CountDownLatch_h
#define Aos_JimoAgentUtil_CountDownLatch_h

#include <pthread.h>

class AosCountDownLatch
{
private:
	int					mCount;
	pthread_mutex_t		mLock;
	pthread_cond_t		mCond;
public:
	AosCountDownLatch();
	AosCountDownLatch(const int count);
	~AosCountDownLatch();

	int getCount() 
	{ 
		int count = 0;
		pthread_mutex_lock(&mLock);
		count = mCount; 
		pthread_mutex_unlock(&mLock);
		return count;
	}

	void setCount(const int count)
	{
		pthread_mutex_lock(&mLock);
		mCount = count;
		pthread_mutex_unlock(&mLock);
	}

	void wait();
	void countDown();
};
#endif

