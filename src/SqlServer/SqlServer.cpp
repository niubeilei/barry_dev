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
// 02/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SqlServer/SqlServer.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SingletonClass/SingletonImpl.cpp"



OmnSingletonImpl(AosSqlServerSingleton,
                 AosSqlServer,
                 AosSqlServerSelf,
                "AosSqlServer");



AosSqlServer::AosSqlServer()
mNumThreads(eDftNumThreads)
{
}


AosSqlServer::~AosSqlServer()
{
}


bool
AosSqlServer::start()
{
	return true;
}


bool
AosSqlServer::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSqlServer::stop()
{
    return true;
}


bool    
AosSqlServer::signal(const int threadLogicId)
{
	return true;
}


bool    
AosSqlServer::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosSqlServer::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		OmnSleepSec(100);
	}

	return true;
}

