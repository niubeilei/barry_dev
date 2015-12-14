////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosArpProxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_AosUtil_aosArpProxy_h
#define Aos_AosUtil_aosArpProxy_h

#include <linux/types.h>

/* ARP Flag values. */
#define ATF_COM				0x02		/* completed entry (ha valid)	*/
#define ATF_PERM			0x04		/* permanent entry		*/
#define ATF_PUBL			0x08		/* publish entry		*/
#define ATF_USETRAILERS 	0x10		/* has requested trailers	*/
#define ATF_NETMASK     		0x20            /* want to use a netmask (only for proxy entries) */
#define ATF_DONTPUB			0x40		/* don't answer this addresses	*/

#define bzero(d, n)         memset((d), 0, (n))

extern int AosArp_add(u32 ip, const char *dev_name, int flag, u32 netmask);
extern int AosArp_del(u32 ip, const char *dev_name, int flag, u32 netmask);

extern int AosSystemArp_init(void);

#endif
