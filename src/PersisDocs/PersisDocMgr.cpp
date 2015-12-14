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
// Modification History:
// 01/20/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "PersisDocs/PersisDocMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "XmlUtil/XmlTag.h"



OmnSingletonImpl(AosPersisDocMgrSingleton,
                 AosPersisDocMgr,
                 AosPersisDocMgrSelf,
                "AosPersisDocMgr");

AosPersisDocMgr::AosPersisDocMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosPersisDocMgr::~AosPersisDocMgr()
{
}


bool
AosPersisDocMgr::start()
{
	return true;
}


bool
AosPersisDocMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosPersisDocMgr::stop()
{
	OmnScreen << "To stop PersisDocMgr!" << endl;
    return true;
}


bool    
AosPersisDocMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosPersisDocMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosPersisDocMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleep(1000);
	}

	return true;
}


