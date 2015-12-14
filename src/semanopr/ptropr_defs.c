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
#include "semanopr/ptropr_defs.h"

#include "alarm_c/alarm.h"
#include "util_c/errmgr.h"
#include "aosUtil/ReturnCode.h"
#include "semantics_c/memdict.h"
#include <stdio.h>

int __aos_pointer_decl(const char * const filename, 
					const int lineno, 
					void *pointer, 
					aos_ptrtype_e pointer_type, 
					char * data_type, 
					void *value)
{
	// 
	// Look up the object dictionary to see whether 'pointer' is
	// a valid address. A valid address is one that either points
	// to the beginning address of an object or some valid address
	// inside an object. Note that one should declare memory before
	// using this operator. Otherwise, we won't be able to find the
	// memory.
	//
	char errmsg[100];

	switch (pointer_type)
	{
	case eAosPtrType_Null:
		 if (pointer != 0)
		 {
			 sprintf(errmsg, "ERRNO: Expecting NULL pointer but failed: %x", 
					 (unsigned int)pointer);
			 aos_errmgr_add(errmsg); 
			 return -eAosRc_SemanticOperatorFailed;
		 }
		 return 0;

	case eAosPtrType_Unknown:
		 // 
		 // Currently we do nothing for Unknown pointers.
		 //
		 return 0;

	case eAosPtrType_Valid:
		 if (!aos_memdict_is_valid_addr((u32)pointer))
		 {
			 // 
			 // The pointer is not a valid pointer. 
			 //
			 sprintf(errmsg, "ERRNO_XXXXXX: Pointer not valid: %x", 
					 (unsigned int)pointer);
			 return -eAosRc_SemanticOperatorFailed;
		 }
		 break;

	case eAosPtrType_Invalid:
		 break;

	default:
		 aos_alarm("Unrecognized pointer type: %d", pointer_type);
		 return -eAosRc_SemanticOperatorFailed;
	}

	aos_should_never_come_here;
	return -eAosRc_ProgramError;
}


