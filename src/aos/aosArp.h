////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosArp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aos_ipv4_arp_h
#define aos_aos_ipv4_arp_h

#include <KernelSimu/types.h>

struct net_device;

extern int arp_set_predefined(int addr_hint, 
				unsigned char * haddr, u32 paddr, struct net_device * dev);

extern int aosGetMacByName(unsigned char *haddr, 
			   unsigned int ipaddr, 
			   const char *devname);
extern int aosGetMac(unsigned char *haddr, 
			   unsigned int ipaddr, 
			   struct net_device *dev);
#endif
