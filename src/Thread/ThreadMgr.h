////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThreadMgr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_ThreadMgr_h
#define Omn_Thread_ThreadMgr_h

#include "Debug/Rslt.h"
#include "Porting/ThreadDef.h"
#include "Message/Ptrs.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Ptrs.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"
#include "Util/SPtr.h"
#include "Util/ValList.h"
#include "Util/HashUtil.h"
#include "Util/RCObjImp.h"
#include "Util1/TimerObj.h"
#include "XmlParser/Ptrs.h"
#include <hash_map>
using namespace std;

OmnDefineSingletonClass(OmnThreadMgrSingleton,
						OmnThreadMgr,
						OmnThreadMgrSelf,
                		OmnSingletonObjId::eThreadMgr, 
						"ThreadMgr");



class OmnThreadMgr : public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum
	{
		//eDefaultHbTimerSec = 30,
		eDefaultHbTimerSec = 5,
		eMaxThreadId = 0x1fff
	};

	typedef hash_map<u64, OmnThreadPtr> 			ThreadMap_t;
	typedef hash_map<u64, OmnThreadPtr>::iterator ThreadMapItr_t;


private:
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mThread;
	uint				mHbTimerSec;
	bool				mIsWakenUpByTimer;
	static ThreadMap_t	mThreadMap;
	static OmnMutex		mLock;
	static int			mMainThreadId;
	OmnString			mErrorThrds;

	// Do not use the following two
	OmnThreadMgr(const OmnThreadMgr &rhs);
	OmnThreadMgr & operator = (const OmnThreadMgr &rhs);

	// Chen Ding, 10/31/2012
	/*
	typedef hash_map<u32, char *> 				ThreadFnameMap_t;
	typedef hash_map<u32, char *>::iterator		ThreadFnameMapItr_t;
	typedef hash_map<u32, int> 					ThreadLineMap_t;
	typedef hash_map<u32, int>::iterator 			ThreadLineMapItr_t;
	ThreadFnameMap_t	mFnameMap;
	ThreadLineMap_t		mLineMap;
	*/

public:
	OmnThreadMgr();
	~OmnThreadMgr();

	static OmnThreadMgr *	getSelf();
	static OmnThreadPtr		getCurrentThread();
	static OmnRslt			setThread(const OmnThreadPtr &thread);
	static OmnThreadPtr		getThread(const u64 tid);
	static bool				setMainThreadId(const int tid);
	static int				getMainThreadId() {return mMainThreadId;}

	bool			start();
	bool			stop();
	bool			config(const AosXmlTagPtr &conf);
	void			addThread(const OmnThreadPtr &thread);
	bool			removeThread(const OmnThreadPtr &thread);
	void			stopAllThreads();			
	bool			sendMsg(const OmnMsgPtr &msg);
    bool			threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    bool			signal(const int threadLogicid);
    virtual void    heartbeat(const int tid); 
	bool			checkThread(OmnString &errmsg, const int tid) const;

	int getCrtThreadLogicId()
	{
		OmnThreadPtr thread = getCurrentThread();
		if (!thread)
		{
			OmnAlarm << "Failed retrieve thread" << enderr;
			return -1;
		}
		return thread->getLogicId();
	}

	bool	registerThreadAction(
					const char *file, 
					const int line, 
					const int timer, 
					const OmnString &action);
	bool	unregisterThreadAction(const OmnString &action);
	void	appendActionLog(
					const char *file,
					const int line,
					const OmnString &log);
	
	void    getErrorThread(OmnString &errorThrds);
};
#endif

