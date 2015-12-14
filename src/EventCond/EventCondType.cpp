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
// 09/15/2010	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "EventCond/EventCondType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



const OmnString AOSCONDTYPE_EQ	= "eq";

AosEventCondType::E
AosEventCondType::toEnum(const OmnString &name)
{
	aos_assert_r(name.length() > 0, eInvalid);
	const char *data = name.data();

	switch (data[0])
	{
	case 'e':
		 if (name == AOSCONDTYPE_EQ) return eEq;
		 break;

	default:
		 break;
	}

	return eInvalid;
}

