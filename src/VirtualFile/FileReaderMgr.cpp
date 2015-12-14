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
// 10/31/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "VirtualFile/FileReaderMgr.h"

#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/CondVar.h"
#include "Thread/Sem.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosFileReaderMgrSingleton,
                 AosFileReaderMgr,
                 AosFileReaderMgrSelf,
                "AosFileReaderMgr");


AosFileReaderMgr::AosFileReaderMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMaxRequests(1000000)
{
	OmnThreadedObjPtr thisptr(this, false);
	for (int i=0; i<1; i++)
	{
		OmnThreadPtr thread = OmnNew OmnThread(thisptr, "FileReaderMgrThrd", 0, true, true, __FILE__, __LINE__); 
		thread->start();
		mThreads.push_back(thread);
	}
}


AosFileReaderMgr::~AosFileReaderMgr()
{
}


bool      	
AosFileReaderMgr::start()
{
	return true;
}


bool        
AosFileReaderMgr::stop()
{
	return true;
}


bool		
AosFileReaderMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);

	return true;
}


bool    
AosFileReaderMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
		mLock->lock();
		if (mRequests.size() <= 0)
		{
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}

		AosFileReadRequestPtr req = mRequests.front();
		mRequests.pop();
		mLock->unlock();
		procOneReq(req);
	}
	return true;
}


bool    
AosFileReaderMgr::signal(const int threadLogicId)
{
	return true;
}


void    
AosFileReaderMgr::heartbeat(const int tid)
{
}


bool    
AosFileReaderMgr::checkThread(OmnString &err, const int thrdLogicId)
{
	return true;
}


bool
AosFileReaderMgr::procOneReq(const AosFileReadRequestPtr &req)
{
	// This function merges the two buff array in 'req'. 
	aos_assert_r(req, false);
	aos_assert_r(req->mFile, false);
	aos_assert_r(req->mCaller, false);
	aos_assert_r(req->mReadSize >= 0, false);

	AosFileReadListenerPtr caller = req->mCaller;
	vector<AosBuffPtr> buffs;
	caller->getMemory(req->mReadId, req->mReadSize, buffs);

	//aos_assert_r((u64)buff->buffLen() >= req->mReadSize, false);
	//req->mFile->lock();
	//req->mFile->seek(req->mOffset);
	int rslt = 0;
	if (req->mReadSize > 0)
		rslt= (req->mFile)->read(buffs, req->mOffset, req->mReadSize, mRundata);
	//req->mFile->unlock();
	if (rslt == -1)
	{
		caller->readError(req->mReadId, mRundata->getErrmsg(), mRundata);
		return true;
	}
	caller->dataRead(req->mReadId, buffs, mRundata);
	return true;
}


bool 
AosFileReaderMgr::readFile( 
		const u64 read_id,
		const AosVirtualFileObjPtr &file, 
		const AosFileReadListenerPtr &caller, 
		const u64 offset,
		const u64 read_size,
		AosRundata *rdata)
{
	mLock->lock();
	AosFileReadRequestPtr req = OmnNew AosFileReadRequest(
			read_id, file, offset, read_size, caller);
	mRequests.push(req);
	mCondVar->signal();
	mLock->unlock();
	return true;
}
#endif

