////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_PROXY_H
#define AOS_PROXY_H

#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/tcp.h>

#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"

#include "aosUtil/TimerPack.h"


#define proxy_alarm(fmt,x...) aos_alarm(eAosMD_TcpProxy, eAosAlarm_General, fmt, ##x) 
#define SERVICE_STATUS_OFF 0
#define SERVICE_STATUS_ON 1


#define MAX_WRAPPER 8
struct AosSockWrapperType;

typedef
struct AosSockWrapperObject
{
	struct ProxyObject	*po;
	struct sock			*sk;

	struct AosSockWrapperType	* wt;
}
AosSockWrapperObject_t;

#define WTNAMELEN 15
#define WT_SETNAME(p,s)	{ strncpy((p)->name,s,WTNAMELEN); (p)->name[WTNAMELEN]=0; }

typedef
struct AosSockWrapperType
{
	__u8		id;
	char	name[WTNAMELEN+1];

	struct module 	* owner;

	struct list_head	list;

	// must be implement 
	struct AosSockWrapperObject* (*create)(void);
	int (*release) ( AosSockWrapperObject_t * wo );

	int (*readyAcceptData)(AosSockWrapperObject_t * wo);
	int (*dataReceived) ( AosSockWrapperObject_t *wo, char * buffer, int len );
	int (*sendData) ( AosSockWrapperObject_t *wo, char * buffer, int len );
	int (*sendFin) ( AosSockWrapperObject_t *wo );
	int (*dataAckReceived) ( AosSockWrapperObject_t *wo );
	int (*peerWritable) ( AosSockWrapperObject_t *wo );

	// can be zero
	int (*activeConnected)(AosSockWrapperObject_t *sw);
	int (*passiveConnected)(AosSockWrapperObject_t *sw);
	int (*closed)(AosSockWrapperObject_t *sw);
	int (*finReceived)(AosSockWrapperObject_t *sw );
	int (*resetReceived)(AosSockWrapperObject_t *sw );
	int (*connectionTimeout)(AosSockWrapperObject_t *sw);

	int (*connect)(AosSockWrapperObject_t *sw, u32 addr, u16 port);
	int (*isWritable)(AosSockWrapperObject_t *sw);
}
AosSockWrapperType_t;

#define PO_RESET_CONNECTION(po) (po)->svc->app->resetConnection(po)
#define PO_CLIENTAUTH(po) ( (po)->svc->clientauth )

struct ProxyObject;
struct ProxyService;

#define PROXY_NAME_LEN 15
#define PROXY_SETNAME(p,s) { strncpy((p)->name,s,PROXY_NAME_LEN); (p)->name[PROXY_NAME_LEN]=0; }
typedef struct AppProxy
{
	struct list_head	list;
	struct module		*owner;

	char	name[PROXY_NAME_LEN+1];
	int		svcCount;
	int	(*backConnected) ( struct ProxyObject * po );

	// must be implemented
	int (*readyAcceptData) ( struct ProxyObject * po, struct AosSockWrapperObject * wo );
	int (*dataReceived) ( struct ProxyObject * po, struct sock * sk, char * buffer, int len );
	int (*dataAckReceived) ( struct ProxyObject * po, struct AosSockWrapperObject * wo );
	int (*passiveConnected) ( struct ProxyObject * po );
	int (*finReceived) ( struct ProxyObject * po, struct sock * sk );
	int (*resetConnection) ( struct ProxyObject * po );
	int (*poReleased) ( struct ProxyService * svc );
}
AppProxy_t;



#define PROXY_SERVICE_NAME_LEN 15

#define PROXY_TIMER_ACCURATE (60*HZ)
#define PROXY_TIMER_TIMEOUT (60*HZ*3) // 3 minutes

/*
typedef struct ProxyDest
{
	struct list_head list;
	__u32 ip;
	__u16 port;
}
ProxyDest_t;
*/

/*
 *	IPVS statistics object
 */
struct proxy_stats
{
	__u32                   conns;          /* connections scheduled */
	__u32                   inpkts;         /* incoming packets */
	__u32                   outpkts;        /* outgoing packets */
	__u64                   inbytes;        /* incoming bytes */
	__u64                   outbytes;       /* outgoing bytes */

	__u32			cps;		/* current connection rate */
	__u32			inpps;		/* current in packet rate */
	__u32			outpps;		/* current out packet rate */
	__u32			inbps;		/* current in byte rate */
	__u32			outbps;		/* current out byte rate */

	spinlock_t              lock;           /* spin lock */
};

/*
 *	The real server destination forwarding entry
 *	with ip address, port number, and so on.
 */
struct proxy_dest {
//	struct list_head	n_list;   /* for the dests in the service */
//	struct list_head	d_list;   /* for table with all the dests */

	__u32			addr;		/* IP address of the server */
	__u16			port;		/* port number of the server */
	volatile unsigned	flags;		/* dest status flags */
	atomic_t		conn_flags;	/* flags to copy to conn */
	atomic_t		weight;		/* server weight */

	atomic_t		refcnt;		/* reference counter */
	struct proxy_stats      stats;          /* statistics */

	/* connection counters and thresholds */
	atomic_t		activeconns;	/* active connections */
	atomic_t		inactconns;	/* inactive connections */
	atomic_t		persistconns;	/* persistent connections */
	__u32			u_threshold;	/* upper threshold */
	__u32			l_threshold;	/* lower threshold */

	/* for destination cache */
	spinlock_t		dst_lock;	/* lock of dst_cache */
	struct dst_entry	*dst_cache;	/* destination cache entry */
	u32			dst_rtos;	/* RT_TOS(tos) for dst */

	/* for virtual service */
	struct proxy_service	*svc;		/* service it belongs to */
	__u16			protocol;	/* which protocol (TCP/UDP) */
	__u32			vaddr;		/* virtual IP address */
	__u16			vport;		/* virtual port number */
	__u32			vfwmark;	/* firewall mark of service */
};


typedef struct proxy_dest ProxyDest_t;

/*
 *	The scheduler object
 */
struct proxy_scheduler {
	struct list_head	n_list;		/* d-linked list head */
	char			*name;		/* scheduler name */
	atomic_t		refcnt;		/* reference counter */
	struct module		*module;	/* THIS_MODULE/NULL */

	/* scheduler initializing service */
	int (*init_service)(struct ProxyService *svc);
	/* scheduling service finish */
	int (*done_service)(struct ProxyService *svc);
	/* scheduler updating service */
	int (*update_service)(struct ProxyService *svc);

	/* selecting a server from the given service */
	struct proxy_dest* (*schedule)(struct ProxyService *svc, struct sock * sk);
};


#define proxy_DEST_F_AVAILABLE	0x0001		/* server is available */
#define proxy_DEST_F_OVERLOAD	0x0002		/* server is overloaded */

#define MAX_REAL_SERVER 1024

typedef struct ProxyService
{
	struct list_head 	list; // svc list
	struct list_head 	poList; // head of the po list
	
	AosSockWrapperType_t	* frontWt;
	AosSockWrapperType_t	* backWt;

	AppProxy_t 				* app;
	struct socket 			* listenSock;

	u8		status;
	u8		clientauth;
	atomic_t	poCount;
	
	AosTimerPack_t	pack;

	// configure info
	char name[PROXY_SERVICE_NAME_LEN+1];
	__u32 listenIp;
	__u16 listenPort;

//	__u32 backIp;
//	__u16 backPort;

	//struct list_head destinations;
	struct proxy_dest * destinations[MAX_REAL_SERVER];
	int	destCount;

	void * sched_data;
	struct proxy_scheduler * scheduler;

}
ProxyService_t;


typedef struct ProxyObject
{
	struct list_head	list;
	ProxyService_t		*svc;

	struct sock 		*frontSk;
	struct sock 		*backSk;

	struct AosSockWrapperObject	* backWo;
	struct AosSockWrapperObject	* frontWo;

	AosPackTimer_t		timer;

	unsigned long		lut;
	struct proxy_dest	*dest;

}
ProxyObject_t;


#endif

