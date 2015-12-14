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
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Event_EventId_h
#define Aos_Event_EventId_h

#include <string>


class AosEventId
{
public:
	enum E 
	{
		eUnknown,

		eHashTableElemAdded,
		eObjCreated,
		eObjDeleted,
		eObjRecreated,
		eObjRenamed,
		eObjModified,
		eLocked, 
		eThreadStarted, 
		eThreadFinished, 
		eThreadGotoSleep, 
		eThreadWakenUp, 
		eToLock,
		eToUnlock,
		eUnlocked, 

		eMaxEventId
	};

public:
	static bool	isCancelingEvent(const AosEventId::E id);
	static bool isPairEvent(const AosEventId::E id1, 
							const AosEventId::E id2, 
							void *inst1, 
							void *inst2);
	static std::string		toStr(const AosEventId::E code);
	static AosEventId::E	toEnum(const std::string &name);
};
#endif

