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
#ifndef AOS_TaskRunnerMgr_TaskRunnerMgr_h
#define AOS_TaskRunnerMgr_TaskRunnerMgr_h

#include "SingletonClass/SingletonObjId.h"
#include "SingletonClass/SingletonTplt.h"
#include "Util/String.h"
#include "XmlInterface/XmlRc.h"
#include "Thread/ThreadedObj.h" 
#include "TaskRunnerMgr/Ptrs.h" 
#include "Thread/Sem.h" 
#include "TaskRunnerMgr/TaskRunnerWrap.h" 
#include "TaskRunnerMgr/Ptrs.h" 
#include "SEInterfaces/TaskObj.h"
#include <map>
#include <deque>
#include <queue>
#include <list>

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
using namespace std;


OmnDefineSingletonClass(AosTaskRunnerMgrSingleton,
						AosTaskRunnerMgr,
						AosTaskRunnerMgrSelf,
						OmnSingletonObjId::eTaskRunnerMgr,
						"TaskRunnerMgr");

class AosTaskRunnerMgr :  public OmnThreadedObj
{
	OmnDefineRCObject;

public:
	enum 
	{
		eProcessThrd,
		eCheckHBThrd,
		eReadThrd,
		eSendThrd,
		eCheckFreqSec = 10,
		eMaxRunner = 1024, 
		eMaxDirs = 100
	};

private:
	OmnMutexPtr				mLock;
	OmnCondVarPtr			mCondVar;
	OmnSemPtr				mSem;
	OmnThreadPtr 			mCheckHBThread;
	OmnThreadPtr 			mProcessThread;
	OmnThreadPtr 			mReadThread;
	OmnThreadPtr 			mSendThread;
	bool 					mIsStopping;
	int						mNumTaskRunners;
	AosXmlTagPtr			mConfig;
	deque<AosBuffPtr>	mPendingTasks;
	OmnString  mDirs[eMaxDirs];
	AosServerRunnerWrapPtr  mServerRunner;

	//start : the resource need to be locked
	deque<AosTaskRunnerWrapPtr>	mRunners;
	deque<AosTaskRunnerWrapPtr>	mIdleRunners;
	map<int, pid_t>			mFdPid;
	map<pid_t, int>			mPidDir;
	map<pid_t, AosTaskRunnerWrapPtr>			mTaskRunners;
	fd_set					mReadFds;
	//end : the resource need to be locked

	AosTaskRunnerMgr();
	~AosTaskRunnerMgr();
	void checkTaskRunner();
	bool startTaskRunner(const int & child_id);
	bool startServer();
	bool		addTaskLocked(const AosBuffPtr &task);
	bool		readMsg();
	void		checkTRValid();
	void		cleanResource(pid_t &pid, int &index);
	void		procTaskLocked();
	void		procMsg(fd_set &fd);
	bool		procTasks();
	bool		checkHeartBeat();
	bool		receiveTask();
	bool		killTaskRunners();
	AosXmlTagPtr	getChildConfig(const int &child_id);


public:
    // Singleton class interface
    static AosTaskRunnerMgr* 	getSelf();
    virtual bool      	start();
    virtual bool        stop();
    virtual bool		config(const AosXmlTagPtr &def);

	// ThreadedObj interface
	virtual bool	threadFunc(OmnThrdStatus::E &state, 
							   const OmnThreadPtr &thread);
	virtual bool	signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;

	bool		startTaskRunners();
	bool		sendCmdToTaskRunner(const AosXmlTagPtr &cmd);
	bool		addTask(const AosBuffPtr &task);
	void		stopTaskRunnerMgr() {mIsStopping = true;}
	bool		stopServer(const int& value);
};

#endif

