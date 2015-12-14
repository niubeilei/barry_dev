////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Sem.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Thread/Sem.h"

#include "Alarm/AlarmMgr.h"
#include "Debug/Debug.h"
#include "Porting/Sem.h"
#include "Porting/GetErrnoStr.h"
#include "Porting/ThreadDef.h"
#include "Event/ThreadEvents.h"
#include "Thread/LockMonitor.h"
#include "Thread/ThreadMgr.h"


/*
OmnSem::OmnSem(int value)
{
	OmnInitSem(mSem);
}


OmnSem::~OmnSem()
{
	OmnDestroySem(mSem);
}
*/
