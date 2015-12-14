////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: kobject.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_kobject_h
#define Omn_aos_KernelSimu_kobject_h

#ifdef AOS_KERNEL_SIMULATE

#define KOBJ_NAME_LEN	20

#include "KernelSimu/list.h"
#include "KernelSimu/kref.h"

#include "Porting/LongTypes.h"


/* counter to tag the hotplug event, read only except for the kobject core */
extern OmnUint64		hotplug_seqnum;

struct kobject {
	char			  * k_name;
	char				name[KOBJ_NAME_LEN];
	struct kref			kref;
	struct list_head	entry;
	struct kobject	  * parent;
	struct kset		  * kset;
	struct kobj_type  * ktype;
	struct dentry	  * dentry;
};

#else
#include <linux/kobject.h>
#endif

#endif

