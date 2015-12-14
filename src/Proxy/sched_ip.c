////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sched_ip.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//#define AOS_DEBUG

#include "proxy_scheduler.h"
#include <linux/module.h>
#include <linux/kernel.h>

static int proxy_ip_init_svc(struct ProxyService *svc)
{
	svc->sched_data = 0;
	return 0;
}


static int proxy_ip_done_svc(struct ProxyService *svc)
{
	return 0;
}


static int proxy_ip_update_svc(struct ProxyService *svc)
{
	svc->sched_data = 0;
	return 0;
}

/*
 * Round-Robin Scheduling
 */
static struct proxy_dest *
proxy_ip_schedule(struct ProxyService *svc, struct sock * sk)
{
	int	result;
	struct sockaddr_in addr;

	if(svc->destCount<1)
	{
		return 0;
	}
	else if(svc->destCount==1)
	{
		return svc->destinations[0];
	}

	aos_inet_getname( sk,(struct sockaddr*)&addr,&result,1 );
	
	result = addr.sin_addr.s_addr % svc->destCount; 
	
	aos_debug( "proxy_ip_schedule(): Scheduling... %d\n",result);
	return svc->destinations[result];
}


static struct proxy_scheduler proxy_ip_scheduler = {
	.name =			"ip",			/* name */
	.refcnt =		ATOMIC_INIT(0),
	.module =		THIS_MODULE,
	.init_service =		proxy_ip_init_svc,
	.done_service =		proxy_ip_done_svc,
	.update_service =	proxy_ip_update_svc,
	.schedule =		proxy_ip_schedule,
};

static int __init proxy_ip_init(void)
{
	int ret;


	INIT_LIST_HEAD(&proxy_ip_scheduler.n_list);
	ret = register_proxy_scheduler(&proxy_ip_scheduler);

	if( ret ) 
	{
		printk("loading ip scheduler failed");
	}

	return ret;

}

static void __exit proxy_ip_cleanup(void)
{
	unregister_proxy_scheduler(&proxy_ip_scheduler);
}

module_init(proxy_ip_init);
module_exit(proxy_ip_cleanup);
MODULE_LICENSE("GPL");
