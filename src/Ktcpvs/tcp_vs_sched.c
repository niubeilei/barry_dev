////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_sched.c
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
 * Version:     $Id: tcp_vs_sched.c,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 * Authors:     Wensong Zhang <wensong@linuxvirtualserver.org>
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
// #include <asm/softirq.h>	/* for local_bh_* */
#include <asm/string.h>

#ifdef CONFIG_KMOD
#include <linux/kmod.h>
#endif

#include "Ktcpvs/tcp_vs.h"


/*
 * KTCPVS scheduler list
 */
static LIST_HEAD(tcp_vs_schedulers);

/*
 * FIXME: the scheduler lookup, insert, remove are all called through
 * /proc fs and the /proc fs uses the global lock (lock_kernel), so
 * there is no need to use the __tcp_vs_sched_lock here!!!!!!
 */
static rwlock_t __tcp_vs_sched_lock = RW_LOCK_UNLOCKED;


/*
 *  Register a scheduler in the scheduler list
 */
int
register_tcp_vs_scheduler(struct tcp_vs_scheduler *scheduler)
{
	if (!scheduler) {
		TCP_VS_ERR("register_tcp_vs_scheduler(): NULL arg\n");
		return -EINVAL;
	}

	if (!scheduler->name) {
		TCP_VS_ERR
		    ("register_tcp_vs_scheduler(): NULL scheduler_name\n");
		return -EINVAL;
	}

	if (scheduler->n_list.next != &scheduler->n_list) {
		TCP_VS_ERR
		    ("register_tcp_vs_scheduler(): scheduler already linked\n");
		return -EINVAL;
	}

	//MOD_INC_USE_COUNT;

	write_lock_bh(&__tcp_vs_sched_lock);

	/*
	 *      Add it into the d-linked scheduler list
	 */
	list_add(&scheduler->n_list, &tcp_vs_schedulers);

	write_unlock_bh(&__tcp_vs_sched_lock);

	TCP_VS_INFO("[%s] scheduler registered.\n", scheduler->name);

	return 0;
}


/*
 *  Unregister a scheduler in the scheduler list
 */
int
unregister_tcp_vs_scheduler(struct tcp_vs_scheduler *scheduler)
{
	if (!scheduler) {
		TCP_VS_ERR("unregister_tcp_vs_scheduler(): NULL arg\n");
		return -EINVAL;
	}

	if (list_empty(&scheduler->n_list)) {
		TCP_VS_ERR
		    ("unregister_tcp_vs_scheduler(): scheduler is not in the list. failed\n");
		return -EINVAL;
	}

	write_lock_bh(&__tcp_vs_sched_lock);

	/*
	 *      Removed it from the d-linked scheduler list
	 */
	list_del(&scheduler->n_list);

	write_unlock_bh(&__tcp_vs_sched_lock);

//	MOD_DEC_USE_COUNT;

	TCP_VS_INFO("[%s] scheduler unregistered.\n", scheduler->name);

	return 0;
}


/*
 *  Bind a service with a scheduler
 */
int
tcp_vs_bind_scheduler(struct tcp_vs_service *svc,
		      struct tcp_vs_scheduler *sched)
{
	if (svc == NULL) {
		TCP_VS_ERR("tcp_vs_bind_scheduler(): vs arg NULL\n");
		return -EINVAL;
	}
	if (sched == NULL) {
		TCP_VS_ERR
		    ("tcp_vs_bind_scheduler(): scheduler arg NULL\n");
		return -EINVAL;
	}

	// 
	// Chen Ding, We are not using it as modules. 
	//
	// if (sched->module)
	// 	__MOD_INC_USE_COUNT(sched->module);

	svc->scheduler = sched;

	if (sched->init_service && sched->init_service(svc) != 0) {
		TCP_VS_ERR("tcp_vs_bind_scheduler(): init error\n");

		// 
		// Chen Ding, We are not using it as modules
		//
		// if (sched->module)
		// 	__MOD_DEC_USE_COUNT(sched->module);
		return -EINVAL;
	}

	return 0;
}


/*
 *  Unbind a service with its scheduler
 */
int
tcp_vs_unbind_scheduler(struct tcp_vs_service *svc)
{
	struct tcp_vs_scheduler *sched;

	if (svc == NULL) {
		TCP_VS_ERR("tcp_vs_unbind_scheduler(): svc arg NULL\n");
		return -EINVAL;
	}

	sched = svc->scheduler;
	if (sched == NULL) {
		TCP_VS_ERR("tcp_vs_unbind_scheduler(): svc isn't bound\n");
		return -EINVAL;
	}

	if (sched->done_service && sched->done_service(svc) != 0) {
		TCP_VS_ERR("tcp_vs_unbind_scheduler(): done error\n");
		return -EINVAL;
	}

	svc->scheduler = NULL;

	//
	// Chen Ding, we are not using it as modules
	//
	// if (sched->module)
	// 	__MOD_DEC_USE_COUNT(sched->module);

	return 0;
}


/*
 *    Get scheduler in the scheduler list by name
 */
static struct tcp_vs_scheduler *
tcp_vs_sched_getbyname(const char *sched_name)
{
	struct tcp_vs_scheduler *sched;
	struct list_head *l, *e;

	TCP_VS_DBG(5, "tcp_vs_sched_getbyname(): sched_name \"%s\"\n",
		   sched_name);

	read_lock_bh(&__tcp_vs_sched_lock);

	l = &tcp_vs_schedulers;
	for (e = l->next; e != l; e = e->next) {
		sched = list_entry(e, struct tcp_vs_scheduler, n_list);

		/*  Test and MOD_INC_USE_COUNT atomically */
		// 
		// Chen Ding, we are not using it as modules
		//
		// if (sched->module && !try_inc_mod_count(sched->module)) {
		// 	continue;
		// }

		if (!strcmp(sched_name, sched->name)) {
			/* HIT */
			read_unlock_bh(&__tcp_vs_sched_lock);
			return sched;
		}

		//
		// Chen Ding, we are not using it as modules
		//
		// if (sched->module)
		// 	__MOD_DEC_USE_COUNT(sched->module);
	}

	read_unlock_bh(&__tcp_vs_sched_lock);
	return NULL;
}


/*
 *  Lookup scheduler and try to load it if it doesn't exist
 */
struct tcp_vs_scheduler *
tcp_vs_scheduler_get(const char *sched_name)
{
	struct tcp_vs_scheduler *sched;

	/*
	 *  Search for the scheduler by sched_name
	 */
	sched = tcp_vs_sched_getbyname(sched_name);

#ifdef CONFIG_KMOD
	/*
	 *  If scheduler not found, load the module and search again
	 */
	if (sched == NULL) {
		char module_name[KTCPVS_SCHEDNAME_MAXLEN];
		sprintf(module_name, "ktcpvs_%s", sched_name);
		request_module(module_name);
		sched = tcp_vs_sched_getbyname(sched_name);
	}
#endif				/* CONFIG_KMOD */

	return sched;
}


void
tcp_vs_scheduler_put(struct tcp_vs_scheduler *sched)
{
	// 
	// Chen Ding, we are not using it as modules
	//
	// if (sched->module)
	// 	__MOD_DEC_USE_COUNT(sched->module);
}
EXPORT_SYMBOL( register_tcp_vs_scheduler );
EXPORT_SYMBOL( unregister_tcp_vs_scheduler );
