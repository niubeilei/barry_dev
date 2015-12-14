////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EventRequester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Event_EventRequester_h
#define Omn_Event_EventRequester_h

#include "Util/String.h"
#include "Util/RCObject.h"


class OmnEventRequester : virtual public OmnRCObject
{
public:
	virtual bool	eventDetected(const OmnEventPtr &event, 
								  void *userData) = 0;
	virtual OmnString	getEventReqName() const = 0;
};

#endif




