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
// 02/20/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "statemachine/types.h"

#include "alarm/Alarm.h"
#include <string.h>


const char *aos_state_2str(aos_state_e type)
{
	switch (type)
	{
	case eAosState_Atomic:
		 return "atomic";

	default:
		 aos_alarm("Unrecognized type: %d", type);
		 return "invalid";
	}

	aos_should_never_come_here;
	return "invalid";
}


aos_state_e aos_state_2enum(const char * const type)
{
	aos_assert_r(type, eAosState_Invalid);

	if (strcmp(type, "atomic") == 0)
	{
		return eAosState_Atomic;
	}

	aos_alarm("Unrecognized type: %s", type);
	return eAosState_Invalid;
}


