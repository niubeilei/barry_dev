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
// An event is something happened on an instance. When an event happens,
// an instance of this class is generated and the instance is added
// into AosSeManager. AosSeManager will then determine who registered
// for such an event. If no one is interested in the event, the event
// is ignored. Otherwise, the event is processed by the interested parties. 
//
// Modification History:
// 11/30/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Event/Event.h"

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "Porting/ThreadDef.h"
#include "Event/Ptrs.h"
#include "Event/EventMgr.h"
#include "Event/EventMgr.h"
#include "SemanticRules/SemanticRule.h"
#include "Semantics/SemanticsRuntime.h"
#include "Util/OmnNew.h"
#include "Util/a_string.h"


int aos_raise_event(const AosEventId::E eventId, 
				 const std::string &filename, 
				 const int lineno, 
				 const std::string &actor, 
				 const std::string &classname, 
				 void *eventData, 
				 const u32 dataLen, 
				 const int flag)
{
	AosSemanticsRuntimeSelf->addEvent(eventId, filename, 
			lineno, actor, classname, eventData, dataLen, flag);
	return 0;
}


AosEvent::AosEvent(
			const AosEventId::E eid,
			const std::string &filename, 
			const int lineno,
			const std::string actor,
			const std::string classname, 
			void *eventData, 
			const u32 dataLen)
:
mEventId(eid),
mEventDataLen(dataLen),
mActor(actor),
mClassName(classname),
mFilename(filename),
mLineno(lineno),
mThreadId(OmnGetCurrentThreadId())
{
	// 
	// If eventData length <= 4, store it directly. Otherwise, need to 
	// allocate a piece of memory for it.
	//
	if (dataLen <= 4)
	{
		mEventData = eventData;
	}
	else
	{
		mEventData = aos_malloc(mEventDataLen);
		memcpy(mEventData, eventData, mEventDataLen);
	}	
}


AosEvent::~AosEvent()
{
}


std::string
AosEvent::getEventName() const
{
	return AosEventId::toStr(mEventId);
}


/*
bool
OmnEvent::serializeTo(OmnSerialTo &s) const
{
	try
	{
		s << OmnMsgId::eEvent;
		s << mEventType;
		s << mFile;
		s << mLine;
		s << mInstId;
		s << mTriggeringEntityId;
		s << mTriggeringThreadId;
		s << mTriggeringTime;
		s << mMsg;
	}

	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}

	return true;
}


bool		
OmnEvent::serializeFrom(OmnSerialFrom &s)
{
	try
	{
		s >> mFile;

		int t;
		s >> t;
		if (OmnEventType::isValidEvent(t))
		{
			mEventType = (OmnEventType::E)t;
		}
		else
		{
			OmnAlarm << "Invalid event type: " << t << enderr;
			mEventType = OmnEventType::eInvalid;
		}

		s >> mLine;
		s >> mInstId;
		s >> mTriggeringEntityId;
		s >> mTriggeringThreadId;
		s >> mTriggeringTime;
		s >> mMsg;
	}
	
	catch (const OmnExcept &e)
	{
		OmnAlarm << "Failed to serialize from: " << e.toString() << enderr;
		return e.getRslt();
	}

	return true;
}
*/


std::string
AosEvent::toString() const
{
	std::string str;
	str <<
		"\n\n+++++++++++++++ Event Entry +++++++++++++++"
		"\nEventId:           " << AosEventId::toStr(mEventId) <<
		"\nEventData:         " << mEventData << 
		"\nEventDataLen:      " << mEventDataLen << 
		"\nActor:             " << mActor <<
		"\nClass Name:        " << mClassName << 
		"\nFile:              " << mFilename <<
		"\nLine:              " << mLineno <<
		"\nThreadId:          " << mThreadId <<
		"\n+++++++++++++++ Event Entry +++++++++++++++\n";

	return str;
}	


bool aos_register_event(
				const AosEventListenerPtr &listener,
				const AosEventId::E eventId,
				void *eventData, 
				const u32 dataLen)
{
	return AosSemanticsRuntimeSelf->registerEvent(
				listener, eventId, eventData, dataLen);
}


bool aos_unregister_event(
				const AosEventListenerPtr &listener,
				const AosEventId::E eventId,
				void *eventData, 
				const u32 dataLen)
{
	return AosSemanticsRuntimeSelf->unregisterEvent(
				listener, eventId, eventData, dataLen);
}


