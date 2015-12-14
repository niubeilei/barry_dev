////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Timer.cpp
// Description:
//
//  Singleton Dependency: AlarmMgr   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/Timer.h" 

#include <stdio.h>
#include <stdlib.h>

#include "Alarm/Alarm.h"
#include "Debug/ExitHandler.h"
#include "Porting/TimeOfDay.h"
#include "Porting/Socket.h"
#include "Porting/IPv6.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/GetHostName.h"
#include "Porting/Select.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "Util/OmnNew.h"
#include "Util1/TimerObj.h"
#include "UtilComm/CommUtil.h"
#include "UtilComm/Udp.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"
#include "XmlUtil/XmlTag.h"


static int sgTimerId = 10;

/*
OmnSingletonImpl(OmnTimerSingleton,
                 OmnTimer,
                 OmnTimerSelf,
                "OmnTimer");
*/

// static OmnTimer::TimerData sgNullTimerData = {0, "", 0, {0,0}, 0};
	
bool
operator >=(const timeval& time1, const timeval& time2)
{
	//
	// It compares two timeval
	//
    if (time1.tv_sec > time2.tv_sec)
    {
        return true;
    }
 
    if (time1.tv_sec == time2.tv_sec &&
        time1.tv_usec + 5000 >= time2.tv_usec)
    {
        return true;
    }
 
    return false;
}
 
 
bool
operator >(const timeval& time1, const timeval& time2)
{
    if (time1.tv_sec > time2.tv_sec)
    {
        return true;
    }
 
    if (time1.tv_sec == time2.tv_sec &&
        time1.tv_usec > time2.tv_usec)
    {
        return true;
    }
    return false;
}
 
 
timeval
operator +(const timeval& time1, const timeval& time2)
{
    timeval newTime;
    newTime.tv_usec = time1.tv_usec + time2.tv_usec;
    if (newTime.tv_usec >= 1000000)
    {
        newTime.tv_usec -= 1000000;
        newTime.tv_sec   = time1.tv_sec + time2.tv_sec + 1;
    }
    else
    {
        newTime.tv_sec = time1.tv_sec + time2.tv_sec;
    }
    return newTime;
}


timeval
operator -(const timeval& time1, const timeval& time2)
{
    timeval newTime;
    if (time2.tv_usec > time1.tv_usec)
    {
        newTime.tv_usec = time1.tv_usec + 1000000 - time2.tv_usec;
        newTime.tv_sec  = time1.tv_sec - time2.tv_sec - 1;
    }
    else
    {
        newTime.tv_usec = time1.tv_usec - time2.tv_usec;
        newTime.tv_sec  = time1.tv_sec  - time2.tv_sec;
    }
    return newTime;
}
 

OmnTimer::OmnTimer()
:
mReadSock(-1),
mWriteSock(-1),
mNfds(0),
mPort(-1),
mLock(OmnNew OmnMutex())
{
	//#ifdef FTT_UNIX_PLATFORM
	//int pfd[2];
    //if (::pipe(pfd) == -1)
    //{
	//	OmnError::log(__FILE__, __LINE__, 0, OmnErrId::E);
	//	OmnExitHandler::handleExit("Failed to start timer");
    //	return;
    //}

    //mReadSock =  pfd[0];  
	//mWriteSock = pfd[1];

	//#elif FTT_MSNT_PLATFORM

	//
	// There are two threads for this class, one opens a server TCP socket
	// and accepts connection. The other opens client TCP socket and 
	// connects to the server socket (so the second one has to be 
	// later than the first one). The client thread function checks the 
	// nearest timer, and then goes to select. If newer timer comes in, 
	// we need to send something onto the client socket to wake it up. 
	// Otherwise, it will wake up itself when its timer expires. 
	//

	//OmnRslt rslt = config(def);
	//if (rslt.failed())
	//{
	//	OmnExcept e(OmnFileLine, "Failed to create Timer");
	//	throw e;
	//}

    FD_ZERO(&mReadfds);
    mTimeval.tv_sec = 0; 
    mTimeval.tv_usec = 0; 
	OmnTimer::TimerData sgNullTimerData;
	mTimerList.setNullValue(sgNullTimerData);
	config(NULL);
	start();
}


OmnTimer::~OmnTimer()
{
}


bool
OmnTimer::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool
OmnTimer::start()
{
	createTimerSock();
	mReadSock = mReader->getSock();

    FD_ZERO(&mReadfds);
    FD_SET(mReadSock, &mReadfds);
    mTimeval.tv_sec = 0; 
    mTimeval.tv_usec = 0; 
    mNfds = mReadSock + 1;
	
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "TimerThread", eClientThread, true, true, __FILE__, __LINE__);

	//
	// It starts the server thread first
	//
	mThread->start();
	return true;
}


bool
OmnTimer::stop()
{
	mThread->stop();
	return true;
}


void
OmnTimer::resetNtimeout()
{
	//
    // This function checks all OmnTimers in the list. If any OmnTimer
    // expires, its function is called, and it is removed from
    // the list. We assume that the mutex is unlocked when this
    // function is called.
	//
    TimerData timerData;
    timeval crtTime;
    OmnGetTimeOfDay(&crtTime);
    timeval timeLeft;
    timeval nextTimer;
    nextTimer.tv_sec = 180;
    nextTimer.tv_usec = 0;
    OmnVList<TimerData> expiredTimerList;
 
    mTimeval.tv_sec = 180;
    mTimeval.tv_usec = 0;

    //
    // Collect all expired OmnTimers and put them in "expiredOmnTimerList"
    //
    mLock->lock();
	// OmnTraceTimer << "Check timer: " << mTimerList.entries() << endl;
	mTimerList.reset();
	while (mTimerList.hasMore())
    {
        timerData = mTimerList.crtValue();
		// OmnTraceTimer << "Check timer: " << timerData.timerName
		// 	<< ":" << timerData.timerId << endl;

        if (crtTime >= timerData.endTime)
        {
            expiredTimerList.append(timerData);
			mTimerList.eraseCrt();
			continue;
        }
        else
        {
            timeLeft = timerData.endTime - crtTime;
            if (nextTimer > timeLeft)
            {
                nextTimer = timeLeft;
            }
        }
		mTimerList.next();
    }
 
    mLock->unlock();

    //
    // Call those expired Timer's callback functions
    //
	expiredTimerList.reset();
	while (expiredTimerList.hasMore())
    {
		// OmnTraceTimer << "Timer: "
		// 	<< timerData.timerName
		// 	<< ":" << timerData.timerId
		// 	<< " expired. " << endl;

        timerData = expiredTimerList.crtValue();
		expiredTimerList.next();

		// OmnTraceTimer << "Before callback" << endl;
        (timerData.caller)->timeout(timerData.timerId, timerData.timerName, 
			timerData.parm);
		// OmnTraceTimer << "After callback" << endl;
    }
 
    //
    // Update the next OmnTimer time
    //
    if (mTimeval > nextTimer)
    {
        mTimeval = nextTimer;
    }
 
}


int
OmnTimer::startTimer(const OmnString &timerName,
					 int timeoutSec,
  					 int timeoutUsec,
          			 const OmnTimerObjPtr &caller,
					 void *parm) 
{
}


bool
OmnTimer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTrace << "Enter OmnTimer client thread." << endl;

	//
	// This is a thread function. It does not return until the state
	// becomes not ACTIVE
	//
    timeval *timeout = 0; 
    fd_set readfds;

	while (state == OmnThrdStatus::eActive)
	{
    	readfds = mReadfds;

   		if (mTimeval.tv_sec > 0 || mTimeval.tv_usec > 0)
   		{
     	    timeout = &mTimeval;
   		}
   		else
		{
     	    timeout = 0;
		}

		mThreadStatus = true;
		int rslt = OmnSocketSelect(mNfds, &readfds, 0, 0, timeout);
		mThreadStatus = true;
		if (rslt < 0)
		{
			// 
			// Select failed. 
			//
			OmnAlarm << "Timer select failed: " << OmnGetStrError(OmnErrType::eSelect)
				<< enderr;
		}
		if (rslt == 0)
		{
			// 
			// Timeout
			//
		}
		else
		{
    		if (FD_ISSET(mReadSock, &readfds))
    		{
      			char  dat[70];
				int length = 70;
      			if (OmnReadSock(mReadSock, dat, length) == -1)
      			{
					OmnAlarm << "Failed to read timer sock: " 
						<< mReadSock << enderr;
      			}
    		}
		}
		
    	resetNtimeout();
    }

	OmnTrace << "Leave OmnTimer thread function" << endl;
	return true;
}


bool
OmnTimer::cancelTimer(const int timerId)
{
	OmnTraceTimer << "Cancel Timer: " << timerId << endl;

    mLock->lock();
    TimerData timerData;
 
	mTimerList.reset();
	bool found = false;
	while (mTimerList.hasMore())
    {
        timerData = mTimerList.crtValue();
        if (timerId == timerData.timerId)
        {
            mTimerList.eraseCrt();
			OmnTraceTimer << "Timer: " << timerId << " cancelled" << endl;
            found = true;
			break;
        }

		mTimerList.next();
    }

    mLock->unlock();
    return found;
}


bool
OmnTimer::cancelTimersForObj(const void *obj)
{
    mLock->lock();
 
    if (!obj)
    {
        mLock->unlock();
        return false;
    }
 
    TimerData theData;
 
	bool found = false;
	mTimerList.reset();
	while (mTimerList.hasMore())
    {
        theData = mTimerList.crtValue();
        if (obj == (theData.caller).getPtr())
        {
            mTimerList.eraseCrt();
            found = true;
        }
		else
		{
			mTimerList.next();
		}
    }
    mLock->unlock();
    return found;
}


bool
OmnTimer::createTimerSock()
{
	//
	// Create the Read Sock
	//

	//char name[100];
	//int rslt = OmnGetHostName(name, 100);
	//if ( rslt == OmnFailedToGetHostName )
	//{
	//	OmnError::log(OmnFileLine, OmnErrId::eFailedToRetrieveHostName);
	//	return false;
	//}
	//mIpAddr = OmnGetIPAddrByName(name); 

	mPort = 0;
	//mReader = OmnNew OmnUdp("TimerReaderUdp", mIpAddr, mPort);
	mPort = mReader->getLocalPort();
	//mWriter = OmnNew OmnUdp("TimerWriterUdp", mIpAddr, 0);
	mWriter->setDebug(false);

	OmnString err;
	mReader->connect(err);

	mWriter->connect(err);

	mPort = mReader->getLocalPort();

	return true;
}


bool 
OmnTimer::signal(const int threadLogicId)
{
	//
	// Need to send a char to wake the thread up
	//
	return true;
}


bool
OmnTimer::checkThread(OmnString &, const int tid) const
{
	//
	// Not implemented yet
	//
    return true;
}

