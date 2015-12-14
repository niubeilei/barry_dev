////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AaaServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef AAA_AaaServer_h
#define AAA_AaaServer_h

#include "aosUtil/List.h"

#include <KernelSimu/types.h>
#include <KernelSimu/atomic.h>

#include "KernelUtil/MgdObj.h"


enum
{
	eAosAaaServerNameLen = 20,
	eAosAaaServerTypeLen = 10,
	eAosAaaProtNameLen = 10,
	eAosMaxAaaServerListLen = 128
};

struct aosAaaServer
{
	// 
	// aosMgdObj
	//
	aosMgdObjDecl;

	struct aos_list_head	*list;

	char				name[eAosAaaServerNameLen];
	char				type[eAosAaaServerTypeLen];
	u32					addr;
	u16					port;
	char				hc[eAosAaaProtNameLen];
	u8					status;
	atomic_t			refcnt;

	// 
	// Statistics
	//
	u64					connAttempts;
	u64					successConns;
	u64					numRequests;
	u64					numAcceptedReqs;
};


struct aosKernelApiParms;
extern int aosAaaServer_addCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_delCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_showCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_clearAllCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_setStatusCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_resetStatCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_clearStatCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_show(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAaaServer_list(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);

extern int aosAaaServer_init(void);
extern struct aosAaaServer * aosAaaServer_get(const char *name);
extern int aosAaaServer_constructor(char *name, 
							char *type, 
							u32 addr, 
							u16 port,
							char *hc, 
							struct aosAaaServer **server);
extern int aosAaaServer_destructor(struct aosAaaServer *self);
extern int aosAaaServer_serverOn(struct aosAaaServer *self);
extern int aosAaaServer_serverOff(struct aosAaaServer *self);
extern int aosAaaServer_resetStat(struct aosAaaServer *self);
extern void aosAaaServer_hold(struct aosAaaServer *self);
extern void aosAaaServer_put(struct aosAaaServer *self);
extern int  aosAaaServer_integrityCheck(void);
extern int  aosAaaServer_dump(void);
extern int  aosAaaServer_integrityCheckOnServer(struct aosAaaServer *server);
extern int  aosAaaServer_getNumServers(void);

#endif

