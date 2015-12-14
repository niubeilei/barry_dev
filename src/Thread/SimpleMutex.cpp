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
// 03/11/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Thread/SimpleMutex.h"

#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h"
#include "Porting/Mutex.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/ThreadDef.h"


AosSimpleMutex::AosSimpleMutex()
{
	OmnInitMutex(mLock);
}


AosSimpleMutex::~AosSimpleMutex()
{
	OmnDestroyMutex(mLock);
}


bool
AosSimpleMutex::lock()
{
	if (OmnMutexLock(mLock))
	{
		return true;
	}
	return false;
}


bool
AosSimpleMutex::unlock()
{
	if (OmnMutexUnlock(mLock))
	{
		return true;
	}
	return false;
}

