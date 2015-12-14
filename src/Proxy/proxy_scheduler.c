////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_scheduler.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "proxy.h"


LIST_HEAD( proxy_schedulers );

/*
 *  Bind a service with a scheduler
 */
int proxy_bind_scheduler(struct ProxyService *svc, struct proxy_scheduler *scheduler)
{
	int ret;

	if (svc == NULL) {
		proxy_alarm("proxy_bind_scheduler(): svc arg NULL\n");
		return -EINVAL;
	}
	if (scheduler == NULL) {
		proxy_alarm("proxy_bind_scheduler(): scheduler arg NULL\n");
		return -EINVAL;
	}

	svc->scheduler = scheduler;

	if (scheduler->init_service) 
	{
		ret = scheduler->init_service(svc);
		if (ret) 
		{
			proxy_alarm("proxy_bind_scheduler(): init error\n");
			return ret;
		}
	}

	return 0;
}


/*
 *  Unbind a service with its scheduler
 */
int proxy_unbind_scheduler(struct ProxyService *svc)
{
	struct proxy_scheduler *sched;

	if (svc == NULL) {
		proxy_alarm("proxy_unbind_scheduler(): svc arg NULL\n");
		return -EINVAL;
	}

	sched = svc->scheduler;
	if (sched == NULL) {
		proxy_alarm("proxy_unbind_scheduler(): svc isn't bound\n");
		return -EINVAL;
	}

	if (sched->done_service) {
		if (sched->done_service(svc) != 0) {
			proxy_alarm("proxy_unbind_scheduler(): done error\n");
			return -EINVAL;
		}
	}

	svc->scheduler = NULL;
	return 0;
}



/*
 *  Get scheduler in the scheduler list by name
 */
struct proxy_scheduler *proxy_sched_getbyname(const char *sched_name)
{
	struct proxy_scheduler *sched;

	aos_debug(2, "proxy_sched_getbyname(): sched_name \"%s\"\n",
		  sched_name);

//	read_lock_bh(&__proxy_sched_lock);

	list_for_each_entry(sched, &proxy_schedulers, n_list) {
		/*
		 * Test and get the modules atomically
		 */
		if (sched->module && !try_module_get(sched->module)) {
			/*
			 * This scheduler is just deleted
			 */
			continue;
		}
		if (strcmp(sched_name, sched->name)==0) {
			/* HIT */
	//		read_unlock_bh(&__proxy_sched_lock);
			return sched;
		}
		if (sched->module)
			module_put(sched->module);
	}

//	read_unlock_bh(&__proxy_sched_lock);
	return NULL;
}


/*
 *  Register a scheduler in the scheduler list
 */
int register_proxy_scheduler(struct proxy_scheduler *scheduler)
{
	struct proxy_scheduler *sched;

	if (!scheduler) {
		proxy_alarm("register_proxy_scheduler(): NULL arg\n");
		return -EINVAL;
	}

	if (!scheduler->name) {
		proxy_alarm("register_proxy_scheduler(): NULL scheduler_name\n");
		return -EINVAL;
	}

	/* increase the module use count */
	//proxy_use_count_inc();

	/*
	 *  Make sure that the scheduler with this name doesn't exist
	 *  in the scheduler list.
	 */
	sched = proxy_sched_getbyname(scheduler->name);
	if (sched) {
		//proxy_scheduler_put(sched);
		//proxy_use_count_dec();
		proxy_alarm("register_proxy_scheduler(): [%s] scheduler "
			  "already existed in the system\n", scheduler->name);
		return -EINVAL;
	}

//	write_lock_bh(&__proxy_sched_lock);

	if (scheduler->n_list.next != &scheduler->n_list) {
		//write_unlock_bh(&__proxy_sched_lock);
		//proxy_use_count_dec();
		proxy_alarm("register_proxy_scheduler(): [%s] scheduler "
			  "already linked\n", scheduler->name);
		return -EINVAL;
	}

	/*
	 *	Add it into the d-linked scheduler list
	 */
	list_add(&scheduler->n_list, &proxy_schedulers);
	//write_unlock_bh(&__proxy_sched_lock);

	aos_trace("[%s] scheduler registered.\n", scheduler->name);

	return 0;
}



/*
 *  Unregister a scheduler from the scheduler list
 */
int unregister_proxy_scheduler(struct proxy_scheduler *scheduler)
{
	if (!scheduler) {
		proxy_alarm( "unregister_proxy_scheduler(): NULL arg\n");
		return -EINVAL;
	}

	//write_lock_bh(&__proxy_sched_lock);
	if (scheduler->n_list.next == &scheduler->n_list) {
		//write_unlock_bh(&__proxy_sched_lock);
		proxy_alarm("unregister_proxy_scheduler(): [%s] scheduler "
			  "is not in the list. failed\n", scheduler->name);
		return -EINVAL;
	}

	/*
	 *	Remove it from the d-linked scheduler list
	 */
	list_del(&scheduler->n_list);
//	write_unlock_bh(&__proxy_sched_lock);

	/* decrease the module use count */
//	proxy_use_count_dec();

	aos_trace("[%s] scheduler unregistered.\n", scheduler->name);

	return 0;
}

EXPORT_SYMBOL(register_proxy_scheduler);
EXPORT_SYMBOL(unregister_proxy_scheduler);



