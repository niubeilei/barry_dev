//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
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
#include "JimoAgentUtil/CountDownLatch.h"

AosCountDownLatch::AosCountDownLatch()
{
	pthread_mutex_init(&mLock, NULL);
	pthread_cond_init(&mCond, NULL);
}


AosCountDownLatch::AosCountDownLatch(const int count)
:
mCount(count)
{
	pthread_mutex_init(&mLock, NULL);
	pthread_cond_init(&mCond, NULL);
}

AosCountDownLatch::~AosCountDownLatch()
{
}

void
AosCountDownLatch::wait()
{
	pthread_mutex_lock(&mLock);
	while(mCount > 0)
	{
		pthread_cond_wait(&mCond, &mLock);
	}
	pthread_mutex_unlock(&mLock);
}

void 
AosCountDownLatch::countDown()
{
	pthread_mutex_lock(&mLock);
	mCount--;
	pthread_cond_broadcast(&mCond);
	pthread_mutex_unlock(&mLock);
}
