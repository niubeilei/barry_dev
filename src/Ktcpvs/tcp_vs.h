////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/*
 * KTCPVS  -    Kernel TCP Virtual Server
 *
 * Copyright (C) 2001, Wensong Zhang <wensong@gnuchina.org>
 *
 * tcp_vs.h: main structure definitions and function prototypes
 *
 * $Id: tcp_vs.h,v 1.2 2015/01/06 08:57:50 andy Exp $
 *
 */

#ifndef _TCP_VS_H
#define _TCP_VS_H

#include "aosUtil/Types.h"		// For __uXX types
#include "aosUtil/Memory.h"

#include "Ktcpvs/regex.h"
#include "Ktcpvs/tcp_vs_def.h"
#include "Ktcpvs/interface.h"

#define TCP_VS_VERSION_CODE		0x000012
#define NVERSION(version)                       \
	(version >> 16) & 0xFF,                 \
	(version >> 8) & 0xFF,                  \
	version & 0xFF

// Moved to tcp_vs_def.h
// #define KTCPVS_IDENTNAME_MAXLEN		16
// #define KTCPVS_SCHEDNAME_MAXLEN		16
#define KTCPVS_PATTERN_MAXLEN           256

/*
 *      KTCPVS socket options
 */
#define TCP_VS_BASE_CTL		(64+1024+64+64)	/* base */

#define TCP_VS_SO_SET_NONE	TCP_VS_BASE_CTL	/* just peek */
#define TCP_VS_SO_SET_ADD	(TCP_VS_BASE_CTL+1)
#define TCP_VS_SO_SET_EDIT	(TCP_VS_BASE_CTL+2)
#define TCP_VS_SO_SET_DEL	(TCP_VS_BASE_CTL+3)
#define TCP_VS_SO_SET_FLUSH	(TCP_VS_BASE_CTL+4)
#define TCP_VS_SO_SET_LIST	(TCP_VS_BASE_CTL+5)
#define TCP_VS_SO_SET_ADDDEST	(TCP_VS_BASE_CTL+6)
#define TCP_VS_SO_SET_DELDEST	(TCP_VS_BASE_CTL+7)
#define TCP_VS_SO_SET_EDITDEST	(TCP_VS_BASE_CTL+8)
#define TCP_VS_SO_SET_ADDRULE	(TCP_VS_BASE_CTL+9)
#define TCP_VS_SO_SET_DELRULE	(TCP_VS_BASE_CTL+10)
#define TCP_VS_SO_SET_START	(TCP_VS_BASE_CTL+11)
#define TCP_VS_SO_SET_STOP	(TCP_VS_BASE_CTL+12)
#define TCP_VS_SO_SET_MAX	TCP_VS_SO_SET_STOP

#define TCP_VS_SO_GET_VERSION	TCP_VS_BASE_CTL
#define TCP_VS_SO_GET_INFO	(TCP_VS_BASE_CTL+1)
#define TCP_VS_SO_GET_SERVICES	(TCP_VS_BASE_CTL+2)
#define TCP_VS_SO_GET_SERVICE	(TCP_VS_BASE_CTL+3)
#define TCP_VS_SO_GET_DESTS	(TCP_VS_BASE_CTL+4)
#define TCP_VS_SO_GET_DEST	(TCP_VS_BASE_CTL+5)	/* not used now */
#define TCP_VS_SO_GET_RULES	(TCP_VS_BASE_CTL+6)
#define TCP_VS_SO_GET_MAX	TCP_VS_SO_GET_RULES


#define TCP_VS_TEMPLATE_TIMEOUT 15*HZ


// 
// This structure definition is moved to "tcp_vs_def.h"
//
// struct tcp_vs_ident {
// 	char name[KTCPVS_IDENTNAME_MAXLEN];
// };
//
//struct tcp_vs_config {
//	/* the IP address and/or port to which the server listens */
//	__u32 		addr;
//	__u16 		port;
//
//	/* scheduler name */
//	char sched_name[KTCPVS_SCHEDNAME_MAXLEN];
//
//	unsigned 	timeout;	// timeout in ticks 
//	int 		startservers;
//	int 		maxSpareServers;
//	int 		minSpareServers;
//
//	// the max number of servers running
//	int 		maxClients;
//	int 		keepAlive;
//	int 		maxKeepAliveRequests;
//	int 		keepAliveTimeout;
//
//	// address/port to redirect
//	__u32 		redirect_addr;
//	__u16 		redirect_port;
//
//	// 
//	// Chen Ding, 07/13/2005
//	//
//	__u8 		front_ssl;	// Client to virtual service is SSL
//	__u8 		back_ssl;	// Virtual service to bacnend server is SSL
//	__u16		app_type;	// Application type
//	__u8 		pmi;		// Turn on or trun off PMI access control switch 
//	__u8		dynamic_dst;// if dynamic_dst=1, dst is decided by the first page, 
//							// if 0, dst is selected from static dst list
//};


struct tcp_vs_dest_u {
	__u32 addr;		/* IP address of real server */
	__u16 port;		/* port number of the service */
	int weight;		/* server weight */
	__u32 conns;		/* active connections */
};


struct tcp_vs_rule_u {
	/* rule pattern */
	int type;
	char pattern[KTCPVS_PATTERN_MAXLEN];
	size_t len;

	/* destination server */
	__u32 addr;
	__u16 port;

	/* special entry for hhttp module */
	int match_num;
};


/* The argument to TCP_VS_SO_GET_INFO */
struct tcp_vs_getinfo {
	/* version number */
	unsigned int version;

	/* number of virtual services */
	unsigned int num_services;
};

/* The argument to TCP_VS_SO_GET_SERVICE */
struct tcp_vs_service_u {
	/* server ident */
	struct tcp_vs_ident ident;

	/* server configuration */
	struct tcp_vs_config conf;

	/* number of real servers */
	unsigned int num_dests;

	/* number of rules */
	unsigned int num_rules;

	/* run-time variables */
	unsigned int conns;	/* connection counter */
	unsigned int running;	/* running flag */
};

/* The argument to TCP_VS_SO_GET_SERVICES */
struct tcp_vs_get_services {
	/* number of virtual services */
	unsigned int num_services;

	/* service table */
	struct tcp_vs_service_u entrytable[0];
};

/* The argument to TCP_VS_SO_GET_DESTS */
struct tcp_vs_get_dests {
	/* server ident */
	struct tcp_vs_ident ident;

	/* number of real servers */
	unsigned int num_dests;

	/* real server table */
	struct tcp_vs_dest_u entrytable[0];
};

/* The argument to TCP_VS_SO_GET_RULES */
struct tcp_vs_get_rules {
	/* server ident */
	struct tcp_vs_ident ident;

	/* number of real servers */
	unsigned int num_rules;

	/* real server table */
	struct tcp_vs_rule_u entrytable[0];
};


#ifdef __KERNEL__

#include <linux/list.h>		/* for list_head */
#include <linux/spinlock.h>	/* for rwlock_t */
#include <asm/atomic.h>		/* for atomic_t */
#include <linux/sysctl.h>	/* for ctl_table */
#include <linux/slab.h>		/* for kmalloc */



#ifdef CONFIG_TCP_VS_DEBUG
extern int tcp_vs_get_debug_level(void);
#define TCP_VS_DBG(level, msg...)			\
    do {						\
	    if (level <= tcp_vs_get_debug_level())	\
		    printk(KERN_DEBUG "TCPVS: " msg);	\
    } while (0)
#else				/* NO DEBUGGING at ALL */
#define TCP_VS_DBG(level, msg...)  do {} while (0)
#endif

#define TCP_VS_ERR(msg...) printk(KERN_ERR "TCPVS: " msg)
#define TCP_VS_INFO(msg...) printk(KERN_INFO "TCPVS: " msg)
#define TCP_VS_WARNING(msg...) \
	printk(KERN_WARNING "TCPVS: " msg)
#define TCP_VS_ERR_RL(msg...)				\
    do {						\
	    if (net_ratelimit())			\
		    printk(KERN_ERR "TCPVS: " msg);	\
    } while (0)

#ifdef CONFIG_TCP_VS_DEBUG
#define EnterFunction(level)						\
    do {								\
	    if (level <= tcp_vs_get_debug_level())			\
		    printk(KERN_DEBUG "Enter: %s, %s line %i\n",	\
			   __FUNCTION__, __FILE__, __LINE__);		\
    } while (0)
#define LeaveFunction(level)						\
    do {								\
	    if (level <= tcp_vs_get_debug_level())			\
			printk(KERN_DEBUG "Leave: %s, %s line %i\n",	\
			       __FUNCTION__, __FILE__, __LINE__);	\
    } while (0)
#else
#define EnterFunction(level)   do {} while (0)
#define LeaveFunction(level)   do {} while (0)
#endif

/* switch off assertions (if not already off) */
#ifdef CONFIG_TCP_VS_DEBUG
#define assert(expr)						\
	if(!(expr)) {						\
		printk( "Assertion failed! %s,%s,%s,line=%d\n",	\
		#expr,__FILE__,__FUNCTION__,__LINE__);		\
	}
#else
#define assert(expr) do {} while (0)
#endif


#define KTCPVS_CHILD_HARD_LIMIT		(512)

#define NET_KTCPVS			20

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

enum {
	NET_KTCPVS_DEBUGLEVEL = 1,
	NET_KTCPVS_UNLOAD = 2,
	NET_KTCPVS_MAXBACKLOG = 3,
	NET_KTCPVS_ZEROCOPY_SEND = 4,
	NET_KTCPVS_KEEPALIVE_TIMEOUT = 5,
	NET_KTCPVS_READ_TIMEOUT = 6,
};


/*
 *	Slow timer for KTCPVS connections
 */
typedef struct slowtimer_struct {
	struct list_head list;
	unsigned long expires;
	unsigned long data;
	void (*function) (unsigned long);
} slowtimer_t;


struct tcp_vs_rule {
	struct list_head list;

	int type;
	char *pattern;
	size_t len;
	regex_t rx;

	struct list_head destinations;

	/* special field for hhttp module */
	int match_num;
};


/*
 *	The information about the KTCPVS service
 */
// struct tcp_vs_service 
// {
// 	struct list_head 		list;			// Virtual service list
// 	struct tcp_vs_ident 	ident;			// Service ID
// 	struct tcp_vs_config 	conf;			// Service Configuration
// 	struct tcp_vs_scheduler *scheduler;		// bound scheduler object 
// 	void 					*sched_data;	// scheduler application data 
// 	struct list_head 		destinations;	// Dest list	
// 	__u32 					num_dests;
// 	struct list_head 		rule_list;		// Rule list
// 	__u32 					num_rules;
// 	rwlock_t 				lock;			// Lock for dest list and rule list
// 	int 					start;			// Service execution control	
// 	int 					stop;			// Service execution control
// 	struct socket 			*mainsock;		// The service socket
// 	atomic_t 				conns;			// connection counter 
// 	atomic_t 				childcount;		// child counter
// 	atomic_t 				running;		// running flag 
// 
// 	// Chen Ding, 08/26/2005
// 	char *					deny_page;
// 
// 	// Chen Ding, 09/04/2005
// 	int						auto_login;
// };


/*
 *	The real server destination forwarding entry
 *	with ip address, port, weight ...
 */
typedef struct tcp_vs_dest {
	struct list_head n_list;// for dest list in its server
	struct list_head r_list;// for dest list in rule
	atomic_t refcnt;		// reference counter

	__u32 addr;				// IP address of real server
	__u16 port;				// port number of the service
	int weight;				// server weight
	unsigned flags;			// dest status flags
	atomic_t conns;			// active connections
} tcp_vs_dest_t;


typedef struct server_conn_struct {
	/* hash keys and list for collision resolution */
	__u32 addr;		/* IP address of the server */
	__u16 port;		/* port number of the server */
	struct list_head list;	/* d-linked list head for hashing */

	/* status flags */
	__u16 flags;

	/* socket connected to a destination server */
	struct socket *sock;
	struct tcp_vs_dest *dest;

	/* timer for keepalive connections */
	slowtimer_t keepalive_timer;
	unsigned long timeout;
	unsigned int nr_keepalives;
} server_conn_t;




/*
 *	The scheduler object
 */
struct tcp_vs_scheduler 
{
	struct list_head 	n_list;		// Scheduer List
	char 				*name;		// scheduler name 
	struct module 		*module;	// THIS_MODULE/NULL 

	// Member Functions

	//
	// Initializing the scheduling elements in the service 
	//
	int (*init_service) (struct tcp_vs_service * svc);

	//
	// Releasing the scheduling elements in the service
	//
	int (*done_service) (struct tcp_vs_service * svc);

	//
	// Updating the scheduling elements in the service 
	//
	int (*update_service) (struct tcp_vs_service * svc);

	//
	// select a server and connect to it
	//
	int (*schedule) (struct tcp_vs_conn * conn,
			 		 struct tcp_vs_service * svc,
			 		 char *req_client,
			 		 int req_len);

	// 
	// Chen Ding, 07/17/2005
	// If set, process all the packets from server to client
	//
	/*
	int (*server_to_client)(struct tcp_vs_service *svc, 
							struct tcp_vs_conn *conn,
							struct socket *fromSock,
							struct socket *toSock);
	*/

	int (*server_to_client)(unsigned char *app_data, 
						  	unsigned int * app_data_len, 
						  	struct tcp_vs_conn *conn,
		                  	int (*callback)(struct aos_app_proc_req *req));


	int (*client_to_server)(unsigned char *app_data, 
						  	unsigned int * app_data_len, 
						  	struct tcp_vs_conn *conn,
		                  	int (*callback)(struct aos_app_proc_req *req));

	// whether app module able to process application data now
	int (*checking_permit)(struct tcp_vs_conn *conn);

	// release the application processing request
	int (*app_req_release)(struct aos_app_proc_req *req);
	int (*app_data_release)(struct tcp_vs_conn *conn);	// release application private data
};


/*
 *	TCPVS service child
 */
struct tcp_vs_child {
	struct tcp_vs_service 	*svc;		// service it belongs to
	int 					pid;		// pid of child
	volatile __u16 			status;		// child status
};


/* from misc.c */
extern int StartListening(struct tcp_vs_service *svc);
extern void StopListening(struct tcp_vs_service *svc);
extern struct socket *tcp_vs_connect2dest(tcp_vs_dest_t * dest);
extern int tcp_vs_sendbuffer(struct socket *sock, const char *buffer,
			     const size_t length, unsigned long flags);
extern int tcp_vs_recvbuffer(struct socket *sock, char *buffer,
			     const size_t buflen, unsigned long flags);
extern int tcp_vs_xmit(struct socket *sock, const char *buffer,
		       const size_t length, unsigned long flags);
extern tcp_vs_dest_t* tcp_vs_finddst(struct tcp_vs_service *svc);


#ifndef strdup
static __inline__ char *strdup(char *str)
{
	char *s;
	int n;

	if (str == NULL)
		return NULL;

	n = strlen(str) + 1;
	//s = kmalloc(n, GFP_ATOMIC);
	s = aos_malloc(n);
	if (!s)
		return NULL;
	return strcpy(s, str);
}
#endif

extern char *tcp_vs_getline(char *s, char *token, int n);
extern char *tcp_vs_getword(char *s, char *token, int n);

/* from tcp_vs_ctl.c */
extern struct list_head tcp_vs_svc_list;
extern rwlock_t __tcp_vs_svc_lock;
extern int sysctl_ktcpvs_unload;
extern int sysctl_ktcpvs_max_backlog;
extern int sysctl_ktcpvs_zerocopy_send;
extern int sysctl_ktcpvs_keepalive_timeout;
extern int sysctl_ktcpvs_read_timeout;

extern int tcp_vs_flush(void);
extern int tcp_vs_control_start(void);
extern void tcp_vs_control_stop(void);

/* from tcp_vs_sched.c */
extern int register_tcp_vs_scheduler(struct tcp_vs_scheduler *scheduler);
extern int unregister_tcp_vs_scheduler(struct tcp_vs_scheduler *scheduler);
extern int tcp_vs_bind_scheduler(struct tcp_vs_service *svc,
				 struct tcp_vs_scheduler *scheduler);
extern int tcp_vs_unbind_scheduler(struct tcp_vs_service *svc);
extern struct tcp_vs_scheduler *tcp_vs_scheduler_get(const char *name);
extern void tcp_vs_scheduler_put(struct tcp_vs_scheduler *sched);
int tcp_vs_conn_handle(struct tcp_vs_conn *conn, struct tcp_vs_service *svc);

/* from redirect.c */
extern int redirect_to_local(struct tcp_vs_conn *conn, __u32 addr,
			     __u16 port);

/* from tcp_vs_srvconn.c */
extern server_conn_t *tcp_vs_srvconn_get(__u32 addr, __u16 port);
extern void tcp_vs_srvconn_put(server_conn_t * sc);
extern server_conn_t *tcp_vs_srvconn_new(tcp_vs_dest_t * dest);
extern void tcp_vs_srvconn_free(server_conn_t * sc);
extern int tcp_vs_srvconn_init(void);
extern void tcp_vs_srvconn_cleanup(void);

/* from tcp_vs_timer.c */
void assert_slowtimer(int pos);
extern void tcp_vs_add_slowtimer(slowtimer_t * timer);
extern int tcp_vs_del_slowtimer(slowtimer_t * timer);
extern void tcp_vs_mod_slowtimer(slowtimer_t * timer, unsigned long expires);
extern void tcp_vs_slowtimer_init(void);
extern void tcp_vs_slowtimer_cleanup(void);
extern void tcp_vs_slowtimer_collect(void);
int tcp_vs_authentication( struct tcp_vs_conn *conn);
extern struct tcp_vs_conn *tcp_vs_conn_lookup_byid(const __u64 session_id);
extern int session_time_control_init(struct tcp_vs_conn *conn);
extern void session_time_control_destroy(struct tcp_vs_conn *conn);

static inline void
init_slowtimer(slowtimer_t * timer)
{
	timer->list.next = timer->list.prev = NULL;
}

static inline int
slowtimer_pending(const slowtimer_t * timer)
{
	return timer->list.next != NULL;
}

#endif				/* __KERNEL__ */

#endif				/* _TCP_VS_H */
