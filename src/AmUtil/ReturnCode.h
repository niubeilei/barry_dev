////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReturnCode.h
// Description:
//   
//
// Modification History:
// 			Created by CHK 2007-04-13
////////////////////////////////////////////////////////////////////////////

#ifndef aos_AmUtil_ReturnCode_h
#define aos_AmUtil_ReturnCode_h

#include "AmUtil/AmRespCode.h"
#include "aos/aosReturnCode.h"
#include <linux/errno.h>

enum
{
	// By CHK 2007-02-28
	// Description: Linux defined error numbers 
	//				Values' definition can be checked by command "man errno"
	// Begin
	eAosRc_Linux_EINVAL = EINVAL, /* 22 */
	eAosRc_Linux_ESRCH  = ESRCH,  /* 3 */
	eAosRc_Linux_EPERM  = EPERM,  /* 1 */
	eAosRc_Linux_EACCES = EACCES, /* 13 */
	// End

	eAosRc_AmUnknownError	= eAosRc_AccessManagerStart | AosAmRespCode::eUnknown,
	
	eAosRc_AmAccessDenied	= eAosRc_AccessManagerStart | AosAmRespCode::eDenied, 			/* . */
	eAosRc_AmInternalError	= eAosRc_AccessManagerStart | AosAmRespCode::eInternalError, 	/* . */
	eAosRc_AmNotStarted 	= eAosRc_AccessManagerStart + 1010, 
	eAosRc_AmInputInvalid, 	
	eAosRc_AmDBStatusStart, 
	eAosRc_AmDBRcdFoundBfInsert= eAosRc_AmDBStatusStart + 297, 
	eAosRc_AmDBRcdNotFound	= eAosRc_AmDBStatusStart + 298, 
	eAosRc_AmDBUnknownErr	= eAosRc_AmDBStatusStart + 299, 
	eAosRc_AmDBStatusEnd	= eAosRc_AmDBStatusStart + 300, 
};

#endif // aos_AmUtil_ReturnCode_h
