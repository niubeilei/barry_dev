////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TimeDriver.cpp
// Description:
//   This class provides a system time. It keeps a tick. Approximately
//  every second, it updates its tick. 
//
//  Singleton Dependency:
//  	OmnAlarmMgr   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util1/TimeDriver.h"

#include "Alarm/Alarm.h"
#include "Porting/TimeOfDay.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Tracer/TraceEntry.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Thread/ThrdStatus.h"
#include "Util/OmnNew.h"
#include "Util1/Wait.h"
#include "Util1/Ptrs.h"
#include "Util1/Time.h"
#include "XmlUtil/XmlTag.h"



OmnSingletonImpl(OmnTimeDriverSingleton,
                 OmnTimeDriver,
                 OmnTimeDriverSelf,
                "OmnTimeDriver");

OmnTimeDriver::OmnTimeDriver()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


OmnTimeDriver::~OmnTimeDriver()
{
}


/*
bool
OmnTimeDriver::getRealtime(uint &sec, uint &usec)
{
	timeval time;
	OmnGetTimeOfDay(&time);
	sec = time.tv_sec;
	usec = time.tv_usec;
	return true;
}
*/


bool
OmnTimeDriver::start()
{
	i64 sec = OmnSystemRelativeSec1();
	OmnTime::setStartSec(sec);

	OmnThreadedObjPtr thisPtr(this, false);
	mSecTickThread = OmnNew OmnThread(thisPtr, "TimeThread", 0, true, true, __FILE__, __LINE__);
	mSecTickThread->start();

	return true;
}	


bool
OmnTimeDriver::stop()
{
	mSecTickThread->stop();
	return true;
}


bool
OmnTimeDriver::config(const AosXmlTagPtr &def)
{
	return true;
}


/*
OmnString
OmnTimeDriver::crttime()
{
	timeval time;
	OmnGetTimeOfDay(&time);
	OmnString str;
	str << "TimeDriver: " << OmnStrUtil::itoa(time.tv_sec)
		<< ":" << OmnStrUtil::itoa(time.tv_usec);
	return str;
}
*/


bool 
OmnTimeDriver::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTrace << "Enter OmnTimeDriver::secTickThreadFunction." << endl;

	while (state == OmnThrdStatus::eActive)
	{
		// Chen Ding, 2014/12/13
		// OmnWaitSelf->wait(0, 50000);	// wait 50 ms
		// jiffies += 50;
		OmnWaitSelf->wait(0, eTimeFreq);
		jiffies += eTimeFreq;
		mThreadStatus = true;

		OmnTime::updateTime();
		// Chen Ding, 2013/01/27
		// OmnTraceEntry::setTick(sec);
	}

	OmnTrace << "Leave OmnTimeDriver::secTickThreadFunc" << endl;
	return true;
}


bool 
OmnTimeDriver::signal(const int threadLogicId)
{
	return true;
}


bool
OmnTimeDriver::checkThread(OmnString &errmsg, const int tid) const 
{
	if (mThreadStatus)
	{
		return mThreadStatus;
	}

	errmsg = "TimeDriver thread failed. ";
	OmnAlarm << errmsg << enderr;
	return false;
}


