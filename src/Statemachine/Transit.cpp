////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 07/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Statemachine/Transit.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Thread/Mutex.h"


static AosTransitPtr	sgTransits[AosTransitId::eMax];
static OmnMutex			sgLock;

AosTransit::AosTransit(const OmnString &name, const AosTransitId::E type, const bool regflag)
:
mTransitName(name),
mTransitType(type)
{
	if (regflag) 
	{
		AosTransitPtr thisptr(this, false);
		if (!registerTransit(thisptr))
		{
			OmnString errmsg = "Failed registering transit: ";
			errmsg << name << ":" << (int)type;
			OmnThrowException(errmsg);
		}
	}
}


AosTransit::~AosTransit()
{
}


bool
AosTransit::registerTransit(const AosTransitPtr &transit)
{
	sgLock.lock();
	if (!AosTransitId::isValid(transit->mTransitType))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect transit id: " << transit->mTransitType << enderr;
		return false;
	}

	if (sgTransits[transit->mTransitType])
	{
		sgLock.unlock();
		OmnAlarm << "Transit already registered: " << transit->mTransitType << enderr;
		return false;
	}

	sgTransits[transit->mTransitType] = transit;
	bool rslt = AosTransitId::addName(transit->mTransitName, transit->mTransitType);
	sgLock.unlock();
	return rslt;
}



AosTransitPtr
AosTransit::getTransit(const OmnString &transit_id)
{
	sgLock.lock();
	AosTransitId::E id = AosTransitId::toEnum(transit_id);
	if (!AosTransitId::isValid(id))
	{
		sgLock.unlock();
		OmnAlarm << "Unrecognized smart doc id: " << transit_id << enderr;
		return 0;
	}
	
	AosTransitPtr transit = sgTransits[id];
	sgLock.unlock();
	aos_assert_r(transit, 0);
	return transit;
}

