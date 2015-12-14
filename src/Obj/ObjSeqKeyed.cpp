////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ObjSeqKeyed.cpp
// Description:
//	This is an object that has a randomly generated integer key.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Obj/ObjSeqKeyed.h"

#include "Thread/Mutex.h"

static OmnMutex sgLock;
static int		sgKey = 10;


OmnSeqKeyedObj::OmnSeqKeyedObj(const OmnClassId::E classId)
:
OmnObject(classId)
{
	sgLock.lock();
	mKey = sgKey++;
	sgLock.unlock();
}

