////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// This type of IIL maintains a list of (string, docid) and is sorted
// based on the string value. 
//
// Modification History:
// 04/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "DataSort/DataSortMgr.h"

#include "DataSort/MergeFileReq.h"
#include "DataSort/SaveFileReq.h"
#include "DataSort/MultiFileCBReq.h"
#include "DataSort/ProcQueueReq.h"

OmnMutexPtr			sgDataSortMgrLock = OmnNew OmnMutex();			
AosDataSortMgr* 	AosDataSortMgr::smDataSortMgr = NULL;

AosDataSortMgr::AosDataSortMgr()
{
}


AosDataSortMgr::~AosDataSortMgr()
{
}
	

AosDataSortMgr*
AosDataSortMgr::getSelf()
{
	sgDataSortMgrLock->lock();
	if (!smDataSortMgr)
	{
		smDataSortMgr = OmnNew AosDataSortMgr();
		smDataSortMgr->start();
	}
	sgDataSortMgrLock->unlock();
	return smDataSortMgr;
}


bool  
AosDataSortMgr::start()
{
	mMergeFileLock = OmnNew OmnMutex();
	mSaveFileLock = OmnNew OmnMutex();
	mCallBackLock = OmnNew OmnMutex();
	mProcQueueLock = OmnNew OmnMutex();
	mMergeFileCondVar = OmnNew OmnCondVar();
	mSaveFileCondVar = OmnNew OmnCondVar();
	mCallBackCondVar = OmnNew OmnCondVar();
	mProcQueueCondVar = OmnNew OmnCondVar();
	OmnThreadedObjPtr thisPtr(this, false);
	OmnString thrd_name;
	for (int i=0; i<eMaxThrds; i++)
	{
		thrd_name = "MergeFileThrd_";
		thrd_name << i;	
		OmnThreadPtr thread = OmnNew OmnThread(thisPtr, thrd_name, eMergeFile, true, true, __FILE__, __LINE__);
		thread->start();

		thrd_name = "SaveFileThrd_";
		thrd_name << i;	
		thread = OmnNew OmnThread(thisPtr, thrd_name, eSaveFile, true, true, __FILE__, __LINE__);
		thread->start();
		
		thrd_name = "ProcQueueThrd_";
		thrd_name << i;	
		thread = OmnNew OmnThread(thisPtr, thrd_name, eProcQueue, true, true, __FILE__, __LINE__);
		thread->start();
	}

	OmnThreadPtr thread = OmnNew OmnThread(thisPtr, "MultiFileCBThrd", eCallBack, true, true, __FILE__, __LINE__);
	thread->start();
	return true;
}

	
bool
AosDataSortMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	while (state == OmnThrdStatus::eActive)
	{
OmnScreen << "DataSortMgr thread ID: " << thread->getLogicId() << endl;
		bool rslt;
		if (thread->getLogicId() == eMergeFile)
		{
OmnMark;
			rslt = mergeFileThrdFunc();
			aos_assert_r(rslt, false);
OmnMark;
		}
		else if(thread->getLogicId() == eSaveFile)
		{
OmnMark;
			rslt = saveFileThrdFunc();
			aos_assert_r(rslt, false);
OmnMark;
		}
		else if(thread->getLogicId() == eCallBack)
		{
OmnMark;
			rslt = callBackThrdFunc();
			aos_assert_r(rslt, false);
OmnMark;
		}
		else if(thread->getLogicId() == eProcQueue)
		{
OmnMark;
			rslt = procQueueThrdFunc();
			aos_assert_r(rslt, false);
OmnMark;
		}
	}
	return true;
}


bool
AosDataSortMgr::mergeFileThrdFunc()
{
	mMergeFileLock->lock();
	if (mMergeFileReqs.size() <= 0)
	{
		mMergeFileCondVar->wait(mMergeFileLock);
		mMergeFileLock->unlock();
		return true;
	}

	AosDataSortReqPtr req = mMergeFileReqs.front();
	mMergeFileReqs.pop_front();
	mMergeFileLock->unlock();
	return req->procReq();
	//procMergeFileReq(req);
}


bool
AosDataSortMgr::saveFileThrdFunc()
{
	mSaveFileLock->lock();
	if (mSaveFileReqs.size() <= 0)
	{
		mSaveFileCondVar->wait(mSaveFileLock);
		mSaveFileLock->unlock();
		return true;
	}

	AosDataSortReqPtr req = mSaveFileReqs.front();
	mSaveFileReqs.pop_front();
	mSaveFileLock->unlock();
	return req->procReq();
	//procSaveFileReq(req);
}


bool
AosDataSortMgr::callBackThrdFunc()
{
	mCallBackLock->lock();
	if (mCallBackReqs.size() <= 0)
	{
		mCallBackCondVar->wait(mCallBackLock);
		mCallBackLock->unlock();
		return true;
	}

	AosDataSortReqPtr req = mCallBackReqs.front();
	mCallBackReqs.pop_front();
	mCallBackLock->unlock();
	return req->procReq();
	//procSaveFileReq(req);
}


bool
AosDataSortMgr::procQueueThrdFunc()
{
OmnScreen << "To enter procQueueThrdFunc: " << this << endl;
	mProcQueueLock->lock();
	if (mProcQueueReqs.size() <= 0)
	{
OmnScreen << "ProcQueueReqs is empty, wait: " << this << endl;
		mProcQueueCondVar->wait(mProcQueueLock);
		mProcQueueLock->unlock();
OmnScreen << "ProcQueueReqs wakes up: " << this << endl;
		return true;
	}

OmnScreen << "ProcQueueReqs is not empty, process one: " << this << endl;
	AosDataSortReqPtr req = mProcQueueReqs.front();
	mProcQueueReqs.pop_front();
	mProcQueueLock->unlock();
	return req->procReq();
	//procSaveFileReq(req);
}


bool 
AosDataSortMgr::addMergeFileReq(const AosDataSortReqPtr &req)
{
	mMergeFileLock->lock();
	mMergeFileReqs.push_back(req);
	//if (mMergeFileReqs.size() <= eMaxThrds)
	//{
	mMergeFileCondVar->broadcastSignal();
	//}
	mMergeFileLock->unlock();
	return true;
}

	
bool 
AosDataSortMgr::addSaveFileReq(const AosDataSortReqPtr &req)
{
	mSaveFileLock->lock();
	mSaveFileReqs.push_back(req);
	//if (mSaveFileReqs.size() <= eMaxThrds)
	//{
	mSaveFileCondVar->broadcastSignal();
	//}
	mSaveFileLock->unlock();
	return true;
}


bool 
AosDataSortMgr::addCallBackReq(const AosDataSortReqPtr &req)
{
	mCallBackLock->lock();
	mCallBackReqs.push_back(req);
	//if (mCallBackReqs.size() <= eMaxCBThrds)
	//{
	mCallBackCondVar->signal();
	//}
	mCallBackLock->unlock();
	return true;
}


bool 
AosDataSortMgr::addProcQueueReq(const AosDataSortReqPtr &req)
{
	mProcQueueLock->lock();
	mProcQueueReqs.push_back(req);
	//if (mSaveFileReqs.size() <= eMaxThrds)
	//{
	mProcQueueCondVar->broadcastSignal();
	//}
	mProcQueueLock->unlock();
	return true;
}


/*
bool 
AosDataSortMgr::procMergeFileReq(const AosMergeFileReqPtr &req)
{
	bool rslt = req->sort();
	aos_assert_r(rslt, false);
	return rslt;
}


bool 
AosDataSortMgr::procSaveFileReq(const AosSaveFileReqPtr &req)
{
	bool rslt = req->sort();
	aos_assert_r(rslt, false);
	return rslt;
}
*/

