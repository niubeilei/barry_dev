////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: dev.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_KernelSimu_dev_h
#define aos_KernelSimu_dev_h

#ifndef __KERNEL__

struct net_device;

extern int dev_init();
extern int dev_queue_xmit(struct sk_buff *skb);
extern struct net_device *__dev_get_by_name(const char *name);
extern struct net_device *dev_get_by_name(const char *name);
extern struct net_device *dev_get_by_index(int index);
extern void dev_set_promiscuity(struct net_device *dev, int inc);


#else
#include <linux/skbuff.h>
#endif

#endif

