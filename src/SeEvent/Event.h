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
#ifndef Aos_SeEvent_Event_h
#define Aos_SeEvent_Event_h

#include "Rundata/Ptrs.h"
#include "SeEvent/Ptrs.h"
#include "SeEvent/EventId.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"



class AosEvent : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:

protected:
	OmnString			mEventName;
	AosEventId::E		mEventType;	

public:
	AosEvent(const OmnString &name, const AosEventId::E type, const bool regflag);
	~AosEvent();

	virtual AosEventPtr 	clone() = 0;

	AosEventPtr getEvent(const OmnString &event_id);

private:
	bool 	registerEvent(const AosEventPtr &event);
};
#endif

