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
// 03/24/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "domain/types.h"

#include "alarm/Alarm.h"
#include <string.h>


const char * aos_domain_2str(aos_domain_e type)
{
	switch (type)
	{
	case eAosDomain_Int:
		 return "Int";

	default:
		 break;
	}

	aos_alarm("Unrecognized domain type: %d", type);
	return "invalid";
}


aos_domain_e aos_domain_2enum(const char * const type)
{
	aos_assert_r(type, eAosDomain_Invalid);

	if (strcmp(type, "Int") == 0) return eAosDomain_Int;

	return eAosDomain_Invalid;
}

