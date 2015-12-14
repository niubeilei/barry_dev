////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: timer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_timer_h
#define Omn_aos_KernelSimu_timer_h

#ifdef AOS_KERNEL_SIMULATE

#include "KernelSimu/aosKernelMutex.h"
//#include "KernelSimu/list.h"
#include "aosUtil/List.h"
#include "aosUtil/Types.h"

struct aos_list_head;
#define TVN_BITS 6
#define TVR_BITS 8
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

typedef struct tvec_s {
		struct aos_list_head vec[TVN_SIZE];
} tvec_t;

typedef struct tvec_root_s {
		struct aos_list_head vec[TVR_SIZE];
} tvec_root_t;

struct tvec_t_base_s {
	spinlock_t lock;
	unsigned long timer_jiffies;
	struct timer_list *running_timer;
	tvec_root_t tv1;
	tvec_t tv2;
	tvec_t tv3;
	tvec_t tv4;
	tvec_t tv5;
};

typedef struct tvec_t_base_s tvec_base_t;

struct timer_list {
	struct aos_list_head 	entry;
	unsigned long 		expires;
	OmnMutexType		lock;
	unsigned long 		magic;
	void (*function)(unsigned long);
	unsigned long 		data;

	struct tvec_t_base_s *base;
};

//typedef struct timer_list AosTimer_t;

extern struct aos_list_head aos_timer_list_head; // this is system timer list;

#ifndef HZ
#define HZ 1000
#endif

#define TIMER_MAGIC	0x4b87ad6e

#ifndef AOS_GET_JIFFIES
#define AOS_GET_JIFFIES() aos_get_jiffies()
#include <sys/time.h>
#include <time.h>
static inline u32 aos_get_jiffies()
{
	struct timeval tv;
	gettimeofday(&tv, 0);
	return ((u32)tv.tv_sec)*1000+tv.tv_usec/1000;
}
#endif // #ifndef AOS_GET_JIFFIES


static inline void init_timer(struct timer_list * timer)
{
	timer->base = NULL;
	timer->magic = TIMER_MAGIC;
	//spin_lock_init(&timer->lock);
}

int inline mod_timer(struct timer_list *timer,unsigned long expires)
{
	timer->expires = AOS_GET_JIFFIES() + expires;
	return 0;
}

int inline add_timer(struct timer_list *timer)
{
	struct aos_list_head *cur, *last;
	struct timer_list *tmp;
	//mod_timer(timer, timer->expires);
	
	// sort as small to big
	last = &aos_timer_list_head;
	aos_list_for_each(cur, &aos_timer_list_head)
	{
		tmp = aos_list_entry(cur, struct timer_list, entry);
		if (tmp->expires > timer->expires) break;
		last = cur;
	}
	aos_list_add_rcu(&timer->entry, last);
	return 0;
}


/*
int inline mod_timer(struct timer_list *timer, unsigned long expires)
{
	BUG_ON(!timer->function);

	check_timer(timer);

	if (timer->expires == expires && timer_pending(timer))
			return 1;

	return __mod_timer(timer, expires);
}
*/

extern void aos_timer_check_and_run(void);
extern int del_timer(struct timer_list * timer);
#else
// This is the kernel portion

#include <linux/jiffies.h>
#include <linux/timer.h>

//typedef struct timer_list AosTimer_t;

/*
 * per-CPU timer vector definitions:
 */

#define TVN_BITS 6
#define TVR_BITS 8
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

typedef struct tvec_s {
	struct list_head vec[TVN_SIZE];
} tvec_t;

typedef struct tvec_root_s {
	struct list_head vec[TVR_SIZE];
} tvec_root_t;

struct tvec_t_base_s {
	spinlock_t lock;
	unsigned long timer_jiffies;
	struct timer_list *running_timer;
	tvec_root_t tv1;
	tvec_t tv2;
	tvec_t tv3;
	tvec_t tv4;
	tvec_t tv5;
};

typedef struct tvec_t_base_s tvec_base_t;


#endif

#endif

