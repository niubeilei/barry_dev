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
// 10/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Thread_LockMonitorMgr_h
#define Omn_Thread_LockMonitorMgr_h

#include "Debug/Rslt.h"
#include "Message/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Util/SPtr.h"
#include "Util/ValList.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"

OmnDefineSingletonClass(AosLockMonitorMgrSingleton,
						AosLockMonitorMgr,
						AosLockMonitorMgrSelf,
                		OmnSingletonObjId::eLockMonitorMgr, 
						"LockMonitorMgr");



class AosLockMonitorMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	enum
	{
		eMaxMonitors = 100
	};

	OmnMutexPtr			mLock;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	AosLockMonitorPtr	mMonitors[eMaxMonitors];
	int					mNumMonitors;

	// Do not use the following two
	AosLockMonitorMgr(const AosLockMonitorMgr &rhs);
	AosLockMonitorMgr & operator = (const AosLockMonitorMgr &rhs);

public:
	AosLockMonitorMgr();
	~AosLockMonitorMgr();

	static AosLockMonitorMgr *	getSelf();
	static OmnThreadPtr		getCurrentThread();
	static OmnRslt			setThread(const OmnThreadPtr &thread);

	bool			start();
	bool			stop();
	bool			config(const AosXmlTagPtr &conf);
    bool			threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    bool			signal(const int threadLogicid);
    virtual void    heartbeat(const int tid); 
	bool			checkMonitors() const;
	bool			checkThread(OmnString &errmsg, const int tid) const;
	bool			addLock(const AosLockMonitorPtr &monitor);
};
#endif

