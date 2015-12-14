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
// Modification History:
// 08/20/2013	Created by Young Pan
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_SystemMonitor_RunCmd_h
#define Omn_SystemMonitor_RunCmd_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "aosUtil/Types.h"
#include "Debug/Debug.h"
#include "Util/DataTypes.h"
#include "Util/String.h"
#include "Util/MemoryChecker.h"
#include "Util/MemoryCheckerObj.h"
#include "Util/Ptrs.h"
#include "Util/OmnNew.h"
#include "UtilComm/Ptrs.h"
#include "Porting/Sleep.h"
#include "StorageMgr/Ptrs.h"
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadedObj.h"

#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>

OmnDefineSingletonClass(AosRunCmdSingleton,
						AosRunCmd,
						AosRunCmdSelf,
						OmnSingletonObjId::eRunCmd,
						"RunCmd");

class AosRunCmd : public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	OmnMutexPtr	 		mLock;
	OmnThreadPtr		mThread;
	OmnCondVarPtr		mCondVar;
	OmnThreadPtr		mSendFmtThrd;
	OmnString 			mCmdExeInfo;

	int					mCrtIdx;
	int					mEpollFd;
	int 				mStdinFd[2];
	int 				mStdoutFd[2];
	int 				mStderrFd[2];
	char* 				mStdinBuff[1024];
	char* 				mStdoutBuff[1024];
	char* 				mStderrBuff[1024];
	bool 				mIsSuccess;
	bool				mIsWaiting;
	bool				mIsLongTimeCmd;
	pid_t 				mPid;

	struct epoll_event  mEpollEvent;

	enum
	{
		eMaxEpollEvent = 5,
		eReadPipeThrdId = 1
	};

private:
	AosRunCmd();

public:
	~AosRunCmd();

public:
    // Singleton class interface
    static AosRunCmd* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// OmnThreadedObj interface
	virtual bool    	threadFunc(
							OmnThrdStatus::E &state,
							const OmnThreadPtr &thread);
	virtual bool   		signal(const int threadLogicId);
	bool    			checkThread(
							OmnString &err, 
							const int thrdLogicId) const;

	OmnString			run(const OmnString &cmd);
	bool				cancel();
	bool				runBackgroud(const OmnString &cmd);

private:
	int 				readPipe(struct epoll_event *e);
	int 				createEvent();
	int					waitEvent(struct epoll_event *e);
	bool 				addEvent(int fd, int mask);
	bool 				delEvent(int fd, int mask);
	bool 				setFdNonblocking(int fd);
	bool 				initInfo();
	bool				checkIsFinished();
};
#endif

