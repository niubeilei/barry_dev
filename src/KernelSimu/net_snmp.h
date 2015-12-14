////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: net_snmp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_net_snmp_h
#define Omn_aos_KernelSimu_net_snmp_h

#ifdef AOS_KERNEL_SIMULATE

struct snmp_mib {
	char *name;
	int entry;
};

#define SNMP_MIB_ITEM(_name,_entry)	{	\
	.name = _name,				\
	.entry = _entry,			\
}

#define SNMP_MIB_SENTINEL {	\
	.name = NULL,		\
	.entry = 0,		\
}

/*
 * We use all unsigned longs. Linux will soon be so reliable that even 
 * these will rapidly get too small 8-). Seriously consider the IpInReceives 
 * count on the 20Gb/s + networks people expect in a few years time!
 */

/* 
 * The rule for padding: 
 * Best is power of two because then the right structure can be found by a 
 * simple shift. The structure should be always cache line aligned.
 * gcc needs n=alignto(cachelinesize, popcnt(sizeof(bla_mib))) shift/add 
 * instructions to emulate multiply in case it is not power-of-two. 
 * Currently n is always <=3 for all sizes so simple cache line alignment 
 * is enough. 
 * 
 * The best solution would be a global CPU local area , especially on 64 
 * and 128byte cacheline machine it makes a *lot* of sense -AK
 */ 

#define __SNMP_MIB_ALIGN__	____cacheline_aligned

/* IPstats */
#define IPSTATS_MIB_MAX	__IPSTATS_MIB_MAX
struct ipstats_mib {
	unsigned long	mibs[IPSTATS_MIB_MAX];
} __SNMP_MIB_ALIGN__;

/* ICMP */
#define ICMP_MIB_DUMMY	__ICMP_MIB_MAX
#define ICMP_MIB_MAX	(__ICMP_MIB_MAX + 1)

struct icmp_mib {
	unsigned long	mibs[ICMP_MIB_MAX];
} __SNMP_MIB_ALIGN__;

/* ICMP6 (IPv6-ICMP) */
#define ICMP6_MIB_MAX	__ICMP6_MIB_MAX
struct icmpv6_mib {
	unsigned long	mibs[ICMP6_MIB_MAX];
} __SNMP_MIB_ALIGN__;

/* TCP */
#define TCP_MIB_MAX	__TCP_MIB_MAX
struct tcp_mib {
	unsigned long	mibs[TCP_MIB_MAX];
} __SNMP_MIB_ALIGN__;

/* UDP */
#define UDP_MIB_MAX	__UDP_MIB_MAX
struct udp_mib {
	unsigned long	mibs[UDP_MIB_MAX];
} __SNMP_MIB_ALIGN__;

/* SCTP */
#define SCTP_MIB_MAX	__SCTP_MIB_MAX
struct sctp_mib {
	unsigned long	mibs[SCTP_MIB_MAX];
} __SNMP_MIB_ALIGN__;

/* Linux */
#define LINUX_MIB_MAX	__LINUX_MIB_MAX
struct linux_mib {
	unsigned long	mibs[LINUX_MIB_MAX];
};


/* 
 * FIXME: On x86 and some other CPUs the split into user and softirq parts
 * is not needed because addl $1,memory is atomic against interrupts (but 
 * atomic_inc would be overkill because of the lock cycles). Wants new 
 * nonlocked_atomic_inc() primitives -AK
 */ 
#define DEFINE_SNMP_STAT(type, name)	\
	__typeof__(type) *name[2]
#define DECLARE_SNMP_STAT(type, name)	\
	extern __typeof__(type) *name[2]

#define SNMP_STAT_BHPTR(name)	(name[0])
#define SNMP_STAT_USRPTR(name)	(name[1])

#define SNMP_INC_STATS_BH(mib, field) 	\
	(per_cpu_ptr(mib[0], _smp_processor_id())->mibs[field]++)
#define SNMP_INC_STATS_OFFSET_BH(mib, field, offset)	\
	(per_cpu_ptr(mib[0], _smp_processor_id())->mibs[field + (offset)]++)
#define SNMP_INC_STATS_USER(mib, field) \
	(per_cpu_ptr(mib[1], _smp_processor_id())->mibs[field]++)
#define SNMP_INC_STATS(mib, field) 	\
	(per_cpu_ptr(mib[!in_softirq()], _smp_processor_id())->mibs[field]++)
#define SNMP_DEC_STATS(mib, field) 	\
	(per_cpu_ptr(mib[!in_softirq()], _smp_processor_id())->mibs[field]--)
#define SNMP_ADD_STATS_BH(mib, field, addend) 	\
	(per_cpu_ptr(mib[0], _smp_processor_id())->mibs[field] += addend)
#define SNMP_ADD_STATS_USER(mib, field, addend) 	\
	(per_cpu_ptr(mib[1], _smp_processor_id())->mibs[field] += addend)


#else
#include <net/snmp.h>
#endif

#endif

