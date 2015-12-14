////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IdMgr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/IdMgr.h"

#include "Thread/Mutex.h"


static OmnMutex	sgLock;
static uint		sgUintId = 1000;
static int64_t	sgLLId = 1000;


uint 
OmnIdMgr::getUintId()
{
	sgLock.lock();
	uint id = sgUintId++;
	sgLock.unlock();
	return id;
}


int64_t
OmnIdMgr::getLLId()
{
	sgLock.lock();
	int64_t id = sgLLId++;
	sgLock.unlock();
	return id;
}



