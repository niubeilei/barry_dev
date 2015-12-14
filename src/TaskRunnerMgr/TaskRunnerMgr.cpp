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
// 	Created: 05/09/2010 by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskRunnerMgr/TaskRunnerMgr.h"
#include "TaskRunnerMgr/TaskRunner.h"
#include "TaskRunnerMgr/ServerRunner.h"
#include "TaskRunnerMgr/ServerRunnerWrap.h"
#include "TaskRunnerMgr/TRMAIN.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "TaskMgr/Task.h"
#include "TaskMgr/TaskMgr.h"

#include <sys/select.h>
#include <iostream>
#include <algorithm>


OmnSingletonImpl(AosTaskRunnerMgrSingleton, AosTaskRunnerMgr, AosTaskRunnerMgrSelf, "AosTaskRunnerMgr");


AosTaskRunnerMgr::AosTaskRunnerMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mIsStopping(false),
mNumTaskRunners(-1)
{
	FD_ZERO(&mReadFds);
}

AosTaskRunnerMgr::~AosTaskRunnerMgr()
{
}

bool 
AosTaskRunnerMgr::start()
{
    OmnThreadedObjPtr thisPtr(this, false); 
	if(!mProcessThread)
	{
		mProcessThread = OmnNew OmnThread(thisPtr, "TaskRunnerMgrThread", eProcessThrd, true, true);
	}

	if(!mCheckHBThread)
	{
		mCheckHBThread = OmnNew OmnThread(thisPtr, "TaskRunnerMgrThread", eCheckHBThrd, true, true);
	}

	if(!mReadThread)
	{
		mReadThread = OmnNew OmnThread(thisPtr, "TaskRunnerMgrThread", eReadThrd, true, true);
	}

	if(!mSendThread)
	{
		mSendThread = OmnNew OmnThread(thisPtr, "TaskRunnerMgrThread", eSendThrd, true, true);
	}

	mProcessThread->start();
	mCheckHBThread->start();

	return true;
}


bool 
AosTaskRunnerMgr::stop()
{
	return true;
}

bool
AosTaskRunnerMgr::signal(const int threadLogicId)
{
	return true;
}

bool    
AosTaskRunnerMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}

bool
AosTaskRunnerMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	if (thread->getLogicId() == eProcessThrd)	
	{
		startTaskRunners();
	}

	if (thread->getLogicId() == eCheckHBThrd)	
	{
		checkHeartBeat();
	}

	if (thread->getLogicId() == eReadThrd)	
	{
		readMsg();
	}

	if (thread->getLogicId() == eSendThrd)
	{
		procTasks();
	}
	state = OmnThrdStatus::eIdle;
	return true;
}


bool
AosTaskRunnerMgr::checkHeartBeat()
{
/*	while(1)
	{
		OmnSleep(10);
		checkTRValid();
	}
*/	return true;
}

bool
AosTaskRunnerMgr::procTasks()
{
	while(1)
	{
		mLock->lock();
		if (mPendingTasks.size() <= 0)
		{
		 	bool is_timedout;
		 	mCondVar->timedWait(mLock, is_timedout, eCheckFreqSec, 0);
		 	mLock->unlock();
		 	continue;
		}
		else
		{
		 	procTaskLocked();
		}
		mLock->unlock();
	}
	return true;
}

bool
AosTaskRunnerMgr::readMsg()
{
	while(1)	
	{

		struct timeval tv = {1, 0};

		mLock->lock();
		fd_set fd = mReadFds;
		mLock->unlock();
/*
map<int, pid_t>::iterator it;
for ( it=mFdPid.begin() ; it != mFdPid.end(); it++ )
{
	int fd_num = (*it).first;
	bool rslt = FD_ISSET(fd_num, &fd);
	if(rslt)
	{
		OmnScreen << " ===================================================== there is exist fd num : " << fd_num << endl;
	}
	else
	{
		OmnScreen << " +++++++++++++++++++++++++++++++++++++++++++++++++++++ there is  no exist fd num : " << fd_num << endl;
	}
}
*/
		int rslt = select(eMaxRunner, &fd, 0, NULL, &tv);
//OmnScreen << " ------------------------------------------------------------- read fd rslt :" << rslt << endl;
//close the heartbeat
//		checkTRValid();
		if (rslt <=0)
		{
		  	if (rslt==0)
		  	{
		  		//time out
		  	}
		  	continue;
		}
		procMsg(fd);
	}
OmnAlarm << " **********************+++++++++++++++===============  it is not posible!!!!!!  " << enderr;
	//test
	exit(0);
	return true;
}

bool 
AosTaskRunnerMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	mConfig = config;
	AosXmlTagPtr cc = config->getFirstChild("taskrunnermgr");
	aos_assert_r(cc, false);
	mNumTaskRunners = cc->getAttrInt("taskrunner_num", 0);
	aos_assert_r(mNumTaskRunners > 0, false);
	OmnString base_dir = config->getAttrStr("base_dir");
	aos_assert_r(base_dir != "", false);
	for(int i=0; i<mNumTaskRunners; i++)
	{
		OmnString dir = base_dir;
		mDirs[i] = dir << i; 
	}
	return true;
}

bool 
AosTaskRunnerMgr::startServer()
{

	pid_t child_pid = -1;
	int fd[4];
	int rslt = pipe(fd);
	aos_assert_r(rslt == 0, false);
	rslt = pipe(fd+2);
	aos_assert_r(rslt == 0, false);
	AosXmlTagPtr config = mConfig->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(config > 0, false);

	if ((child_pid = fork()) == 0) // child process
	{
		cout << "------------------------ start server !----------------------" << endl;
		::signal(SIGINT, SIG_IGN);
		AosServerRunner::getSelf()->setPidAndPipe(child_pid, fd[2], fd[1]);
		AosServerRunner::getSelf()->start();
		serverProcessMain(config);
		exit(0);
		return 0;
	}
	else if(child_pid > 0) // parnet process
	{
		mLock->lock();
		mServerRunner = OmnNew AosServerRunnerWrap(child_pid, fd[0], fd[3]);
		mFdPid[fd[0]] = child_pid;
		FD_SET(fd[0], &mReadFds);
		mLock->unlock();
	}
	else
	{
		OmnAlarm << "Failed to create this server process" << enderr;
		//test
		exit(0);
	}
	return true;
}

bool 
AosTaskRunnerMgr::killTaskRunners()
{
	bool rslt = true;
	map<pid_t, AosTaskRunnerWrapPtr>::iterator it;
	for ( it=mTaskRunners.begin() ; it != mTaskRunners.end(); it++ )
	{
		rslt = (*it).second->kill();
	}
	return rslt;
}

bool 
AosTaskRunnerMgr::stopServer(const int& value)
{
	mIsStopping = true;
	mLock->lock();
	bool rslt = killTaskRunners();
	mLock->unlock();
	if(rslt)
	{
		mServerRunner->stopServer(value);
cout << "==================  Success  to kill all process!  :)  ====================" << endl;
	}
	else
	{
		OmnAlarm << "************** Failed to kill all taskrunners :( **************" << enderr;
	}
	exit(0);
	return true;
}

bool 
AosTaskRunnerMgr::startTaskRunners()
{
	startServer();
	OmnSleep(10);

	bool startFlag = false;
	for (int i=0; i<mNumTaskRunners; i++)
	{
OmnScreen << "^^^^^^^^^^^^^^^^^^^^inited taskrunner id : " << i << " ^^^^^^^^^^^^^^^^^^  " << endl;
		startFlag = startTaskRunner(i);
	}
	if(!startFlag)
	{
		OmnAlarm << "Failed to create these taskrunners" << enderr;
		exit(0);
		return false;
	}

	mReadThread->start();
	mSendThread->start();
	while(1)
	{
		int status;
		pid_t pid = wait(&status);
		bool isMyPid = false;
		map<int, pid_t>::iterator it;
		for ( it=mFdPid.begin() ; it != mFdPid.end(); it++ )
		{
			pid_t pid_num = (*it).second;
			if(pid == pid_num)
			{
				isMyPid = true;
				break;
			}
		}
		if(!isMyPid)
		{
			continue;
		}
		
		if (!mIsStopping)
		{
			// child process coredump, clear the resource with the pid	

			if(mServerRunner->getPid() == pid)
			{
				//clean server resource
				mLock->lock();
				int fd = mServerRunner->getReadFd();
				FD_CLR(fd, &mReadFds);
				mFdPid.erase(fd);
				mServerRunner->reset();
				mLock->unlock();
				
				//start server
				startServer();
				continue;
			}

			int index;
			cleanResource(pid, index);
			//start a new taskrunnner
			OmnScreen << "start a new taskrunner ----------" << pid << endl;
//			exit(0);
			startTaskRunner(index);
		}
	}
	return true;
}

AosXmlTagPtr 
AosTaskRunnerMgr::getChildConfig(const int &child_id)
{
	AosXmlTagPtr child_config = mConfig->clone(AosMemoryCheckerArgsBegin);
	OmnString base_dir = mDirs[child_id];
	child_config->setAttr("base_dir", base_dir);
	AosXmlTagPtr network_config = child_config->getFirstChild("networkmgr");
	aos_assert_r(network_config, NULL);
	int phyid = network_config->getAttrInt(AOSTAG_PHYSICAL_CLIENT_ID, -2);
	aos_assert_r(phyid >= 0, NULL);
	int clientId = phyid + child_id + 1;
	network_config->setAttr(AOSTAG_PHYSICAL_CLIENT_ID, clientId);
	return child_config;
}

bool 
AosTaskRunnerMgr::startTaskRunner(const int &child_id)
{
	pid_t child_pid = -1;
	int fd[4];
	int rslt = pipe(fd);
	aos_assert_r(rslt == 0, false);
	rslt = pipe(fd+2);
	aos_assert_r(rslt == 0, false);
	AosXmlTagPtr config = getChildConfig(child_id);
	aos_assert_r(config > 0, false);

	if ((child_pid = fork()) == 0) // child process
	{
//		OmnScreen << "child process " << " fd been created :"  
//			<< fd[1] << "," << fd[2] << ", child process : " << getpid()  << endl;
		cout << "child process " << " fd been created :" << fd[1] << "," << fd[2] << ", child process : " << getpid() << endl;
		::signal(SIGINT, SIG_IGN);
		AosTaskRunner::getSelf()->setPidAndPipe(child_pid, fd[2], fd[1]);
		AosTaskRunner::getSelf()->start();
		childProcessMain(config);

		exit(0);
		return 0;
	}
	else if(child_pid > 0) // parnet process
	{
		OmnScreen << "parent process " << " fd been created :"  
			<< fd[0] << "," << fd[3] << endl;
		AosTaskRunnerWrapPtr taskRunner = OmnNew AosTaskRunnerWrap(child_pid, fd[0], fd[3]);
		mLock->lock();
		mTaskRunners[child_pid] = taskRunner;
		mFdPid[fd[0]] = child_pid;
		FD_SET(fd[0], &mReadFds);
		mIdleRunners.push_back(taskRunner);
		mPidDir[child_pid] = child_id;
		mCondVar->signal();
		mLock->unlock();
	}
	else
	{
		OmnAlarm << "Failed to create this process" << enderr;
		//test
		exit(0);
	}
	return true;
}

bool 
AosTaskRunnerMgr::sendCmdToTaskRunner(const AosXmlTagPtr &cmd)
{
	return true;
}

bool
AosTaskRunnerMgr::receiveTask()
{
	AosBuffPtr task = mServerRunner->readTask();
	aos_assert_r(task, false);
	addTaskLocked(task);
	return true;
}

bool
AosTaskRunnerMgr::addTask(const AosBuffPtr &task)
{
	mLock->lock();
	mPendingTasks.push_back(task);
	mCondVar->signal();
	mLock->unlock();
	return true;
}

bool
AosTaskRunnerMgr::addTaskLocked(const AosBuffPtr &task)
{
	mPendingTasks.push_back(task);
	mCondVar->signal();
	return true;
}

void
AosTaskRunnerMgr::cleanResource(pid_t &pid, int &index)
{
	//1.check task status
	//2.clean resource
	mLock->lock();
	map<pid_t, AosTaskRunnerWrapPtr>::iterator itr = mTaskRunners.find(pid);
	if (itr == mTaskRunners.end())
	{
		OmnAlarm << "Failed retrieving the task runner: " << pid << enderr;
	}
	else
	{
		AosTaskRunnerWrapPtr task_wrapper = itr->second;
		AosBuffPtr task = task_wrapper->getTask();
		if (!task)
		{
			OmnAlarm << "Task is null" << enderr;
		}
		if(!task_wrapper->isFinished())
		{
			addTaskLocked(task);
		}

		FD_CLR(task_wrapper->getReadFd(), &mReadFds);

		map<pid_t, int>::iterator itr = mPidDir.find(pid);
		if (itr == mPidDir.end())
		{
			OmnAlarm << "Failed retrieving directory index by pid : " << pid << enderr;
		}
		index = (*itr).second;
		mPidDir.erase(pid);

		mFdPid.erase(task_wrapper->getReadFd());
		remove(mRunners.begin(), mRunners.end(), task_wrapper);
		remove(mIdleRunners.begin(), mIdleRunners.end(), task_wrapper);
		task_wrapper->taskRunnerFailed();
		mTaskRunners.erase(pid);

	}
	mLock->unlock();
}

void
AosTaskRunnerMgr::checkTRValid()
{
	map<pid_t, AosTaskRunnerWrapPtr>::iterator it;
	for ( it=mTaskRunners.begin() ; it != mTaskRunners.end(); it++ )
	{
		pid_t pid = (*it).first;
		if(!pid)
		{
			OmnAlarm << "there is no this pid in the taskrunners" << enderr;
		}
		AosTaskRunnerWrapPtr runner = (*it).second;
		bool rslt = runner->checkValid();
		if(rslt)
		{
			continue;
		}
		else
		{
			OmnAlarm << "kill the pid : " << pid << enderr;
			kill(pid, SIGKILL);
		}
	}
}

void
AosTaskRunnerMgr::procTaskLocked()
{
	if (!mIdleRunners.empty() && mPendingTasks.size() >0) 
	{
		AosBuffPtr task = mPendingTasks.front();
		mPendingTasks.pop_front();

		AosTaskRunnerWrapPtr taskRunner = mIdleRunners.back();
		mIdleRunners.pop_back();

		taskRunner->addTask(task);
		mRunners.push_back(taskRunner);		
	}
}

void
AosTaskRunnerMgr::procMsg(fd_set &fd)
{
	mLock->lock();
	map<int, pid_t>::iterator it;
	for ( it=mFdPid.begin() ; it != mFdPid.end(); it++ )
	{
		int fd_num = (*it).first;
		bool rslt = FD_ISSET(fd_num, &fd);
		if (rslt > 0)
		{
			//server message
			if(mServerRunner->getReadFd() == fd_num)
			{
				receiveTask();
				break;
			}

			//taskrunner message
			pid_t pid = mFdPid[fd_num];
			if(!pid)
			{
				OmnAlarm << "there is no this pid in the mFdPid" << enderr;
				break;
			}

			AosTaskRunnerWrapPtr runner = mTaskRunners[pid];

			int len = runner->readMsg();
			if(len <= 0)
			{
				OmnAlarm << "the message is error!" << enderr;
			}
			//1.check runner is finished
			if(runner->isFinished())
			{
				remove(mRunners.begin(), mRunners.end(), runner);
				mIdleRunners.push_back(runner);
				mCondVar->signal();
			}
			//2. maybe do other things
		}
	}
	mLock->unlock();
}
