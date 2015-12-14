////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 10/18/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Thread/LockMonitorMgr.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Thread/Thread.h"
#include "Thread/LockMonitor.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "XmlParser/XmlParser.h"
#include "XmlParser/XmlItem.h"
//#include "XmlParser/XmlItemName.h"
#include "XmlUtil/XmlTag.h"


OmnSingletonImpl(AosLockMonitorMgrSingleton,
                 AosLockMonitorMgr,
                 AosLockMonitorMgrSelf,
                "AosLockMonitorMgr");


AosLockMonitorMgr::AosLockMonitorMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mNumMonitors(0)
{
	//cout << __FILE__ << ":" << __LINE__ << "====================Creating Log Monitor Manager" << ":" << this << endl;
}


AosLockMonitorMgr::~AosLockMonitorMgr()
{
	//cout << __FILE__ << ":" << __LINE__ << "=====================Deleting Log Monitor Manager" << ":" << endl;
}


bool
AosLockMonitorMgr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "LockMonitorMgr", 0, false, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosLockMonitorMgr::stop()
{
	mThread->stop();
	return true;
}


bool
AosLockMonitorMgr::config(const AosXmlTagPtr &conf)
{
	return true;
}


bool
AosLockMonitorMgr::addLock(const AosLockMonitorPtr &monitor)
{
	mLock->lock();
	aos_assert_rl(mNumMonitors < eMaxMonitors, mLock, false);
	mMonitors[mNumMonitors++] = monitor;
	mLock->unlock();
	return true;
}


bool
AosLockMonitorMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnWait::getSelf()->wait(3, 0);
		checkMonitors();
    }
    return true;
}


bool
AosLockMonitorMgr::signal(const int threadLogicid)
{
    return true;
}


void    
AosLockMonitorMgr::heartbeat(const int tid)
{
}


bool
AosLockMonitorMgr::checkMonitors() const
{
	mLock->lock();
	for (int i=0; i<mNumMonitors; i++)
	{
		if (!mMonitors[i]->checkLock())
		{
			OmnAlarm << "Failed the lock check: " << mNumMonitors << enderr;

			for (int k=0; k<mNumMonitors; k++)
			{
				OmnScreen << "Monitor: \n" << mMonitors[k]->toString() << endl;
			}
			OmnScreen << "End of Listing" << endl;
			mLock->unlock();
			return false;
		}
	}
	mLock->unlock();
	return true;
}


bool			
AosLockMonitorMgr::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}


