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
#include "SeEvent/Event.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Thread/Mutex.h"


static AosEventPtr		sgEvents[AosEventId::eMax];
static OmnMutex			sgLock;

AosEvent::AosEvent(const OmnString &name, const AosEventId::E type, const bool regflag)
:
mEventName(name),
mEventType(type)
{
	if (regflag) 
	{
		AosEventPtr thisptr(this, false);
		if (!registerEvent(thisptr))
		{
			OmnCreateException << "Failed registering event: " << name << ":" << (int)type;
			throw except;
		}
	}
}


AosEvent::~AosEvent()
{
}


bool
AosEvent::registerEvent(const AosEventPtr &event)
{
	sgLock.lock();
	if (!AosEventId::isValid(event->mEventType))
	{
		sgLock.unlock();
		OmnAlarm << "Incorrect event id: " << event->mEventType << enderr;
		return false;
	}

	if (sgEvents[event->mEventType])
	{
		sgLock.unlock();
		OmnAlarm << "Event already registered: " << event->mEventType << enderr;
		return false;
	}

	sgEvents[event->mEventType] = event;
	bool rslt = AosEventId::addName(event->mEventName, event->mEventType);
	sgLock.unlock();
	return rslt;
}



AosEventPtr
AosEvent::getEvent(const OmnString &event_id)
{
	sgLock.lock();
	AosEventId::E id = AosEventId::toEnum(event_id);
	if (!AosEventId::isValid(id))
	{
		sgLock.unlock();
		OmnAlarm << "Unrecognized smart doc id: " << event_id << enderr;
		return 0;
	}
	
	AosEventPtr event = sgEvents[id];
	sgLock.unlock();
	aos_assert_r(event, 0);
	return event;
}

