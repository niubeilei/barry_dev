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
// This is a utility to select docs.
//
// Modification History:
// 04/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "CounterUtil/CounterOperations.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"

AosCounterOperation::E 
AosCounterOperation::toEnum(const OmnString &name)
{
	const char *data = name.data();
	int len = name.length();
	if (len < 3) return eInvalid;

	switch (data[0])
	{
	case 'a':
		 if (name == AOSCOUNTEROPR_ADDCOUNTER) return eAddCounter;
		 if (name == AOSCOUNTEROPR_ADDCOUNTER2) return eAddCounter2;
		 break;

	case 's':
		 if (name == AOSCOUNTEROPR_SETCOUNTER) return eSetCounter;
		 break;

	default:
		 break;
	}

	OmnAlarm << "Unrecognized Counter Operation: " << name << enderr;
	return eInvalid;
}

