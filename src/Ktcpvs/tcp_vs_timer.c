////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_timer.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/*
 * KTCPVS       An implementation of the TCP Virtual Server daemon inside
 *              kernel for the LINUX operating system. KTCPVS can be used
 *              to build a moderately scalable and highly available server
 *              based on a cluster of servers, with more flexibility.
 *
 * tcp_vs_timer.c: slow timer for collecting stale entries
 *
 * Version:     $Id: tcp_vs_timer.c,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 * Authors:     Wensong Zhang <wensong@linuxvirtualserver.org>
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#include "Ktcpvs/tcp_vs.h"

/*
 * The following block implements slow timers for KTCPVS, most code is stolen
 * from linux/kernel/timer.c.
 */
#define SHIFT_BITS	6
#define TVN_BITS	8
#define TVR_BITS	10
#define TVN_SIZE	(1 << TVN_BITS)
#define TVR_SIZE	(1 << TVR_BITS)
#define TVN_MASK	(TVN_SIZE - 1)
#define TVR_MASK	(TVR_SIZE - 1)

struct slowtimer_vec {
	int index;
	struct list_head vec[TVN_SIZE];
};

struct slowtimer_vec_root {
	int index;
	struct list_head vec[TVR_SIZE];
};

static struct slowtimer_vec sltv3 = { 0 };
static struct slowtimer_vec sltv2 = { 0 };
static struct slowtimer_vec_root sltv1 = { 0 };

static struct slowtimer_vec *const sltvecs[] = {
	(struct slowtimer_vec *) &sltv1, &sltv2, &sltv3
};

#define NOOF_SLTVECS (sizeof(sltvecs) / sizeof(sltvecs[0]))

static inline void
init_slowtimer_vecs(void)
{
	int i;

	for (i = 0; i < TVN_SIZE; i++) {
		INIT_LIST_HEAD((struct list_head *) sltv3.vec + i);
		INIT_LIST_HEAD(sltv2.vec + i);
	}
	for (i = 0; i < TVR_SIZE; i++)
		INIT_LIST_HEAD(sltv1.vec + i);
}

static unsigned long slowtimer_jiffies = 0;

static inline void
internal_add_slowtimer(slowtimer_t * timer)
{
	/*
	 * must hold the slowtimer lock when calling this
	 */
	unsigned long expires = timer->expires;
	unsigned long idx = expires - slowtimer_jiffies;
	struct list_head *vec;

	if (idx < 1 << (SHIFT_BITS + TVR_BITS)) {
		int i = (expires >> SHIFT_BITS) & TVR_MASK;
		vec = sltv1.vec + i;
	} else if (idx < 1 << (SHIFT_BITS + TVR_BITS + TVN_BITS)) {
		int i = (expires >> (SHIFT_BITS + TVR_BITS)) & TVN_MASK;
		vec = sltv2.vec + i;
	} else if ((signed long) idx < 0) {
		/*
		 * can happen if you add a timer with expires == jiffies,
		 * or you set a timer to go off in the past
		 */
		vec = sltv1.vec + sltv1.index;
	} else if (idx <= 0xffffffffUL) {
		int i =
		    (expires >> (SHIFT_BITS + TVR_BITS + TVN_BITS)) &
		    TVN_MASK;
		vec = sltv3.vec + i;
	} else {
		/* Can only get here on architectures with 64-bit jiffies */
		INIT_LIST_HEAD(&timer->list);
	}
	/*
	 * Timers are FIFO!
	 */
	list_add(&timer->list, vec->prev);
}

static spinlock_t tcp_vs_slowtimer_lock = SPIN_LOCK_UNLOCKED;

void
tcp_vs_add_slowtimer(slowtimer_t * timer)
{
	spin_lock(&tcp_vs_slowtimer_lock);
	if (timer->list.next)
		goto bug;
	internal_add_slowtimer(timer);
      out:
	spin_unlock(&tcp_vs_slowtimer_lock);
	return;

      bug:
	printk("bug: kernel slowtimer added twice at %p.\n",
	       __builtin_return_address(0));
	goto out;
}

static inline int
detach_slowtimer(slowtimer_t * timer)
{
	if (!slowtimer_pending(timer))
		return 0;
	list_del(&timer->list);
	return 1;
}

void
tcp_vs_mod_slowtimer(slowtimer_t * timer, unsigned long expires)
{
	int ret;

	spin_lock(&tcp_vs_slowtimer_lock);
	timer->expires = expires;
	ret = detach_slowtimer(timer);
	internal_add_slowtimer(timer);
	spin_unlock(&tcp_vs_slowtimer_lock);
}

int
tcp_vs_del_slowtimer(slowtimer_t * timer)
{
	int ret;

	spin_lock(&tcp_vs_slowtimer_lock);
	ret = detach_slowtimer(timer);
	timer->list.next = timer->list.prev = 0;
	spin_unlock(&tcp_vs_slowtimer_lock);
	return ret;
}


static inline void
cascade_slowtimers(struct slowtimer_vec *tv)
{
	/*
	 * cascade all the timers from tv up one level
	 */
	struct list_head *head, *curr, *next;

	head = tv->vec + tv->index;
	curr = head->next;

	/*
	 * We are removing _all_ timers from the list, so we don't  have to
	 * detach them individually, just clear the list afterwards.
	 */
	while (curr != head) {
		slowtimer_t *tmp;

		tmp = list_entry(curr, slowtimer_t, list);
		next = curr->next;
		list_del(curr);	// not needed
		internal_add_slowtimer(tmp);
		curr = next;
	}
	INIT_LIST_HEAD(head);
	tv->index = (tv->index + 1) & TVN_MASK;
}

static inline void
run_slowtimer_list(void)
{
	spin_lock(&tcp_vs_slowtimer_lock);
	while ((long) (jiffies - slowtimer_jiffies) >= 0) {
		struct list_head *head, *curr;
		if (!sltv1.index) {
			int n = 1;
			do {
				cascade_slowtimers(sltvecs[n]);
			} while (sltvecs[n]->index == 1
				 && ++n < NOOF_SLTVECS);
		}
	      repeat:
		head = sltv1.vec + sltv1.index;
		curr = head->next;
		if (curr != head) {
			slowtimer_t *timer;
			void (*fn) (unsigned long);
			unsigned long data;

			timer = list_entry(curr, slowtimer_t, list);
			fn = timer->function;
			data = timer->data;

			detach_slowtimer(timer);
			timer->list.next = timer->list.prev = NULL;
			spin_unlock(&tcp_vs_slowtimer_lock);
			fn(data);
			spin_lock(&tcp_vs_slowtimer_lock);
			goto repeat;
		}
		slowtimer_jiffies += 1 << SHIFT_BITS;
		sltv1.index = (sltv1.index + 1) & TVR_MASK;
	}
	spin_unlock(&tcp_vs_slowtimer_lock);
}


/*
 *	The function to collect stale timers must be activated from the
 *	outside periodically (such as every second).
 */
void
tcp_vs_slowtimer_collect(void)
{
	run_slowtimer_list();
}


void
tcp_vs_slowtimer_init(void)
{
	/* initialize the slowtimer vectors */
	init_slowtimer_vecs();
}


void
tcp_vs_slowtimer_cleanup(void)
{
	/* nothing to do now */
}
