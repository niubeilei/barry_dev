////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RwLock.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Thread/RwLock.h"

#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h"
#include "Porting/RwLock.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/ThreadDef.h"
#include "Event/ThreadEvents.h"
#include "Thread/LockMonitor.h"
#include "Thread/ThreadMgr.h"


OmnRwLock::OmnRwLock()
{
	init();
}


OmnRwLock::~OmnRwLock()
{
	OmnDestroyRwLock(mLock);
}


bool
OmnRwLock::init()
{
	OmnInitRwLock(mLock);
	return true;
}
