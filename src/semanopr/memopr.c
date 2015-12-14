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
#include "semanopr/memopr.h"

#include "alarm_c/alarm.h"
#include "util_c/errmgr.h"
#include "aosUtil/ReturnCode.h"
#include <stdio.h>

// 
// This function assumes the object 'obj' contains a member
// '__semantic_iden' and it should be equal to 'type'. 
// This is normally used to cast an object from void *
// to a specific type, just want to make sure the cast 
// is not too wrong.
//
int __aos_verify_cast(const char * const filename, 
					  const int lineno, 
					  const u32 actual, 
					  const u32 expected)
{
	char errmsg[100];
	if (actual != expected)
	{
		sprintf(errmsg, "Object type mismatch: %u, %u", 
				actual, expected);
		aos_errmgr_add_err(filename, lineno, errmsg);
		return -eAosRc_TypeMismatch;
	}

	return 0;
}

