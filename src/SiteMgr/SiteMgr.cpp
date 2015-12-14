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
// 12/17/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SiteMgr/SiteMgr.h"

#include "alarm_c/alarm.h"
#include "IdGen/IdGen.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Util/OmnNew.h"
#include "Util1/Ptrs.h"
#include "Util1/Timer.h"
#include "Util1/Wait.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "UtilComm/TcpServerGrp.h"

OmnSingletonImpl(AosSiteMgrSingleton,
                 AosSiteMgr,
                 AosSiteMgrSelf,
                "AosSiteMgr");


AosSiteMgr::AosSiteMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
}


AosSiteMgr::~AosSiteMgr()
{
}


bool
AosSiteMgr::start()
{
	return true;
}


bool
AosSiteMgr::stop()
{
	return true;
}


OmnRslt
AosSiteMgr::config(const OmnXmlParserPtr &def)
{
	return true;
}


bool    
AosSiteMgr::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSiteMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSiteMgr::threadFunc(
        OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
	return true;
}

#endif
