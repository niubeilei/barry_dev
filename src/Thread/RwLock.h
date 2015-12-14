////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RwLock.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_RwLock_h 
#define Omn_Thread_RwLock_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Semantics/Semantics.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Porting/RwLock.h"
#include <string>

class OmnRwLock : public OmnRCObject
{
	OmnDefineRCObject;

private:
    OmnRwLockType	mLock;

public:
	OmnRwLock();
    ~OmnRwLock();

    bool readlock()
	{
		return OmnRwLockReadLock(mLock);
	}
    bool writelock()
	{
		return OmnRwLockWriteLock(mLock);
	}
    bool unlock()
	{
		return OmnRwLockUnLock(mLock);
	}
	bool tryReadlock()
	{
		return OmnRwLockTryReadLock(mLock);
	}
	bool tryWritelock()
	{
		return OmnRwLockTryWriteLock(mLock);
	}

private:
	bool				init();
};

#endif

