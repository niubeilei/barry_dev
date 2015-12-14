////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Netmask.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_Netmask_h
#define Omn_Util_Netmask_h

#define eAosMaxMaskLen 50

static inline int aosIsValidArrayMask(unsigned char *mask, const int len)
{
	int i;

	if (len <= 0 || len >= eAosMaxMaskLen)
	{
		return 0;
	}

	for (i=0; i<len; i++)
	{
		if (!(mask[i] == 0xff ||
			  mask[i] == 0xfe ||
			  mask[i] == 0xfc ||
			  mask[i] == 0xf8 ||
			  mask[i] == 0xf0 ||
			  mask[i] == 0xe0 ||
			  mask[i] == 0xc0 ||
			  mask[i] == 0x80 ||
			  mask[i] == 0x00))
		{
			return 0;
		}
	}

	return 1;
}




#endif

