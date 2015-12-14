////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EventListener.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Event_EventListener_h
#define Omn_Event_EventListener_h

#include "Event/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/String.h"

class AosEventListener : virtual public OmnRCObject
{
public:
	virtual bool		procEvent(const AosEventPtr &event, bool &cont) = 0;
	virtual OmnString	getListenerName() const = 0;
};

#endif




