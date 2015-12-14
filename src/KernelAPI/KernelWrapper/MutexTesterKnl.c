////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MutexTesterKnl.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelAPI/KernelWrapper/MutexTesterKnl.h"

#include "aosUtil/Mutex.h"
#include <asm/atomic.h>


int aos_mutex_tester_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    AOS_DECLARE_MUTEX(mylock);
    AOS_INIT_MUTEX(mylock);

    aos_lock(mylock);
    aos_unlock(mylock);

	*length = 0;
	return 0;
}

