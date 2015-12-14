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
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semanopr/ptrtype.h"

#include "alarm_c/alarm.h"
#include <string.h>


char * aos_pointer_type_to_str(const aos_ptrtype_e code)
{
	switch (code)
	{
	case eAosPtrType_Null:
		 return "null";

	case eAosPtrType_Unknown: 
		 return "unknown";

	case eAosPtrType_Valid: 
		 return "valid";

	case eAosPtrType_Invalid:
		 return "invalid";

	default:
		 aos_alarm("Unrecognized pointer type: %d", (int)code);
		 return "unrecognized";
	}

	aos_should_never_come_here;
	return "";
}
		 

aos_ptrtype_e aos_pointer_type_to_enum(const char * const name)
{
	if (strcmp(name, "null") == 0) return eAosPtrType_Null;
	if (strcmp(name, "unknown") == 0) return eAosPtrType_Unknown;
	if (strcmp(name, "valid") == 0) return eAosPtrType_Valid;
	if (strcmp(name, "invalid") == 0) return eAosPtrType_Invalid;

	aos_alarm("Unrecognized pointer type: %s", name);
	return eAosPtrType_Unknown;
}

