////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Util.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Util.h"

#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/Debug.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/string.h"


int AosUtil_showCli(
			char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen)
{
    //
    // ssl client show ciphers 
    //
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
    char local[200];

	aos_enter_func(eFunc_AosSsl_showCiphersCli);

	sprintf(local, "-----------------------------------");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));


	sprintf(local, "\n-----------------------------------");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		
	*length = rsltIndex;
	return 0;
}


int AosUtil_clearConfCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
	// 
	// Reset the ciphers to their default values
	//
	return 0;
}


int AosUtil_saveConfCli(
                char *data,
                unsigned int *length,
                struct aosKernelApiParms *parms,
                char *errmsg,
                const int errlen)
{
    //
    // ssl client save config
    //
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    char *rsltBuff = aosKernelApi_getBuff(data);
 	char local[200];

	if (AosMemory_Status == eAosMemoryStatus_Set)
    {
        sprintf(local, "<Cmd>memory set status off</Cmd>");
    }
    else
    {
        sprintf(local, "<Cmd>memory set status on</Cmd>");
    }
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}

