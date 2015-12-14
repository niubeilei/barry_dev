////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: netfilter.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_netfilter_h
#define Omn_aos_KernelSimu_netfilter_h

#ifdef AOS_KERNEL_SIMULATE

#include "KernelSimu/list.h"

struct net_device;
struct sk_buff;
struct nf_sockopt_ops;
struct nf_info;


extern int nf_hook_slow(int pf, unsigned int hook, struct sk_buff *skb,
		 struct net_device *indev,
		 struct net_device *outdev,
		 int (*okfn)(struct sk_buff *));

extern int nf_register_sockopt(struct nf_sockopt_ops *reg);
extern void nf_unregister_sockopt(struct nf_sockopt_ops *reg);


#define NF_HOOK nf_hook_slow

struct sock;

struct nf_sockopt_ops
{
	struct list_head list;

	int pf;

	/* Non-inclusive ranges: use 0/0/NULL to never get called. */
	int set_optmin;
	int set_optmax;
	int (*set)(struct sock *sk, int optval, void *user, unsigned int len);

	int get_optmin;
	int get_optmax;
	int (*get)(struct sock *sk, int optval, void *user, int *len);

	/* Number of users inside set() or get(). */
	unsigned int use;
	struct task_struct *cleanup_task;
};

struct sk_buff;
struct net_device;
struct module;

typedef unsigned int nf_hookfn(unsigned int hooknum,
    struct sk_buff **skb,
	const struct net_device *in,
	const struct net_device *out,
	int (*okfn)(struct sk_buff *));

struct nf_hook_ops
{
	struct list_head list;
	 
	nf_hookfn *hook;
	struct module *owner;
	int pf;
	int hooknum;
	int priority;
};

#define NF_DROP 0
#define NF_ACCEPT 1
#define NF_STOLEN 2
#define NF_QUEUE 3
#define NF_REPEAT 4
#define NF_STOP 5
#define NF_MAX_VERDICT NF_STOP

inline void nf_reinject(struct sk_buff *skb,
	struct nf_info *info,
	unsigned int verdict) {}

#define nf_register_hook(x) 
#define nf_unregister_hook(x) 

#else
#include <linux/netfilter.h>
#endif

#endif

