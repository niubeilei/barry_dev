////////////////////////////////////////////////////////////////////////
// 
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved. 
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
// 
// Description:
// This class manages a pool of threads, called OmnThreadShell. 
// These threads are used to process something. If one wants to 
// processing something but that processing may wait for messages, 
// one can derive a class from OmnThrdShellProc and overrides
// the processing function. After that, it can create an instance
// of the derived class and call the member function ::proc(...). 
// This function will queue the instance and return immediately.
// If there are idle threads, this class will get a shell
// and let the shell call OmnThreadShell::startProcess(...).
// This function will wake up the thread and return immediately.
//
// The thread that was waken up will then call 
// OmnThrdShellProc::threadShellProc(...). The caller can call
// the function OmnThrdShellProc::waitUntilFinish(...) to wait
// until the processing finishes. Or it can call 
// OmnThrdShellProc::isFinished() to check whether the processing
// has finished.
//
// Example:
// Step 1: Create a derived class:
// class AosMyThrdShellProc : public OmnThrdShellProc {...};
//
// Step 2: Override OmnThrdShellProc::threadShellProc(...); 
//
// Step 3: Create an instance of AosMyThrdShellProc:
// 
// Step 4: Call OmnThreadShellMgr::proc(...)
//
// Step 5: If needed, we can call "AosThrdShellProc::waitUntilFinished()"
//         or "AosThrdShellProc::isFinished()" to check the status.
//
// int main(...)
// {
// 		AosMyThrdShellProcPtr proc = OmnNew AosMyThrdShellProc();
//
// 		OmnThreadShellMgr::getSelf()->proc(proc);
//
// 		proc->waitUntilFinished();
// 		if (proc->isSuccessful())
// 		{
// 			cout << "Processing finished" << endl;
// 		}
// 		else
// 		{
// 			cout << "Processing failed" << endl;
// 		}
//
// 		return 0;
// }
//
// Change History:
//	10/03/2000
//
///////////////////////////////////////////////////////////////
#include "Thread/ThreadShellMgr.h"

/*
#include "Event/Event.h"
#include "Message/Req.h"
#include "Message/Resp.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Thread/Sem.h"
#include "Thread/ThreadShell.h"
#include "Thread/ThrdShellProc.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "UtilComm/ConnBuff.h"
#include "XmlUtil/XmlTag.h"

OmnSingletonImpl(OmnThreadShellMgrSingleton,
                 OmnThreadShellMgr1,
                 OmnThreadShellMgr1Self,
                "OmnThreadShellMgr1");



OmnThreadShellMgr1::OmnThreadShellMgr1()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mReqLock(OmnNew OmnMutex()),
mReqCondVar(OmnNew OmnCondVar()),
mGetShellTimer(10)
{
}


OmnThreadShellMgr1::~OmnThreadShellMgr1()
{
}


bool		
OmnThreadShellMgr1::proc(const OmnThrdShellProcPtr &proc)
{
	// 
	// The caller wants to process something. This function
	// puts the processor 'proc' into the processor queue
	// and return. 
	// 
	// The thread is waken up by the processor queue (if it is
	// waiting). It allocates a thread shell and process it.
	//
	mReqLock->lock();
	mProcessors.push_back(proc);
	mReqCondVar->signal();
	mReqLock->unlock();
	return true;
}


bool		
OmnThreadShellMgr1::procSync(vector<OmnThrdShellProcPtr> &procs)
{
	// 
	// The caller wants to process something. This function
	// puts the processor 'proc' into the processor queue
	// and return. 
	// 
	// The thread is waken up by the processor queue (if it is
	// waiting). It allocates a thread shell and process it.
	//
	if (procs.empty())
	{
		return true;
	}
	//OmnSemPtr  sem = OmnNew OmnSem((procs.size()-1)*-1);
	OmnSemPtr  sem = OmnNew OmnSem(0);
	mReqLock->lock();
	for (size_t i=0; i<procs.size(); i++)
	{
		procs[i]->setSem(sem);
		mProcessors.push_back(procs[i]);
	}
	mReqCondVar->signal();
	mReqLock->unlock();
	for (size_t i=0 ;i<procs.size(); i++)
	{
		sem->wait();
	}
	return true;
}


bool		
OmnThreadShellMgr1::procAsync(vector<OmnThrdShellProcPtr> &procs)
{
	// 
	// The caller wants to process something. This function
	// puts the processor 'proc' into the processor queue
	// and return. 
	// 
	// The thread is waken up by the processor queue (if it is
	// waiting). It allocates a thread shell and process it.
	//
	if (procs.empty())
	{
		return true;
	}
	//OmnSemPtr  sem = OmnNew OmnSem((procs.size()-1)*-1);
	mReqLock->lock();
	for (size_t i=0; i<procs.size(); i++)
	{
		mProcessors.push_back(procs[i]);
	}
	mReqCondVar->signal();
	mReqLock->unlock();
	return true;
}


bool		
OmnThreadShellMgr1::returnShell(const OmnThreadShellPtr &theShell)
{
	// A thread shell has finished its task. This function
	// wakes the thread up so that if it is waiting for a 
	// thread shell, it is the time to get it.
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


bool 
OmnThreadShellMgr1::threadFunc(OmnThrdStatus::E &state, 
							  const OmnThreadPtr &thread)
{
	// 
	// This thread function waits on the mProcessors queue. 
	// If there is nothing in the queue, it will sleep. When
	// there is something in the queue, this thread funciton
	// is waken up. After that, it retrieves the first 
	// element from mProcessors and process the task. 
	// After that, it repeats the above process again.
	//
	OmnTrace << "Enter OmnThreadShellMgr::threadFunc." << endl;

	OmnThreadShellPtr shell;
	OmnThrdShellProcPtr processor;
	while (state == OmnThrdStatus::eActive)
	{
		processor = 0;
		// Start the timer (one second)
		mReqLock->lock();
		mThreadStatus = true;
		if (mProcessors.size() <= 0)
		{
			// No more to process. 
			mReqCondVar->wait(mReqLock);
			mReqLock->unlock();
			continue;
		}

		// The queue is not empty. Get the first one
		processor = mProcessors.front();
		mProcessors.pop_front();
		mReqLock->unlock();

		// Need to get a thread shell. The function 'getShell(...)'
		// will not return until it gets a thread shell.
		bool alarmed = false;
		shell = 0;
		while (!shell)
		{
			shell = getShell(mGetShellTimer);
			if (shell.isNull())
			{
				// This is a serious problem. Most likely all shells 
				// got stuck in their processing. This should not happen
				// if programming is correct. 
				if (!alarmed)
				{
					alarmed = true;
					OmnAlert << "Run out of thread shell" << endl;
					mLock->lock();
					for (u32 i=0; i<mShells.size(); i++)
					{
						cout << "Shell: " << mShells[i]->getProcName().data() 
							<< ", Finished: " << mShells[i]->isFinished()
							<< ", Duration: " << mShells[i]->getProcLength() 
							<< ", processes: " << mProcessors.size() << endl;
					}
					mLock->unlock();
				}
			}
		}

		shell->startProcess(processor);
	}

	OmnTrace << "Leave OmnThreadShellMgr::threadFunc" << endl;
	return true;

}


bool 
OmnThreadShellMgr1::signal(const int threadLogicId)
{
	mReqLock->lock();
	mReqCondVar->signal();
	mReqLock->unlock();
	return true;
}


bool 
OmnThreadShellMgr1::checkThread(OmnString &errmsg, const int tid) const
{
	if (mThreadStatus)
	{
		return mThreadStatus;
	}

	errmsg = "ThreadShellMgr thread failed. ";
	OmnAlarm << errmsg << enderr;
	return false;
}


bool			
OmnThreadShellMgr1::start()
{
	if (!mThread)
	{
		OmnThreadedObjPtr thisPtr(this, false);
		mThread = OmnNew OmnThread(thisPtr, "ThreadShellMgrThread", 0, true, true);
	}

	mThread->start();
	return true;
}


bool			
OmnThreadShellMgr1::stop()
{
	if (!mThread.isNull())
	{
		mThread->stop();
	}
	return true;
}


bool
OmnThreadShellMgr1::config(const AosXmlTagPtr &def)
{
	return true;
}


OmnThreadShellPtr
OmnThreadShellMgr1::getShell(const uint timerSec)
{
	// This is a blocking call. If there are shells, it returns
	// immediately. Otherwise, it will wait until a thread shell
	// becomes available or timeout. 
	mLock->lock();
	//bool waited = false;
	bool isTimeout = false;

	OmnThreadShellCallerPtr thisptr(this, false);

	u64 startTime = OmnTime::getCrtSec();
	int timeRemained = timerSec - (int)(OmnTime::getCrtSec() - startTime);
	while (!isTimeout && timeRemained > 0)
	{
		for (u32 i=0; i<mShells.size(); i++)
		{
			if (!mShells[i]->isActive())
			{
				// Found an idle shell. 
				OmnThreadShellPtr shell = mShells[i];
				mLock->unlock();
				return shell;
			}
		}

		if (mShells.size() < eMaxThreadShells)
		{
			OmnString name = "ThreadShellMgr_";
			name << mShells.size();
			OmnThreadShellPtr shell = OmnNew OmnThreadShell(name, thisptr);
			mShells.push_back(shell);
			mLock->unlock();
			return shell;
		}

		// All shells are busy. Need to wait.
		//OmnShellMgrLog << "Waiting for shells: " 
		//	<< reqId << endlog;
		//waited = true;
		timeRemained = timerSec - (int)(OmnTime::getCrtSec() - startTime);
		if (timeRemained <= 0)
		{
			// Timeout. 
			break;
		}

		mCondVar->timedWait(mLock, isTimeout, timeRemained);
	}
	mLock->unlock();
	OmnAlert << "Failed to get a shell: " << timerSec << endl;
	return 0;
}


bool		
OmnThreadShellMgr1::msgRcved(const OmnConnBuffPtr &buff, const u32 transId)
{
	// 
	// It checks which thread is listening for the transaction "transId". 
	// If found, it adds the message to that thread.
	//
	mLock->lock();
	for (u32 i=0; i<mShells.size(); i++)
	{
		if (mShells[i]->isActive() && mShells[i]->isWaitingFor(transId))
		{
			mShells[i]->msgRcved(buff, transId);
			mLock->unlock();
			return true;
		}
	}
	
	mLock->unlock();
	OmnAlarm << "Received a message but no one is waiting for: " 
		<< transId << enderr;
	return false;
}

*/
