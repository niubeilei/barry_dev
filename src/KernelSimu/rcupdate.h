////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rcupdate.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_rcupdate_h
#define Omn_aos_KernelSimu_rcupdate_h

#ifdef AOS_KERNEL_SIMULATE

struct rcu_head {
	struct rcu_head *next;
	void (*func)(struct rcu_head *head);
};

#define RCU_HEAD_INIT(head) { .next = NULL, .func = NULL }
#define RCU_HEAD(head) struct rcu_head head = RCU_HEAD_INIT(head)
#define INIT_RCU_HEAD(ptr) do { \
       (ptr)->next = NULL; (ptr)->func = NULL; \
} while (0)


#else
#include <linux/???.h>
#endif

#endif

