////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: LogSetLinesKernelApi.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include "KernelAPI/KernelWrapper/LogSetLinesKernelApi.h"
#include "aos/aosKernelApi.h"

int aosSetLogLinesKapi(
            char *data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char *errmsg,
            const int errlen)
{
	int i,j;
	int numberlines = parms->mIntegers[0];
	for (i = 1; i < numberlines; i++)
	{
		//generat 1000Bytes
		for (j = 1; j < 100; j++) 
		{
			printk("..........");
		}
		// 24Bytes
		printk("........................");
		
	}
	*length = 0;
	return 0;
}

