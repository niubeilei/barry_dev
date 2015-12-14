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
#include "BatchQuery/BatchQueryThrd.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "BatchQuery/BatchQuery.h"
#include "BatchQuery/BatchQueryMgr.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"


AosBatchQueryThrd::AosBatchQueryThrd(
		const AosBatchQueryMgrPtr &query_mgr)
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mQueryMgr(query_mgr)
{
	OmnThreadedObjPtr thisptr(this, false);
	mThread = OmnNew OmnThread(thisptr, "batchquerythrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}


AosBatchQueryThrd::~AosBatchQueryThrd()
{
}


bool
AosBatchQueryThrd::stop()
{
	return mThread->stop();
}


bool    
AosBatchQueryThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (!mQuery)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		AosBatchQueryPtr query = mQuery;
		mQuery = 0;
		mLock->unlock();
		procQueryPriv(query);
	}

	return true;
}


bool    
AosBatchQueryThrd::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool    
AosBatchQueryThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosBatchQueryThrd::procQueryPriv(const AosBatchQueryPtr &query)
{
	// This function processes the query. After finishing, it will
	// get a query from QueryMgr. 
	AosBatchQueryThrdPtr thisptr(this, false);

	while (1)
	{
		procOneQuery(query);

		AosBatchQueryPtr qq = mQueryMgr->threadFinished(thisptr);
		if (!qq)
		{
			// No more queries to process.
			return true;
		}
	}

	return true;
}


bool
AosBatchQueryThrd::procOneQuery(const AosBatchQueryPtr &query)
{
	// This function processes 'query'. 
	OmnNotImplementedYet;
	return false;
}


bool 
AosBatchQueryThrd::setQuery(const AosBatchQueryPtr &query, const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(!mQuery, mLock, false);
	mQuery = query;
	mLock->unlock();
	return true;
}

