////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MgdObj.h
// Description:
//	This facility is used to manage objects. Each type of objects is 
//  considered as a set. It has its maximum number, the number of 
//  instances created, the number of instances used, a function 
//  to remove leaked, etc. 
// 
//  The container is implemented as a double-linked list. Whenever 
//  to create an instance, it should call the creation function 
//  through this facility. Similarly, when freeing an instance, it
//  should call the function through this facility. 
// 
//  Each managed object must contain aosMgdObj at the beginning of 
//  its structure.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_KernelUtil_MgdObj_h
#define Omn_KernelUtil_MgdObj_h

#include "aos/aosKernelAlarm.h"
#include "aosUtil/Types.h"
#include "KernelUtil/ObjList.h"


struct AosSlab;

enum
{
	eAosObjType_TcpSockBridge,
	eAosObjType_AppProxy,
	eAosObjType_AppServer,
	eAosObjType_AaaServerEntry,
	eAosObjType_AaaServer,
	eAosObjType_ServerGroup,
	eAosObjType_ServerGroupMember
};


enum
{
	eAosMaxObjType = 1000,
	eAosMgdObjNameLen = 20,
	eAosMaxMgdObj = 1000000000	// 1G
};


// 
// 'listprev' and 'listnext' are used for list
// 'prevobj' and 'nextobj' are for managed objects
//
#define aosMgdObjDecl \
	struct aosList 		*listprev;			\
	struct aosList		*listnext;			\
	struct aosMgdObj	*prevobj;			\
	struct aosMgdObj	*nextobj;			\
	u16					 objtype

struct aosMgdObj
{
	aosMgdObjDecl;
};



struct aosMgdObjDef
{
	char					name[eAosMgdObjNameLen];
	u16						objtype;
	u16						objsize;
	int						maxObjs;
	struct aosObjListHead	list;
	struct AosSlab *		slab;
	u64						alloced;
	u64						freed;
	u64						numInsts;	
	u64						allocFail;
	int (*healthcheck)(char *data, u32 *length);
	int (*purge)(int level);
};


extern struct aosMgdObjDef aosMgdObjList[];

extern int aosMgdObj_moduleInit(void);
extern int aosMgdObj_register(char *name, u16 objtype, u16 objsize, u32  maxObjs);
extern struct aosMgdObj * aosMgdObj_alloc(u16 objtype, u16 objsize);
extern int aosMgdObj_release(struct aosMgdObj *obj);

static inline struct aosObjListHead *aosMgdObj_getObjList(u16 objtype)
{
	aosKernelAssert(objtype < eAosMaxObjType, 0);
	return &aosMgdObjList[objtype].list;
}


static inline u64 aosMgdObj_getNumObjs(u16 objtype)
{
	if (objtype >= eAosMaxObjType)
	{
		aosAlarm(eAosAlarm);
		return 0;
	}

	return aosMgdObjList[objtype].numInsts;
}





#endif

