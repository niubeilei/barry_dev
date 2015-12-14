////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosMemCheck.h
// Description:
//		Add Memory Check Function in this file, the Memory Checking will 
// be run as a thread   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __AOS_MEMORY_CHECK_H__
#define __AOS_MEMORY_CHECK_H__

extern int aosMem_checkStartCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

extern int aosMem_checkTimeoutCli(
    char *data,
    unsigned int *length,
    struct aosKernelApiParms *parms,
    char *errmsg,
    const int errlen);

#endif // __AOS_MEMORY_CHECK_H__
