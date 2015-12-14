////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: MgdObj.cpp
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
//  +++++++++++++++++  IMPORTANT  ++++++++++++++++++
//	All managed object must implement "aosMgdObj", which means that 
//  the object first members must be the ones defined in 'aosMgdObj'. 
//  ++++++++++++++++++++++++++++++++++++++++++++++++   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelUtil/MgdObj.h"

#include "AppProxy/AppProxy.h"
#include "AAA/AaaServer.h"
#include "KernelSimu/string.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/gfp.h"
#include "aos/aosKernelAlarm.h"
#include "aos_core/TcpSockBridge.h"
#include "aosUtil/Slab.h"
#include "Servers/AppServer.h"
#include "Servers/ServerGroup.h"

#ifdef __KERNEL__
#include "linux/module.h"
#endif

struct aosMgdObjDef		aosMgdObjList[eAosMaxObjType];



int aosMgdObj_moduleInit(void)
{
	int i;
	for (i=0; i<eAosMaxObjType; i++)
	{
		memset(&aosMgdObjList[i], 0, sizeof(struct aosMgdObjDef));
		AOS_INIT_OBJ_LIST_HEAD(&aosMgdObjList[i].list);
	}

	aosMgdObj_register("TcpSockBridge", 
		eAosObjType_TcpSockBridge, sizeof(struct aosTcpSockBridge), 5000);

	aosMgdObj_register("AppProxy", 
		eAosObjType_AppProxy, sizeof(struct aosAppProxy), 100);

	aosMgdObj_register("AppServer", 
		eAosObjType_AppServer, sizeof(struct aosAppServer), 100);

	aosMgdObj_register("AaaServerEntry", 
		eAosObjType_AaaServerEntry, sizeof(struct aosAaaServerEntry), 1000);

	aosMgdObj_register("AaaServer", 
		eAosObjType_AaaServer, sizeof(struct aosAaaServer), 100);

	aosMgdObj_register("ServerGroup", 
		eAosObjType_ServerGroup, sizeof(struct aosServerGroup), 100);

	aosMgdObj_register("ServerGroupMember", eAosObjType_ServerGroupMember, 
		sizeof(struct aosServerGroupMember), 2000);
	
	return 0;
}


int aosMgdObj_register(char *name, u16 objtype, u16 objsize, u32  maxObjs)
{
	if (objtype >= eAosMaxObjType)
	{
		return aosAlarmInt(eAosAlarm, objtype);
	}

	if (strlen(name) >= eAosMgdObjNameLen)
	{
		aosAlarmInt(eAosAlarm, strlen(name));
		strncpy(aosMgdObjList[objtype].name, name, eAosMgdObjNameLen-1);
		aosMgdObjList[objtype].name[eAosMgdObjNameLen-1] = 0;
	}
	else
	{
		strcpy(aosMgdObjList[objtype].name, name);
	}

	aosMgdObjList[objtype].objtype = objtype;
	aosMgdObjList[objtype].objsize = objsize;
	aosMgdObjList[objtype].maxObjs = maxObjs;
	aosMgdObjList[objtype].alloced = 0;
	aosMgdObjList[objtype].freed = 0;
	aosMgdObjList[objtype].numInsts = 0;
	aosMgdObjList[objtype].allocFail = 0;
	aosMgdObjList[objtype].healthcheck = 0;
	aosMgdObjList[objtype].purge = 0;

	AosSlab_constructor(name, objsize, maxObjs, &aosMgdObjList[objtype].slab);
	if (!aosMgdObjList[objtype].slab)
	{
		return aosAlarm(eAosAlarm);
	}
	
printk("Slab allocated %d, %d, %x\n", objtype, objsize, 
	(unsigned int)aosMgdObjList[objtype].slab);

	return 0;
}


// 
// It allocates a new object from the object's slab. If the object is 
// allocated, it will insert it into the object list.
//
struct aosMgdObj * aosMgdObj_alloc(u16 objtype, u16 objsize)
{
	struct aosMgdObjDef *mgr;
	struct aosObjListHead *obj;
	if (objtype >= eAosMaxObjType)
	{
		aosAlarmInt(eAosAlarm, objtype);
		return 0;
	}

	mgr = &aosMgdObjList[objtype];
	if (!mgr->slab)
	{
		aosAlarm(eAosAlarm);
		return 0;
	}

	aosKernelAssert(mgr->objsize == objsize, 0);

	obj = (struct aosObjListHead *)AosSlab_get(mgr->slab);
	if (!obj)
	{
		mgr->allocFail++;
		return 0;
	}

	obj->objtype = objtype;
	mgr->alloced++;
	mgr->numInsts++;
	aosObjList_append(obj, &mgr->list);
	return (struct aosMgdObj *)obj;
}


int aosMgdObj_release(struct aosMgdObj *obj)
{
	struct aosMgdObjDef *mgr;

	aosKernelAssert(obj, eAosAlarmNullPointer);
	aosKernelAssert(obj->objtype < eAosMaxObjType, eAosAlarmProgErr);

	aosObjList_del((struct aosObjListHead *)obj);
	mgr = &aosMgdObjList[obj->objtype];
	aosKernelAssert(mgr->slab, eAosAlarmProgErr);
	AosSlab_release(mgr->slab, obj);
	mgr->freed++;
	mgr->numInsts--;	
	
	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosMgdObj_release );
EXPORT_SYMBOL( aosMgdObjList );
EXPORT_SYMBOL( aosMgdObj_alloc );
#endif

