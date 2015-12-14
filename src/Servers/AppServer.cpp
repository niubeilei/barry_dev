////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Servers/AppServer.h"

#include <KernelSimu/string.h>

#include "aos/aosKernelApi.h"
#include "aos/aosReturnCode.h"
#include "aos/aosKernelAlarm.h"
#include "aosUtil/StringUtil.h"
#include "KernelUtil/MgdObj.h"
#include "aosUtil/List.h"
#include "KernelSimu/in.h"
#include "KernelSimu/aosKernelDebug.h"

// #include <linux/in.h>

// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>



int aosAppServer_init()
{
	return 0;
}


int aosAppServer_getNumServers(void)
{
	return (int)aosMgdObj_getNumObjs(eAosObjType_AppServer);
}


int aosAppServer_createCli(char *data, 
						   unsigned int *length, 
						   struct aosKernelApiParms *parms,
						   char *errmsg, 
						   const int errlen)
{
	// 
	// app proxy create <appname> <app-type> <localaddr> <remoteaddr> <remoteport> <protocol>
	//
	char *appname = parms->mStrings[0];
	char *type = parms->mStrings[1];
	char *prot = parms->mStrings[2];
	struct aosAppServer *ap;
	int localaddr = parms->mIntegers[0];
	int remoteaddr = parms->mIntegers[1];
	int remoteport = parms->mIntegers[2];
	int ret;
	*length = 0;

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	ap = aosAppServer_get(appname);
	if (ap)
	{
		strcpy(ap->mType, type);
		ap->mLocalAddr = localaddr;
		ap->mRemoteAddr = remoteaddr;
		ap->mRemotePort = remoteport;
		strcpy(ap->mProtocol, prot);
		aosAppServer_put(ap);
		return 0;
	}

	ret = aosAppServer_constructor(appname, type, localaddr, remoteaddr, remoteport, prot, &ap);
	if (!ap)
	{
		strcpy(errmsg, "Failed to create the server");
		return ret;
	}

	// 
	// The application proxy has been created.
	// 
	return 0;
}


int aosAppServer_showCli(char *data, 
						 unsigned int *length, 
						 struct aosKernelApiParms *parms,
						 char *errmsg, 
						 const int errlen)
{
	// 
	// app proxy show [<appname>]
	//
	int numArgs = parms->mNumIntegers;
	if (numArgs == 0)
	{
		return aosAppServer_list(data, length, parms, errmsg, errlen);
	}

	return aosAppServer_show(data, length, parms, errmsg, errlen);
}


int aosAppServer_list(char *data, 
					  unsigned int *length, 
					  struct aosKernelApiParms *parms,
					  char *errmsg, 
					  const int errlen)
{
	// 
	// AppName  Type	Addr    Port   Protocol
	// -----------------------------------------------
	// 
	struct aosAppServer *app;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AppServer);
	char local[200];
	unsigned int rsltIndex = 0;
	unsigned int optlen = *length;
	char *rsltBuff = aosKernelApi_getBuff(data);

	aosKernelAssert(head, eAosAlarmProgErr);
	if (aosMgdObj_getNumObjs(eAosObjType_AaaServer) == 0)
	{
	    sprintf(local, "No application servers found!\n");
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
        *length = rsltIndex;
		return 0;
	}

	sprintf(local, "App Name             Type         Local Addr      Remote Addr     Remote Port Protocol\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "--------------------------------------------------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
	aosObjList_for_each(entry, head)
	{
		app = (struct aosAppServer *)entry;
	    sprintf(local, "%-20s %-12s %-15s %-15s %-6d      %-9s\n", 
			app->mName, app->mType, aosAddrToStr(app->mLocalAddr), 
			aosAddrToStr(app->mRemoteAddr),
			app->mRemotePort, app->mProtocol);
        aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
        *length = rsltIndex;
	}

	*length = rsltIndex;
	return 0;
}


struct aosAppServer *
aosAppServer_get(const char *name)
{
	struct aosAppServer *app;
	struct aosObjListHead *entry;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AppServer);
	aosKernelAssert(head, 0);

	aosObjList_for_each(entry, head)
	{
		app = (struct aosAppServer *)entry;
		if (strcmp(app->mName, name) == 0)
		{
			aosAppServer_hold(app);
			return app;
		}
	}

	// 	
	// Did not find
	//
	return 0;
}


int aosAppServer_constructor(char *name, 
							 char *type, 
							 u32  localaddr, 
							 u32  remoteaddr, 
							 u16  remoteport,
							 char *protocol, 
							 struct aosAppServer **app)
{
	struct aosAppServer *theApp = (struct aosAppServer *)
		aosMgdObj_alloc(eAosObjType_AppServer, sizeof(struct aosAppServer));
	*app = theApp;
	if (!theApp)
	{
		return aosAlarm(eAosAlarmMemErr);
	}
	
	strcpy(theApp->mName, name);
	strcpy(theApp->mType, type);
	theApp->mLocalAddr = localaddr;
	theApp->mLocalPort = 0;
	theApp->mRemoteAddr = remoteaddr;
	theApp->mRemotePort = remoteport;
	strcpy(theApp->mProtocol, protocol);
	theApp->mStatus = 0;
	atomic_set(&theApp->mRefcnt, 1);

	theApp->mConnAttempts = 0;
	theApp->mSuccessConns = 0;
	theApp->mNumBytes = 0;
	theApp->mNumPackets = 0;
	theApp->mNumRequests = 0;
	theApp->mNumAcceptedReqs = 0;

	return 0;
}


int aosAppServer_show(char *data, 
					  unsigned int *length, 
					  struct aosKernelApiParms *parms,
					  char *errmsg, 
					  const int errlen)
{
	// 
	// This is to show an individual application proxy. 
	//
	char *appname;
	char theName[100];
	struct aosAppServer *ap;
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

	ap = aosAppServer_get(appname);
	if (!ap)
	{
		sprintf(errmsg, "Server not found: %s", appname);
		return eAosRc_AppServerNotDefined;
	}

	strcpy(theName, appname);

	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "App Name:                    %-s\n", theName);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Status:                      %-s\n", (ap->mStatus)?"on":"off");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Refcnt:                      %-d\n", atomic_read(&ap->mRefcnt)-1);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Type:                        %-s\n", ap->mType);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Local Address:               %-s\n", aosAddrToStr(ap->mLocalAddr));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Remote Address:              %-s\n", aosAddrToStr(ap->mRemoteAddr));
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "RemotePort:                  %-d\n", ap->mRemotePort);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Protocol:                    %-s\n", ap->mProtocol);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Connection Attempts:         %-lld\n", ap->mConnAttempts);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Successful Connections:      %-lld\n", ap->mSuccessConns);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Number of Bytes:             %-lld\n", ap->mNumBytes);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Number of Packets:           %-lld\n", ap->mNumPackets);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Number of Requests:          %-lld\n", ap->mNumRequests);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "Number of Accepted Reqs:     %-lld\n", ap->mNumAcceptedReqs);
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	sprintf(local, "------------------------------------------\n");
    aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, local, strlen(local));
	
    *length = rsltIndex;
	aosAppServer_put(ap);
	return 0;
}


int aosAppServer_delCli(char *data, 
						unsigned int *length, 
					  	struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	// 
	// app proxy remove <appname> 
	//
	char *appname;
	struct aosAppServer *server;
	int ret = aosKernelApi_getStr1(1, &appname, data, *length);
	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	server = aosAppServer_get(appname);
	if (!server)
	{
		sprintf(errmsg, "Server not found: %s", appname);
		return eAosRc_AppServerNotDefined;
	}

	if (atomic_read(&server->mRefcnt) > 2)
	{
		aosAppServer_put(server);
		sprintf(errmsg, "Server used by others");
		return eAosRc_ServerUsedByOthers;
	}

	aosAppServer_destructor(server);
	return 0;
}
	

int aosAppServer_destructor(struct aosAppServer *self)
{
	self->mName[0] = 0;
	self->mLocalAddr = 0;
	self->mRemoteAddr = 0;
	self->mRemotePort = 0;
	self->mProtocol[0] = 0;
	self->mStatus = 0;
	
	aosMgdObj_release((struct aosMgdObj *)self);
	return 0;
}


int aosAppServer_clearAllCli(char *data, 
					unsigned int *length, 
					struct aosKernelApiParms *parms,
					char *errmsg, 
					const int errlen)
{
	struct aosAppServer *server;
	struct aosObjListHead *head = aosMgdObj_getObjList(eAosObjType_AppServer);
	aosKernelAssert(head, eAosAlarmProgErr);

	*length = 0;
	while (!aosObjList_empty(head))
	{
		server = (struct aosAppServer *)head->nextobj;
		if (atomic_read(&server->mRefcnt) > 1)
		{
			sprintf(errmsg, "Server is used by others: %s", server->mName);
			return eAosRc_ServerUsedByOthers;
		}

		aosAppServer_destructor(server);	
	}

	return 0;
}


int aosAppServer_setStatusCli(char *data, 
						unsigned int *length, 
						struct aosKernelApiParms *parms,
						char *errmsg, 
						const int errlen)
{
	char *status;
	char *appname;
	struct aosAppServer *app;
	int ret = aosKernelApi_getStr2(1, &appname, &status, data, *length);
	*length = 0;
	if (ret)
	{
		strcpy(errmsg, "Failed to retrieve values");
		return ret;
	}

	app = aosAppServer_get(appname);
	if (!app)
	{
		sprintf(errmsg, "Application proxy not found: %s", appname);
		return eAosRc_AppProxyNotDefined;
	}

	if (strcmp(status, "on") == 0)
	{
		if (app->mStatus == 0)
		{
			aosAppServer_serverOn(app);
		}
		app->mStatus = 1;
	}
	else if (strcmp(status, "off") == 0)
	{
		if (app->mStatus == 1)
		{
			aosAppServer_serverOff(app);
		}
		app->mStatus = 0;
	}

	aosAppServer_put(app);
	return 0;
}


int aosAppServer_serverOn(struct aosAppServer *self)
{
	// 
	// This function is called when the application server 'self' is turned on.
	//
	return 0;
}


int aosAppServer_serverOff(struct aosAppServer *self)
{
	// 
	// This function is called when the application server 'self' is turned off.
	//
	return 0;
}

