////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Ptrs.h
// Description:
//   
//
// Modification History:
// 11/30/2007: Created by Chen Ding
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_EventUtil_h
#define Aos_Event_EventUtil_h

#include "Event/Ptrs.h"
#include "Event/EventId.h"
#include <list>

typedef std::list<AosEventPtr>	AosEventList;

class AosEventRc
{
public:
	enum E
	{
		eInvalid,
		eContinue,	// Event can be consumed by others
		eStop		// Event shall not be consumed anymore
	};
};

#endif

