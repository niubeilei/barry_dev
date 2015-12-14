////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: KernelStr.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelUtil/KernelStr.h"

#include "KernelSimu/kernel.h"
#include <stdio.h>


int aosAsciiToBinary(char *from, char *to, unsigned int len)
{
	unsigned int i = 0;
	int index = 0;

	while (i < len)
	{
		if (from[index] >= '0' && from[index] <= '9')
		{
			to[i] = (from[index] - '0') << 4;
		}
		else if (from[index] >= 'a' && from[index] <= 'f')
		{
			to[i] = (from[index] - 'a') << 4;
		}
		else
		{
			return eAosRc_IncorrectHexAscii;
		}

		index++;
		if (from[index] >= '0' && from[index] <= '9')
		{
			to[i] += (from[index] - '0');
		}
		else if (from[index] >= 'a' && from[index] <= 'f')
		{
			to[i] += (from[index] - 'a');
		}
		else
		{
			return eAosRc_IncorrectHexAscii;
		}

		index++;
		i++;
	}

	return 0;
}


char * aosAddrToStr(u32 addr)
{
	static char lsName[20];
	sprintf(lsName, "%d.%d.%d.%d", 
		(unsigned int)(addr & 0xff),
		(unsigned int)((addr >> 8) & 0xff), 
		(unsigned int)((addr >> 16) & 0xff), 
		(unsigned int)((addr >> 24) & 0xff));
	return lsName;
}




