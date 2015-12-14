////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sched_rr.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#define AOS_DEBUG

#include "proxy_scheduler.h"
#include <linux/module.h>
#include <linux/kernel.h>

static int proxy_rr_init_svc(struct ProxyService *svc)
{
	svc->sched_data = 0;
	return 0;
}


static int proxy_rr_done_svc(struct ProxyService *svc)
{
	return 0;
}


static int proxy_rr_update_svc(struct ProxyService *svc)
{
	svc->sched_data = 0;
	return 0;
}

/*
 * Round-Robin Scheduling
 */
static struct proxy_dest *
proxy_rr_schedule(struct ProxyService *svc, struct sock * sk)
{
	int	result;

	if(svc->destCount<1)
		return 0;

	result = (int)svc->sched_data;
	if( result>=svc->destCount )
	{
		result = 0;
	}

	aos_debug( "proxy_rr_schedule(): Scheduling... %d\n",result);

	svc->sched_data = (void *) ( (result+1) % svc->destCount );
	return svc->destinations[result];
}


static struct proxy_scheduler proxy_rr_scheduler = {
	.name =			"rr",			/* name */
	.refcnt =		ATOMIC_INIT(0),
	.module =		THIS_MODULE,
	.init_service =		proxy_rr_init_svc,
	.done_service =		proxy_rr_done_svc,
	.update_service =	proxy_rr_update_svc,
	.schedule =		proxy_rr_schedule,
};

static int __init proxy_rr_init(void)
{
	int ret;


	INIT_LIST_HEAD(&proxy_rr_scheduler.n_list);
	ret = register_proxy_scheduler(&proxy_rr_scheduler);

	if( ret ) 
	{
		printk("loading rr scheduler failed");
	}

	return ret;
}

static void __exit proxy_rr_cleanup(void)
{
	unregister_proxy_scheduler(&proxy_rr_scheduler);
}

module_init(proxy_rr_init);
module_exit(proxy_rr_cleanup);
MODULE_LICENSE("GPL");


