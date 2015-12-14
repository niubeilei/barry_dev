////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelError.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosKernelAlarm.h"

#define aosKernelErrorArraySize 10000

static int sgIndex = 0;
int aosTotalErrors = 0;
int aosNumErrors = 0;

static unsigned short sgFileId[aosKernelErrorArraySize];
static unsigned short sgLine[aosKernelErrorArraySize];
static unsigned short sgErrId[aosKernelErrorArraySize];
 

void aosKernelErrorInit()
{
	int i;
	for (i=0; i<aosKernelErrorArraySize; i++)
	{
		sgFileId[i] = 0;
		sgLine[i] = 0;
		sgErrId[i] = 0;
	}
}


void aosKernelError(unsigned short fileId, 
					unsigned short line, 
					unsigned short errId)
{
	// 
	// AOS keeps an array of errors. 
	// 
	int index = sgIndex++;

	aosTotalErrors++;
	aosNumErrors++;

	if (sgIndex >= aosKernelErrorArraySize)
	{
		sgIndex = 0;
	}

	if (index >= 0 && index < aosKernelErrorArraySize)
	{
		sgFileId[index] = fileId;
		sgLine[index] = line;
		sgErrId[index] = errId;
	}
}

