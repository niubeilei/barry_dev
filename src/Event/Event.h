////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Event.h
// Description:
//	This is the super class for all event.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Event_Event_h
#define Omn_Event_Event_h

#include "aosUtil/Types.h"
#include "aosUtil/ReturnCode.h"
#include "Event/EventId.h"
#include "Event/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include <string>


extern int aos_raise_event(
				const AosEventId::E eventId, 
				const std::string &filename, 
				const int lineno, 
				const std::string &actor, 
				const std::string &classname, 
				void * inst, 
				const int flag);
extern bool  aos_register_event(
				const AosEventListenerPtr &listner,
				const AosEventId::E eventId,
				void *eventData,
				const u32 dataLen);
extern bool  aos_unregister_event(
				const AosEventListenerPtr &listner,
				const AosEventId::E eventId,
				void *eventData,
				const u32 dataLen);

class AosEvent : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	AosEventId::E		mEventId;
	void *				mEventData;
	u32					mEventDataLen;
	std::string			mActor;
	std::string			mClassName;
	std::string			mFilename;
	int					mLineno;
	u32					mThreadId;

public:
	AosEvent(const AosEventId::E eid,
			const std::string &filename, 
			const int lineno,
			const std::string actor,
			const std::string className, 
			void *eventData, 
			const u32 dataLen);
	~AosEvent();

	AosEventId::E		getEventId() const {return mEventId;}
	std::string			getEventName() const;
	std::string			getClassName() const {return mClassName;}
	void *				getEventData() const {return mEventData;}
	u32					getEventDataLen() const {return mEventDataLen;}
	std::string			toString() const;
	std::string			getFilename() const {return mFilename;}
	int					getLineno() const {return mLineno;}
};

#endif

