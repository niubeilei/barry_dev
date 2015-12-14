////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Sem.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Thread_Sem_h 
#define Omn_Thread_Sem_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/TimeOfDay.h"
#include "Semantics/Semantics.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Porting/Sem.h"
#include <string>

class OmnSem : public OmnRCObject
{
	OmnDefineRCObject;

private:
    OmnSemType	mSem;

public:
	explicit OmnSem(int value)
	{
		OmnInitSem(mSem, value);
	}
    ~OmnSem()
	{
		OmnDestroySem(mSem);
	}

    bool post()
	{
		return OmnSemPost(mSem);
	}
    bool wait()
	{
		return OmnSemWait(mSem);
	}
    bool timedWait(int ms, bool &timeout)
	{
		return OmnSemTimedWait(mSem, timeout, ms);
	}
	bool tryWait()
	{
		return OmnSemTryWait(mSem);
	}
};

#endif

