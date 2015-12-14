////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 10/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_VirtualFile_FileReaderMgr_h
#define AOS_VirtualFile_FileReaderMgr_h

#include "SEInterfaces/FileReadListener.h"
#include "SEInterfaces/FileReadReq.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Sorter/Ptrs.h"
#include "Thread/Ptrs.h"
#include <queue>


OmnDefineSingletonClass(AosFileReaderMgrSingleton,
						AosFileReaderMgr,
						AosFileReaderMgrSelf,
						OmnSingletonObjId::eFileReaderMgr,
						"FileReaderMgr");


class AosFileReaderMgr :  public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr					mLock;
	OmnCondVarPtr				mCondVar;
	queue<AosFileReadRequestPtr>	mRequests;
	int							mNumThreads;
	vector<OmnThreadPtr>		mThreads;
	u32							mMaxRequests;
	AosRundataPtr				mRundata;

public:
	AosFileReaderMgr();
	~AosFileReaderMgr();

    // Singleton class interface
	static AosFileReaderMgr*	getSelf();
	virtual bool			start();
	virtual bool			stop();
	virtual bool			config(const AosXmlTagPtr &config);

	// ThreadedObj Interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
	virtual void    heartbeat(const int tid);
	virtual bool    checkThread(OmnString &err, const int thrdLogicId);

	bool readFile( 
					const u64 read_id,
					const AosVirtualFileObjPtr &file, 
					const AosFileReadListenerPtr &caller, 
					const u64 offset,
					const u64 read_size,
					AosRundata *rdata);

private:
	bool procOneReq(const AosFileReadRequestPtr &req);
};
#endif
#endif
