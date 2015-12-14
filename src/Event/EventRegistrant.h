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
#ifndef Aos_Event_EventRegistrant_h
#define Aos_Event_EventRegistrant_h

#include "Event/EventId.h"
#include "Event/EventEntry.h"
#include "Event/EventListener.h"
#include "Util/RCObject.h"
#include <list>

class AosEventRegistrant : virtual public AosEventListener
{
protected:
	std::list<AosEventEntry>	mEvents;

public:
	bool	registerEvents();
	bool	unregisterEvents();
	void	addEntry(const AosEventId::E eventId, void *eventData, const u32);
	void	removeEntry(const AosEventId::E eventId, void *eventData,const u32);
};	
#endif

