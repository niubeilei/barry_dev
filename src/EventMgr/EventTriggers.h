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
#ifndef Omn_EventMgr_EventTriggers_h
#define Omn_EventMgr_EventTriggers_h

#include "Util/String.h"

#define AOSEVENTTRIGGER_DOC_CREATED					"create"
#define AOSEVENTTRIGGER_DOC_DELETE					"delete"
#define AOSEVENTTRIGGER_DOC_MODIFY					"modify"
#define AOSEVENTTRIGGER_DOC_READ					"read"
#define AOSEVENTTRIGGER_DOC_PRE_CREATED				"pre_create"
#define AOSEVENTTRIGGER_DOC_PRE_DELETE				"pre_delete"
#define AOSEVENTTRIGGER_DOC_PRE_MODIFY				"pre_modify"
#define AOSEVENTTRIGGER_DOC_PRE_READ				"pre_read"
#define AOSEVENTTRIGGER_SYSTEM						"system"
#define AOSEVENTTRIGGER_TIMER						"timer"
#define AOSEVENTTRIGGER_LOG_CREATED					"log_create"
#define AOSEVENTTRIGGER_JOB_FINISHED				"job_finished"

struct AosEventTrigger
{
	enum E
	{
		eInvalid, 

		eDocCreated,
		eDocDeleted,
		eDocModify,
		eDocRead,
		eDocPreCreated,
		eDocPreDeleted,
		eDocPreModify,
		eDocPreRead,
		eSystem,
		eTimer,

		eMax
	};

	static bool isValid(const E id) {return id > eInvalid && id < eMax;}
	static E toEnum(const OmnString &id)
	{
		if (id.length() < 3) return eInvalid;
		const char *data = id.data();
		switch (data[0])
		{
		case 'c':
			 if (id == AOSEVENTTRIGGER_DOC_CREATED) return eDocCreated;
			 break;

		case 'd':
			 if (id == AOSEVENTTRIGGER_DOC_DELETE) return eDocDeleted;
			 break;

		case 'm':
			 if (id == AOSEVENTTRIGGER_DOC_MODIFY) return eDocModify;
			 break;

		case 'r':
			 if (id == AOSEVENTTRIGGER_DOC_READ) return eDocRead;
			 break;

		case 's':
			 if (id == AOSEVENTTRIGGER_SYSTEM) return eSystem;
			 break;

		case 't':
			 if (id == AOSEVENTTRIGGER_TIMER) return eTimer;
			 break;

		case 'p':
			 if (data[1]=='r' && data[2]=='e' && data[3]=='_')
			 {
				 E e = toEnum(OmnString(&data[4], id.length()-4));
				 if (e > eInvalid && e < eMax)
				 {
					 e = (E)(e + eDocCreated-eDocPreCreated);
				 }
			 }
			 break;

		default:
			 break;
		}
		return eInvalid;
	}

	static bool isPostProcTriggers(const OmnString &trigger)
	{
		// In the current implementations, the following are considered
		// post processing triggers:
		// 		create
		// 		delete
		// 		modify
		// 		read
		aos_assert_r(trigger.length() >= 3, false);
		const char *data = trigger.data();
		switch (data[0])
		{
		case 'c':
			 if (trigger == AOSEVENTTRIGGER_DOC_CREATED) return true;
			 break;

		case 'd':
			 if (trigger == AOSEVENTTRIGGER_DOC_DELETE) return true;
			 break;

		case 'm':
			 if (trigger == AOSEVENTTRIGGER_DOC_MODIFY) return true;
			 break;

		case 'r':
			 if (trigger == AOSEVENTTRIGGER_DOC_READ) return true;
			 break;

		default:
			 break;
		}
		return false;
	}
};
#endif

