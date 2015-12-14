////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 2013/06/18	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocFileMgr/DfmLogProcThrd.h"

#include "DocFileMgr/DfmLog.h"
#include "DocFileMgr/DfmLogUtil.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"

AosDfmLogProcThrd::AosDfmLogProcThrd()
:
mMaxFileSize(eMaxFileSize),
mNumThrds(1)
{
}


AosDfmLogProcThrd::~AosDfmLogProcThrd()
{
}


bool
AosDfmLogProcThrd::config(const AosXmlTagPtr &app_conf)
{
	AosXmlTagPtr conf = app_conf->getFirstChild("dfmlog"); 
	if (conf)
	{
		mNumThrds = conf->getAttrU32("num_threads", 2);
		aos_assert_r(mNumThrds, false);

		u64 max_file_size = conf->getAttrU64("max_file_size", 0);
		if (max_file_size > 0) mMaxFileSize = max_file_size;
	}

	for (u32 i = 0; i < mNumThrds; i++)
	{
		mLock[i] = OmnNew OmnMutex();
		mCondVar[i] = OmnNew OmnCondVar();
		mStopCache[i].reset(false, false);

		OmnThreadedObjPtr thisptr(this, false);
		mThread[i] = OmnNew OmnThread(thisptr, "dfmlogprocthrd", i, false, true, __FILE__, __LINE__);
		mThread[i]->start();
	}
	return true;
}


bool
AosDfmLogProcThrd::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while(state == OmnThrdStatus::eActive)
	{
		u32 logicid = thread->getLogicId();
		mLock[logicid]->lock();
		if (mStopCache[logicid].stop_proc_log || mQueue[logicid].empty())
		{
			if (mStopCache[logicid].stop_proc_log) mStopCache[logicid].finish_crt_log = true;
			mCondVar[logicid]->wait(mLock[logicid]);
			mLock[logicid]->unlock();
			continue;
		}

		DfmLogReq req = mQueue[logicid].front();
		mLock[logicid]->unlock();

		AosDfmLogPtr dfm_log = req.mDfmLog;
		dfm_log->saveDoc(req.mRdata, req.mFileId, false);

		mLock[logicid]->lock();
		mQueue[logicid].pop(); 
		mLock[logicid]->unlock();
	}
	return true;
}


bool
AosDfmLogProcThrd::signal(const int threadLogicId)
{
	return true;
}


bool
AosDfmLogProcThrd::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosDfmLogProcThrd::addRequest(
		const AosRundataPtr &rdata,
		const AosDfmLogPtr &dfmlog,
		const u64 &fileid)
{
	u32 virtual_id = dfmlog->getVirtualId();
	u32 idx = virtual_id % mNumThrds;

	DfmLogReq req(dfmlog, fileid, rdata);
	mLock[idx]->lock();
	mQueue[idx].push(req);	
	mCondVar[idx]->signal();
	mLock[idx]->unlock();
	return true;
}


bool
AosDfmLogProcThrd::stop()
{
	for (u32 i = 0; i < mNumThrds; i++)
	{
		while(!mQueue[i].empty() && !mStopCache[i].stop_proc_log)
		{
			OmnScreen << " wait wait wait save Log thrd: " << i << endl;
			OmnSleep(1);
		}
	}

	OmnScreen << "dfm log all finished " << endl;
	return true;
}


bool
AosDfmLogProcThrd::stopProcCache()
{
	for (u32 i = 0; i < mNumThrds; i++)
	{
		mLock[i]->lock();

		mStopCache[i].reset(true, false);

		mCondVar[i]->signal();
		mLock[i]->unlock();
	}

	while(1)
	{
		OmnSleep(1);
		OmnScreen << " wait wait wait stopProcCache " << endl;

		bool find = true;
		for (u32 i = 0; i < mNumThrds; i++)
		{
			if (!mStopCache[i].finish_crt_log) find = false;
		}
		if (find) break;
	}
	return true;
}


bool
AosDfmLogProcThrd::continueProcCache()
{
	for (u32 i = 0; i < mNumThrds; i++)
	{
		mLock[i]->lock();

		mStopCache[i].reset(false, true);

		mCondVar[i]->signal();
		mLock[i]->unlock();
	}
	return true;
}


bool
AosDfmLogProcThrd::flushContentsByVirtualId(const u32 virtual_id)
{
	u32 idx = virtual_id % mNumThrds;
	mLock[idx]->lock();
	while(!mQueue[idx].empty() && !mStopCache[idx].stop_proc_log)
	{
		mLock[idx]->unlock();
		OmnScreen << " wait wait wait save Log thrd: " << idx << ";virtual_id:" << virtual_id << endl;
		OmnSleep(1);
		mLock[idx]->lock();
	}
	mLock[idx]->unlock();
	return true;
}


