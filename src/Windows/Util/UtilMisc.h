////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: UtilMisc.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __UTIL_MISC_H__
#define __UTIL_MISC_H__

#include "AosWinModules.h"


#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

static char *inet_ltoa(unsigned long ip) 
{    
	static int  iBufIndex = 0;
	static char inet_ltoa_buf[16][16];
	unsigned int index = (iBufIndex++) & 0x0f;

	_snprintf( inet_ltoa_buf[index], 16, "%d.%d.%d.%d", NIPQUAD(ip));
	return (inet_ltoa_buf[index]); 
} 

#endif //#ifndef __UTIL_MISC_H__
