////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_cli.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "proxy_scheduler.h"
#include "proxy_service.h"
#include "proxy_wrapper.h"
#include "aos/aosKernelApi.h"

extern int g_proxy_concurrent;

static int
AosProxyCli_concurrent(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen)
{

	// 
	// proxy concurrent <appname> 
	//
	int con = parms->mIntegers[0];
	*length = 0;

	if( con>0 && con<20000 )
	{
		g_proxy_concurrent = con;
	}

	sprintf(errmsg, "proxy concurrent = %d",g_proxy_concurrent);
	aos_trace(KERN_INFO "proxy concurrent=%d\n", g_proxy_concurrent);
	return 0;
}


static int
AosProxyCli_proxyAdd(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen)
{
	// 
	// proxy add <appname> <apptype> <addr> <port> <frontend ssl_flag> <backend ssl flag> <scheduler>
	//
	char 	*appname = parms->mStrings[0];
	char 	*type = parms->mStrings[1];
	char 	*frontend_wrapper = parms->mStrings[2];
	char 	*backend_wrapper = parms->mStrings[3];
	char 	*scheduler_name = parms->mStrings[4];
	int 	addr = parms->mIntegers[0];
	int 	port = parms->mIntegers[1];
	int 	ret;
	int		fw_id,bw_id;
	struct proxy_scheduler * scheduler;

	*length = 0;

	fw_id = find_sw_by_name( frontend_wrapper );
	if( fw_id < 0 )
	{
		strcpy(errmsg, "Invalid frontend wrapper flag");
		return -1;
	}

	bw_id = find_sw_by_name( backend_wrapper );
	if( bw_id < 0 )
	{
		strcpy(errmsg, "Invalid backend wrapper flag");
		return -1;
	}

	scheduler = proxy_sched_getbyname( scheduler_name );
	if( !scheduler )
	{
		sprintf(errmsg, "no such scheduler: %s", scheduler_name);
		return -1;
	}

	aos_trace(KERN_INFO "to add service: %s %s wrapper=%s,%s addr=%d port=%d, scheduer=%s\n", 
			appname,type,
			frontend_wrapper,backend_wrapper,addr,port,
			scheduler_name);
	ret = AosProxy_serviceAdd(appname, type, addr, port, gSockWrapperTypes[fw_id],gSockWrapperTypes[bw_id], scheduler, errmsg);
	
	if(ret)
	{
		aos_trace(KERN_ERR "add service %s failed\n", appname);
	}
	else
	{
		aos_trace(KERN_INFO "add service %s success\n", appname);
	}
	
	return ret;
}

static int
AosProxyCli_proxyDel(char *data, unsigned int *length, struct aosKernelApiParms *parms, char *errmsg, const int errlen)
{
	ProxyService_t * svc;

	// 
	// app proxy remove <appname> 
	//
	char *appname = parms->mStrings[0];
	*length = 0;

	// 
	// Retrieved the parameters. Check whether the app has been defined.
	//
	svc = AosProxy_getServiceByName(appname);
	if (!svc)
	{
		strcpy(errmsg, "Application proxy not found");
		return 1;
	}

	if ( svc->status == SERVICE_STATUS_ON )
	{
		strcpy(errmsg, "Application proxy cannot be deleted because it is running");
		return 1;
	}

	aos_trace(KERN_INFO "to remove service %s \n", appname);

	// we do not need to kill connections because the service status is off
	AosProxy_serviceRemove(svc);
	aos_trace(KERN_INFO "remove service %s success\n", appname);
	return 0;
}

static int
AosProxyCli_proxyRsAdd(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // proxy rs add <appname> <addr> <port> [<weight>]
    //
    char *appname = parms->mStrings[0];
    int addr = parms->mIntegers[0];
    int port = parms->mIntegers[1];
    int weight = parms->mIntegers[2];
    int ret;

	*length = 0;
	if ( !((parms->mNumIntegers == 2 || parms->mNumIntegers == 3) && (parms->mNumStrings == 1)) )
	{
		strcpy(errmsg, "Incorrect parameters!");
		return -1;
	}

	if (!appname)
	{
		strcpy(errmsg, "Missing virtual service name!");
		return -1;
	}

	aos_trace(KERN_INFO "to add real server %s \n", appname);
	ret = AosProxy_rsAdd(appname, addr, port, weight, errmsg);
	if(ret)
	{
		aos_trace(KERN_ERR "add real server %s failed\n", appname);
	}
	else
	{
		aos_trace(KERN_INFO "add real server %s success\n", appname);
	}

	return ret;
}


static int AosProxyCli_rsRemove(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
	//
    // proxy rs remove <appname> <addr> <port> 
    //
    char *appname = parms->mStrings[0];
    int addr = parms->mIntegers[0];
    int port = parms->mIntegers[1];
    int ret;

	*length = 0;
	if ( !((parms->mNumIntegers == 2 ) && (parms->mNumStrings == 1)) )
	{
		strcpy(errmsg, "Incorrect parameters!");
		return -1;
	}

	if (!appname)
	{
		strcpy(errmsg, "Missing virtual service name!");
		return -1;
	}

	ret = AosProxy_rsRemove(appname, addr, port, errmsg);

	return ret;
}


static int AosProxyCli_setStatus(char *data, 
							 unsigned int *length, 
							 struct aosKernelApiParms *parms,
							 char *errmsg, 
							 const int errlen)
{
	char *appname = parms->mStrings[0];
	char *status = parms->mStrings[1];
	int	ret;

	*length = 0;

	if (strcmp(status, "on") == 0)
	{
		aos_trace(KERN_INFO "to set status %s on \n", appname);
		ret = AosProxy_serviceStart(appname, errmsg);
		aos_trace(KERN_INFO "set status %s on %s\n",appname,ret?"failed":"success");
		
	}
	else if (strcmp(status, "off") == 0)
	{
		aos_trace(KERN_INFO "to set status %s off \n", appname);
		ret = AosProxy_serviceStop(appname, errmsg);
		aos_trace(KERN_INFO "set status %s off %s",appname,ret?"failed":"success");
	}
	else
	{
		sprintf(errmsg, "Invalid parameter: %s", status);
		ret = -1;
	}
	
	return ret;
}

static int AosProxyCli_editClientAuth(char *data, 
							 unsigned int *length, 
							 struct aosKernelApiParms *parms,
							 char *errmsg, 
							 const int errlen)
{
	char *appname = parms->mStrings[0];
	char *status = parms->mStrings[1];

	*length = 0;

	aos_trace(KERN_INFO "to set clientauth status %s %s \n", appname, status);

	ProxyService_t	*svc;

	svc=AosProxy_getServiceByName(appname);
	if(!svc)
	{
		if(errmsg)
		{
			sprintf(errmsg,"service %s not found",appname);
		}

		return 1;
	}

	svc->clientauth = strcmp("on",status)?0:1;

	return 0;
}


int
AosProxy_registerCli(void)
{
	int ret=0;
	
	ret |= OmnKernelApi_addCliCmd("ProxyAdd", AosProxyCli_proxyAdd);
	ret |= OmnKernelApi_addCliCmd("ProxyDel", AosProxyCli_proxyDel);
	ret |= OmnKernelApi_addCliCmd("ProxyRsAdd", AosProxyCli_proxyRsAdd);
	ret |= OmnKernelApi_addCliCmd("ProxyRsRemove", AosProxyCli_rsRemove);
	ret |= OmnKernelApi_addCliCmd("ProxySetStatus",	AosProxyCli_setStatus);
	ret |= OmnKernelApi_addCliCmd("ProxyEditClientAuth",	AosProxyCli_editClientAuth);
	ret |= OmnKernelApi_addCliCmd("ProxyConcurrent",	AosProxyCli_concurrent);

	/*
	ret |= OmnKernelApi_addCliCmd("AppProxyShow",		aosAppProxy_showCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyClearAll",	aosAppProxy_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaAdd",		aosAppProxy_aaaAddCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaDel",		aosAppProxy_aaaDelCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaClear",	aosAppProxy_aaaClearCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySetSvrGrp",	aosAppProxy_setGroupCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyResetGroup",	aosAppProxy_resetGroupCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyShowConn",	aosAppProxy_showConnCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyConnDurSet",	aosAppProxy_setMaxDurationCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySaveConfig",	aosAppProxy_saveConfigCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyClearConfig",aosAppProxy_clearConfigCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySetPMI",		aosAppProxy_setPMICli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslClientAuth",	aosAppProxy_sslClientAuthCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslFront",	aosAppProxy_sslFrontCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslBackend",	aosAppProxy_sslBackendCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyPmiSysname",	aosAppProxy_pmiSysnameCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyForwardTable",aosAppProxy_forwardTableCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyMaxClients",aosAppProxy_maxClientsCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyMaxConns",	aosAppProxy_maxConnsCli);
	*/
	return ret;
}

void
AosProxy_unregisterCli(void)
{
	OmnKernelApi_delCliCmd("ProxyAdd");
	OmnKernelApi_delCliCmd("ProxyDel");
	OmnKernelApi_delCliCmd("ProxyRsAdd");
	OmnKernelApi_delCliCmd("ProxyRsRemove");
	OmnKernelApi_delCliCmd("ProxySetStatus");
	OmnKernelApi_delCliCmd("ProxyEditClientauth");
}


