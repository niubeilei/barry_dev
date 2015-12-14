////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HouseKp.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "NMS/HouseKp.h"

#include "Alarm/Alarm.h"
#include "NMS/Ptrs.h"
#include "NMS/HouseKpObj.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Tracer/Tracer.h"
#include "Util1/Timer.h"
#include "Util1/Time.h"
#include "XmlParser/XmlItem.h"
#include "XmlParser/XmlItemName.h"





OmnHouseKp::OmnHouseKp(const OmnXmlItemPtr &def)
:
mEvent(eNothing),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mFrequency(eDefaultKpFreq),
mIsStarted(false)
{
	OmnRslt rslt = config(def);
    if (!rslt)
    {
        OmnExcept e(OmnFileLine, rslt.getErrId(), "Failed to create HourseKp");
        throw e;
    }
}


bool
OmnHouseKp::start()
{
    // 
    // Listening to register/discovery requests
    //
	mIsStarted = true;
	OmnThreadedObjPtr thisPtr(this, false);
    mThread = OmnNew OmnThread(thisPtr, "HouseKp", 0, true, true, __FILE__, __LINE__);

	// 
	// At the start time, we should not start the house kp too soon.
	//
    // mTimerId = OmnTimerSelf->startTimer("HouseKeeping", mFrequency, 0, thisPtr);
	OmnTimerObjPtr tmPtr(this, false);
  //  mTimerId = OmnTimerSelf->startTimer("HouseKeeping", 10, 0, tmPtr);
	mThread->start();
    mTimerId = OmnTimerSelf->startTimer("HouseKeeping", 0, 10000, tmPtr);

	return true;
}


bool
OmnHouseKp::stop()
{
	mThread->stop();
	return true;
}


OmnHouseKp::~OmnHouseKp()
{
}


OmnRslt
OmnHouseKp::config(const OmnXmlItemPtr &def)
{
    //
    // Configure this class. If fails, use default
    //
    //  <HouseKeeper>
    //      <Frequency>
    //  </HouseKeeper>
    //

	if (def.isNull())
	{
		return true;
	}

    mFrequency = def->getInt(OmnXmlItemName::eFrequency, 10);
    return true;
}

bool 
OmnHouseKp::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    OmnTrace << "Enter OmnHouseKp::threadFunc." << endl;

    while (state == OmnThrdStatus::eActive)
    {

		mLock->lock();
		mThreadStatus = true;
        mCondVar->wait(mLock);
		mThreadStatus = true;
        mLock->unlock();

		procEvent();
    }
        
    OmnTraceThread << "Leaving OmnAlgDiscMgr::threadFunc" << endl;
    return true;
}


bool 
OmnHouseKp::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


void
OmnHouseKp::addObj(OmnHouseKpObj *obj)
{
	mLock->lock();
	mObjects.reset();
    while (mObjects.hasMore())
	{
		if (mObjects.crtValue() == obj)
		{
			// 
			// Already exists. Do nothing.
			//
			mLock->unlock();
			return;
		}

		mObjects.next();
	}

	mObjects.append(obj);
	mLock->unlock();
}


void
OmnHouseKp::procEvent()
{
	switch (mEvent)
	{
	case eNothing:
		 return;

	case eHouseKeeping:
		 procHouseKeeping();
		 return;

	case eExit:
		 return;

	default:
		 OmnWarn << "Unrecognized House Keeping Event: " 
			<< mEvent << enderr;
		 return;
	}
}


void
OmnHouseKp::procHouseKeeping()
{
/*	mObjects.reset();
	OmnLL tick = OmnTime::getSecTick();
	while (mObjects.hasMore())
	{
		mLock->lock();
		OmnHouseKpObj *obj = mObjects.crtValue();
		mObjects.next();
		mLock->unlock();
		if (obj)
		{
			//
			// Chen Ding, 05/18/2003
			//
			OmnTraceHK << "Calling housepeer: " << obj->getName() << endl;
			mThreadStatus = true;
			obj->procHouseKeeping(tick);
			mThreadStatus = true;
			OmnTraceHK << "Finished" << endl;
		}
	}

	// 
	// Need to check the tracer files
	//
	OmnTracer::getSelf()->procHouseKeeping();
*/

	//
	// Need to start the house keeping timer.
	//
	mEvent = eNothing;
	OmnTimerObjPtr thisPtr(this, false);
//    mTimerId = OmnTimerSelf->startTimer("HouseKeeping", mFrequency, 0, thisPtr);
    mTimerId = OmnTimerSelf->startTimer("HouseKeeping", 0, 10000, thisPtr);

}
	

void
OmnHouseKp::timeout(const int timerId, const OmnString &name, void *parm)
{
	mLock->lock();
	mEvent = eHouseKeeping;
	mCondVar->signal();
	mLock->unlock();
}


bool
OmnHouseKp::checkThread(OmnString &errmsg, const int tid) const
{
	if (!mIsStarted)
	{
		//
		// The house keeper has not been started yet
		//
		return true;
	}

    if (!mThreadStatus)
    {
		errmsg = "HouseKp thread failed. ";
        OmnAlarm << errmsg << ". Last heartbeat: " << (int)mHeartbeatStartSec
			<< ". Now: " << (int)OmnTime::getSecTick() << enderr;
        return false;
    }

    return true;
}

