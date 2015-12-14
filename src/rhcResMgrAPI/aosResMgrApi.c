////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosResMgrApi.c
// Description:
//   
//
// Modification History:
// 2007-02-26 Created by CHK
////////////////////////////////////////////////////////////////////////////

#include "aosResMgrApi.h"

#include "rhcUtil/aosResMgrToolkit.h"
#include "aos/aosReturnCode.h"

// only for system nice 
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/errno.h>
#include <string.h>

// API:
// same as CLI:
// system nice set <pid> <nice-value>
int ros_nice(int nPid, int nNiceVal)
{
//	const int errlen = 256;
//	char errmsg[256] = "";
//	int nRslt = 0;
//
//	nRslt = setpriority(PRIO_PROCESS, nPid, nNiceVal);
//	if(nRslt < 0)
//	{
//		strcpy(errmsg,strerror(errno));
//		errmsg[errlen-1] = 0;
//#ifdef _DEBUG_RES_MGR_TRACE_ON_
//		if(_DEBUG_RES_MGR_TRACE_ON_)
//		{
//			aos_rhc_mgr_create_log_entry(__FILE__, __LINE__ , errmsg);
//		}
//#endif
//		return -1;
//	}
    return 0;
}

