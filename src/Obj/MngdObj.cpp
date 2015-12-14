////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MngdObj.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/MngdObj.h"

#include "Obj/MngdObjMgr.h"
#include "Thread/Mutex.h"

static int64_t sgObjId = 10;
static OmnMutex sgLock;

OmnMngdObj::OmnMngdObj(const OmnClassId::E classId)
:
OmnObject(classId)
{
	sgLock.lock();
	mObjId = sgObjId++;
	sgLock.unlock();
}


OmnMngdObj::~OmnMngdObj()
{
}


