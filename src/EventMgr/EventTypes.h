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
// 10/25/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef Omn_EventMgr_EventTypes_h
#define Omn_EventMgr_EventTypes_h

#include "Util/String.h"

//#define AOSEVENTTYPE_EVENT			"event"
//#define AOSEVENTTYPE_ACTION			"action"
#define AOSEVENTTYPE_DOC				"doc"
#define AOSEVENTTYPE_TIMER				"timer"
#define AOSEVENTTYPE_SYSTEM				"system"

struct AosEventType
{
	enum E
	{
		eInvalid, 

		//eEvent,
		//eAction,
		eDoc,
		eTimer,
		eSystem,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &id)
	{
		if (id.length() < 3) return eInvalid;
		const char *data = id.data();
		switch (data[0])
		{
		case 'd':
			 if (id == AOSEVENTTYPE_DOC) return eDoc;
			 break;

		case 's':
			 if (id == AOSEVENTTYPE_SYSTEM) return eSystem;
			 break;
		
		case 't':
			 if (id == AOSEVENTTYPE_TIMER) return eTimer;
			 break;

		default:
			 break;
		}
		return eInvalid;
	}
};
#endif
#endif
