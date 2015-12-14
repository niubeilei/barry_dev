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
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_ServerRunner_ServerRunner_h
#define AOS_ServerRunner_ServerRunner_h

#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "TaskMgr/Ptrs.h"
#include "SEInterfaces/TaskObj.h"
#include "Thread/ThreadedObj.h" 
#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"

OmnDefineSingletonClass(AosServerRunnerSingleton,
						AosServerRunner,
						AosServerRunnerSelf,
						OmnSingletonObjId::eServerRunner,
						"ServerRunner");

class AosServerRunner :  public OmnThreadedObj 
{
	OmnDefineRCObject;
public:
	enum 
	{
		eReadThrd,
		eRunTaskThrd,
		eHeartBeatTime = 60 
	};

private:
	AosRundataPtr 			mRunData;
	OmnThreadPtr 			mReadThread;
	pid_t					mPid;
	int 					mReadFd;
	int 					mWriteFd;
public:
	AosServerRunner();
	~AosServerRunner();

    // Singleton class interface
    static AosServerRunner* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);


	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, 
							   const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool readMsg();
	bool sendTask(const AosBuffPtr &buff);
	bool sendMsg(const OmnString &content);
	bool sendMsg(const AosBuffPtr &buff);
	bool setPidAndPipe(pid_t &pid, int &fd0, int &fd1);
	bool stopServer(const int &value);
	bool procMsg(const AosXmlTagPtr &msg);
};
#endif

