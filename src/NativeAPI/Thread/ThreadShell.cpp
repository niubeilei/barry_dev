/////////////////////////////////////////////////////////////// 
// Created by:	Open Source Society 
// Created:		10/3/2000 
// Comments: 
// 
// Change History: 
//	10/03/2000 - File created 
// 
/////////////////////////////////////////////////////////////// 
#include "Thread/ThreadShell.h" 
 
#include "Alarm/Alarm.h" 
#include "Event/Event.h" 
#include "Message/Req.h" 
#include "Message/Resp.h" 
#include "Porting/TimeOfDay.h"
#include "Thread/ThreadMgr.h" 
#include "Thread/Mutex.h" 
#include "Thread/ThrdShellProc.h" 
#include "Thread/ThreadShellCaller.h" 
#include "Thread/ThrdTrans.h" 
#include "Thread/Thread.h" 
#include "Thread/CondVar.h" 
#include "Util/OmnNew.h" 
#include "Util1/Time.h" 
#include "UtilComm/ConnBuff.h" 
 
 
OmnThreadShell::OmnThreadShell(
		const OmnString &name, 
		const OmnString &file_name,
		const int file_line,
		const OmnThreadShellCallerPtr &caller) 
: 
//mInLock(OmnNew OmnMutex()), 
//mInCondVar(OmnNew OmnCondVar()), 
mCntlLock(OmnNew OmnMutex()), 
mCntlCondVar(OmnNew OmnCondVar()), 
mName(name), 
mState(eStateIdle), 
//mEventLock(OmnNew OmnMutex()), 
//mEventCondVar(OmnNew OmnCondVar()), 
//mTransIdWaitOn(0),
mStartTime(0),
mCaller(caller),
mFileName(file_name),
mFileLine(file_line)
{ 
    OmnThreadedObjPtr thisPtr(this, false); 
	mCntlLock->lock();
    if (!mThread) 
    { 
        mThread = OmnNew OmnThread(thisPtr, mName, 0, true, false, mFileName, mFileLine); 
    } 
 
	OmnRslt rslt = mThread->start(); 
	mCntlLock->unlock();
	//OmnTrace << "Thread shell created: " << mName << endl; 

	if(!caller)
	{
		OmnAlarm << "missing caller," << name << enderr;
	}
} 
 
 
OmnThreadShell::~OmnThreadShell() 
{
	OmnScreen << "Deleting thread shell: " << this << endl;
}


bool 			
OmnThreadShell::startProcess(const OmnThrdShellProcPtr &processor)
{
	mCntlLock->lock();
	if (!mProcessor.isNull())
	{
		// Before calling this function, the caller is sure mProcessor
		// is null. This is true because the caller gets this thread
		// shell from ThreadShellMgr, which will ensure that. 
		OmnAlarm << OmnErrId::eAlarmProgramError
			<< "mProcessor is not null" << enderr;
		mCntlLock->unlock();
		return false;
	}

	if (mState == eStateStopped) 
	{
//OmnScreen  << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx1111111111111111111" << endl;
    	OmnThreadedObjPtr thisPtr(this, false); 
        mThread = OmnNew OmnThread(thisPtr, mName, 0, true, false, mFileName, mFileLine); 
		mThread->start();
	}	

	mState = eStateActive;
	mProcessor = processor;
	mCntlCondVar->signal();
	mCntlLock->unlock();
	mStartTime = OmnGetSecond();

	return true;
}


/*
bool
OmnThreadShell::getTrans(const OmnThrdTransPtr &trans, const u32 timerSec) 
{ 
	//  
	// This function retrieves a message from the message queue.  
	// If no messages are in the queue and timerSec > 0, it will  
	// wait maximum "timerSec". It either gets a message or time out. 
	// If it is timed out, false is returned. Otherwise, true  
	// is returned. 
	// 
	u64 crtSec = OmnTime::getCrtSec(); 
	Entry entry; 
OmnTrace << "To get trans" << endl; 
	while (1) 
	{ 
		mInLock->lock(); 
		if (mInQueue.entries() <= 0) 
		{ 
			if (OmnTime::getCrtSec() - crtSec >= timerSec) 
			{ 
				//  
				// Failed to receive anything. Timed out. Return 0. 
				//  
				mInLock->unlock(); 
				return 0; 
			} 
 
			//  
			// No more messages. Will wait.  
			//  
			bool isTimedout = false; 
OmnTrace << "To wait" << endl; 
			mInCondVar->timedWait(mInLock, isTimedout, timerSec); 
			mInLock->unlock(); 
OmnTrace << "Waken up" << endl; 
			continue; 
		} 
 
		//  
		// There are some messages in the queue. Pop up the first one.  
		//  
		entry = mInQueue.popFront(); 
		mInLock->unlock(); 
		trans->msgRcved(entry.mBuff); 
		return true; 
	} 
 
	return false; 
} 
 
 
bool 
OmnThreadShell::waitOn(const AosEventId::E eventId,  
					   AosEventPtr &event, 
					   const int ttl, 
					   bool &isTimedout) 
{ 
	//  
	// This function waits on an event. Each thread shell has an event 
	// queue. When an event is added to the queue, this function will 
	// be waken up. It checks whether the event is the one it is looking 
	// for ('eventId'). If yes, it returns the event. Note that it  
	// uses the timed wait, which can be timed out. If timed out, it 
	// will return false, and isTimedout is set to true. 
	// 
	isTimedout = false; 
	u64 crtSec = OmnTime::getCrtSec(); 
	while (1) 
	{ 
		mEventLock->lock(); 
		if (mEventQueue.entries() <= 0) 
		{ 
			int diff = (int)(OmnTime::getCrtSec() - crtSec); 
			if (diff >= ttl) 
			{ 
				//  
				// Failed to receive anything. Timed out. Return false. 
				//  
				mEventLock->unlock(); 
				isTimedout = true; 
				return false; 
			} 
 
			//  
			// No more messages. Will wait.  
			//  
			int timeToWait = ttl - diff; 
			mEventCondVar->timedWait(mEventLock, isTimedout, timeToWait); 
			continue; 
		} 
 
		//  
		// There are some thing happened  
		//  
		mEventQueue.reset(); 
		while (mEventQueue.hasMore()) 
		{ 
			if ((mEventQueue.crtValue())->getEventId() == eventId) 
			{ 
				event = mEventQueue.crtValue(); 
				mEventQueue.eraseCrt(); 
				mEventLock->unlock(); 
				return true; 
			} 
 
			mEventQueue.next(); 
		} 
 
		mEventLock->unlock(); 
	} 
 
	OmnShouldNeverComeToThisPoint; 
	return false; 
} 
 
 
void 
OmnThreadShell::signalEvent(const AosEventPtr &event) 
{ 
	mEventLock->lock(); 
	mEventQueue.append(event); 
	mEventCondVar->signal(); 
	mEventLock->unlock(); 
	return; 
} 
 
 
bool	 
OmnThreadShell::peekTrans(OmnThrdTransPtr &trans) 
{ 
	//  
	// This function checks whether there are transactions in the  
	// queue. If there are, it returns the first one. Otherwise, it  
	// returns 0. 
	// 
	Entry entry; 
	mInLock->lock(); 
	if (mInQueue.entries() > 0) 
	{ 
		entry = mInQueue.popFront(); 
		mInLock->unlock(); 
		trans->msgRcved(entry.mBuff); 
		return true; 
	} 
 
	mInLock->unlock(); 
	return false; 
} 
 
 
void			 
OmnThreadShell::clear() 
{ 
	//  
	// This function clears the transaction and event queues.  
	// It is normally called when it start processing. 
	// 
	mInLock->lock(); 
	mInQueue.clear(); 
	mInLock->unlock(); 
 
	mEventLock->lock(); 
	mEventQueue.clear(); 
	mEventLock->unlock(); 
} 
*/


bool	
OmnThreadShell::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{

	//OmnTrace << "Enter thread shell function: " 
	//	<< OmnGetCurrentThreadId() << mName << endl;

	//OmnThreadShellPtr thisptr(this, false);
	while (state == OmnThrdStatus::eActive)
	{
		OmnThreadShellPtr thisptr(this, false);
		mCntlLock->lock();
		if (mProcessor.isNull())
		{
			// Nothing to do. Go sleep.
			bool timeout;
			static int sec = 600;
			mCntlCondVar->timedWait(mCntlLock, timeout, sec, 0);
			if (timeout)
			{
//OmnScreen << "xxxxxxxxxxxxxxxxxxxxxxxxx==========" << endl;
				state = OmnThrdStatus::eExit;
				mState = eStateStopped;
				mThread = NULL;
			}	
			mCntlLock->unlock();

			continue;
		}

		// mProcessor is not null. Call its function to start process. 
		mCntlLock->unlock();

		// Note that no one should set mProcessor to other processor or to 0.
		// When mProcessor finishes the processing, we will reset it to 0.
		mProcessor->run();

		// Chen Ding, 06/27/2012
		mProcessor->procFinished();
		mProcessor->postSem();
		mProcessor->threadFinished();

		// mProcessor finished processing.  
		mCntlLock->lock();
		mProcessor = 0;
		mState = eStateIdle;
		mCntlLock->unlock();

		mCaller->returnShell(thisptr);

		// Clear the event queue
		//mEventLock->lock();
		//mEventQueue.clear();
		//mEventLock->unlock();
	}

	return true;
}


bool	
OmnThreadShell::signal(const int threadLogicId)
{
	mCntlLock->lock();
	mCntlCondVar->signal();
	mCntlLock->unlock();
	return true;
}


bool    
OmnThreadShell::checkThread(OmnString &err, const int thrdLogicId) const
{
	return mThreadStatus;
}


/*
bool		
OmnThreadShell::procEvent(const AosEventPtr &event, bool &cont)
{
	// An event is received from the event manager. Add it to the event queue.
	cont = true;
	mEventLock->lock();
	mEventQueue.append(event);
	mEventCondVar->signal();
	mEventLock->unlock();
	return true;
}


bool			
OmnThreadShell::waitForMsg(const OmnThrdTransPtr &trans, const u32 timer)
{
	// This function is the same as getTrans(...)
	mTransIdWaitOn = trans->getTransId();
	return getTrans(trans, timer);
}


bool	
OmnThreadShell::msgRcved(const OmnConnBuffPtr &buff, const u32 transId) 
{ 
	//  
	// The connection received a message. The message is stored in 
	// 'buff'. The message's transaction is 'transId'. This function 
	// stores the message into mInQueue. If anyone is waiting for 
	// messages, it will signal it. 
	// 
	Entry entry(buff, transId); 
	if (mTransIdWaitOn != transId) 
	{ 
		OmnAlarm << "Got an incorrect message: " << transId 
			<< ", but expecting: " << mTransIdWaitOn << enderr; 
		return false; 
	} 
 
	mTransIdWaitOn = 0; 
	mInLock->lock(); 
	mInQueue.append(entry); 
	mInLock->unlock(); 
	return true; 
} 
 
 
void 
OmnThreadShell::setWaitForTransId(const u32 transId) 
{ 
	mTransIdWaitOn = transId; 
} 
 

u32
OmnThreadShell::getProcLength() const
{
	return OmnGetSecond() - mStartTime;
}


OmnString
OmnThreadShell::getProcName() const
{
	if (!mProcessor) return "NoProcessor";
	return mProcessor->getName();
}


bool
OmnThreadShell::isFinished() const
{
	if (!mProcessor) return true;
	return mProcessor->isFinished();
}
*/
