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
#ifndef AOS_DataSort_DataSortMgr_h
#define AOS_DataSort_DataSortMgr_h

#include "API/AosApi.h"
#include "DataSort/Ptrs.h"
#include "Sorter/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Ptrs.h"
#include "Util/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/BuffArray.h"
#include <queue>

using namespace std;

class AosDataSortMgr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;
private:
	enum
	{
		eMergeFile = 0,
		eSaveFile = 1,
		eCallBack = 2,
		eProcQueue = 3,
		eMaxThrds = 5,
		eMaxCBThrds = 1
	};

	OmnMutexPtr 					mMergeFileLock; 
	OmnMutexPtr 					mSaveFileLock; 
	OmnMutexPtr 					mCallBackLock; 
	OmnMutexPtr 					mProcQueueLock; 
	OmnCondVarPtr					mMergeFileCondVar;
	OmnCondVarPtr					mSaveFileCondVar;
	OmnCondVarPtr					mCallBackCondVar;
	OmnCondVarPtr					mProcQueueCondVar;
	deque<AosDataSortReqPtr>		mMergeFileReqs;
	deque<AosDataSortReqPtr>		mSaveFileReqs;
	deque<AosDataSortReqPtr>		mCallBackReqs;
	deque<AosDataSortReqPtr>		mProcQueueReqs;
	//vector<OmnThreadPtr>			mThreads;

	static AosDataSortMgr* 			smDataSortMgr;
public:
	AosDataSortMgr();
	~AosDataSortMgr();
	
	// Singleton class interface
	static AosDataSortMgr*   getSelf();
	bool  start();
	
	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId) {return true;}
	virtual void    heartbeat(const int tid) {}
	virtual bool    checkThread(OmnString &err, const int thrdLogicId) {return true;}

	bool addMergeFileReq(const AosDataSortReqPtr &req);
	bool addSaveFileReq(const AosDataSortReqPtr &req);
	bool addCallBackReq(const AosDataSortReqPtr &req);
	bool addProcQueueReq(const AosDataSortReqPtr &req);

private:
	bool		mergeFileThrdFunc();
	bool		saveFileThrdFunc();
	bool		callBackThrdFunc();
	bool		procQueueThrdFunc();
	bool 		procMergeFileReq(const AosDataSortReqPtr &req);
	bool 		procSaveFileReq(const AosDataSortReqPtr &req);
	bool 		procCallBackReq(const AosDataSortReqPtr &req);
	bool 		procProcQueueReq(const AosDataSortReqPtr &req);
};

#endif

