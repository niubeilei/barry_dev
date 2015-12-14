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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BatchQuery/BatchQueryMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BatchQuery/BatchQuery.h"
#include "BatchQuery/BatchQueryThrd.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "XmlUtil/XmlTag.h"

AosBatchQueryMgr::AosBatchQueryMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mOutstandingThreads(0),
mMaxOustandingThreads(eDftMaxOustandingThreads),
mMaxIdleThreads(eDftMaxIdleThreads)
{
}


AosBatchQueryMgr::~AosBatchQueryMgr()
{
}


bool 
AosBatchQueryMgr::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool 
AosBatchQueryMgr::start()
{
	return true;
}


bool 
AosBatchQueryMgr::stop()
{
	return true;
}


bool 
AosBatchQueryMgr::addBatchQuery(const AosBatchQueryPtr &query, const AosRundataPtr &rdata)
{
	// 1. If there are too many oustanding threads, it simply adds the query and returns. 
	// 2. If there are idle threads, it pops one and asks the thread to process the query. 
	// 3. Otherwise, it creates a new thread and asks the thread to process the query.
	static AosBatchQueryMgrPtr lsThisptr(this, false);

	mLock->lock();
	if (mOutstandingThreads < 0)
	{
		OmnAlarm << "mOutstanding thread is negative: " << mOutstandingThreads << enderr;
		mOutstandingThreads = 0;
	}

	if ((u32)mOutstandingThreads > mMaxOustandingThreads)
	{
		// Case 1: There are too many oustanding threads.
		mQueryQueue.push(query);
		mLock->unlock();
		return true;
	}

	if (mIdleThreads.size() > 0)
	{
		// Case 2: There are idle threads.
		AosBatchQueryThrdPtr thread = mIdleThreads.front();
		mIdleThreads.pop();
		mOutstandingThreads++;
		mLock->unlock();
		thread->setQuery(query);
		return true;
	}

	// Case 3: no idle threads. Need to create a new thread.
	mOutstandingThreads++;
	mLock->unlock();
	AosBatchQueryThrdPtr thread = OmnNew AosBatchQueryThrd(lsThisptr);
	thread->setQuery(query);
	return true;
}


AosBatchQueryPtr
AosBatchQueryMgr::threadFinished(const AosBatchQueryThrdPtr &thread)
{
	// A thread finished processing. It checks whether there are 
	// more jobs. If not, it returns to the thread queuey. If 
	// there are too many idle threads, the thread is removed.
	mLock->lock();
	if (mQueryQueue.size() > 0)
	{
		// There are requests. Remove the first one and process it.
		AosBatchQueryPtr query = mQueryQueue.front();
		mQueryQueue.pop();
		mLock->unlock();
		return query;
	}

	// No more requests. Check whether there are too many idle threads.
	mOutstandingThreads--;
	if (mOutstandingThreads < 0)
	{
		OmnAlarm << "Outstanding threads are negative: " << mOutstandingThreads << enderr;
		mOutstandingThreads = 0;
	}

	if (mIdleThreads.size() > mMaxIdleThreads)
	{
		mLock->unlock();
		OmnScreen << "Batch Query thread stopped!" << endl;
		thread->stop();
		return 0;
	}

	// Need to put the thread back to the idle thread queue 
	mIdleThreads.push(thread);
	OmnScreen << "Batch Query thread put back to idle: " << mIdleThreads.size() << endl;
	mLock->unlock();
	return 0;
}

