////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: BandWidthMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "Util1/BandWidthMgr.h"

#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"

AosBandWidthMgr::AosBandWidthMgr()
:
mStarted(false),
mBytesLimit(0),
mCurrentVolume(0),
mIntervalSec(0),
mIntervalUSec(0),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosBandWidthMgr::~AosBandWidthMgr()
{
}

void			
AosBandWidthMgr::SetBandWidth(const int sec,const int usec,const int bytesLimit)
{
	mBytesLimit		= bytesLimit;
	mIntervalSec 	= sec;
	mIntervalUSec	= usec;	
}


bool
AosBandWidthMgr::start()
{
	if(mIntervalSec < 0 || 
	   mIntervalUSec < 0 ||
	   mIntervalSec + mIntervalUSec ==0)
	{
		return false;
	}

	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "attacker", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	
	return true;	
}

bool
AosBandWidthMgr::stop()
{
	if(mThread.isNull())
	{
		return true;
	}
	mThread->stop();
	while(mThread->isStopped())
	{
		OmnWait::getSelf()->wait(0,10000);
	}
	return true;
}

bool	
AosBandWidthMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTimerObjPtr thisPtr(this, false);
    while (state == OmnThrdStatus::eActive)
    {
	    OmnTimerSelf->startTimer("Attacker",mIntervalSec,mIntervalUSec,thisPtr,0);
	    mLock->lock();
	    mCondVar->wait(mLock);
	    mLock->unlock();
    	
	}
	return true;
}

bool	
AosBandWidthMgr::signal(const int threadLogicId)
{
	mLock->lock();
	mCurrentVolume = 0;
	mCondVar->signal();
	mLock->unlock();
	return true;	
}

bool    
AosBandWidthMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


void	
AosBandWidthMgr::timeout(const int timerId, 
					   const OmnString &timerName,
					   void *parm)
{
	signal(0);
}
					   
bool
AosBandWidthMgr::getPermission(const int requestLen,int &lenAllowed)
{
	if(!mStarted)
	{
		lenAllowed = requestLen;
		return true;
	}
	
	while(1)
	{	
		mLock->lock();
		if(	mCurrentVolume < mBytesLimit)
		{
			// can get some volume
			lenAllowed = mBytesLimit - mCurrentVolume;
			if(lenAllowed > requestLen)
			{
				lenAllowed = requestLen;
			}
			mCurrentVolume += lenAllowed;
			mCondVar->signal();
			mLock->unlock();
			return true;
		}
		else
		{
			// can not get volume, wait
		    mCondVar->wait(mLock);
		    mLock->unlock();
		}
	}		
	
	// should not run here
	return false;
}

