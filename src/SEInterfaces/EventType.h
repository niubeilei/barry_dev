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
// 09/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SEInterfaces_EventType_h
#define Aos_SEInterfaces_EventType_h

#include "Util/String.h"

#define AOS_EVENT_INVALID				"invalid"


class AosEventType
{
public:
	enum E
	{
		eInvalid, 

 		eDoc,
 		eTimer,
 		eSystem,
		eOperation,
 
		eMax
	};


public:
	inline static bool isValid(const E code)
	{
		return code > eInvalid && code < eMax;
	}
	static E toEnum(const OmnString &id);
	static OmnString toStr(const E code);
	static bool check();
	static bool addName(const OmnString &name, const E code);
};
#endif

