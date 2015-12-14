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
// For the definition of Semantics Runtime, please refer to Document:
// "Program Semantics", the sectino "Semantics Runtime". 
//
// Modification History:
// 12/05/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "Semantics/SemanticsRuntime.h"

#include "alarm/Alarm.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Event/Event.h"
#include "Event/EventMgr.h"
#include "SemanticRules/SemanticRule.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Util/OmnNew.h"

OmnSingletonImpl(AosSemanticsRuntimeSingleton,
		 		 AosSemanticsRuntime,
		 		 AosSemanticsRuntimeSelf,
		 		 "AosSemanticsRuntime");


AosSemanticsRuntime::AosSemanticsRuntime()
{
	mEventMgr = OmnNew AosEventMgr();
	aos_assert(mEventMgr);
}


AosSemanticsRuntime::~AosSemanticsRuntime()
{
}


bool
AosSemanticsRuntime::start()
{
	return true;
}


bool
AosSemanticsRuntime::stop()
{
	OmnTrace << "Singleton class AosSemanticsRuntime stopped!" << endl;
	return true;
}
 

OmnRslt
AosSemanticsRuntime::config(const OmnXmlParserPtr &def)
{
	return true;
}
 
 
bool
AosSemanticsRuntime::threadFunc(OmnThrdStatus::E &state, 
								const OmnThreadPtr &thread)
{
	OmnTrace << "Enter AosSemanticsRuntime::threadFunc." << endl;
	
	AosEventPtr event;
	while (state == OmnThrdStatus::eActive)
	{
		break;
	}
	
	OmnTraceThread << "Leaving AosSemanticsRuntime::threadFunc" << endl;
	return true;
}


bool
AosSemanticsRuntime::registerEvent(const AosEventListenerPtr &listener,
						 	const AosEventId::E eventId, 
							void *eventData, 
							const u32 dataLen)
{
	return mEventMgr->registerEvent(listener, eventId, eventData, dataLen);
}

bool 	
AosSemanticsRuntime::unregisterEvent(const AosEventListenerPtr &listener,
						  const AosEventId::E eventId, 
						  void *eventData, 
						  const u32 dataLen)
{
	return mEventMgr->unregisterEvent(listener, eventId, eventData, dataLen);
}


// 
// Description:
// It adds an event.
//
// Parameters:
// 		syncFlag: If it is 0, the event processing should be synchronous. 
// 		Otherwise, an event is created and put into the event queue. 
// 		The processing of the event is achronous.
//
bool	
AosSemanticsRuntime::addEvent(const AosEventId::E eventId,
				 	 const std::string &filename,
				 	 const int lineno,
				 	 const std::string &actor,
				 	 const std::string &classname,
					 void *eventData, 
					 const u32 dataLen, 
					 const int syncFlag)
{
	AosEventPtr event = OmnNew AosEvent(eventId,
			filename, lineno, actor, classname, eventData, dataLen);
	if (!event)
	{
		OmnAlarm << "Run out of memory" << enderr;
		return false;
	}
	
	mEventMgr->addEvent(event);
	return true;
}

bool 
AosSemanticsRuntime::signal(const int threadLogicId)
{
	return true;
}


void 
AosSemanticsRuntime::heartbeat(const int tid)
{
}


bool 
AosSemanticsRuntime::checkThread(OmnString &errmsg, const int tid) const
{
	return true;
}

