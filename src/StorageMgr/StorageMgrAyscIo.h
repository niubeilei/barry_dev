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
// 	Created: 04/28/2013 by Linda Lin 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_StorageMgrAyscIo_h
#define AOS_StorageMgr_StorageMgrAyscIo_h

#include "alarm_c/alarm.h"
#include "Rundata/Ptrs.h"
#include "Porting/Sleep.h"
#include "StorageMgr/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"

#include <queue>

OmnDefineSingletonClass(AosStorageMgrAyscIoSingleton,
						AosStorageMgrAyscIo,
						AosStorageMgrAyscIoSelf,
						OmnSingletonObjId::eStorageMgrAyscIo,
						"StorageMgrAyscIo");

class AosStorageMgrAyscIo : public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eMaxThrds = 10,
		eMaxMemory = 1000000000 // 1G
	};

private:
	OmnThreadPtr								mThread[eMaxThrds];
	OmnMutexPtr     							mLock[eMaxThrds];
	OmnCondVarPtr   							mCondVar[eMaxThrds];
	priority_queue<AosAioRequestPtr>			mProcData[eMaxThrds];
	int											mNumThrds;
	u64											mTotalReceivedSize;
	// dev_id, thrd_id
	map<int64_t, int>							mDeviceId;


public:

	AosStorageMgrAyscIo();
	~AosStorageMgrAyscIo();

public:
    // Singleton class interface
    static AosStorageMgrAyscIo* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

	bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool	addRequest(const AosAioRequestPtr &request);

	bool	addRequest(const vector<AosAioRequestPtr> &request);

private:
	bool 	readFileToBuff(const AosAioRequestPtr &request);

	bool 	writeFile(const AosAioRequestPtr &request);

	void	init();

	int getDevId(const OmnLocalFilePtr &file);
};

#endif

