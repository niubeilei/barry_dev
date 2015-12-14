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
//  This class is used to manage queries. Each query that involves
//  multiple queries will create a query session. It is created
//  by this class. When a subsequent query is received, it retrieves
//  the query session from this class. When a query finishes, 
//  the query is removed. 
//
//  We assume all the subsequent queries will hit the same server. 
//  For this reason, there is no need to share the files (that store
//  query sessions with others). 
//
//  Query sessions will expire automatically after a given period of
//  time (configurable). 
//
//  The class maintains a circular queue. When requesting a new query,
//  it checks whether the queue is full. If not, it uses the next slot
//  to store the query. Otherwise, it kicks off the oldest one and 
//  replaces it with a newly created one.
//
// Modification History:
// 01/05/2010 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Query/QueryMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Query/QueryReq.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"

OmnSingletonImpl(AosQueryMgrSingleton,
                 AosQueryMgr,
                 AosQueryMgrSelf,
                "AosQueryMgr");


AosQueryMgr::AosQueryMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mStartIdx(100),
mCrtQid(100),
mCrtPos(0),
mCrtStartPos(0),
mTotalQueries(0)
{
}


AosQueryMgr::~AosQueryMgr()
{
}


bool
AosQueryMgr::start()
{
	return true;
}


bool
AosQueryMgr::stop()
{
    if(mThread.isNull())
    {
        return true;
    }
    mThread->stop();
    return true;
}


OmnRslt
AosQueryMgr::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool    
AosQueryMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosQueryMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosQueryMgr::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(100);
	}
	return true;
}


AosQueryReqPtr	
AosQueryMgr::getQuery(const OmnString &queryid)
{
	i64 qid = -1;
	if (queryid != "") qid = atoi(queryid.data());
	mLock->lock();
	if (qid >= mStartIdx && qid < mCrtQid) 
	{
		i64 idx = qid - mStartIdx + mCrtStartPos;
		if (idx >= eMaxQueries) idx -= eMaxQueries;
		aos_assert_r(idx >= 0 && idx < eMaxQueries, 0);
		AosQueryReqPtr query = mQueries[idx];
		if (query)
		{
			mLock->unlock();
			return query;
		}
	}

	// It is a new query. Need to create one.
	qid = mCrtQid++;
	mTotalQueries++;
	if (mTotalQueries > eMaxQueries) 
	{
		mStartIdx++;
		mCrtStartPos++;
		if (mCrtStartPos == eMaxQueries) mCrtStartPos = 0;
	}

	AosQueryReqPtr query = OmnNew AosQueryReq(qid);
	aos_assert_r(query, 0);

	mQueries[mCrtPos++] = query;
	if (mCrtPos == eMaxQueries) mCrtPos = 0;
	mLock->unlock();
	return query;
}


bool
AosQueryMgr::queryFinished(const AosQueryReqPtr &query)
{
	OmnString queryid = query->getQueryId();
	aos_assert_r(queryid != "", false);

	i64 qid = atoi(queryid.data());
	mLock->lock();
	i64 delta = qid - mStartIdx;
	if (delta < 0 || delta >= eMaxQueries)
	{
		// The query is out of the scope. Hopefully, it has been 
		// aged out properly. Ignored!
		mLock->unlock();
		return true;
	}

	mQueries[delta] = 0;
	mLock->unlock();
	return true;
}
#endif
