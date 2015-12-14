////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServerGroup.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Servers_aosServerGroup_h
#define Servers_aosServerGroup_h

#include "aosUtil/List.h"

#include "aosUtil/Types.h"
#include <KernelSimu/compiler.h>
#include <KernelSimu/atomic.h>

#include "KernelUtil/MgdObj.h"


enum
{
	eAosServerGroupNameLen = 20,
	eAosServerGroupMaxPriority = 1000,

	eAosServerGroupMethod_RR = 0
};

struct aosServerGroupMember
{
	aosMgdObjDecl;

	struct aosAppServer	*server;
	int					 priority;
};

struct aosServerGroup
{
	aosMgdObjDecl;

	char				mName[eAosServerGroupNameLen];
	atomic_t			mRefcnt;
	u8					mMethod;
	struct aos_list_head mMembers;
};


struct aosKernelApiParms;
extern int aosServerGroup_addCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_delCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_showCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_clearAllCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_setStatusCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_addMemberCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_delMemberCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_clearMemberCli(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_show(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);
extern int aosServerGroup_list(char *data, 
				unsigned int *length, 
				struct aosKernelApiParms *parms,
				char *, 
				const int);

extern int aosServerGroup_getNextServer(struct aosServerGroup *self, struct aosAppServer **server);
extern int aosServerGroup_init(void);
extern struct aosServerGroup * aosServerGroup_get(const char *name);
extern int aosServerGroup_constructor(char *name, 
				char *method, struct aosServerGroup **server);
extern int aosServerGroup_getNumGroups(void);
static inline int aosServerGroup_hold(struct aosServerGroup *self)
{
	atomic_inc(&self->mRefcnt);
	return 0;
}

static inline int aosServerGroup_put(struct aosServerGroup *self)
{
	atomic_dec(&self->mRefcnt);
	return 0;
}


extern struct aosServerGroupMember *
			aosServerGroupMember_constructor(struct aosAppServer *server, int priority);
extern int aosServerGroupMember_destructor(struct aosServerGroupMember *self);
#endif

