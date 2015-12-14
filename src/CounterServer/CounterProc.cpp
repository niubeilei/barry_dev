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
// 05/26/2011	Created by Lynch Yang
////////////////////////////////////////////////////////////////////////////
#if 0
#include "CounterServer/CounterProc.h"

#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


static AosCounterProcPtr 	sgCounterProcs[AosCounterOperation::eMax];
static OmnMutex				sgLock;

AosCounterProc::AosCounterProc(const AosCounterOperation::E id, const bool regflag)
:
mId(id)
{
	if (regflag)
	{
		registerProc();
	}
}


bool
AosCounterProc::registerProc()
{
	sgLock.lock();
	if (!AosCounterOperation::isValid(mId))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect counter proc operation: " << mId << enderr;
		return false;
	}

	if (sgCounterProcs[mId])
	{
		OmnAlarm << "Counter Proc already registered: " << mId << enderr;
		sgLock.unlock();
		return false;
	}

	AosCounterProcPtr thisptr(this, false);
	sgCounterProcs[mId] = thisptr;
	sgLock.unlock();
	return true;
}


AosCounterProcPtr
AosCounterProc::getProc(const OmnString &idstr)
{
	AosCounterOperation::E id = AosCounterOperation::toEnum(idstr);
	if (!AosCounterOperation::isValid(id)) return 0;
	return sgCounterProcs[id];
}


AosCounterProcPtr
AosCounterProc::getProc(const AosCounterOperation::E id)
{
	if (!AosCounterOperation::isValid(id)) return 0;
	return sgCounterProcs[id];
}
#endif
