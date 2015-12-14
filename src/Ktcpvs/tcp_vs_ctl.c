////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_ctl.c
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
 * Version:     $Id: tcp_vs_ctl.c,v 1.2 2015/01/06 08:57:50 andy Exp $
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
#include <linux/spinlock.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>

#include <net/ip.h>
#include <net/sock.h>
#include <net/tcp.h>

//#include <linux/netfilter_ipv4.h>

#include <asm/uaccess.h>
#include <asm/semaphore.h>

#include "AppProc/ReturnCode.h"
#include "AppProc/http_proc/AutoLogin.h"
#include "aos/aosKernelApi.h"
#include "aosUtil/StringUtil.h"
#include "Ktcpvs/aosTcpVsCntl.h"
#include "Ktcpvs/tcp_vs.h"
#include "AppProxy/DenyPage.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Mutex.h"
//#include "ssl/aosSslProc.h"
#include "ssl/SslCommon.h"

#include <linux/netfilter.h>
/*
 * tcp_vs_svc_list:	  TCPVS service list head
 * tcp_vs_num_services:	  number of TCPVS services
 * __tcp_vs_mutex:	  semaphore for TCPVS sockopts,
 *			  [gs]etsockopt thread may sleep.
 * __tcp_vs_svc_lock:	  lock for service table
 *
 * Only the master_daemon thread and the [gs]etsockopt threads will access
 * the tcp_vs_svc_list. The master_daemon thread only read it, and the
 * [gs]etsockopt threads may read/write it, but the __tcp_vs_mutex will
 * gurantee that only one [gs]etsockopt thread can enter. So, only the
 * setsockopt thread (using write operations) need do write locking before
 * access, and the master_daemon thread need to do read locking.
 *
 * Note that accessing the members of struct tcp_vs_service *svc (such as
 * the destination list) need use svc->lock.
 */
struct list_head tcp_vs_svc_list;
static int tcp_vs_num_services = 0;
extern int aos_tcpvs_maxconns;
extern atomic_t aos_tcpvs_curconns;
//
// Chen Ding, 08/24/2005
// Changed to global variable since aosTcpCntl.c needs to use it.
//
// static DECLARE_MUTEX(__tcp_vs_mutex);
DECLARE_MUTEX(__tcp_vs_mutex);

rwlock_t __tcp_vs_svc_lock = RW_LOCK_UNLOCKED;


/* sysctl variables */
int sysctl_ktcpvs_unload = 0;
int sysctl_ktcpvs_max_backlog = 2048;
int sysctl_ktcpvs_zerocopy_send = 0;
int sysctl_ktcpvs_keepalive_timeout = 120;
int sysctl_ktcpvs_read_timeout = 180;

// #ifdef CONFIG_TCP_VS_DEBUG
static int sysctl_ktcpvs_debug_level = 5;

int
tcp_vs_get_debug_level(void)
{
	return sysctl_ktcpvs_debug_level;
}
// #endif

int tcp_vs_sysctl_register(struct tcp_vs_service *svc);
int tcp_vs_sysctl_unregister(struct tcp_vs_service *svc);
struct proc_dir_entry *proc_net_ktcpvs_vs_create(struct tcp_vs_service
						 *svc);
void proc_net_ktcpvs_vs_release(struct tcp_vs_service *svc);


/*
 *  Lookup destination by {addr,port} in the given service
 */
static tcp_vs_dest_t *
tcp_vs_lookup_dest(struct tcp_vs_service *svc, __u32 daddr, __u16 dport)
{
	tcp_vs_dest_t *dest;
	struct list_head *l, *e;

	read_lock_bh(&svc->lock);
	/*
	 * Find the destination for the given virtual server
	 */
	l = &svc->destinations;
	for (e = l->next; e != l; e = e->next) {
		dest = list_entry(e, tcp_vs_dest_t, n_list);
		if ((dest->addr == daddr) && (dest->port == dport)) {
			/* HIT */
			read_unlock_bh(&svc->lock);
			return dest;
		}
	}
	read_unlock_bh(&svc->lock);
	return NULL;
}


/*
 *  Add a destination into an existing KTCPVS service
 */
// Chen Ding, 08/25/2005
// Changed it to a global function since it is needed by other 
// functions.
//
// Added a parameter 'errmsg' to get the error messages.
//
int
tcp_vs_add_dest(struct tcp_vs_service *svc,
				__u32 daddr, 
				__u16 dport, 
				int weight,
				char *errmsg)
{
	tcp_vs_dest_t *dest;

	EnterFunction(2);

	if (weight < 0) {
		if(errmsg)
		       	strcpy(errmsg, "Server weight less than zero");
		return -eAosRc_InvalidWeight;
	}

	/*
	 * Check if the dest already exists in the list
	 */
	dest = tcp_vs_lookup_dest(svc, daddr, dport);
	if (dest != NULL) {
		if(errmsg)
			strcpy(errmsg, "Real server already exists");
		return -eAosRc_RealServerExists;
	}

	/*
	 * Allocate and initialize the dest structure
	 */
	dest = aos_malloc(sizeof(tcp_vs_dest_t));
	if (dest == NULL) {
		if(errmsg)
			strcpy(errmsg, "vs dest memory allocation failed");
		return -eAosRc_ProgErr;
	}
	memset(dest, 0, sizeof(tcp_vs_dest_t));

	dest->addr = daddr;
	dest->port = dport;
	dest->weight = weight;

	atomic_set(&dest->conns, 0);
	atomic_set(&dest->refcnt, 0);
	INIT_LIST_HEAD(&dest->r_list);

	write_lock_bh(&svc->lock);

	/* add the dest entry into the list */
	list_add(&dest->n_list, &svc->destinations);
	atomic_inc(&dest->refcnt);
	svc->num_dests++;

	write_unlock_bh(&svc->lock);

	TCP_VS_DBG(2, "Add dest addr=%u.%u.%u.%u port=%u weight=%d\n",
		   NIPQUAD(daddr), ntohs(dport), weight);

	LeaveFunction(2);

	return 0;
}



/*
 *  Delete a destination from the given virtual server
 */
static inline void
__tcp_vs_del_dest(struct tcp_vs_service *svc, tcp_vs_dest_t * dest)
{
	/*
	 *  Remove it from the lists.
	 */
	list_del(&dest->n_list);
	/*  list_del(&dest->r_list); */
	svc->num_dests--;

	/*
	 *  Decrease the refcnt of the dest, and free the dest
	 *  if nobody refers to it (refcnt=0). Otherwise, throw
	 *  the destination into the trash.
	 */
	if (atomic_dec_and_test(&dest->refcnt))
		aos_free(dest);
}


// 
// Chen Ding, 08/25/2005
// Changed it to a global funciton.
// Added a parameter "errmsg"
//
int
tcp_vs_del_dest(struct tcp_vs_service *svc, 
				__u32 daddr, 
				__u16 dport,
				char *errmsg)
{
	tcp_vs_dest_t *dest;

	EnterFunction(2);

	/*
	 *    Lookup the destination list
	 */
	dest = tcp_vs_lookup_dest(svc, daddr, dport);
	if (dest == NULL) {
		strcpy(errmsg, "Destination not found!");
		return -eAosRc_DestNotFound;
	}

	// 
	// We don't care about the rules now. But it should always be 
	// empty.
	//
	if (!list_empty(&dest->r_list)) {
		strcpy(errmsg, "Rule list is not empty");
		return -eAosRc_RuleListNotEmpty;
	}

	write_lock_bh(&svc->lock);

	/*
	 *  Remove dest from the destination list
	 */
	__tcp_vs_del_dest(svc, dest);

	/*
	 *  Called the update_service function of its scheduler
	 */
	svc->scheduler->update_service(svc);

	write_unlock_bh(&svc->lock);

	LeaveFunction(2);

	return 0;
}

static void
__tcp_vs_flush_rule(struct tcp_vs_service *svc)
{
	struct list_head *l, *d;
	struct tcp_vs_rule *r;
	tcp_vs_dest_t *dest;

	EnterFunction(2);
	for (l = &svc->rule_list; l->next != l;) {
		r = list_entry(l->next, struct tcp_vs_rule, list);
		list_del(&r->list);
		TCP_VS_DBG(2, "flush the rule %s in the service %s\n",
			   r->pattern, svc->ident.name);
		for (d = &r->destinations; d->next != d;) {
			dest = list_entry(d->next, tcp_vs_dest_t, r_list);
			list_del_init(&dest->r_list);
		}
		//regfree(&r->rx);
		kfree(r->pattern);
		kfree(r);
	}
	LeaveFunction(2);
}



struct tcp_vs_service *
tcp_vs_lookup_byident(const struct tcp_vs_ident *id)
{
	struct list_head *e;
	struct tcp_vs_service *svc;

	list_for_each(e, &tcp_vs_svc_list) {
		svc = list_entry(e, struct tcp_vs_service, list);
		if (!strcmp(id->name, svc->ident.name))
			/* HIT */
			return svc;
	}

	return NULL;
}


// 
// Chen Ding, 08/24/2005
// Changed to non-static function because we need to call this function 
// to add service.
//
// Added a parameter 'errmsg'. 
//
int
tcp_vs_add_service(struct tcp_vs_ident *ident, 
				   struct tcp_vs_config *conf, 
				   char *errmsg)
{
	struct tcp_vs_service *svc;
	struct tcp_vs_scheduler *sched;
	int ret = 0;

	EnterFunction(2);

	/* lookup scheduler here */
	sched = tcp_vs_scheduler_get(conf->sched_name);
	if (sched == NULL) {
		sprintf(errmsg, "Did not find the scheduler: %s", 
			conf->sched_name);
		printk("%s\n", errmsg);
		return -eAosRc_ObjectNotFound;
	}

	svc = aos_malloc(sizeof(*svc));
	if (!svc) {
		strcpy(errmsg, "no available memory");
		printk("%s\n", errmsg);
		ret = -eAosRc_MemErr;
		goto out;
	}

	memset(svc, 0, sizeof(*svc));
	INIT_LIST_HEAD(&svc->destinations);
	INIT_LIST_HEAD(&svc->connections);
	INIT_LIST_HEAD(&svc->rule_list);
	//INIT_LIST_HEAD(&svc->alogin_config.alogin_file_list);
	memcpy(&svc->ident, ident, sizeof(*ident));
	memcpy(&svc->conf, conf, sizeof(*conf));
	if (svc->conf.maxClients > KTCPVS_CHILD_HARD_LIMIT)
		svc->conf.maxClients = KTCPVS_CHILD_HARD_LIMIT;
	svc->lock = RW_LOCK_UNLOCKED;

	ret = tcp_vs_bind_scheduler(svc, sched);
	if (ret != 0) {
		aos_free(svc);
		strcpy(errmsg, "Failed to bind scheduler");
		goto out;
	}

	write_lock_bh(&__tcp_vs_svc_lock);
	list_add(&svc->list, &tcp_vs_svc_list);
	tcp_vs_num_services++;
	write_unlock_bh(&__tcp_vs_svc_lock);

out:
	tcp_vs_scheduler_put(sched);
	LeaveFunction(2);
	return ret;
}


int
tcp_vs_edit_service(struct tcp_vs_service *svc, struct tcp_vs_config *conf, char *errmsg)
{
	struct tcp_vs_scheduler *sched;

	EnterFunction(2);

	/* lookup scheduler here */
	if (strcmp(svc->scheduler->name, conf->sched_name)) {
		sched = tcp_vs_scheduler_get(conf->sched_name);
		if (sched == NULL) {
			sprintf(errmsg, 
				"Scheduler module tcp_vs_%s.o not found\n",
			     	conf->sched_name);
			TCP_VS_INFO
			    ("Scheduler module tcp_vs_%s.o not found\n",
			     conf->sched_name);
			return -ENOENT;
		}
		tcp_vs_unbind_scheduler(svc);
		tcp_vs_bind_scheduler(svc, sched);
		tcp_vs_scheduler_put(sched);
	}

	memcpy(&svc->conf, conf, sizeof(*conf));
	if (svc->conf.maxClients > KTCPVS_CHILD_HARD_LIMIT)
		svc->conf.maxClients = KTCPVS_CHILD_HARD_LIMIT;

	LeaveFunction(2);
	return 0;
}

static inline int
__tcp_vs_del_service(struct tcp_vs_service *svc)
{
	struct list_head *l;
	tcp_vs_dest_t *dest;

	if (atomic_read(&svc->running)) {
		TCP_VS_ERR("The VS is running, you'd better stop it first"
			   "before deleting it.\n");
		return -EBUSY;
	}

	tcp_vs_num_services--;

	/* unlink the whole destination list */
	write_lock_bh(&svc->lock);
	__tcp_vs_flush_rule(svc);
	for (l = &svc->destinations; l->next != l;) {
		dest = list_entry(l->next, tcp_vs_dest_t, n_list);
		__tcp_vs_del_dest(svc, dest);
	}
	tcp_vs_unbind_scheduler(svc);
	write_unlock_bh(&svc->lock);

	list_del(&svc->list);
	aos_free(svc);
	return 0;
}


// 
// Chen Ding, 08/24/2005
// Change it to a global function since it is needed by other
// functions.
//
int
tcp_vs_del_service(struct tcp_vs_service *svc)
{
	int ret;

	EnterFunction(2);

	write_lock_bh(&__tcp_vs_svc_lock);
	ret = __tcp_vs_del_service(svc);
	write_unlock_bh(&__tcp_vs_svc_lock);

	LeaveFunction(2);
	return ret;
}


int
tcp_vs_flush(void)
{
	struct list_head *l;
	struct tcp_vs_service *svc;
	int ret = 0;

	EnterFunction(2);

	write_lock_bh(&__tcp_vs_svc_lock);
	for (l = &tcp_vs_svc_list; l->next != l;) {
		svc = list_entry(l->next, struct tcp_vs_service, list);
		if ((ret = __tcp_vs_del_service(svc)))
			break;
	}
	write_unlock_bh(&__tcp_vs_svc_lock);

	LeaveFunction(2);
	return 0;
}

static int
tcp_vs_stop_all(void)
{
	struct list_head *e;
	struct tcp_vs_service *svc;
	char	someone_running=0;

	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(e, &tcp_vs_svc_list) {
		svc = list_entry(e, struct tcp_vs_service, list);
		svc->stop = 1;
	}
	write_unlock_bh(&__tcp_vs_svc_lock);

	while(1)
	{
		someone_running=0;
		write_lock_bh(&__tcp_vs_svc_lock);
		list_for_each(e, &tcp_vs_svc_list) 
		{
			svc = list_entry(e, struct tcp_vs_service, list);
			if(atomic_read(&svc->running))
				someone_running=1;
		}
		write_unlock_bh(&__tcp_vs_svc_lock);
		if(!someone_running)
			break;

		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
	}

	return 0;
}

static inline int
__tcp_vs_get_service_entries(const struct tcp_vs_get_services *get,
			     struct tcp_vs_get_services *uptr)
{
	int count = 0;
	struct tcp_vs_service *svc;
	struct list_head *l;
	struct tcp_vs_service_u entry;
	int ret = 0;

	if (down_interruptible(&__tcp_vs_mutex))
		return -ERESTARTSYS;
	list_for_each(l, &tcp_vs_svc_list) {
		if (count >= get->num_services)
			break;
		svc = list_entry(l, struct tcp_vs_service, list);
		memcpy(&entry.ident, &svc->ident,
		       sizeof(struct tcp_vs_ident));
		memcpy(&entry.conf, &svc->conf,
		       sizeof(struct tcp_vs_config));
		entry.num_dests = svc->num_dests;
		entry.num_rules = svc->num_rules;
		entry.conns = atomic_read(&svc->conns);
		entry.running = atomic_read(&svc->running);
		if (copy_to_user(&uptr->entrytable[count],
				 &entry, sizeof(entry))) {
			ret = -EFAULT;
			break;
		}
		count++;
	}
	up(&__tcp_vs_mutex);
	return ret;
}

static inline int
__tcp_vs_get_dest_entries(const struct tcp_vs_get_dests *get,
			  struct tcp_vs_get_dests *uptr)
{
	struct tcp_vs_service *svc;
	int ret = 0;

	if (down_interruptible(&__tcp_vs_mutex))
		return -ERESTARTSYS;
	svc = tcp_vs_lookup_byident(&get->ident);
	if (svc) {
		int count = 0;
		tcp_vs_dest_t *dest;
		struct list_head *l;
		struct tcp_vs_dest_u entry;

		list_for_each(l, &svc->destinations) {
			if (count >= get->num_dests)
				break;
			dest = list_entry(l, tcp_vs_dest_t, n_list);
			entry.addr = dest->addr;
			entry.port = dest->port;
			entry.weight = dest->weight;
			entry.conns = atomic_read(&dest->conns);
			if (copy_to_user(&uptr->entrytable[count],
					 &entry, sizeof(entry))) {
				ret = -EFAULT;
				break;
			}
			count++;
		}
	} else
		ret = -ESRCH;
	up(&__tcp_vs_mutex);
	return ret;
}


static inline int
__tcp_vs_get_rule_entries(const struct tcp_vs_get_rules *get,
			  struct tcp_vs_get_rules *uptr)
{
	struct tcp_vs_service *svc;
	int ret = 0;
	int count = 0;
	struct list_head *l, *e;

	down_interruptible(&__tcp_vs_mutex);
	svc = tcp_vs_lookup_byident(&get->ident);
	if (!svc) {
		ret = -ESRCH;
		goto out;
	}

	list_for_each(l, &svc->rule_list) {
		struct tcp_vs_rule *rule;

		rule = list_entry(l, struct tcp_vs_rule, list);
		list_for_each(e, &rule->destinations) {
			tcp_vs_dest_t *dest;
			struct tcp_vs_rule_u entry;

			if (count >= get->num_rules)
				goto out;
			dest = list_entry(e, tcp_vs_dest_t, r_list);
			strcpy(entry.pattern, rule->pattern);
			entry.len = rule->len;
			entry.match_num = rule->match_num;
			entry.addr = dest->addr;
			entry.port = dest->port;
			if (copy_to_user(&uptr->entrytable[count],
					 &entry, sizeof(entry))) {
				ret = -EFAULT;
				goto out;
			}
			count++;
		}
	}
      out:
	up(&__tcp_vs_mutex);
	return ret;
}


static int
do_tcp_vs_get_ctl(struct sock *sk, int cmd, void __user *user, int *len)
{
	int ret = 0;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;

	switch (cmd) {
	case TCP_VS_SO_GET_VERSION:
		{
			char buf[64];

			sprintf(buf, "TCP Virtual Server version %d.%d.%d",
				NVERSION(TCP_VS_VERSION_CODE));
			if (*len < strlen(buf) + 1)
				return -EINVAL;
			if (copy_to_user(user, buf, strlen(buf) + 1) != 0)
				return -EFAULT;
			*len = strlen(buf) + 1;
		}
		break;

	case TCP_VS_SO_GET_INFO:
		{
			struct tcp_vs_getinfo info;
			info.version = TCP_VS_VERSION_CODE;
			info.num_services = tcp_vs_num_services;
			if (copy_to_user(user, &info, sizeof(info)) != 0)
				return -EFAULT;
		}
		break;

	case TCP_VS_SO_GET_SERVICES:
		{
			struct tcp_vs_get_services get;

			if (*len < sizeof(get)) {
				TCP_VS_ERR("length: %u < %u\n", *len,
					   sizeof(get));
				return -EINVAL;
			}
			if (copy_from_user(&get, user, sizeof(get)))
				return -EFAULT;
			if (*len !=
			    (sizeof(get) +
			     sizeof(struct tcp_vs_service_u) *
			     get.num_services)) {
				TCP_VS_ERR("length: %u != %u\n", *len,
					   sizeof(get) +
					   sizeof(struct tcp_vs_service_u)
					   * get.num_services);
				return -EINVAL;
			}
			ret = __tcp_vs_get_service_entries(&get, user);
		}
		break;

	case TCP_VS_SO_GET_SERVICE:
		{
			struct tcp_vs_service_u get;
			struct tcp_vs_service *svc;

			if (*len != sizeof(get)) {
				TCP_VS_ERR("length: %u != %u\n", *len,
					   sizeof(get));
				return -EINVAL;
			}
			if (copy_from_user(&get, user, sizeof(get)))
				return -EFAULT;

			if (down_interruptible(&__tcp_vs_mutex))
				return -ERESTARTSYS;
			svc = tcp_vs_lookup_byident(&get.ident);
			if (svc) {
				memcpy(&get.ident, &svc->ident,
				       sizeof(struct tcp_vs_ident));
				memcpy(&get.conf, &svc->conf,
				       sizeof(struct tcp_vs_config));
				get.num_dests = svc->num_dests;
				get.num_rules = svc->num_rules;
				get.conns = atomic_read(&svc->conns);
				get.running = atomic_read(&svc->running);
				if (copy_to_user(user, &get, *len) != 0)
					ret = -EFAULT;
			} else
				ret = -ESRCH;
			up(&__tcp_vs_mutex);
		}
		break;

	case TCP_VS_SO_GET_DESTS:
		{
			struct tcp_vs_get_dests get;

			if (*len < sizeof(get)) {
				TCP_VS_ERR("length: %u < %u\n", *len,
					   sizeof(get));
				return -EINVAL;
			}
			if (copy_from_user(&get, user, sizeof(get)))
				return -EFAULT;
			if (*len != (sizeof(get) +
				     sizeof(struct tcp_vs_dest_u) *
				     get.num_dests)) {
				TCP_VS_ERR("length: %u != %u\n", *len,
					   sizeof(get) +
					   sizeof(struct tcp_vs_dest_u) *
					   get.num_dests);
				return -EINVAL;
			}
			ret = __tcp_vs_get_dest_entries(&get, user);
		}
		break;

	case TCP_VS_SO_GET_RULES:
		{
			struct tcp_vs_get_rules get;

			if (*len < sizeof(get)) {
				TCP_VS_ERR("length: %u < %u\n", *len,
					   sizeof(get));
				return -EINVAL;
			}
			if (copy_from_user(&get, user, sizeof(get)))
				return -EFAULT;
			if (*len != (sizeof(get) +
				     sizeof(struct tcp_vs_rule_u) *
				     get.num_rules)) {
				TCP_VS_ERR("length: %u != %u\n", *len,
					   sizeof(get) +
					   sizeof(struct tcp_vs_rule_u) *
					   get.num_rules);
				return -EINVAL;
			}
			ret = __tcp_vs_get_rule_entries(&get, user);
		}
		break;

	default:
		ret = -EINVAL;
	}

	return ret;
}

/*
static struct nf_sockopt_ops tcp_vs_sockopts = {
	{NULL, NULL}, PF_INET,
	//TCP_VS_BASE_CTL, TCP_VS_SO_SET_MAX + 1, do_tcp_vs_set_ctl,
	TCP_VS_BASE_CTL, TCP_VS_SO_SET_MAX + 1, NULL,
	TCP_VS_BASE_CTL, TCP_VS_SO_GET_MAX + 1, do_tcp_vs_get_ctl,
	0,0
};
*/
static struct nf_sockopt_ops tcp_vs_sockopts = {
	.pf = PF_INET,
	.set_optmin = TCP_VS_BASE_CTL, 
	.set_optmax = TCP_VS_SO_SET_MAX + 1, 
	.get_optmin = TCP_VS_BASE_CTL, 
	.get_optmax = TCP_VS_SO_GET_MAX + 1, 
	.get = do_tcp_vs_get_ctl,
};


static struct ctl_table_header *ktcpvs_table_header;

static ctl_table ktcpvs_table[] = {
#ifdef CONFIG_TCP_VS_DEBUG
	{NET_KTCPVS_DEBUGLEVEL, "debug_level", &sysctl_ktcpvs_debug_level,
	 sizeof(int), 0644, NULL, &proc_dointvec},
#endif
	{NET_KTCPVS_UNLOAD, "unload", &sysctl_ktcpvs_unload,
	 sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_KTCPVS_MAXBACKLOG, "max_backlog", &sysctl_ktcpvs_max_backlog,
	 sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_KTCPVS_ZEROCOPY_SEND, "zerocopy_send",
	 &sysctl_ktcpvs_zerocopy_send,
	 sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_KTCPVS_KEEPALIVE_TIMEOUT, "keepalive_timeout",
	 &sysctl_ktcpvs_keepalive_timeout,
	 sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_KTCPVS_READ_TIMEOUT, "read_timeout",
	 &sysctl_ktcpvs_read_timeout,
	 sizeof(int), 0644, NULL, &proc_dointvec},
	{0}
};

static ctl_table ktcpvs_dir_table[] = {
	{NET_KTCPVS, "ktcpvs", NULL, 0, 0555, ktcpvs_table},
	{0}
};

static ctl_table ktcpvs_root_table[] = {
	{CTL_NET, "net", NULL, 0, 0555, ktcpvs_dir_table},
	{0}
};


int
tcp_vs_control_start(void)
{
	int ret;

	INIT_LIST_HEAD(&tcp_vs_svc_list);

	ret = nf_register_sockopt(&tcp_vs_sockopts);
	if (ret) {
		TCP_VS_ERR("cannot register sockopt.\n");
		return ret;
	}

	ktcpvs_table_header = register_sysctl_table(ktcpvs_root_table, 0);
	return ret;
}


void
tcp_vs_control_stop(void)
{
	unregister_sysctl_table(ktcpvs_table_header);
	nf_unregister_sockopt(&tcp_vs_sockopts);
}

// 
// Chen Ding, 08/24/2005
//
static int aos_tcpvs_show_one_service(
			struct tcp_vs_service *svc,
			char *data, 
			unsigned int length,
			unsigned int *index,
			char printTail)
{
    tcp_vs_dest_t *dest;
    struct list_head *l, *e;
	int ret = 0;
	char local[200];
	sprintf(local, "---------------------------------------\n");
	aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "Service Name:      %-s\n", svc->ident.name);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	if (svc->start && !svc->stop)
	{
		sprintf(local, "Status:            Started\n");
	}
	else
	{
		sprintf(local, "Status:            Stopped\n");
	}

	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));
	sprintf(local, "Local Address:     %-s\n", aosAddrToStr(svc->conf.addr));
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "Local Port:        %-d\n", ntohs(svc->conf.port));
	// sprintf(local, "Local Port:        %-d\n", svc->conf.port);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "Timeout:           %-d\n", svc->conf.timeout);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "App Type:          %-s\n", svc->conf.sched_name);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Start Server:      %-d\n", svc->conf.startservers);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Max Servers:       %-d\n", svc->conf.maxSpareServers);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Min Servers:       %-d\n", svc->conf.minSpareServers);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Max Clients:       %-d\n", svc->conf.maxClients);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Keep Alive:        %-d\n", svc->conf.keepAlive);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Max Keepalive Req: %-d\n", svc->conf.maxKeepAliveRequests);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Keepalive Timer:   %-d\n", svc->conf.keepAliveTimeout);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Front SSL:         %s %s\n", 
		svc->conf.front_ssl_flags?"on":"off",
		svc->conf.front_ssl_flags&eAosSSLFlag_ClientAuth?"clientauth":"");
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

    sprintf(local, "Backend SSL:       %s %s\n", 
		svc->conf.back_ssl_flags?"on":"off",
		svc->conf.back_ssl_flags&eAosSSLFlag_ClientAuth?"clientauth":"");
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));
	
	sprintf(local, "Real Servers:      %-d\n", svc->num_dests);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "Connections:       %-d\n", atomic_read(&svc->conns));
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "Running:           %-d\n", atomic_read(&svc->running));
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "PMI:               %s\n", svc->conf.pmi?"on":"off");
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "PMI Sysname:       %s\n", svc->conf.pmi_sysname);
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	sprintf(local, "Forward table:     %s\n", svc->conf.dynamic_dst?"on":"off");
	ret |= aosCheckAndCopy(data, index, length, local, strlen(local));

	// 
	// List the real servers.
	//
    read_lock_bh(&svc->lock);
    l = &svc->destinations;
    for (e = l->next; e != l; e = e->next) {
        dest = list_entry(e, tcp_vs_dest_t, n_list);

		sprintf(local, "Real Server:       %-s:%-d\n", 
			aosAddrToStr(dest->addr), ntohs(dest->port));
		ret |= aosCheckAndCopy(data, index, length, local, strlen(local));
    }
    read_unlock_bh(&svc->lock);
		
	if (printTail)
	{
		sprintf(local, "---------------------------------------\n");
		aosCheckAndCopy(data, index, length, local, strlen(local));
	}

	return ret;
}


int aos_tcpvs_show_service(
			char *name,
			char *data, 
			unsigned int length,
			unsigned int *index, 
			char *errmsg)
{
    int ret = 0;
    struct tcp_vs_ident ident;
    struct tcp_vs_service *svc;

    errmsg[0] = 0;

    if (!name)
    {
        strcpy(errmsg, "Service name is a null pointer");
        return -eAosRc_NullPointer;
    }

    // Create 'ident'
    if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
    {
        sprintf(errmsg, "Service name too long. Maximum allowed: %d",
            KTCPVS_IDENTNAME_MAXLEN);
        return -eAosRc_NameTooLong;
    }
    strcpy(ident.name, name);

    //
    // Lock
    //
    down(&__tcp_vs_mutex);

    svc = tcp_vs_lookup_byident(&ident);

    if (!svc)
    {
        strcpy(errmsg, "Service not found");
        ret = -eAosRc_ObjectNotFound;
        goto out;
    }

	ret = aos_tcpvs_show_one_service(svc, data, length, index, 1);

out:
    //
    // Unlock
    //
    up(&__tcp_vs_mutex);

    return ret;
}


int aos_vs_get_service_entries(
			char *data, 
			unsigned int length,
			unsigned int *index, 
			char *errmsg)
{
	int ret = 0;
	struct tcp_vs_service *svc;
	struct list_head *l;
	char local[200];
	int count = 0;

	if (down_interruptible(&__tcp_vs_mutex)) {
		strcpy(errmsg, "Failed to lock");
		return -eAosRc_FailedToLock;
	}

	sprintf(local, "Max Supported Concurrent Conns: %d\n", aos_tcpvs_maxconns);
	aosCheckAndCopy(data, index, length, local, strlen(local));
	sprintf(local, "Concurrent Conns: %d\n", atomic_read(&aos_tcpvs_curconns));
	aosCheckAndCopy(data, index, length, local, strlen(local));

	list_for_each(l, &tcp_vs_svc_list) {
		svc = list_entry(l, struct tcp_vs_service, list);

		if ((ret = aos_tcpvs_show_one_service(svc, data, length, index, 0)))
		{
			break;
		}

		count++;
	}
	up(&__tcp_vs_mutex);

	if (count > 0)
	{
		sprintf(local, "---------------------------------------\n");
		aosCheckAndCopy(data, index, length, local, strlen(local));
	}

	return ret;
}

void realserver_saveconf(struct tcp_vs_service *svc, char *buf, unsigned int *index, const unsigned int length)
{
 	tcp_vs_dest_t *dest;
	struct list_head *ldest;
	char tmpbuf[256];

	// app proxy rs add <vs-name> <rs-addr> <rs-port> [<weight>]
	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(ldest, &svc->destinations) 
	{
		dest = list_entry(ldest, tcp_vs_dest_t, n_list);
		sprintf(tmpbuf, 
			"<Cmd>app proxy rs add %s %s %d %d</Cmd>\n",
			svc->ident.name,
			aosAddrToStr(dest-> addr),
			ntohs(dest->port),
			dest->weight);
		aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
	}
	write_unlock_bh(&__tcp_vs_svc_lock);
}

void aos_ktcpvs_saveconfig(char *buf, unsigned int *index, const unsigned int length)
{
	struct list_head *lsvc;
	struct tcp_vs_service *svc;
	char tmpbuf[256];

	// kevin, 08/10/2006, HAC
 	//sprintf(tmpbuf, "<Cmd>ktcpvs init</Cmd>\n");
	//aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
 
    //sprintf(tmpbuf, "<Cmd>app proxy set max conn %d</Cmd>\n", aos_tcpvs_maxconns);
    //aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));

	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(lsvc, &tcp_vs_svc_list) 
	{
		svc = list_entry(lsvc, struct tcp_vs_service, list);
		// app proxy add <name> <apptype> <addr> <port> < flag>
		sprintf(tmpbuf,
			"<Cmd>app proxy add %s %s %s %d %s</Cmd>\n", 
			svc->ident.name,
			svc->conf.sched_name,
			aosAddrToStr(svc->conf.addr), 
			ntohs(svc->conf.port), 
			svc->conf.front_ssl_flags?"ssl":"plain");
		aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));

		// app proxy ssl front <name> <on/off >
		if (svc->conf.front_ssl_flags)
		{
        	sprintf(tmpbuf,
            	"<Cmd>app proxy ssl front %s on</Cmd>\n",
            	svc->ident.name);
        	aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
		}
		
		// app proxy ssl clientauth <name> <on/off >
		if (svc->conf.front_ssl_flags&eAosSSLFlag_ClientAuth)
		{
        	sprintf(tmpbuf,
            	"<Cmd>app proxy ssl clientauth %s on</Cmd>\n",
            	svc->ident.name);
        	aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
		}

		// app proxy ssl backend <name> <on/off >
		if (svc->conf.back_ssl_flags)
		{
        	sprintf(tmpbuf,
            	"<Cmd>app proxy ssl backend %s on</Cmd>\n",
            	svc->ident.name);
        	aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
		}

		// app proxy rs add <vs-name> <rs-addr> <rs-port> [<weight>]
		realserver_saveconf(svc, buf, index, length);
		
		// app proxy pmi sysname <appname> <pmisysname>
		if (strlen(svc->conf.pmi_sysname)>0)
			sprintf(tmpbuf, 
				"<Cmd>app proxy pmi sysname %s %s</Cmd>\n",
				svc->ident.name,
				svc->conf.pmi_sysname);	
		aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));

		// app proxy set status <name> <on/off >
		sprintf(tmpbuf,
			"<Cmd>app proxy set status %s %s</Cmd>\n", 
			svc->ident.name,
			svc->start?"on":"off");
		aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
		
		// app proxy set pmi <name> <on/off>
		if (svc->conf.pmi)
		{	sprintf(tmpbuf,
				"<Cmd>app proxy set pmi %s on</Cmd>\n", 
				svc->ident.name);
			aosCheckAndCopy(buf, index, length, tmpbuf, strlen(tmpbuf));
		}
	}
	write_unlock_bh(&__tcp_vs_svc_lock);

	
}


int aos_ktcpvs_clearconfig(char *errmsg, const int errlen)
{

	aos_tcpvs_maxconns = MAX_CONCURRENT_CONNECTIONS;

	tcp_vs_stop_all();
	tcp_vs_flush();
	return 0;
}


/*
int aos_ktcpvs_set_autologin(const char *vsname, 
							 const char *flag,
							 char *errmsg)
{
	struct tcp_vs_service *service;

	write_lock_bh(&__tcp_vs_svc_lock);
	service = tcp_vs_get(vsname);
	if (!service)
	{
		write_unlock_bh(&__tcp_vs_svc_lock);
		strcpy(errmsg, "Service not found");
		return -eAosRc_ServiceNotFound;
	}

    if (strcmp(flag, "on") == 0)
    {
		service->alogin_config.auto_login = 1;
    }
    else if (strcmp(flag, "off") == 0)
    {
		service->alogin_config.auto_login = 0;
    }
    else
    {
        strcpy(errmsg, "Invalid flag");
        return -eAosRc_InvalidFlag;
    }

	write_unlock_bh(&__tcp_vs_svc_lock);
	return 0;
}
*/

int aos_ktcpvs_saveAutologin(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	struct list_head *lsvc;
	struct tcp_vs_service *svc;
	//char local[256];
	//char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int index = 0;
	//unsigned int optlen = *length;
	int ret = 0;

	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(lsvc, &tcp_vs_svc_list) 
	{
		svc = list_entry(lsvc, struct tcp_vs_service, list);
/*		if (!svc->alogin_config)
		{
			continue;
		}

		sprintf(local,
			"<Cmd>auto login config %s %s %s %s %s</Cmd>\n", 
			svc->ident.name,
			svc->alogin_config->login_url,
			svc->alogin_config->part1,
			svc->alogin_config->part2,
			svc->alogin_config->part3);
		ret = aosCheckAndCopy(rsltBuff, &index, optlen, local, strlen(local));

		if (svc->auto_login)
		{
			sprintf(local, "<Cmd/auto login set status %s on</Cmd>", 
				svc->ident.name);
			ret = aosCheckAndCopy(rsltBuff, &index, optlen, local, strlen(local));
		}*/
	}
	write_unlock_bh(&__tcp_vs_svc_lock);

	*length = index;

	return ret;
}

int tcp_vs_denypage_remove(struct aos_deny_page *page)
{
	struct list_head *e;
	struct tcp_vs_service *svc;

	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(e, &tcp_vs_svc_list) 
	{
		svc = list_entry(e, struct tcp_vs_service, list);
		if (svc->deny_page == page)
			svc->deny_page = NULL;
	}
	write_unlock_bh(&__tcp_vs_svc_lock);
	return eAosRc_Success;
}

int tcp_vs_denypage_assigned_show(char* buff, 
			unsigned int *rsltIndex, 
			int bufflen, 
			struct aos_deny_page *page)
{
	struct tcp_vs_service *svc;
	struct list_head *e;
	char tmpbuf[256];
	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(e, &tcp_vs_svc_list) 
	{
		svc = list_entry(e, struct tcp_vs_service, list);
		if (svc->deny_page != page)
			continue;
		
		snprintf(tmpbuf, 256,"\tassigned to \"%s\"\n", svc->ident.name);
		aosCheckAndCopy(buff, rsltIndex, bufflen, tmpbuf, strlen(tmpbuf));
	}
	write_unlock_bh(&__tcp_vs_svc_lock);
	return eAosRc_Success;
}


int tcp_vs_denypage_save_config(char* buff, 
				unsigned int *rsltIndex, 
				int bufflen, 
				struct aos_deny_page *page)
{
	struct tcp_vs_service *svc;
	struct list_head *e;
	char tmpbuf[256];
	
	write_lock_bh(&__tcp_vs_svc_lock);
	list_for_each(e, &tcp_vs_svc_list) 
	{
		svc = list_entry(e, struct tcp_vs_service, list);
		if (svc->deny_page != page)
			continue;
		
		snprintf(tmpbuf, 256, "<Cmd>deny page assign %s %s</Cmd>\n", svc->ident.name, page->page_name);
		aosCheckAndCopy(buff, rsltIndex, bufflen, tmpbuf, strlen(tmpbuf));
	}
	write_unlock_bh(&__tcp_vs_svc_lock);
	return eAosRc_Success;
}


