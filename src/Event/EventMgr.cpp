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
// 11/30/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#include "Event/EventMgr.h"

#include "Alarm/Alarm.h"
#include "alarm_c/alarm.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"
#include "Thread/CondVar.h"
#include "Event/Event.h"
#include "SemanticRules/SemanticRule.h"
#include "Util/OmnNew.h"


AosEventMgr::AosEventMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar()),
mRegLock(OmnNew OmnMutex())
{
}


AosEventMgr::~AosEventMgr()
{
}


bool
AosEventMgr::start()
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "EventMgr", 0, true, true, __FILE__, __LINE__);
	mThread->start();
	return true;
}


bool
AosEventMgr::stop()
{
	OmnTrace << "Singleton class AosEventMgr stopped!" << endl;
	if (mThread)
	{
		mThread->stop();
		return true;
	}
	 
	OmnAlarm << "Thread not started!" << enderr;
	return false;
}
 

bool
AosEventMgr::config(const OmnXmlParserPtr &def)
{
	return true;
}
 
 
bool
AosEventMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	OmnTrace << "Enter AosEventMgr::threadFunc." << endl;
	
	AosEventPtr event;
	while (state == OmnThrdStatus::eActive)
	{
		mThreadStatus = true;
		mLock->lock();
		if (mEvents.empty())
		{
			//
			// There is no more messages.
			//
			mCondVar->wait(mLock);
			mLock->unlock();
			continue;
		}
		
		event = mEvents.front();
		mEvents.pop();
		mLock->unlock();
		
		procEvent(event);
	}
	
	OmnTraceThread << "Leaving AosEventMgr::threadFunc" << endl;
	return true;
}


bool
AosEventMgr::addEvent(const AosEventPtr &event)
{
	mLock->lock();
	mEvents.push(event);
	mCondVar->signal();
	mLock->unlock();
	return true;
}


// 
// The function retrieves all entries that are interested in 
// the event and on the instance 'event->getInst()'. 
// For each matched rule, it calls its member function 
// 'procEvent(...)'. If the function indicates do not continue
// to process the event, the event process is finished. Otherwise,
// the next rule is called to process the event until all
// entries are processed. 
//
void
AosEventMgr::procEvent(const AosEventPtr &event)
{
	AosEventId::E eventId = event->getEventId();
	aos_assert(eventId >= 0 && eventId < AosEventId::eMaxEventId);

	AosEventEntry entry(0, event->getEventId(), event->getEventData(), 
			event->getEventDataLen());
	AosEventEntryPtr tmp(&entry, false);
	mRegLock->lock();
	std::list<AosEventEntryPtr> entries;
	mEventRegisters.get(tmp, entries);
	mRegLock->unlock();

	std::list<AosEventEntryPtr>::iterator itr;
	bool cont;	
	for (itr = entries.begin(); itr != entries.end(); itr++)
	{
		((*itr)->mListener)->procEvent(event, cont);

		// 
		// If 'cont' is false, it means no need to call the 
		// next rule to process it. The event is consumed
		// by the current rule.
		//
		if (!cont) break;
	}
}


bool
AosEventMgr::registerEvent(const AosEventListenerPtr &listener,
						 	const AosEventId::E eventId,
							void *eventData, 
							const u32 dataLen)
{
	aos_assert_r(eventId >= 0 && eventId < AosEventId::eMaxEventId, false);

	mRegLock->lock();
	AosEventEntry entry(listener, eventId, eventData, dataLen);
	AosEventEntryPtr tmp(&entry, false);
	AosEventEntryPtr ee = mEventRegisters.get(tmp);
	if (ee)
	{
		OmnAlarm << "Listener: "
		    << listener->getListenerName()
	   		<< " has already registered for the event: "
			<< eventId << enderr;
		mRegLock->unlock();
		return false;
	}
	mEventRegisters.add(OmnNew AosEventEntry(listener, eventId, 
				eventData, dataLen));
	mRegLock->unlock();

	return true;
}


bool
AosEventMgr::unregisterEvent(const AosEventListenerPtr &listener,
						 	  const AosEventId::E eventId, 
							  void *eventData, 
							  const u32 dataLen)
{
	aos_assert_r(eventId >= 0 && eventId < AosEventId::eMaxEventId, false);

	mRegLock->lock();
	AosEventEntry entry(listener, eventId, eventData, dataLen);
	AosEventEntryPtr tmp(&entry, false);
	AosEventEntryPtr ee = mEventRegisters.get(tmp, true);
	if (!ee)
	{
		OmnAlarm << "To unregister listener: " 
			<< listener->getListenerName()
			<< " on Event ID: " << eventId
			<< " but not found!" << enderr;
	}
	mRegLock->unlock();

	return true;
}


bool 
AosEventMgr::signal(const int threadLogicId)
{
	mLock->lock();
	mCondVar->signal();
	mLock->unlock();
	return true;
}


void 
AosEventMgr::heartbeat(const int tid)
{
	mThreadStatus = false;
	signal(tid);
}


bool 
AosEventMgr::checkThread(OmnString &errmsg, const int tid) const
{
	return mThreadStatus;
}

