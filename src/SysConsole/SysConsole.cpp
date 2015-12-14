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
// 2013/05/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SysConsole/SysConsole.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SysConsole/ConsoleProc.h"


OmnSingletonImpl(AosSysConsoleSingleton,
                 AosSysConsole,
                 AosSysConsoleSelf,
                "AosSysConsole");


AosSysConsole::AosSysConsole()
{
	/*
	AosConsoleProc::init();
	AosSysConsoleRegisterMsgs();

	smPhysicalId = AosGetSelfServerId();
	if (smPhysical < 0 || smPhysicalId >= AosGetNumPhysicals())
	{
		OmnThrowException("Invalid physical id");
		return;
	}
	*/
}


AosSysConsole::~AosSysConsole()
{
}


bool
AosSysConsole::start()
{
	return true;
}


bool
AosSysConsole::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosSysConsole::stop()
{
    return true;
}


bool
AosSysConsole::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
	//	mLock->lock();
		OmnSleep(100);
	//	mLock->unlock();
	}

	OmnScreen << "Leaving SysConsole thread" << endl;
	return true;
}
