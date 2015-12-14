////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Servers_AppServer_h
#define Servers_AppServer_h

#include "aosUtil/List.h"
#include "KernelUtil/MgdObj.h"

#include <KernelSimu/types.h>
#include <KernelSimu/compiler.h>
#include <KernelSimu/atomic.h>


enum
{
	eAosAppServerNameLen = 20,
	eAosAppTypeLen = 10,
	eAosProtoNameLen = 10
};

struct aosAppServer
{
	aosMgdObjDecl;		// Must be the first one

	char				mName[eAosAppServerNameLen];
	char				mType[eAosAppTypeLen];
	u32					mLocalAddr;
	u16					mLocalPort;
	u32					mRemoteAddr;
	u16					mRemotePort;
	char				mProtocol[eAosProtoNameLen];
	u8					mStatus;
	atomic_t			mRefcnt;	

	// 
	// Statistics
	//
	u64					mConnAttempts;
	u64					mSuccessConns;
	u64					mNumBytes;
	u64					mNumPackets;
	u64					mNumRequests;
	u64					mNumAcceptedReqs;
};


struct aosKernelApiParms;
extern int aosAppServer_createCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAppServer_delCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAppServer_showCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAppServer_clearAllCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAppServer_setStatusCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAppServer_show(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosAppServer_list(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);

extern int aosAppServer_init(void);
static inline int aosAppServer_hold(struct aosAppServer *self)
{
	atomic_inc(&self->mRefcnt);
	return 0;
}

static inline int aosAppServer_put(struct aosAppServer *self)
{
	atomic_dec(&self->mRefcnt);
	return 0;
}

extern struct aosAppServer * aosAppServer_get(const char *name);
extern int aosAppServer_constructor(char *name, 
							char *type, 
							u32 localaddr, 
							u32 remoteaddr, 
							u16 remoteport,
							char *protocol, 
							struct aosAppServer **server);
extern int aosAppServer_destructor(struct aosAppServer *self);
extern int aosAppServer_serverOn(struct aosAppServer *self);
extern int aosAppServer_serverOff(struct aosAppServer *self);
extern int aosAppServer_getNumServers(void);

#endif

