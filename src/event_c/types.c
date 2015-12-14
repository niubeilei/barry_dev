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
// 02/21/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "event_c/types.h"

#include "alarm_c/alarm.h"
#include <string.h>


const char *aos_event_2str(const aos_event_e type)
{
	switch (type)
	{
	case eAosEvent_DataRcvd:
		 return "DataRcvd";

	case eAosEvent_MsgRcvd:
		 return "MsgRcvd";

	case eAosEvent_StateFinished:
		 return "StateFinished";

	case eAosEvent_ThrdFinished:
		 return "ThrdFinished";

	default:
		 break;
	}

	aos_alarm("Unrecognized type: %d", type);
	return "invalid";
}


aos_event_e aos_event_2enum(const char * const type)
{
	aos_assert_r(type, eAosEvent_Invalid);

	if (strcmp(type, "DataRcvd") == 0) return eAosEvent_DataRcvd;
	if (strcmp(type, "MsgRcvd") == 0) return eAosEvent_MsgRcvd;
	if (strcmp(type, "StateFinished") == 0) return eAosEvent_StateFinished;
	if (strcmp(type, "ThrdFinished") == 0) return eAosEvent_ThrdFinished;

	return eAosEvent_Invalid;
}



