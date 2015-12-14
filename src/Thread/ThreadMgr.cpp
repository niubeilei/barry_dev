////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ThreadMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Thread/ThreadMgr.h"

#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util1/Timer.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(OmnThreadMgrSingleton,
                 OmnThreadMgr,
                 OmnThreadMgrSelf,
                "OmnThreadMgr");


// OmnThreadPtr OmnThreadMgr::mThreadArray[OmnThreadMgr::eMaxThreadId+1];
OmnThreadMgr::ThreadMap_t OmnThreadMgr::mThreadMap;
OmnMutex OmnThreadMgr::mLock;
int OmnThreadMgr::mMainThreadId = -1;

// Chen Ding, 10/31/2012
static volatile bool sgThreadMgrInited = false;

OmnThreadMgr::OmnThreadMgr()
:
mCondVar(OmnNew OmnCondVar()),
mHbTimerSec(eDefaultHbTimerSec),
mErrorThrds("")
{

}


OmnThreadMgr::~OmnThreadMgr()
{
	OmnTrace << "Delete ThreadMgr: " << this << endl;
}


OmnThreadPtr
OmnThreadMgr::getCurrentThread()
{
	// OmnThreadIdType threadId = OmnGetCurrentThreadId();
	mLock.lock();
	u64 tid = OmnGetCurrentThreadId();
	if (mThreadMap.empty())
	{
		mLock.unlock();
		return NULL;
	}
	ThreadMapItr_t itr = mThreadMap.find(tid);
	if (itr == mThreadMap.end()) 
	{
		mLock.unlock();
		return 0;
	}

	OmnThreadPtr thrd = itr->second;
	mLock.unlock();
	return thrd;
}


bool
OmnThreadMgr::start()
{
	sgThreadMgrInited = true;
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "ThreadMgr", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
OmnThreadMgr::stop()
{
	mThread->stop();
	return true;
}


bool
OmnThreadMgr::config(const AosXmlTagPtr &conf)
{
	return true;
}


OmnRslt
OmnThreadMgr::setThread(const OmnThreadPtr &thread)
{
	// cout << __FILE__ << ":" << __LINE__ << ":" << endl;
	if (!sgThreadMgrInited) OmnThreadMgr::getSelf()->start();

	mLock.lock();
	u64 tid = thread->getThreadId();
	// cout << __FILE__ << ":" << __LINE__ << ":" << tid << ":" << endl;
	mThreadMap[tid] = thread;
	mThreadMap[tid].setDelFlag(false);
	// cout << __FILE__ << ":" << __LINE__ << ":" << tid << ":" << endl;
	mLock.unlock();
	return true;
}


bool
OmnThreadMgr::removeThread(const OmnThreadPtr &thread)
{
	mLock.lock();
	u32 tid = thread->getThreadId();
	mThreadMap.erase(tid);
	mLock.unlock();
	return false;
}


OmnThreadPtr
OmnThreadMgr::getThread(const u64 tid)
{
	mLock.lock();
	ThreadMapItr_t itr = mThreadMap.find(tid);
	if (itr == mThreadMap.end()) 
	{
		mLock.unlock();
		return 0;
	}
	OmnThreadPtr thrd = itr->second;
	mLock.unlock();
	return thrd;
}


void
OmnThreadMgr::addThread(const OmnThreadPtr &thread)
{
	// mLock.lock();
	// mThreads.append(thread);
	// mLock.unlock();
}


void
OmnThreadMgr::stopAllThreads()
{
	/*
	const size_t s = mThreads.entries();
	mThreads.reset();
	while (mThreads.hasMore())
	{
		(mThreads.crtValue())->stop();
		mThreads.next();
	}
	*/
}


bool
OmnThreadMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter OmnThreadMgr::threadFunc." << endl;

	// E007, 07/13/2003, 2003-0073
	OmnString errmsg;
    while (state == OmnThrdStatus::eActive)
    {
		// Start the heartbeat first. Then wait a while and then 
		// check whether each thread is ok.
		mLock.lock();
		ThreadMap_t threads = mThreadMap;
		mLock.unlock();
		ThreadMapItr_t itr;
		for (itr = threads.begin(); itr != threads.end(); itr++)
		{
			if (itr->second != thread) itr->second->heartbeat();
		}
		
		for (itr = threads.begin(); itr != threads.end(); itr++)
		{
			if (itr->second != thread) itr->second->signal();
		}

		if (!OmnTimerSelf)
		{
			// The timer has not been started yet. We will wait a little bit
			OmnSleep(1);
			continue;
		}

		OmnSleep(mHbTimerSec);
		mThreadStatus = true;

		// It woke up. Check whether it was waken by the timer
		// mHbResp->setStatus(true);
		// mHbResp->setErrMsg("");

		if (!mIsWakenUpByTimer)
		{
			OmnString  errorThrds = "<errors>";
			for (itr = threads.begin(); itr != threads.end(); itr++)
			{
				OmnThreadPtr thread = itr->second;
				if (!thread->isManaged()) continue;

				errmsg = "";
				if (!thread->checkThread(errmsg))
				{
					// The thread is not good. Determine whether it is a 
					// critical thread.
					//
					if (thread->isCriticalThread())
					{
						//felicia, 2013/06/21
						OmnString key = thread->getName();
						key << "_" << thread->getLogicId() << "_" << AosGetSelfServerId();

						OmnString time = OmnGetTime(AosLocale::eChina);
						OmnString str = "<error thread_name=\"";
						str << thread->getName() << "\" "
							<< "zky_key=\"" << key << "\" "
							<< "zky_filemsg=\"" << thread->getFileName() << ":" << thread->getFileLine() << "\" "
							<< "thread_logicid=\"" << thread->getLogicId() << "\" "
							<< "serverid=\"" << AosGetSelfServerId() << "\" "
							<< "error_time=\"" << time << "\" />";
						
						//errorThrds.push_back(key);
						errorThrds << str;

						//OmnScreen << "Thread failed: " 
						//	<< thread->getName() << "(" << thread->getLogicId() <<
						//	"):  " << thread->getFileName() << ":" << thread->getFileLine() << ":" << errmsg << endl;
						
						//
						// It is a critical thread. This unit fails.
						//
					}
				}
			}

			errorThrds << "</errors>";
			mLock.lock();
			mErrorThrds = errorThrds;
			mLock.unlock();
		}

		// OmnTrace << "AAAAA To unlock ThreadMgr: " << OmnGetCurrentThreadId() << endl;
    }

    OmnTrace << "Leaving OmnThreadMgr::threadFunc" << endl;
    return true;
}


bool
OmnThreadMgr::signal(const int threadLogicid)
{
	mLock.lock();
	mIsWakenUpByTimer = false;
	mCondVar->signal();
	mLock.unlock();
    return true;
}


void    
OmnThreadMgr::heartbeat(const int tid)
{
    mLock.lock();
    mThreadStatus = false;
    mCondVar->signal();
    mLock.unlock();
}


bool
OmnThreadMgr::checkThread(OmnString &errmsg, const int tid) const
{
	if (mThreadStatus)
	{
		return true;
	}

	// If the status is false and it has been too long 
	// since the time mHeartbeatStatus was set to false,
	// it indicates this thread is in trouble.
	if (OmnTime::getCrtSec() - mHeartbeatStartSec >= mHbTimerSec + 1)
	{
		// E007, 07/13/2003, 2003-0073
		errmsg = "ThreadMgr failed. ";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return true;
}


bool
OmnThreadMgr::registerThreadAction(
		const char *file, 
		const int line, 
		const int timer,
		const OmnString &action)
{
	OmnThreadPtr thread = getCurrentThread();
	if (!thread)
	{
		OmnAlarm << "Failed get thread: " << enderr;
		return false;
	}

	return thread->registerAction(file, line, timer, action);
}


bool
OmnThreadMgr::unregisterThreadAction(const OmnString &action)
{
	OmnThreadPtr thread = getCurrentThread();
	if (!thread)
	{
		OmnAlarm << "Failed get thread: " << enderr;
		return false;
	}
	return thread->unregisterAction(action);
}


void
OmnThreadMgr::appendActionLog(
		const char *file, 
		const int line, 
		const OmnString &log)
{
	OmnThreadPtr thread = getCurrentThread();
	if (!thread)
	{
		OmnAlarm << "Failed get thread: " << enderr;
		return;
	}
	thread->appendActionLog(file, line, log);
}


bool
OmnThreadMgr::setMainThreadId(const int id)
{
	aos_assert_r(mMainThreadId == -1, false);
	mMainThreadId = id;
	return true;
}

void
OmnThreadMgr::getErrorThread(OmnString &errorThrds)
{
	mLock.lock();
	errorThrds = mErrorThrds;
	mLock.unlock();
}

