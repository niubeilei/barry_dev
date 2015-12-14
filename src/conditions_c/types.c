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
// 03/06/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "conditions_c/types.h"

#include "alarm_c/alarm.h"
#include <string.h>

const char *aos_cond_type_2str(const aos_cond_type_e type)
{
	switch (type)
	{
	case eAosCondType_Const:
		 return "Const";

	default:
		 break;
	}

	return "invalid";
}


aos_cond_type_e aos_cond_type_2enum(const char * const type)
{
	aos_assert_r(type, eAosCondType_Invalid);

	if (strcmp(type, "Const") == 0) return eAosCondType_Const;

	aos_alarm("Unrecognized type: %s", type);
	return eAosCondType_Invalid;
}


