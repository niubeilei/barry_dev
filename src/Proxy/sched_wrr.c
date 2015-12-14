////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sched_wrr.c
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

/*
 * current destination pointer for weighted round-robin scheduling
 */
struct ip_vs_wrr_mark {
	int cl;	/* current list head */
	int cw;			/* current weight */
	int mw;			/* maximum weight */
	int di;			/* decreasing interval */
};


/*
 *    Get the gcd of server weights
 */
static int gcd(int a, int b)
{
	int c;

	while ((c = a % b)) {
		a = b;
		b = c;
	}
	return b;
}

static int ip_vs_wrr_gcd_weight(struct ProxyService *svc)
{
	int weight;
	int g = 0;
	int	i;

	for(i=0;i<svc->destCount;i++)
	{
		weight = atomic_read(&svc->destinations[i]->weight);
		if (weight > 0) {
			if (g > 0)
				g = gcd(weight, g);
			else
				g = weight;
		}
	}
	return g ? g : 1;
}


/*
 *    Get the maximum weight of the service destinations.
 */
static int ip_vs_wrr_max_weight(struct ProxyService *svc)
{
	int weight = 0;
	int	t;
	int	i;

	for(i=0;i<svc->destCount;i++)
	{
		t = atomic_read(&svc->destinations[i]->weight); 
		if (t > weight)
			weight = t;
	}

	return weight;
}



static int proxy_wrr_init_svc(struct ProxyService *svc)
{
	struct ip_vs_wrr_mark *mark;

	/*
	 *    Allocate the mark variable for WRR scheduling
	 */
	mark = kmalloc(sizeof(struct ip_vs_wrr_mark), GFP_ATOMIC);
	if (mark == NULL) {
		proxy_alarm("ip_vs_wrr_init_svc(): no memory\n");
		return -ENOMEM;
	}
	mark->cl = -1;
	mark->cw = 0;
	mark->mw = ip_vs_wrr_max_weight(svc);
	mark->di = ip_vs_wrr_gcd_weight(svc);
	svc->sched_data = mark;

	return 0;

}


static int proxy_wrr_done_svc(struct ProxyService *svc)
{
	kfree(svc->sched_data);
	return 0;
}


static int proxy_wrr_update_svc(struct ProxyService *svc)
{
	struct ip_vs_wrr_mark *mark = svc->sched_data;

	mark->cl = -1;
	mark->mw = ip_vs_wrr_max_weight(svc);
	mark->di = ip_vs_wrr_gcd_weight(svc);

	return 0;
}

/*
 * Round-Robin Scheduling
 */
static struct proxy_dest *
proxy_wrr_schedule(struct ProxyService *svc, struct sock * sk)
{

	struct proxy_dest *dest;
	struct ip_vs_wrr_mark *mark = svc->sched_data;
	int p;

	aos_debug("ip_vs_wrr_schedule(): Scheduling...\n");

	/*
	 * This loop will always terminate, because mark->cw in (0, max_weight]
	 * and at least one server has its weight equal to max_weight.
	 */
	//write_lock(&svc->sched_lock);
	p = mark->cl;
	while (1) 
	{
		if (mark->cl == -1) 
		{ /* it is at the head of the destination list */
			if ( svc->destCount<1 ) 
			{
				/* no dest entry */
				dest = NULL;
				goto out;
			}

			mark->cl = 0;
			mark->cw -= mark->di;
			if (mark->cw <= 0) 
			{
				mark->cw = mark->mw;
				/*
				 * Still zero, which means no availabe servers.
				 */
				if (mark->cw == 0) 
				{
					mark->cl = -1;
					proxy_alarm("ip_vs_wrr_schedule(): "
						   "no available servers\n");
					dest = NULL;
					goto out;
				}
			}
		} 
		else
		{
			mark->cl = (mark->cl+1)%svc->destCount ; //?????????????
			if(mark->cl == 0)
				mark->cl = -1;
		}

		if (mark->cl != -1) 
		{
			/* not at the head of the list */
			dest = svc->destinations[mark->cl];
			if (atomic_read(&dest->weight) >= mark->cw) 
			{
				aos_debug("ip_vs_wrr_schedule(): Scheduling... %d\n", mark->cl);
				/* got it */
				break;
			}
		}

		if (mark->cl == p) {
			/* back to the start, and no dest is found.
			   It is only possible when all dests are OVERLOADED */
			dest = NULL;
			goto out;
		}
	}

	/*
	aos_debug( "WRR: server %u.%u.%u.%u:%u "
		  "activeconns %d refcnt %d weight %d\n",
		  NIPQUAD(dest->addr), ntohs(dest->port),
		  atomic_read(&dest->activeconns),
		  atomic_read(&dest->refcnt),
		  atomic_read(&dest->weight));
	*/

  out:
	return dest;



}


static struct proxy_scheduler proxy_wrr_scheduler = {
	.name =			"wrr",			/* name */
	.refcnt =		ATOMIC_INIT(0),
	.module =		THIS_MODULE,
	.init_service =		proxy_wrr_init_svc,
	.done_service =		proxy_wrr_done_svc,
	.update_service =	proxy_wrr_update_svc,
	.schedule =		proxy_wrr_schedule,
};

static int __init proxy_wrr_init(void)
{
	int ret;


	INIT_LIST_HEAD(&proxy_wrr_scheduler.n_list);
	ret = register_proxy_scheduler(&proxy_wrr_scheduler);

	if( ret ) 
	{
		printk("loading wrr scheduler failed");
	}

	return ret;

}

static void __exit proxy_wrr_cleanup(void)
{
	unregister_proxy_scheduler(&proxy_wrr_scheduler);
}

module_init(proxy_wrr_init);
module_exit(proxy_wrr_cleanup);
MODULE_LICENSE("GPL");


