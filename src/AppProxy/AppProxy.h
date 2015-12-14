////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AppProxy_AppProxy_h
#define AppProxy_AppProxy_h

#include "aosUtil/List.h"
#include "KernelUtil/MgdObj.h"

#include <KernelSimu/types.h>
#include <KernelSimu/spinlock.h>
#include <KernelSimu/compiler.h>
#include <KernelSimu/atomic.h>

struct aosServerGroup;
struct sock;
struct aosSockBridge;
struct aosAppServer;
struct aosMgdObj;
struct aosTcpSockBridge;

struct aosAaaServerEntry
{
	struct aos_list_head	*next;
	struct aos_list_head	*prev;
	struct aosMgdObj 		*prevobj;
	struct aosMgdObj		*nextobj;
	u16						 objtype;

	u16						priority;
	struct aosAaaServer 	*server;
};

enum
{
	eAosMaxAppNameLen = 30,
	eAosMaxDnLen = 30,
	eAosAppProxyDftBacklog = 10,

	eAosAppProxy_SSL = 1,
	eAosAppProxy_IPSec = 2,
	eAosAppProxy_Plain = 3
};

struct aosAppProxy
{
	// 
	// aosAppProxy is an aosMgdObj
	//
	aosMgdObjDecl;

	char					name[eAosMaxAppNameLen];
	char					dn[eAosMaxDnLen];
	u32						addr;
	u16						port;
	int						backlog;
	u8						protocol;
	u8						security;
	u8						status;
	u8						clientAuthFlag;
	u8						ocspFlag;
	int						maxConns;
	int						maxDuration;
	struct aosAaaServer		*ca;
	struct aos_list_head	aaaServers;
	struct aosServerGroup 	*group;
	struct socket			*sock;
	atomic_t				refcnt;

	int (*createSockBridge)(struct aosSockBridge **bridge, struct aosAppProxy *self);
	int (*getNextServer)(struct aosAppProxy *proxy, struct aosAppServer **server);

	// 
	// Statistics
	//
	u64					connAttempts;
	u64					successConns;
	u64					numBytes;
	u64					numPackets;
	u64					numRequests;
	u64					numAcceptedReqs;
};


struct aosKernelApiParms;
extern int aosAppProxy_saveConfigCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_clearConfigCli(char *data, 
			   	unsigned int *length, 
			   	struct aosKernelApiParms *parms,
			   	char *errmsg, 
			   	const int errlen);
extern int aosAppProxy_addCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_delCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_showCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_clearAllCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_setStatusCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_aaaAddCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_aaaDelCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_aaaClearCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_setGroupCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_resetGroupCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_showConnCli(char *data, 
 				unsigned int *length, 
 				struct aosKernelApiParms *parms, 
 				char *errmsg, 
 				const int errlen);
extern int aosAppProxy_delConnCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);
extern int aosAppProxy_setMaxDurationCli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);

extern int aosAppProxy_init(void);
extern struct aosAppProxy * aosAppProxy_get(const char *name);
extern int aosAppProxy_constructor(char *name, 
							char *dn, 
							u32 addr, 
							u16 port,
							u8  protocol, 
							u8  security, 
							struct aosAppProxy **proxy);
extern int aosAppProxy_show(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *, 
				const int);
extern int aosAppProxy_list(char *data, unsigned int *length, char *, const int);
extern int aosAppProxy_getNames(u8 prot, char *protname, u8 sec, char *secname);
extern int aosAppProxy_appProxyOn(struct aosAppProxy *self);
extern int aosAppProxy_appProxyOff(struct aosAppProxy *self);
extern void aosAppProxy_resetStat(struct aosAppProxy *self);
extern int aosAppProxy_clearAaaServers(struct aosAppProxy *self);
extern int aosAppProxy_getAaaServerCount(struct aosAaaServer *server);
extern int aosAppProxy_getNumAppProxies(void);
extern int aosAppProxy_listAllConns(char *data, unsigned int *length);
extern int aosAppProxy_listProxyConns(char *data, unsigned int *length);
extern int aosAppProxy_listSenderConns(char *data, unsigned int *length);
extern int aosAppProxy_delAllConns(char *data, unsigned int *length);
extern int aosAppProxy_delProxyConns(char *data, unsigned int *length);
extern int aosAppProxy_delSenderConns(char *data, unsigned int *length);
extern int aosAppProxy_getNextServer(struct aosAppProxy *proxy, struct aosAppServer **server);
extern int aosAppProxy_printOneBridge(struct aosTcpSockBridge *bridge, 
					char *rsltBuff, 
					unsigned int *rsltIndex, 
					unsigned int optlen);

extern int aosAppProxy_setPMICli(char *data,
				unsigned int *length, 
				struct aosKernelApiParms *parms, 
				char *errmsg, 
				const int errlen);

int aosAppProxy_sslClientAuthCli(
        char *data,
        unsigned int *length,
        struct aosKernelApiParms *parms,
        char *errmsg,
        const int errlen);

int aosAppProxy_sslFrontCli(
        char *data,
        unsigned int *length,
        struct aosKernelApiParms *parms,
        char *errmsg,
        const int errlen);

int aosAppProxy_sslBackendCli(
        char *data,
        unsigned int *length,
        struct aosKernelApiParms *parms,
        char *errmsg,
        const int errlen);


static inline int aosAppProxy_hold(struct aosAppProxy *self)
{
	atomic_inc(&self->refcnt);
	return 0;
}


static inline int aosAppProxy_put(struct aosAppProxy *self)
{
	atomic_dec(&self->refcnt);
	return 0;
}



#endif

