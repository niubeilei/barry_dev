////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Thread.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Thread/Thread.h"

#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Debug/Error.h"
#include "Debug/Except.h"
#include "Porting/ThreadDef.h"
#include "Thread/ThrdStatus.h"
#include "Thread/ThreadMgr.h"
#include "Util/OmnNew.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"

#include <ostream>

extern OmnThreadMgr* OmnThreadMgrSelf;

static volatile int sgThreadNum = 0;


OmnThread::OmnThread(const OmnThreadedObjPtr &obj, 
					 const OmnString &name,
					 const int thrdLogicId,
					 const bool managedFlag,
					 const bool isCriticalThread,
			  		 const OmnString &file_name,
					 const int	file_line,
					 const bool highpriority)
:
mMutex(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMutexWait(OmnNew OmnMutex()),
mCondVarWait(OmnNew OmnCondVar()),
mThreadId(0),
mName(name),
mThreadObj(obj),
mThreadStatus(OmnThrdStatus::eIdle),
mLogicId(thrdLogicId),
mHighPriority(highpriority),
mIsCriticalThread(isCriticalThread),
mIsManaged(managedFlag),
//mTransid(0),
mNeedWait(true),
mActionLineno(-1),
mActionTimer(-1),
mActionStart(-1),
mErrorReported(false),
mFileName(file_name),
mFileLine(file_line)
{
	OmnString fname = "log_";
	fname << (int)OmnGetCurrentThreadId();

	sgThreadNum++;
	//OmnScreen << "thread added, now thread num:" << sgThreadNum << endl;
}


OmnThread::OmnThread(
		const unsigned long threadid, 
		const OmnString &name,
		const OmnString &file_name,
		const int file_line)
:
mMutex(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mMutexWait(OmnNew OmnMutex()),
mCondVarWait(OmnNew OmnCondVar()),
mThreadId(threadid),
mName(name),
mThreadObj(NULL),
mThreadStatus(OmnThrdStatus::eActive),
mIsManaged(true),
//mTransid(0),
mFileName(file_name),
mFileLine(file_line)
{
	// Need to add this threaded object to ThreadMgr.
	OmnThreadPtr thisPtr(this, false);
	OmnThreadMgr::setThread(thisPtr);
	
	sgThreadNum++;
	//OmnScreen << "thread added, now thread num:" << sgThreadNum << endl;
}


OmnThread::~OmnThread()
{
	sgThreadNum--;
	//OmnScreen << "thread deleted, now thread num:" << sgThreadNum << endl;
}


OmnRslt
OmnThread::start()
{
	//
	// Before calling this function, the thread should be idle or stopped.
	// If it is not, it is a program error. Log the error and return.
	//
	// OmnTrace << "To start thread: " << mName << " " << this << endl;
	OmnThreadPtr thisPtr(this, false);
	mMutex->lock();
	int ret = 0;
	//cout << __FILE__ << ":" << __LINE__ << endl;
	switch (mThreadStatus)
	{
	case OmnThrdStatus::eIdle:
		{
			mThreadStatus = OmnThrdStatus::eActive;
			// cout << __FILE__ << ":" << __LINE__ << endl;
			mThreadId = OmnCreateThread(OmnThread::msgThreadFunc, 
					this, mHighPriority);
			OmnThreadMgr::setThread(thisPtr);
			std::ostringstream os;
			os << hex << "<0x" << (u32)mThreadId << dec
				<< ":" << OmnGetTime(AosLocale::eChina)		// Ken 2013/04/24
				<< ":" << __FILE__ << ":" << __LINE__ << "> "
				<< "Starting thread: " <<  mName ;
			cout << os.str() << endl;
			// cout << __FILE__ << ":" << __LINE__ << ":" << mThreadId << endl;
			mMutex->unlock();
			if(ret)
			{
				OmnAlarm << "Start Thread Failed! : "<< ret << enderr;
			}
			return !ret;
		}

	case OmnThrdStatus::eStop:
	case OmnThrdStatus::eStopped:
		 //cout << __FILE__ << ":" << __LINE__ << endl;
		 mThreadStatus = OmnThrdStatus::eActive;
	 	 mCondVar->signal();
		 OmnTrace << "To stop thread: " << mName << endl;
		 mMutex->unlock();
		 return true;

	case OmnThrdStatus::eActive:
		 // It should not happen
		 //cout << __FILE__ << ":" << __LINE__ << endl;
		 OmnAlarm << "To Start An Active Thread!" << enderr;
		 mMutex->unlock();
		 return false;
		 
	case OmnThrdStatus::eExit:
		 // It should not happen
		 cout << __FILE__ << ":" << __LINE__ << endl;
		 OmnAlarm << "Cannot Start An Exit Thread!" << enderr;
		 mMutex->unlock();
		 return false;

	case OmnThrdStatus::eExited:
		 // It should not happen
		 mMutex->unlock();
		 cout << __FILE__ << ":" << __LINE__ << endl;
		 OmnAlarm << "Cannot Start An Exited Thread!" << enderr;
		 return false;

	default:
		 // This is a program error.
		 mMutex->unlock();
		 OmnAlarm << "Thread status Unrecognized: " 
			<< mThreadStatus << enderr;
		 return false;
	}
}


OmnRslt
OmnThread::stop()
{
	//
	// Stop a thread requires to exit the thread function. A stopped 
	// thread can not be resumed, but it is possible to restart the thread.
	//

	//
	// If it has not active, it is an error. Log the error and return false.
	//
	mMutex->lock();
	if (mThreadStatus != OmnThrdStatus::eActive)
	{
		//
		// Be nice to the caller. Just return. No error reported!
		//
		mMutex->unlock();
		return true;
	}

	//
	// Otherwise, it changes its status to "eStop"
	//
	mThreadStatus = OmnThrdStatus::eStop;

	//
	// And then signal the thread processor in case it is sleeping. After
	// the signal, the thread processor is guaranteed to recheck the 
	// thread status. Since we changed the status to "eStop", it should
	// finish its infinite loop and return.
	//
	if (mThreadObj) mThreadObj->signal(mLogicId);

	// Chen Ding, 05/10/2010
	void *state;
	pthread_join(mThreadId, &state);

	mMutex->unlock();
	return true;
}


OmnRslt
OmnThread::exitThread()
{
	//
	// Exit a thread requires to exit the thread function. An exited
	// thread can not be resumed.
	//

	//
	// If it has not active, it is an error. Log the error and return false.
	//
	mMutex->lock();
	mThreadStatus = OmnThrdStatus::eExit;

	//
	// And then signal the thread processor in case it is sleeping. After
	// the signal, the thread processor is guaranteed to recheck the 
	// thread status. Since we changed the status to "eStop", it should
	// finish its infinite loop and return.
	//
	if (mThreadObj) mThreadObj->signal(mLogicId);
	mMutex->unlock();
	return true;
}


OmnThreadReturnType		
OmnThread::msgThreadFunc(OmnThreadParmType obj)
{
	if (((OmnThread *)obj)->mThreadObj)
	{
		((OmnThread *)obj)->threadFunc();
	}
	return 0;
}


void 
OmnThread::threadFunc()
{
	//
	// Go to the thread function
	//
	OmnThreadPtr thisPtr(this, false);
	mThreadId = OmnGetCurrentThreadId();
	//cout << __FILE__ << ":" << __LINE__ << ":" << OmnGetCurrentThreadId() << endl;
	OmnThreadMgr::setThread(thisPtr);
	//cout << __FILE__ << ":" << __LINE__ << ":" << OmnGetCurrentThreadId() << endl;

	//OmnTrace << "To enter Thread: " << mName << endl;

try
{
	while (1)
	{
		//
		// Call the mThreadObj's member function "threadFunc()". This function 
		// should not return until it finishes the processing. Normally this is
		// an infinite loop. This function does not return until mThreadStatus
		// changes to a state other than "eActive". 
		//
		if(mThreadObj) mThreadObj->threadFunc(mThreadStatus, this);

		mMutex->lock();
		switch (mThreadStatus)
		{
		case OmnThrdStatus::eIdle:
			 //
			 // It shouldn't be eIdle. We don't need to do anything. Just wait
			 // on the signal.
			 //
			 mCondVar->wait(mMutex);
			 mMutex->unlock();
			 break;

		case OmnThrdStatus::eStop:
			 //
			 // Someone requested to stop this thread.  
			 // Change the status to eStoped, and then wait on its signal. 
			 // This thread will not wake
			 // up unless someone signals it. 
			 //
			 mThreadStatus = OmnThrdStatus::eStopped;
			 mCondVar->wait(mMutex);
			 mMutex->unlock();
			 break;

		case OmnThrdStatus::eActive:
			 //
			 // It should go back to process.
			 //
			 mMutex->unlock();
			 break;

		case OmnThrdStatus::eExit:
			 //
			 // It should exit from this function, which will terminate 
			 // the thread.
			 //
			 mThreadStatus = OmnThrdStatus::eExited;
			 mThreadObj = 0;
			 mMutex->unlock();
			 {
				 OmnThreadPtr thisptr(this, false);
			 	 OmnThreadMgr::getSelf()->removeThread(thisptr);
			 }
		 	 OmnScreen << "Exiting thread: " << mName <<  endl;
			 //OmnExitThread(0);
			 //return;
			 goto end;

		case OmnThrdStatus::eExited:
			 //
			 // Should not happen
			 //
			 OmnTrace << "Exited thread: " << mName << " " << mThreadId << endl;
			 mMutex->unlock();
			 //return;
			 goto end;
			 
		case OmnThrdStatus::eStopped:
			 // 
			 // It should be in these states. We don't need to do anything. 
			 // Keep on waiting. 
			 //
			 mCondVar->wait(mMutex);
			 mMutex->unlock();
			 break;

		default:
			 //
			 // This is a program error.
			 //
			 mMutex->unlock(); // QQQQ
			 OmnAlarm << OmnErrId::eAlarmProgramError
				<< "Unrecognized OmnThrdStatus value: " 
				<< mThreadStatus 
				<< ". Thread Name: " << mName << enderr;
			 //return;
			 goto end;

		}
	}
	if(mThreadObj) mThreadObj->postProc();
}

catch (const OmnExcept &e)
{
	if (e.getErrid()  == OmnErrId::eIOError)
	{
		OmnScreen << "Caught an execption: " << e.getErrmsg() << endl;
		//OmnAlarm << "Caught an execption: " << e.getErrmsg() << enderr;	
		bool rslt = AosSendDiskBadMsg();
		aos_assert(rslt);
	}
	else
	{
		OmnAlarm << OmnErrId::eAlarmProgramError
			<< "Caught an execption: " << e.getErrmsg() << enderr;
		OmnTrace << "What to do???" << endl;
	}
}
catch (const exception &e)
{
	OmnAlarm << e.what() << enderr;
}
catch (...)
{
	OmnAlarm << "Unkown Exception!" << enderr;
}

end:
	return;
}


void
OmnThread::heartbeat()
{
	// If the thread has a pending action, it checks whether it 
	// is too long. 
	if (mActionTimer > 0)
	{
		// There is a pending action
		if (mActionStart <= 0)
		{
			OmnAlarm << "Action start incorrect: " << mActionStart << enderr;
		}
		else
		{
			u32 sec = OmnGetSecond();
			if (sec < (u32)mActionStart)
			{
				OmnAlarm << "Action start incorrect: " << sec << ":" << mActionStart << enderr;
			}
			else
			{
				int delta = sec - mActionStart;
				if (delta >= mActionTimer && !mErrorReported)
				{
					// mErrorReported = true;
					OmnAlarm << "Thread " << mName << ":" << getThreadId() 
						<< " timed out: " << mActionTimer 
						<< ". Action name: " << mAction << enderr;
				}
			}
		}
	}

	if (mThreadObj) mThreadObj->heartbeat(mLogicId);
}


bool
OmnThread::signal()
{
	if (mThreadObj) mThreadObj->signal(mLogicId);
	return true;
}


bool    
OmnThread::checkThread(OmnString &err) const
{
	if (mThreadObj) return mThreadObj->checkThread111(err, mLogicId);
	return true;
}


bool
OmnThread::isStopped()
{
	bool stopped = false;
	mMutex->lock(); 
	stopped = (mThreadStatus == OmnThrdStatus::eStopped)||(mThreadStatus == OmnThrdStatus::eExited);
	mMutex->unlock(); 
	return stopped;
}


bool
OmnThread::isExited()
{
	bool exited = false;
	mMutex->lock(); 
	exited = (mThreadStatus == OmnThrdStatus::eExited);
//if(!exited)cout << __FILE__ << __LINE__ << mThreadStatus  <<endl;
	mMutex->unlock(); 
	return exited;
}


/*
bool
OmnThread::wait(
		const u32 time_sec, 
		const u32 time_usec,
		bool &timedout,
		const u64 &trans_id)
{
	// Ketty 2012/10/17
	u64 starttime = OmnGetTimestamp();
	u64 duration = time_sec * 1000000 + time_usec;
	u32 timeleft_sec = time_sec;
	u32 timeleft_usec = time_usec;
	bool first_time = true;
	
	u64 tid = 0;
	bool finished = false;
	while (!finished)
	{
		if (mTransid == trans_id)
		{
			return true;
		}

		mTransid = 0;
		if (first_time)
		{
			first_time = false;
		}
		else
		{
			u64 endtime = OmnGetTimestamp();
			if (endtime >= starttime + duration)
			{
				timedout = true;
				return 0;
			}
			else
			{
				u64 timeleft = starttime + duration - endtime;
				timeleft_sec = timeleft / 1000000;
				timeleft_usec = timeleft % 1000000;
			}
		}
		int eno = mCondVarWait->timedWait(mMutexWait, timedout, timeleft_sec, timeleft_usec);
		// OmnScreen << "Woke up: " << eno << ":" << mTransid << endl;
		if (eno == 0)
		{
			// This is a correct response;
			//finished = true;
			// OmnScreen << "Got something: " << mTransid << ":" << mResponse.getPtr() << endl;
			tid = mTransid;
			if (tid == trans_id) 
			{
				return true;
			}
			// OmnScreen << "Transid mismatch: " << tid << ":" << trans_id << endl;
			// return 0;
			continue;	// Ketty 2012/02/02
		}

		switch (eno)
		{
		case ETIMEDOUT:
			 // Timed out. 
			 timedout = true;
			 finished = true;
			 // OmnScreen << "Timedout: " << trans_id << ":" << mTransid << endl;
			 break;

		case EINVAL:
			 // Time is invalid. Need to return.
			 OmnAlarm << "Time incorrect: " << timeleft_sec << ":" 
				 << timeleft_usec << enderr;
			 finished = true;
			 break;

		case EPERM:
			 // The mutex was not owned by the current thread at the time of the call.
			 OmnAlarm << "Failed locking!" << enderr;
			 finished = true;
			 break;

		default:
			 OmnAlarm << "Unknown error: " << eno << enderr;
			 break;
		}

		tid = mTransid;

		if (tid == trans_id ) return true;
	}

	if (tid == trans_id ) return true;
	return false;
}

bool
OmnThread::wakeup(const u64 trans_id)
{
	// Ketty 2012/10/17
	if (trans_id == 0) 
	{
		mTransid = 0;
		mResponse = 0;
		return false;
	}

	mTransid = trans_id;
	mResponse = 0;
	mCondVarWait->signal();
	return true;
}
*/

bool
OmnThread::wait(
		const u32 time_sec, 
		const u32 time_usec,
		bool &timedout,
		//const u64 &trans_id)
		const AosTransId &trans_id)
{
	//aos_assert_r(trans_id && trans_id == mTransid, 0);
	aos_assert_r(trans_id != AosTransId::Invalid && trans_id == mTransid, 0);

	AosBuffPtr resp;
	mMutexWait->lock();
	if(mNeedWait)
	{
		//waitLocked(time_sec, time_usec, timeedout, trans_id); 
		mCondVarWait->timedWait(mMutexWait, timedout, time_sec);
	}
	
	mResponse = 0;
	//mTransid = 0;
	mTransid = AosTransId::Invalid;
	mNeedWait = true;
	mMutexWait->unlock();

	return true;
}


bool
//OmnThread::wakeup(const u64 trans_id)
OmnThread::wakeup(const AosTransId &trans_id)
{
	//aos_assert_r(trans_id, false);
	aos_assert_r(trans_id != AosTransId::Invalid, false);
		    
	mMutexWait->lock();
	if(mTransid != trans_id)
	{
		mMutexWait->unlock();
		return true;
	}
				    
	mNeedWait = false;
	mResponse = 0;
	mCondVarWait->signal();
	mMutexWait->unlock();
	return true;
}


void
//OmnThread::setWaitTransId(const u64 trans_id)
OmnThread::setWaitTransId(const AosTransId &trans_id)
{
	aos_assert(trans_id != AosTransId::Invalid);
		    
	mMutexWait->lock();         
	mTransid = trans_id; 
	mMutexWait->unlock();           
}


void
//OmnThread::setResponse(const AosBuffPtr &response, const u64 &trans_id)
OmnThread::setResponse(const AosBuffPtr &response, const AosTransId &trans_id)
{
	//aos_assert(trans_id && response);
	aos_assert(trans_id != AosTransId::Invalid && response);

	mMutexWait->lock();
	if(mTransid != trans_id)
	{
		mMutexWait->unlock();
		return;
	}

	mResponse = response;
	mCondVarWait->signal();
	mMutexWait->unlock();
}


AosBuffPtr
OmnThread::getResponse(
		const u32 time_sec, 
		const u32 time_usec,
		bool &timedout,
		//const u64 &trans_id)
		const AosTransId &trans_id)
{
	// It retrieves the response. The response must be for the transaction
	// 'trans_id'. If not, it will ignore it and keep on waiting.
	
	aos_assert_r(trans_id == mTransid, 0);

	AosBuffPtr resp;
	mMutexWait->lock();
	if(mResponse)
	{
		resp = mResponse;
	}
	else
	{
		//resp = getRespLocked();
		mCondVarWait->timedWait(mMutexWait, timedout, time_sec);
		resp = mResponse;
	}
	mResponse = 0;
	//mTransid = 0;
	mTransid = AosTransId::Invalid;
	mMutexWait->unlock();

	return resp;
}


/*
void
OmnThread::setResponse(const AosBuffPtr &response, const u64 &trans_id)
{
	if (trans_id == 0 || !response) 
	{
		mTransid = 0;
		mResponse = 0;
		return;
	}

	mTransid = trans_id;
	mResponse = response;
	mCondVarWait->signal();
}



//OmnString
AosBuffPtr
OmnThread::getResponse(
			const u32 time_sec, 
			const u32 time_usec,
			bool &timedout,
			const u64 &trans_id)
{
	// It retrieves the response. The response must be for the transaction
	// 'trans_id'. If not, it will ignore it and keep on waiting.
	u64 starttime = OmnGetTimestamp();
	u64 duration = time_sec * 1000000 + time_usec;
	u32 timeleft_sec = time_sec;
	u32 timeleft_usec = time_usec;
	bool first_time = true;
	//AosXmlTagPtr response;
	//OmnString response;
	AosBuffPtr response;
	u64 tid = 0;
	bool finished = false;
	while (!finished)
	{
		//mMutexWait->lock();
		response = mResponse;
		if (mTransid == trans_id && mResponse)
		{
			//mMutexWait->unlock();
			return response;
		}

		mResponse = 0;
		mTransid = 0;
		if (first_time)
		{
			first_time = false;
		}
		else
		{
			u64 endtime = OmnGetTimestamp();
			if (endtime >= starttime + duration)
			{
				timedout = true;
				//mMutexWait->unlock();
				return 0;
			}
			else
			{
				// endtime < starttime + duration, or
				// 0 < starttime + duration - endtime
				u64 timeleft = starttime + duration - endtime;
				timeleft_sec = timeleft / 1000000;
				timeleft_usec = timeleft % 1000000;
			}
		}
		int eno = mCondVarWait->timedWait(mMutexWait, timedout, timeleft_sec, timeleft_usec);
		// OmnScreen << "Woke up: " << eno << ":" << mTransid << endl;
		if (eno == 0)
		{
			// This is a correct response;
			//finished = true;
			// OmnScreen << "Got something: " << mTransid << ":" << mResponse.getPtr() << endl;
			tid = mTransid;
			response = mResponse;
			//mMutexWait->unlock();
			if (tid == trans_id) 
			{
				// OmnScreen << "Found the response: " << response.getPtr() << endl;
				return response;
			}
			// OmnScreen << "Transid mismatch: " << tid << ":" << trans_id << endl;
			// return 0;
			continue;	// Ketty 2012/02/02
		}

		switch (eno)
		{
		case ETIMEDOUT:
			 // Timed out. 
			 timedout = true;
			 finished = true;
			 // OmnScreen << "Timedout: " << trans_id << ":" << mTransid << endl;
			 break;

		case EINVAL:
			 // Time is invalid. Need to return.
			 OmnAlarm << "Time incorrect: " << timeleft_sec << ":" 
				 << timeleft_usec << enderr;
			 finished = true;
			 break;

		case EPERM:
			 // The mutex was not owned by the current thread at the time of the call.
			 OmnAlarm << "Failed locking!" << enderr;
			 finished = true;
			 break;

		default:
			 OmnAlarm << "Unknown error: " << eno << enderr;
			 break;
		}

		response = mResponse;
		tid = mTransid;
		//mMutexWait->unlock();

		if (tid == trans_id && response) return response;
	}

	if (tid == trans_id && response) return response;
	return 0;
}
*/

bool
OmnThread::registerAction(
		const char *fname, 
		const int line, 
		const int timer,
		const OmnString &action)
{
	mActionFname = fname;
	mActionLineno = line;
	mActionTimer = timer;
	mActionStart = OmnGetSecond();
	mAction = action;
	mErrorReported = false;
	return true;
}


bool
OmnThread::unregisterAction(const OmnString &action)
{
	aos_assert_r(mAction == action, false);
	mAction = "";
	mActionTimer = -1;
	mActionFname = "";
	mActionLineno = -1;
	mActionStart = -1;
	mErrorReported = false;
	return true;
}


void
OmnThread::appendActionLog(
		const char *file, 
		const int line, 
		const OmnString &log)
{
	if (mActionLog.length() > eMaxLogLength)
	{
		mActionLog.remove(0, eRemoveLogLength);
	}
	mActionLog << "\n<" << file << ":" << line << ">";
	if (log != "") mActionLog << log;
}


void
OmnThread::resetActionLog()
{
	mActionLog = "";
}


int
OmnThread::cancel()
{
	return OmnCancelThread(mThreadId);
}
