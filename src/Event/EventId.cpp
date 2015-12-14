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
// 12/06/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Event/EventId.h"

#include "alarm_c/alarm.h"


static std::string sgNames[AosEventId::eMaxEventId+1] = 
	{
		"eUnknown",
		"eLocked", 
		"eThreadStarted", 
		"eThreadFinished", 
		"eThreadGotoSleep", 
		"eThreadWakenUp", 
		"eToLock",
		"eToUnlock",
		"eUnlocked",

		"eMaxEventId"
	};


bool	
AosEventId::isCancelingEvent(const AosEventId::E id)
{
	switch (id)
	{
	case eThreadFinished:
	case eThreadWakenUp:
	case eUnlocked:
		 return true;

	default:
		 return false;
	}

	return false;
}


bool 
AosEventId::isPairEvent(const AosEventId::E id1, 
						const AosEventId::E id2, 
						void *inst1, 
						void *inst2)
{
	switch (id1)
	{
	case eThreadFinished:
		 if (id2 == eThreadStarted && inst1 == inst2) return true;
		 return false;

	case eUnlocked:
		 if (id2 == eLocked && inst1 == inst2) return true;
		 return false;

	default:
		 return false;
	}

	return false;
}


std::string		
AosEventId::toStr(const AosEventId::E code)
{
	aos_assert_r(code >= 0 && code <eMaxEventId, "InvalidCode");
	return sgNames[code];
}


AosEventId::E	
AosEventId::toEnum(const std::string &name)
{
	for (int i=0; i<eMaxEventId; i++)
	{
		if (sgNames[i] == name) return (AosEventId::E)i;
	}

	return eUnknown;
}

