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
// 12/09/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Event/EventRegistrant.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Semantics/SemanticsRuntime.h"


bool	
AosEventRegistrant::registerEvents()
{
	AosEventListenerPtr thisPtr(this, false);
	std::list<AosEventEntry>::iterator itr;
	for (itr = mEvents.begin(); itr != mEvents.end(); itr++)
	{
		AosSemanticsRuntimeSelf->registerEvent(
				thisPtr, (*itr).mEventId, (*itr).mData, (*itr).mLength);
	}

	return true;
}


bool	
AosEventRegistrant::unregisterEvents()
{
	AosEventListenerPtr thisPtr(this, false);
	std::list<AosEventEntry>::iterator itr;
	for (itr = mEvents.begin(); itr != mEvents.end(); itr++)
	{
		AosSemanticsRuntimeSelf->unregisterEvent(
				thisPtr, (*itr).mEventId, (*itr).mData, (*itr).mLength);
	}

	return true;
}


void
AosEventRegistrant::addEntry(const AosEventId::E eventId, 
							 void *eventData, 
							 const u32 length)
{
	std::list<AosEventEntry>::iterator itr;
	for (itr = mEvents.begin(); itr != mEvents.end(); itr++)
	{
		aos_assert((*itr).mEventId != eventId || 
				   (*itr).mData != eventData ||
				   (*itr).mLength != length);
	}

	mEvents.push_back(AosEventEntry(0, eventId, eventData, length));
}


void
AosEventRegistrant::removeEntry(const AosEventId::E eventId, 
							void *eventData, 
							const u32 length)
{
	std::list<AosEventEntry>::iterator itr;
	for (itr = mEvents.begin(); itr != mEvents.end(); itr++)
	{
		if ((*itr).mEventId == eventId && 
			(*itr).mData == eventData &&
			(*itr).mLength == length)
		{
			mEvents.erase(itr);
			return;
		}
	}

	OmnAlarm << "To remove an entry but not found: " 
		<< (u32)eventId << ":" << eventData 
		<< ":" << length << enderr;
	return;
}


