////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxy.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProxy/AppProxy.h"

#include <KernelSimu/spinlock.h>
#include <KernelSimu/string.h>
#include <KernelSimu/in.h>
#include <KernelSimu/sock.h>
#include <KernelSimu/net.h>
#include <KernelSimu/aosKernelDebug.h>
#include <KernelSimu/ip.h>

#include "AAA/AaaServer.h"
#include "aos/aosKernelApi.h"
#include "aos/aosReturnCode.h"
#include "aos/aosKernelAlarm.h"
#include "aosUtil/StringUtil.h"
#include "aos_core/TcpSockBridge.h"
#include "aos_core/aosSocket.h"
#include <KernelPorting/ip.h>
#include "KernelUtil/MgdObj.h"
//#include "KernelUtil/List.h"
#include "aosUtil/List.h"
#include "Ktcpvs/aosTcpVsCntl.h"
#include "Servers/AppServer.h"
#include "Servers/ServerGroup.h"


int aosAppProxy_init()
{
	return 0;
}

int aosAppProxy_addCli(char *data, 
					   unsigned int *length, 
					   struct aosKernelApiParms *parms,
					   char *errmsg, 
					   const int errlen)
{
	// 
	// app proxy add <appname> <apptype> <addr> <port> <ssl_flag>
	//
	char *appname = parms->mStrings[0];
	char *type = parms->mStrings[1];
	char *sslFlag = parms->mStrings[2];
	int addr = parms->mIntegers[0];
	int port = parms->mIntegers[1];
	u8 ss;

	*length = 0;

	if (strcmp(sslFlag, "ssl") == 0)
	{
		ss = eAosAppProxy_SSL;
	}
	else if (strcmp(sslFlag, "plain") == 0)
	{
		ss = eAosAppProxy_Plain;
	}
	else
	{
		strcpy(errmsg, "Invalid security flag");
		return eAosRc_InvalidAppProxyProtocol;
	}		

	/*
	 * Chen Ding, 08/24/2005
	 * Currently we are using ktcpvs. The following is commented out
	 * for now.
	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	ap = aosAppProxy_get(appname);
	if (ap)
	{
		// 
		// It is to modify the app proxy
		//
		strcpy(ap->dn, dn);
		ap->addr = addr;
		ap->port = port;
		ap->protocol = protocol;
		ap->security = ss;
		aosAppProxy_put(ap);
		return 0;
	}

	ret = aosAppProxy_constructor(appname, dn, addr, port, protocol, ss, &ap);
	if (!ap)
	{
		strcpy(errmsg, "Failed to create the application proxy");
		return ret;
	}
	*/

	return aos_tcpvs_add(appname, type, addr, port, ss, errmsg);
}


int aosAppProxy_showCli(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	// 
	// app proxy show [<appname>]
	//
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int index = 0;
	unsigned int optlen = *length;
	int ret;
	
	*length = 0;

	if (parms->mNumStrings == 0)
	{
		ret = aos_vs_get_service_entries(rsltBuff, optlen, &index, errmsg);
	}
	else
	{
		char *name = parms->mStrings[0];
		if (!name)
		{
			// 
			// This should not happen.
			//
			strcpy(errmsg, "Command incorrect");
			return -eAosRc_ProgErr;
		}

		ret = aos_tcpvs_show_service(name, rsltBuff, optlen, &index, errmsg);
	}

	*length = index;
	return ret;

	/*
	int numArgs = parms->mNumStrings;
	if (numArgs == 0)
	{
		return aosAppProxy_list(data, length, errmsg, errlen);
	}

	return aosAppProxy_show(data, length, parms, errmsg, errlen);
	*/
}


int aosAppProxy_list(char *data, 
					 unsigned int *length, 
					 char *errmsg, 
					 const int errlen)
{
	// 
	// AppName  Domain Name    Addr    Port   Protocol Security
	// --------------------------------------------------------
	// 
	struct aosAppProxy *app;
	struct aosObjListHead *head;
	struct aosObjListHead *entry;
	char local[200];
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char protname[20], secname[20];
	char *rsltBuff = aosKernelApi_getBuff(data);

	if (aosMgdObj_getNumObjs(eAosObjType_AppProxy) == 0)
	{
	    sprintf(local, "No Application Proxy found!\n");
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
        *length = rsltIndex;
		return 0;
	}

	sprintf(local, "App Name             Domain Name          Addr         Port   Protocol"
		"  Security\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "--------------------------------------------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	head = aosMgdObj_getObjList(eAosObjType_AppProxy);
	aosKernelAssert(head, eAosAlarmProgErr);
	aosObjList_for_each(entry, head)
	{
		app = (struct aosAppProxy *)entry;
		aosAppProxy_getNames(app->protocol, protname, app->security, secname);	
	    sprintf(local, "%-20s %-20s %-12s %-6d %-9s %-s\n", 
			app->name, app->dn, aosAddrToStr(app->addr), 
			app->port, protname, secname);
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
        *length = rsltIndex;
	}

	*length = rsltIndex;
	return 0;
}


struct aosAppProxy *
aosAppProxy_get(const char *name)
{
	struct aosAppProxy *app;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AppProxy);
	aosObjList_for_each(entry, head)
	{
		app = (struct aosAppProxy *)entry;
		if (strcmp(app->name, name) == 0)
		{
			aosAppProxy_hold(app);
			return app;
		}
	}

	// 	
	// Did not find
	//
	return 0;
}


/*
int aosAppProxy_constructor(char *name, 
							char *dn, 
							u32  addr, 
							u16  port,
							u8   protocol, 
							u8   security,
							struct aosAppProxy **app)
{
	struct aosAppProxy *theApp = (struct aosAppProxy *)
		aosMgdObj_alloc(eAosObjType_AppProxy, sizeof(struct aosAppProxy));
	*app = theApp;
	if (!theApp)
	{
		return aosAlarm(eAosAlarmMemErr);
	}
	
	strcpy(theApp->name, name);
	strcpy(theApp->dn, dn);
	theApp->addr = addr;
	theApp->port = port;
	theApp->protocol = protocol;
	theApp->security = security;
	theApp->status = 0;
	theApp->group = 0;
	theApp->sock = 0;
	theApp->clientAuthFlag = 0;
	theApp->ocspFlag = 0;
	theApp->maxConns = -1;
	theApp->maxDuration = -1;
	theApp->ca = 0;
	theApp->getNextServer = aosAppProxy_getNextServer;
	theApp->backlog = eAosAppProxyDftBacklog;
	AOS_INIT_LIST_HEAD(&theApp->aaaServers);
	atomic_set(&theApp->refcnt, 1);

	switch (protocol)
	{
	case IPPROTO_TCP:
		 theApp->createSockBridge = aosTcpSockBridge_create;
		 break;

	default:
		 // OmnKernelFree(theApp);
		 aosMgdObj_release((struct aosMgdObj *)theApp);
		 *app = 0;
		 return aosAlarmInt(eAosAlarm_aosAppProxy_constructor, protocol);
	}

	aosAppProxy_resetStat(theApp);
	return 0;
}
*/

void aosAppProxy_resetStat(struct aosAppProxy *self)
{
	self->connAttempts = 0;
	self->successConns = 0;
	self->numBytes = 0;
	self->numPackets = 0;
	self->numRequests = 0;
	self->numAcceptedReqs = 0;
}

/*

static int aosAppProxy_destructor(struct aosAppProxy *self)
{
	aosAppProxy_resetStat(self);
	
	aosAppProxy_clearAaaServers(self);

	self->name[0] = 0;
	self->dn[0] = 0;
	self->addr = 0;
	self->port = 0;
	self->protocol = 0;
	self->status = 0;
	
	if (self->group) {aosServerGroup_put(self->group); self->group = 0;}
	if (self->ca)    {aosAaaServer_put(self->ca); self->ca = 0;}
	if (self->sock)  {aosSocket_free(self->sock); self->sock = 0;}
	
	while ((struct aos_list_head *)self->aaaServers.next != &self->aaaServers)
	{
		struct aosAaaServer *server = (struct aosAaaServer *)self->aaaServers.next;
		aosListDel(&self->aaaServers, self->aaaServers.next);
		aosAaaServer_put(server);
	}

	aosMgdObj_release((struct aosMgdObj *)self);
	return 0;
}
*/

int aosAppProxy_getNames(u8 protocol, char *protname, u8 security, char *secname)
{
	switch (security)
	{
	case eAosAppProxy_SSL:
		 strcpy(secname, "SSL");
		 break;

	case eAosAppProxy_IPSec:
		 strcpy(secname, "IPSec");
		 break;

	case eAosAppProxy_Plain:
		 strcpy(secname, "Plain");
		 break;

	default:
		 strcpy(secname, "Invalid");
		 break;
	}

	switch (protocol)
	{
	case IPPROTO_TCP:
		 strcpy(protname, "TCP");
		 break;

	case IPPROTO_UDP:
		 strcpy(protname, "UDP");
		 break;

	default:
		 strcpy(protname, "Invalid");
		 break;
	}

	return 0;
}


int aosAppProxy_show(char *data, 
					 unsigned int *length, 
					 struct aosKernelApiParms *parms,
					 char *errmsg, 
					 const int errlen)
{
	// 
	// This is to show an individual application proxy. 
	//
	char *appname = parms->mStrings[0];
	char theName[100];
	struct aosAppProxy *ap;
	char protname[20], secname[20];
	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	*length = 0;
	ap = aosAppProxy_get(appname);
	if (!ap)
	{
		strcpy(errmsg, "Application proxy not found");
		return eAosRc_AppProxyNotDefined;
	}

	strcpy(theName, appname);
	aosAppProxy_getNames(ap->protocol, protname, ap->security, secname);	

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "App Name:                    %-s\n", theName);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Status:                      %-s\n", (ap->status)?"on":"off");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Refcnt:                      %-d\n", atomic_read(&ap->refcnt));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "Domain Name:                 %-s\n", ap->dn);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Local Address:               %-s\n", aosAddrToStr(ap->addr));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Port:                        %-d\n", ap->port);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Protocol:                    %-s\n", protname);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Client Authentication Flag:  %-d\n", ap->clientAuthFlag);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "OCSP Flag:                   %-d\n", ap->ocspFlag);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "CA Server:                   %-s\n", (ap->ca)?ap->ca->name:"No CA Server");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Maximum Connections:         %-d\n", ap->maxConns);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Maximum Connection Duration: %-d\n", ap->maxDuration);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Server Group:                %-s\n", 
		(ap->group)?ap->group->mName:"No Group");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Security:                    %-s\n", secname);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	// if (ap->aaaServers.qlen == 0)
	if (aos_list_empty(&(ap->aaaServers)))
	{
		sprintf(local, "AAA Server:                  None\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		struct aosAaaServerEntry *server;

		sprintf(local, "AAA Servers:\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		sprintf(local, "   Server Name               Priority\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

		server = (struct aosAaaServerEntry *)ap->aaaServers.next;
		while (server != (struct aosAaaServerEntry *)&ap->aaaServers)
		{
			sprintf(local, "   %-20s      %-d\n", 
				server->server->name, server->priority);
    		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

			server = (struct aosAaaServerEntry *)server->next;
		}

		sprintf(local, "\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	sprintf(local, "Statistics:\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	sprintf(local, "  Connection Attempts:       %-lld\n", ap->connAttempts);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "  Successful Connections:    %-lld\n", ap->successConns);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "  Number of Bytes:           %-lld\n", ap->numBytes);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "  Number of Packets:         %-lld\n", ap->numPackets);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "  Number of Requests:        %-lld\n", ap->numRequests);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "  Number of Accepted Reqs:   %-lld\n", ap->numAcceptedReqs);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
    *length = rsltIndex;
	aosAppProxy_put(ap);
	return 0;
}


int aosAppProxy_delCli(char *data, 
					   unsigned int *length, 
					   struct aosKernelApiParms *parms,
					   char *errmsg, 
					   const int errlen)
{
	// 
	// app proxy remove <appname> 
	//
	char *appname = parms->mStrings[0];
	*length = 0;

	/*
	 * Chen Ding, 08/24/2005
	 * Commented out. Will use ktcpvs
	 *
	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	ap = aosAppProxy_get(appname);
	if (!ap)
	{
		strcpy(errmsg, "Application proxy not found");
		return eAosRc_AppProxyNotDefined;
	}

	if (atomic_read(&ap->refcnt) > 2)
	{
		aosAppProxy_put(ap);
		strcpy(errmsg, "Application proxy cannot be deleted because it is used by others");
		return eAosRc_AppProxyUsedByOthers;
	}

	aosAppProxy_destructor(ap);
	*/

	return aos_tcpvs_remove(appname, errmsg);
}
	

int aosAppProxy_clearAllCli(char *data, 
							unsigned int *length, 
							struct aosKernelApiParms *parms,
							char *errmsg, 
							const int errlen)
{
	struct aosAppProxy *app;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AppProxy);
	aosKernelAssert(head, eAosAlarmProgErr);

	*length = 0;
	while (!aosObjList_empty(head))
	{
		app = (struct aosAppProxy *)head->nextobj;
		if (atomic_read(&app->refcnt) > 1)
		{
			sprintf(errmsg, "Application proxy: %s cannot be removed", app->name);
			return eAosRc_AppProxyUsedByOthers;
		}

	//	aosAppProxy_destructor(app);
	}

	return 0;
}

int aosAppProxy_resetGroupCli(char *data, 
							  unsigned int *length, 
							  struct aosKernelApiParms *parms,
							  char *errmsg, 
							  const int errlen)
{
	char *appname = parms->mStrings[0];
	struct aosAppProxy *app;
	*length = 0;

	app = aosAppProxy_get(appname);
	if (!app)
	{
		strcpy(errmsg, "Application proxy not found");
		return eAosRc_AppProxyNotDefined;
	}

	if (app->group)
	{
		aosServerGroup_put(app->group);
		app->group = 0;
	}

	aosAppProxy_put(app);
	return 0;
}


int aosAppProxy_setGroupCli(char *data, 
							unsigned int *length, 
							struct aosKernelApiParms *parms,
							char *errmsg, 
							const int errlen)
{
	char *appname = parms->mStrings[0];
	char *grpname = parms->mStrings[1];
	struct aosAppProxy *app;
	struct aosServerGroup *group;
	*length = 0;
	app = aosAppProxy_get(appname);
	if (!app)
	{
		strcpy(errmsg, "Application proxy not found");
		return eAosRc_AppProxyNotDefined;
	}

	group = aosServerGroup_get(grpname);
	if (!group)
	{
		aosAppProxy_put(app);
		strcpy(errmsg, "Group not found");
		return eAosRc_ServerGroupNotDefined;
	}

	if (app->group)
	{
		aosServerGroup_put(app->group);
	}

	app->group = group;
	aosAppProxy_put(app);
	return 0;
}
	

int aosAppProxy_setStatusCli(char *data, 
							 unsigned int *length, 
							 struct aosKernelApiParms *parms,
							 char *errmsg, 
							 const int errlen)
{
	char *appname = parms->mStrings[0];
	char *status = parms->mStrings[1];
	*length = 0;

	/*
	 * Chen Ding, 08/24/2005
	 * Commented temporarily. Will use ktcpvs
	 * 
	app = aosAppProxy_get(appname);
	if (!app)
	{
		strcpy(errmsg, "Application proxy not found");
		return eAosRc_AppProxyNotDefined;
	}

	if (strcmp(status, "on") == 0)
	{
		if (app->status == 0)
		{
			aosAppProxy_appProxyOn(app);
		}
		app->status = 1;
	}
	else if (strcmp(status, "off") == 0)
	{
		if (app->status == 1)
		{
			aosAppProxy_appProxyOff(app);
		}
		app->status = 0;
	}

	aosAppProxy_put(app);
	*/

	if (strcmp(status, "on") == 0)
	{
		return aos_tcpvs_start(appname, errmsg);
	}
	else if (strcmp(status, "off") == 0)
	{
		return aos_tcpvs_stop(appname, errmsg);
	}
	else
	{
		sprintf(errmsg, "Invalid parameter: %s", status);
		return -eAosRc_InvalidParm;
	}
	
	return 0;
}


/*
int aosAppProxy_appProxyOn(struct aosAppProxy *self)
{
	// 
	// This function is called when the application proxy 'self' is turned on.
	//
	int ret;

	if (self->sock)
	{
		aosSocket_free(self->sock);
	}

	ret = aosSocket_createAndListen(&self->sock, self->addr, self->port, self->backlog);
	if (ret)
	{
		aosAlarmInt(eAosAlarm, ret);
	}

	if (self->sock)
	{
		self->sock->sk->aos_data.type = eAosSockType_AppProxy;
		self->sock->sk->aos_data.data = (void *)self;
	}
	else
	{
		return aosAlarm(eAosAlarm);
	}
	return ret;
}
*/

int aosAppProxy_appProxyOff(struct aosAppProxy *self)
{
	// 
	// This function is called when the application proxy 'self' is turned off.
	//
	if (self->sock)
	{
		aosSocket_free(self->sock);
	}

	self->sock = 0;
	return 0;
}


static inline int aosAaaServerEntry_destructor(struct aosAaaServerEntry *self)
{
	if (self->server)
	{
		aosAaaServer_put(self->server);
	}

	self->server = (struct aosAaaServer *)AOS_POINTER_POISON;
	self->priority = 0;
	aosMgdObj_release((struct aosMgdObj *)self);

	return 0;
}


static inline struct aosAaaServerEntry * 
aosAaaServerEntry_constructor(int priority, 
							  struct aosAaaServer *server)
{
	struct aosAaaServerEntry *entry = (struct aosAaaServerEntry*)
		aosMgdObj_alloc(eAosObjType_AaaServerEntry, sizeof(struct aosAaaServerEntry));
	if (!entry)
	{
		aosAlarm(eAosAlarmMemErr);
		return 0;
	}

	entry->prev = 0;
	entry->next = 0;
	entry->priority = priority;
	entry->server = server;
	return entry;
}


/*
int aosAppProxy_aaaAddCli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms,
						  char *errmsg, 
						  const int errlen)
{
	// 
	// app proxy add aaa server <appname> <server-name> <priority>
	//
	char *appname = parms->mStrings[0];
	char *servername = parms->mStrings[1];
	struct aosAppProxy *ap;
	struct aosAaaServer *server;
	struct aosAaaServerEntry *entry;
	int priority = parms->mIntegers[0];
	*length = 0;

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	ap = aosAppProxy_get(appname);
	if (!ap)
	{
		sprintf(errmsg, "Application proxy not found: %s", appname);
		return eAosRc_AppProxyNotDefined;
	}

	server = aosAaaServer_get(servername);
	if (!server)
	{
		aosAppProxy_put(ap);
		sprintf(errmsg, "Server not found: %s", servername);
		return eAosRc_ServerNotDefined;
	}

	entry = aosAaaServerEntry_constructor(priority, server);
	if (!entry)
	{
		aosAppProxy_put(ap);
		strcpy(errmsg, "Failed to create Server entry");
		return eAosAlarmMemErr;
	}

	aosListAppend(&ap->aaaServers, (struct aos_list_head*)entry);
	aosAppProxy_put(ap);
	return 0;
}

int aosAppProxy_aaaDelCli(char *data, 
						  unsigned int *length, 
						  struct aosKernelApiParms *parms,
						  char *errmsg, 
						  const int errlen)
{
	// 
	// app proxy remove aaa server <app-name> <server-name>
	//
	char *appname = parms->mStrings[0];
	char *servername = parms->mStrings[1];
	struct aosAppProxy *ap;
	struct aosAaaServerEntry *server;
	*length = 0;

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	ap = aosAppProxy_get(appname);
	if (!ap)
	{
		sprintf(errmsg, "Application proxy not found: %s", appname);
		return eAosRc_AppProxyNotDefined;
	}

	server = (struct aosAaaServerEntry *)ap->aaaServers.next;
	while (server != (struct aosAaaServerEntry *)&ap->aaaServers)
	{
		if (strcmp(server->server->name, servername) == 0)
		{
			aosListDel(&ap->aaaServers, (struct aos_list_head *)server);
			aosAaaServerEntry_destructor(server);
			return 0;
		}

		server = (struct aosAaaServerEntry *)server->next;
	}

	sprintf(errmsg, "Server not found: %s", servername);
	return eAosRc_ServerNotFound;
}


int aosAppProxy_aaaClearCli(char *data, 
							unsigned int *length, 
							struct aosKernelApiParms *parms,
							char *errmsg, 
							const int errlen)
{
	// 
	// app proxy clear aaa server <app-name> 
	//
	char *appname = parms->mStrings[0];
	struct aosAppProxy *ap;
	*length = 0;

	ap = aosAppProxy_get(appname);
	if (!ap)
	{
		sprintf(errmsg, "Application proxy not found: %s", appname);
		return eAosRc_AppProxyNotDefined;
	}

	aosAppProxy_clearAaaServers(ap);
	aosAppProxy_put(ap);
	return 0;
}


int aosAppProxy_clearAaaServers(struct aosAppProxy *self)
{
	struct aosAaaServerEntry *server = (struct aosAaaServerEntry *)self->aaaServers.next;
	while (server != (struct aosAaaServerEntry *)&self->aaaServers)
	{
		aosListDel(&self->aaaServers, (struct aosListNode *)server);
		aosAaaServerEntry_destructor(server);
		server = (struct aosAaaServerEntry *)self->aaaServers.next;
	}

	return 0;
}


int aosAppProxy_getAaaServerCount(struct aosAaaServer *server)
{
	// 
	// This function searches all the application proxies to count on the number 
	// of AAA server 'server' used by them.
	// 
	int count = 0;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AppProxy);
	aosObjList_for_each(entry, head)
	{
		// 
		// Check whether the app proxy uses the aaa server 
		//
		struct aosAppProxy *app = (struct aosAppProxy *)entry;
		struct aosAaaServerEntry *aaa = (struct aosAaaServerEntry *)app->aaaServers.next;
		while (aaa != (struct aosAaaServerEntry *)&app->aaaServers)
		{
			if (aaa->server == server)
			{
				count++;
			}

			aaa = (struct aosAaaServerEntry *)aaa->next;
		}
	}
			
	return count;
}


*/	
int aosAppProxy_getNumAppProxies(void)
{
	return (int)aosMgdObj_getNumObjs(eAosObjType_AppProxy);
}


/*
int aosAppProxy_showConnCli(char *data, 
 				unsigned int *length, 
 				struct aosKernelApiParms *parms, 
 				char *errmsg, 
 				const int errlen)
{
	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;

	int numArgs = parms->mNumIntegers + parms->mNumStrings;
	if (numArgs == 0)
	{
		return aosAppProxy_listAllConns(data, length);
	}

	if (numArgs == 1)
	{
		return aosAppProxy_listProxyConns(data, length);
	}

	if (numArgs == 2)
	{
		return aosAppProxy_listSenderConns(data, length);
	}

	sprintf(local, "Command incorrect\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}

int aosAppProxy_listAllConns(char *data, unsigned int *length)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	struct aosObjListHead *entry;
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char local[200];
	struct aosObjListHead *head;

	*length = 0;
	head = aosMgdObj_getObjList(eAosObjType_TcpSockBridge);
	aosKernelAssert(head, eAosAlarmProgErr);

	if (aosObjList_empty(head))
	{
		sprintf(local, "No Connections Found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		sprintf(local, "AppProxy    Left Addr/Port   Right Addr/Port    Server Name\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		sprintf(local, "-----------------------------------------------------------\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		
		aosObjList_for_each(entry, head)
		{
			aosAppProxy_printOneBridge((struct aosTcpSockBridge *)entry, 
				rsltBuff, &rsltIndex, optlen);
		}
	}
		
	*length = rsltIndex;
	return 0;
}


// 
// It lists all the connections to the specified application proxy
//
int aosAppProxy_listProxyConns(char *data, unsigned int *length)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	struct aosTcpSockBridge *bridge;
	struct aosObjListHead *entry;
	unsigned int optlen = *length;
	char local[200];
	char *proxyname;
	int found = 0;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_TcpSockBridge);

	int ret = aosKernelApi_getStr1(1, &proxyname, data, *length);
	*length = 0;
	if (ret)
	{
		return ret;
	}

	aosKernelAssert(head, eAosAlarmProgErr);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	if (aosObjList_empty(head))
	{
		sprintf(local, "No Connections Found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		sprintf(local, "AppProxy    Left Addr/Port   Right Addr/Port    Server Name\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		sprintf(local, "-----------------------------------------------------------\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		
		aosObjList_for_each(entry, head)
		{
			bridge = (struct aosTcpSockBridge *)entry;
			aosKernelAssert(bridge->mAppProxy, eAosAlarmProgErr);
			if (strcmp(bridge->mAppProxy->name, proxyname) == 0)
			{
				found++;
				aosAppProxy_printOneBridge(bridge, rsltBuff, &rsltIndex, optlen);
			}
		}
	}

	if (!found)
	{
		sprintf(local, "No connections found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	*length = rsltIndex;
	return 0;
}


// 
// It lists all connections from the specified sender to the specified application proxy
//
int aosAppProxy_listSenderConns(char *data, unsigned int *length)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	struct aosTcpSockBridge *bridge;
	struct aosObjListHead *entry;
	unsigned int optlen = *length;
	char local[200];
	char *proxyname;
	int found = 0;
	int addr;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_TcpSockBridge);

	int ret = aosKernelApi_getStr1(1, &proxyname, data, *length) |
			  aosKernelApi_getInt1(1, &addr, data);
	*length = 0;
	if (ret)
	{
		return ret;
	}

	aosKernelAssert(head, eAosAlarmProgErr);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	if (aosObjList_empty(head))
	{
		sprintf(local, "No Connections Found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		sprintf(local, "AppProxy    Left Addr/Port   Right Addr/Port    Server Name\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		sprintf(local, "-----------------------------------------------------------\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
		
		aosObjList_for_each(entry, head)
		{
			bridge = (struct aosTcpSockBridge *)entry;
			aosKernelAssert(bridge->mAppProxy, eAosAlarmProgErr);
			aosKernelAssert(bridge->mLeft, eAosAlarmProgErr);
			if (strcmp(bridge->mAppProxy->name, proxyname) == 0 && 
				aosInetSock_saddr(bridge->mLeft) == (u32)addr)
			{
				found++;
				aosAppProxy_printOneBridge(bridge, rsltBuff, &rsltIndex, optlen);
			}
		}
	}

	if (!found)
	{
		sprintf(local, "No connections found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}

	*length = rsltIndex;
	return 0;
}


int aosAppProxy_delConnCli(char *data, 
						   unsigned int *length, 
						   struct aosKernelApiParms *parms,
						   char *errmsg, 
						   const int errlen)
{
	char local[200];
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;

	int numArgs = parms->mNumIntegers + parms->mNumStrings;
	if (numArgs == 0)
	{
		return aosAppProxy_delAllConns(data, length);
	}

	if (numArgs == 1)
	{
		return aosAppProxy_delProxyConns(data, length);
	}

	if (numArgs == 2)
	{
		return aosAppProxy_delSenderConns(data, length);
	}

	sprintf(local, "Command incorrect\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));

	*length = rsltIndex;
	return 0;
}


int aosAppProxy_delAllConns(char *data, unsigned int *length)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	struct aosObjListHead *entry;
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char local[200];
	int deleted = 0; 
	struct aosObjListHead *head;

	*length = 0;
	head = aosMgdObj_getObjList(eAosObjType_TcpSockBridge);
	aosKernelAssert(head, eAosAlarmProgErr);

	if (aosObjList_empty(head))
	{
		sprintf(local, "No Connections Found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		aosObjList_for_each(entry, head)
		{
			aosTcpSockBridge_release((struct aosTcpSockBridge *)entry);
			deleted++;
		}

		sprintf(local, "Total connections deleted: %d\n", deleted);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	
	*length = rsltIndex;
	return 0;
}


// 
// Delete all connections for the specified app proxy
//
int aosAppProxy_delProxyConns(char *data, unsigned int *length)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	struct aosTcpSockBridge *bridge;
	struct aosObjListHead *entry;
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char local[200];
	int deleted = 0; 
	char *proxyname;
	int ret = aosKernelApi_getStr1(1, &proxyname, data, *length);
	struct aosObjListHead *head;

	*length = 0;
	if (ret)
	{
		return ret;
	}

	head = aosMgdObj_getObjList(eAosObjType_TcpSockBridge);
	aosKernelAssert(head, eAosAlarmProgErr);

	if (aosObjList_empty(head))
	{
		sprintf(local, "No Connections Found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		aosObjList_for_each(entry, head)
		{
			bridge = (struct aosTcpSockBridge *)entry;
			aosKernelAssert(bridge->mAppProxy, eAosAlarmProgErr);
			if (strcmp(bridge->mAppProxy->name, proxyname) == 0)
			{			
				aosTcpSockBridge_release(bridge);
				deleted++;
			}
		}

		sprintf(local, "Total connections deleted: %d\n", deleted);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	
	*length = rsltIndex;
	return 0;
}

*/

/*
int aosAppProxy_delSenderConns(char *data, unsigned int *length)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	struct aosTcpSockBridge *bridge;
	struct aosObjListHead *entry;
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char local[200];
	int deleted = 0; 
	char *proxyname;
	int addr;
	struct aosObjListHead *head;
	int ret = aosKernelApi_getStr1(1, &proxyname, data, *length) |
			  aosKernelApi_getInt1(1, &addr, data);

	*length = 0;
	if (ret)
	{
		return ret;
	}

	head = aosMgdObj_getObjList(eAosObjType_TcpSockBridge);
	aosKernelAssert(head, eAosAlarmProgErr);

	if (aosObjList_empty(head))
	{
		sprintf(local, "No Connections Found\n");
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	else
	{
		aosObjList_for_each(entry, head)
		{
			bridge = (struct aosTcpSockBridge *)entry;
			aosKernelAssert(bridge->mAppProxy, eAosAlarmProgErr);
			aosKernelAssert(bridge->mLeft, eAosAlarmProgErr);
			if (strcmp(bridge->mAppProxy->name, proxyname) == 0 &&
				aosInetSock_saddr((struct inet_sock*)bridge->mLeft) == (u32)addr)
			{			
				aosTcpSockBridge_release(bridge);
				deleted++;
			}
		}

		sprintf(local, "Total connections deleted: %d\n", deleted);
    	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	}
	
	*length = rsltIndex;
	return 0;
}


int aosAppProxy_printOneBridge(struct aosTcpSockBridge *bridge, 
					char *rsltBuff, 
					unsigned int *rsltIndex, 
					unsigned int optlen)
{
	char local[200];

	struct inet_sock *left  = (struct inet_sock*)bridge->mLeft;
	struct inet_sock *right = (struct inet_sock*)bridge->mRight;
	aosKernelAssert(left, eAosAlarmProgErr);
	aosKernelAssert(bridge->mAppProxy, eAosAlarmProgErr);
	aosKernelAssert(bridge->mServer, eAosAlarmProgErr);

	sprintf(local, "%-20s %14s:%-8d  %14s:%-8d    %14s:%-8d  %14s:%-8d    %-20s\n", 
		bridge->mAppProxy->name,  
		aosAddrToStr(aosInetSock_saddr(left)), aosInetSock_sport(left), 
		aosAddrToStr(aosInetSock_rcv_addr(left)), aosInetSock_dport(left), 
		(right)?aosAddrToStr(aosInetSock_saddr(right)):"Null", 
			(right)?aosInetSock_sport(right):0, 
		(right)?aosAddrToStr(aosInetSock_rcv_addr(right)):"Null", 
			(right)?aosInetSock_dport(right):0, 
		bridge->mServer->mName);
    aosCheckAndCopy(rsltBuff, rsltIndex, optlen, local, strlen(local));

	return 0;
}


int aosAppProxy_setMaxDurationCli(char *data, 
								  unsigned int *length, 
								  struct aosKernelApiParms *parms,
								  char *errmsg, 
								  const int errlen)
{
	struct aosAppProxy *proxy;
	char *proxyname = parms->mStrings[0];
	int duration = parms->mIntegers[0];

	*length = 0;
	proxy = aosAppProxy_get(proxyname);
	if (!proxy)
	{
		sprintf(errmsg, "Application proxy not found: %s", proxyname);
		return eAosRc_AppProxyNotDefined;
	}

	if (duration < 0)
	{
		sprintf(errmsg, "Invalid duration value: %d", duration);
		return eAosRc_InvalidValue;
	}

	proxy->maxDuration = duration;
	aosAppProxy_put(proxy);
	return 0;
}


int aosAppProxy_getNextServer(struct aosAppProxy *proxy, struct aosAppServer **server)
{
	if (!proxy->group)
	{
		*server = 0;
		return aosAlarm(eAosAlarm);
	}

	return aosServerGroup_getNextServer(proxy->group, server);
}


*/
int aosAppProxy_saveConfigCli(char *data, 
						   unsigned int *length, 
						   struct aosKernelApiParms *parms,
						   char *errmsg, 
						   const int errlen)
{
	char *rsltBuff = aosKernelApi_getBuff(data);
	unsigned int optlen = *length;
	unsigned int rsltIndex = 0;

	aos_ktcpvs_saveconfig(rsltBuff, &rsltIndex, optlen);

	*length = rsltIndex;
	return 0;
}

int aosAppProxy_clearConfigCli(
		char *data, 
		unsigned int *length, 
		struct aosKernelApiParms *parms,
		char *errmsg, 
		const int errlen)
{
	*length = 0;
	return aos_ktcpvs_clearconfig(errmsg, errlen);
}

/*
int aosAppProxy_setPMICli(
		char *data, 
		unsigned int *length, 
		struct aosKernelApiParms *parms,
		char *errmsg, 
		const int errlen)
{
	char *appname = parms->mStrings[0];
	char *status = parms->mStrings[1];
	*length = 0;

	if (strcmp(status, "on") == 0)
	{
		return aos_tcpvs_pmi(appname, 1, errmsg);
	}
	else if (strcmp(status, "off") == 0)
	{
		return aos_tcpvs_pmi(appname, 0, errmsg);
	}
	else
	{
		sprintf(errmsg, "Invalid parameter: %s", status);
		return -eAosRc_InvalidParm;
	}
	
	return 0;
}

int aosAppProxy_sslClientAuthCli(
		char *data, 
		unsigned int *length, 
		struct aosKernelApiParms *parms,
		char *errmsg, 
		const int errlen)
{
	char *appname = parms->mStrings[0];
	char *status = parms->mStrings[1];
	*length = 0;

	if (strcmp(status, "on") == 0)
	{
		return aos_tcpvs_ssl_clientauth(appname, 1, errmsg);
	}
	else if (strcmp(status, "off") == 0)
	{
		return aos_tcpvs_ssl_clientauth(appname, 0, errmsg);
	}
	else
	{
		sprintf(errmsg, "Invalid parameter: %s", status);
		return -eAosRc_InvalidParm;
	}
	
	return 0;
}

int aosAppProxy_sslFrontCli(
        char *data,
        unsigned int *length,
        struct aosKernelApiParms *parms,
        char *errmsg,
        const int errlen)
{
    char *appname = parms->mStrings[0];
    char *status = parms->mStrings[1];
    *length = 0;

    if (strcmp(status, "on") == 0)
    {
        return aos_tcpvs_ssl_front(appname, 1, errmsg);
    }
    else if (strcmp(status, "off") == 0)
    {
        return aos_tcpvs_ssl_front(appname, 0, errmsg);
    }
    else
    {
        sprintf(errmsg, "Invalid parameter: %s", status);
        return -eAosRc_InvalidParm;
    }

    return 0;
}
*/

/*
int aosAppProxy_sslBackendCli(
        char *data,
        unsigned int *length,
        struct aosKernelApiParms *parms,
        char *errmsg,
        const int errlen)
{
    char *appname = parms->mStrings[0];
    char *status = parms->mStrings[1];
    *length = 0;

    if (strcmp(status, "on") == 0)
    {
        return aos_tcpvs_ssl_backend(appname, 1, errmsg);
    }
    else if (strcmp(status, "off") == 0)
    {
        return aos_tcpvs_ssl_backend(appname, 0, errmsg);
    }
    else
    {
        sprintf(errmsg, "Invalid parameter: %s", status);
        return -eAosRc_InvalidParm;
    }

    return 0;
}
*/

