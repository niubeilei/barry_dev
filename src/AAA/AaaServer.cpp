////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AaaServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AAA/AaaServer.h"

#include <KernelSimu/string.h>

#include "AppProxy/AppProxy.h"
#include "aos/aosKernelApi.h"
#include "aos/aosReturnCode.h"
#include "aos/aosKernelAlarm.h"
#include "aosUtil/StringUtil.h"
#include "KernelUtil/MgdObj.h"
#include "aosUtil/List.h"

#ifdef __KERNEL__
#include "linux/module.h"
#endif

static unsigned int sgTotalAlloc = 0;
static unsigned int sgTotalFreed = 0;


int aosAaaServer_init()
{
	return 0;
}


int aosAaaServer_getNumServers(void)
{
	return (int)aosMgdObj_getNumObjs(eAosObjType_AaaServer);
}


// 
// Call this function whenever one holds a pointer to an AAA server
//
void aosAaaServer_hold(struct aosAaaServer *self)
{
	atomic_inc(&self->refcnt);
}


// 
// Call this function whenever one releases a pointer to an AAA server
//
void aosAaaServer_put(struct aosAaaServer *self)
{
	atomic_dec(&self->refcnt);
}


int aosAaaServer_addCli(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	// 
	// aaa server add <name> <type> <addr> <port> <healthcheck>
	//
	char *name, *type, *hc;
	struct aosAaaServer *server;
	int addr; 
	int port;
	int ret = aosKernelApi_getStr3(1, &name, &type, &hc, data, *length) || 
			  aosKernelApi_getInt2(1, &addr, &port, data);

	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	server = aosAaaServer_get(name);
	if (server)
	{
		// 
		// The server exists. Modify it.
		//
		strcpy(server->type, type);
		server->addr = (u32)addr;
		server->port = (unsigned short)port;
		strcpy(server->hc, hc);
		aosAaaServer_put(server);
		return 0;
	}

	return aosAaaServer_constructor(name, type, addr, port, hc, &server);
}


int aosAaaServer_showCli(char *data, 
						 unsigned int *length, 
						 struct aosKernelApiParms *parms,
						 char *errmsg, 
						 const int errlen)
{
	// 
	// aaa server show [<name>]
	//
	int numArgs = 0;
	int ret = aosKernelApi_getInt1(0, &numArgs, data); 
	if (ret)
	{
		*length = 0;
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	if (numArgs == 0)
	{
		return aosAaaServer_list(data, length, parms, errmsg, errlen);
	}

	return aosAaaServer_show(data, length, parms, errmsg, errlen);
}


int aosAaaServer_list(char *data, 
					  unsigned int *length, 
					  struct aosKernelApiParms *parms,
					  char *errmsg, 
					  const int errlen)
{
	// 
	// Name   Type	Addr    Port   Health Check 
	// -----------------------------------------------
	// 
	struct aosAaaServer *server;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AaaServer);
	char local[200];
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);

	aosKernelAssert(head, eAosAlarmProgErr);

	if (aosMgdObj_getNumObjs(eAosObjType_AaaServer) == 0)
	{
	    sprintf(local, "No AAA Server found!\n");
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
        *length = rsltIndex;
		return 0;
	}

	sprintf(local, "Server Name          Type        Addr            Port   Health Check\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "---------------------------------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	aosObjList_for_each(entry, head)
	{
		server = (struct aosAaaServer *)entry;
	    sprintf(local, "%-20s %-11s %-15s %-6d %-9s\n", 
			server->name, server->type, aosAddrToStr(server->addr), 
			server->port, server->hc);
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	sprintf(local, "---------------------------------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}


struct aosAaaServer *
aosAaaServer_get(const char *name)
{
	// 
	// If 'hold' is true, it will increment the reference count.
	//
	struct aosAaaServer *server;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AaaServer);
	aosKernelAssert(head, 0);

	aosObjList_for_each(entry, head)
	{
		server = (struct aosAaaServer *)entry;
		if (strcmp(server->name, name) == 0)
		{
			aosAaaServer_hold(server);
			return server;
		}
	}

	// 	
	// Did not find
	//
	return 0;
}


int aosAaaServer_constructor(char *name, 
							 char *type, 
							 u32  addr, 
							 u16  port,
							 char *hc, 
							 struct aosAaaServer **server)
{
	struct aosAaaServer *ss = (struct aosAaaServer *)
		aosMgdObj_alloc(eAosObjType_AaaServer, sizeof(struct aosAaaServer));
	*server = ss;
	if (!ss)
	{
		return aosAlarm(eAosAlarmMemErr);
	}

	sgTotalAlloc++;
	
	strcpy(ss->name, name);
	strcpy(ss->type, type);
	ss->addr = addr;
	ss->port = port;
	strcpy(ss->hc, hc);
	ss->status = 0;
	atomic_set(&ss->refcnt, 1);

	aosAaaServer_resetStat(ss);

	return 0;
}


int aosAaaServer_destructor(struct aosAaaServer *self)
{
	// 
	// Make sure the entry has been removed from its list. This means that
	// the list field must be 0. 
	// 
	// Chen Ding, 03/04/2007
	//
	// if (self->list != 0 && self->list != (struct aosList*)AOS_LIST_POISON3)
	if (!aos_list_empty(self->list))
	{
		return aosAlarmRated(eAosAlarm_aosAaaServer_destructor1, 1, 
			(int)self, (int)self->list, 0, 0);
	}

	self->name[0] = 0;
	self->type[0] = 0;
	self->addr = 0;
	self->port = 0;
	self->hc[0] = 0;
	self->status = 0;
	
	aosAaaServer_resetStat(self);
	return 0;
}
	

int aosAaaServer_show(char *data, 
					  unsigned int *length, 
					  struct aosKernelApiParms *parms,
					  char *errmsg, 
					  const int errlen)
{
	// 
	// This is to show an individual application proxy. 
	//
	char *appname;
	char theName[101];
	struct aosAaaServer *server;
	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	int ret = aosKernelApi_getStr1(1, &appname, data, *length);
	unsigned int optlen = *length;
	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	server = aosAaaServer_get(appname);
	if (!server)
	{
		sprintf(errmsg, "Server not found: %s", appname);
		return eAosRc_ServerNotDefined;
	}

	strcpy(theName, appname);

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Server Name:                 %-s\n", theName);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Status:                      %-s\n", (server->status)?"on":"off");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Reference Count:             %-d\n", atomic_read(&server->refcnt));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Type:                        %-s\n", server->type);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Local Address:               %-s\n", aosAddrToStr(server->addr));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Port:                        %-d\n", server->port);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Health Check:                %-s\n", server->hc);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Statistics\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "   Connection Attempts:      %-lld\n", server->connAttempts);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "   Successful Connections:   %-lld\n", server->successConns);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "   Number of Requests:       %-lld\n", server->numRequests);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "   Number of Accepted Reqs:  %-lld\n", server->numAcceptedReqs);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
    *length = rsltIndex;
	aosAaaServer_put(server);
	return 0;
}


int aosAaaServer_delCli(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	// 
	// aaa server remove <name> 
	//
	char *name;
	struct aosAaaServer *server;
	int ret = aosKernelApi_getStr1(1, &name, data, *length);
	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	server = aosAaaServer_get(name);
	if (!server)
	{
		sprintf(errmsg, "Server not found: %s", name);
		return eAosRc_ServerNotDefined;
	}

	if (atomic_read(&server->refcnt) > 2)
	{
		aosAaaServer_put(server);
		sprintf(errmsg, "Server used by others");
		return eAosRc_ServerUsedByOthers;
	}

	aosAaaServer_destructor(server);
	aosMgdObj_release((struct aosMgdObj *)server);
	return 0;
}
	

int aosAaaServer_clearAllCli(char *data, 
							 unsigned int *length, 
							 struct aosKernelApiParms *parms,
							 char *errmsg, const int errlen)
{
	struct aosAaaServer *server;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AaaServer);
	aosKernelAssert(head, eAosAlarmProgErr);

	while (!aosObjList_empty(head))	
	{
		server = (struct aosAaaServer *)head->nextobj;
		if (atomic_read(&server->refcnt) != 1)
		{
			sprintf(errmsg, "Server used by others: %s", server->name);
			return eAosRc_ServerUsedByOthers;
		}

		aosMgdObj_release((struct aosMgdObj *)server);
		sgTotalFreed++;
	}

	*length = 0;
	return 0;
}


int aosAaaServer_setStatusCli(char *data, 
							  unsigned int *length, 
							  struct aosKernelApiParms *parms,
							  char *errmsg, 
							  const int errlen)
{
	char *status;
	char *name;
	struct aosAaaServer *server;
	int ret = aosKernelApi_getStr2(1, &name, &status, data, *length);
	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	server = aosAaaServer_get(name);
	if (!server)
	{
		sprintf(errmsg, "Server not found: %s", name);
		return eAosRc_ServerNotDefined;
	}

	if (strcmp(status, "on") == 0)
	{
		if (server->status == 0)
		{
			aosAaaServer_serverOn(server);
		}
		server->status = 1;
	}
	else if (strcmp(status, "off") == 0)
	{
		if (server->status == 1)
		{
			aosAaaServer_serverOff(server);
		}
		server->status = 0;
	}

	aosAaaServer_put(server);
	return 0;
}


int aosAaaServer_serverOn(struct aosAaaServer *self)
{
	// 
	// This function is called when the application server 'self' is turned on.
	//
	return 0;
}


int aosAaaServer_serverOff(struct aosAaaServer *self)
{
	// 
	// This function is called when the application server 'self' is turned off.
	//
	return 0;
}


int aosAaaServer_resetStatCli(char *data, 
							  unsigned int *length, 
							  struct aosKernelApiParms *parms,
							  char *errmsg, 
							  const int errlen)
{
	// 
	// aaa server stat reset <name> 
	//
	char *name;
	struct aosAaaServer *server;
	int ret = aosKernelApi_getStr1(1, &name, data, *length);
	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	server = aosAaaServer_get(name);
	if (!server)
	{
		sprintf(errmsg, "Server not found: %s", name);
		return eAosRc_ServerNotDefined;
	}

	aosAaaServer_resetStat(server);
	aosAaaServer_put(server);
	return 0;
}


int aosAaaServer_resetStat(struct aosAaaServer *self)
{
	self->connAttempts = 0;
	self->successConns = 0;
	self->numRequests = 0;
	self->numAcceptedReqs = 0;
	
	return 0;
}


int aosAaaServer_clearStatCli(char *data, 
							  unsigned int *length, 
							  struct aosKernelApiParms *parms,
							  char *errmsg, 
							  const int errlen)
{
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AaaServer);

	aosKernelAssert(head, eAosAlarmProgErr);
	
	aosObjList_for_each(entry, head)
	{
		aosAaaServer_resetStat((struct aosAaaServer *)entry);
	}

	*length = 0;
	return 0;
}


int aosAaaServer_integrityCheck()
{
	// 
	// This function checks the integrity of all the AAA servers defined 
	// in the system. 
	// 

	// 
	// 1. Check the reference counters
	//    The only place that uses AAA servers are the AppProxy. If an AAA is used 
	//    by an AppProxy, that AppProxy will hold a pointer to that AAA and there should
	//    be a reference count for it.
	// 
	// 2. All AAA servers should be properly link listed. 
	// 
	// 3. The link list qlen should be correct
	// 
	// 4. There should be no loop in the link list
	//
	// 5. 'name', 'type', and 'hc' should not exceed their maximum lengths
	// 
	// 6. The IP address and port for each AAA should be correct.
	// 
	// 7. If an AAA is active, there should be a socket for it and the socket
	//    should be in the listening state
	// 
	// 8. If an AAA is not active, there should be no socket for it.
	//
	// 9. There shall be no more than the maximum AAA servers in the list
	//
	// 10. Total allocated - Total freed = existing
	// 

	// 
	// 1. Check the reference counters
	//
/*
	int ret = 0;
	struct aosAaaServer *ss;
	struct aosAaaServer *server = (struct aosAaaServer *)aosAaaServerList.next;
	struct aosAaaServer *prev = (struct aosAaaServer *)&aosAaaServerList;
	unsigned int qlen= 0;
	
	while (qlen < aosAaaServerList.qlen && 
		   server != (struct aosAaaServer *)&aosAaaServerList)
	{
		int refcnt = aosAppProxy_getAaaServerCount(server);
		qlen++;
		if (refcnt != atomic_read(&server->refcnt) - 1)
		{
			aosAlarmInt2(eAosAlarm_IntegrityCheckFail, refcnt, atomic_read(&server->refcnt));
			ret = 1;
		}

		// 
		// Check the prev pointer
		//
		if (server->prev != (struct aosList *)prev)
		{
			// 
			// Pointers not set correctly. Fix it.
			//
			aosAlarm(eAosAlarm_IntegrityCheckFail);
			server->prev = (struct aosList *)prev;
		}

		// 
		// Make sure the next does not point to an entry in front of it. Otherwise, 
		// it is a loop.
		//
		ss = (struct aosAaaServer *)aosAaaServerList.next;
		while (ss != server && server->next != (struct aosList *)ss) 
		{
			ss = (struct aosAaaServer *)ss->next;
		}

		if (ss != server)
		{
			// 
			// It points back to an entry in front of it. This is a loop. 
			// 
			aosAlarm(eAosAlarm_IntegrityCheckFail);
			aosAaaServer_dump();
		}	

		prev = server;
		server = (struct aosAaaServer *)server->next;

		aosAaaServer_integrityCheckOnServer(server);

	}

	if (qlen != aosAaaServerList.qlen)
	{
		// 
		// Queue counting incorrect. Correct it.
		//
		aosAlarmInt2(eAosAlarm_IntegrityCheckFail, qlen, aosAaaServerList.qlen);	
		aosAaaServerList.qlen = qlen;
	}

	// 
	// Check list length
	//
	if (qlen >= eAosMaxAaaServerListLen)
	{
		aosAlarmInt(eAosAlarm_IntegrityCheckFail, qlen);
	}
		
	if (sgTotalAlloc != sgTotalFreed + qlen)
	{
		aosAlarmInt2(eAosAlarm_IntegrityCheckFail, sgTotalAlloc, sgTotalFreed);
	}
*/

	return 0;
}


int aosAaaServer_dump()
{
	return 0;
}


int aosAaaServer_integrityCheckOnServer(struct aosAaaServer *server)
{
	//
	// 1. 'name', 'type', and 'hc' should not exceed their maximum lengths
	// 
	// 2. The IP address and port for each AAA should be correct.
	// 
	// 3. If an AAA is active, there should be a socket for it and the socket
	//    should be in the listening state
	// 
	// 4. If an AAA is not active, there should be no socket for it.
	//

	if (strlen(server->name) >= eAosAaaServerNameLen)
	{
		aosAlarmInt(eAosAlarm_IntegrityCheckFail, strlen(server->name));
		server->name[eAosAaaServerNameLen] = 0;
	}

	if (strlen(server->type) >= eAosAaaServerTypeLen)
	{
		aosAlarmInt(eAosAlarm_IntegrityCheckFail, strlen(server->type));
		server->type[eAosAaaServerTypeLen] = 0;
	}

	if (strlen(server->hc) >= eAosAaaProtNameLen)
	{
		aosAlarmInt(eAosAlarm_IntegrityCheckFail, strlen(server->hc));
		server->type[eAosAaaProtNameLen] = 0;
	}

	if (server->addr == 0 || server->port == 0)
	{
		aosAlarmInt2(eAosAlarm_IntegrityCheckFail, server->addr, server->port);
	}

	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( aosAaaServer_put );
EXPORT_SYMBOL( aosAaaServer_get );
#endif 
