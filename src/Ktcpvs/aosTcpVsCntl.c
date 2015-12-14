////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosTcpVsCntl.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/sched.h>
#include "Ktcpvs/aosTcpVsCntl.h"

#include "aos/aosReturnCode.h"
#include "aos/aosKernelApi.h"
#include "AppProxy/AppProxy.h"
#include "KernelSimu/aosKernelMutex.h"
#include "aosUtil/Memory.h"
#include "Ktcpvs/tcp_vs.h"
//#include "ssl/SslCommon.h"
#include "aosUtil/Mutex.h"
#include "Ktcpvs/aosLogFunc.h"

extern int aos_tcpvs_maxconns;

int aos_ktcpvs_init_cli(char *data, 
				   unsigned int *length, 
				   struct aosKernelApiParms *parms,
				   char *errmsg, 
				   const int errlen)
{
	*length = 0;
	return 0;
}

//
//	Function:
//		check the svc name, find the svc by name and check whether the svc is stopped
//	Parameters:
//	  name: IN, the name of the virtual service
//	  errmsg: OUT, the error message when error occur
//	  svc: OUT, the svc found by the name
//  Return:
//	  0: success
//	  <0: error
// 
int aos_tcpvs_get_stoppedsvc_byname(const char *name, char *errmsg, struct tcp_vs_service **svc, int stop)
{
	struct tcp_vs_ident ident;
	
	errmsg[0] = 0;
	if (!name)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}
	
	// Create 'ident'
	if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, name);
	
	// Check whether the service has already been defined
    *svc = tcp_vs_lookup_byident(&ident);
	if (!(*svc))
	{
		strcpy(errmsg, "Service not found");
		return -eAosRc_ObjectNotFound;
	}	

	if (stop && (*svc)->start)
	{
		strcpy(errmsg, "Service should be stopped first");
		return  -eAosRc_ObjectNotFound;
	}
	
	return eAosRc_Success;	
}

// 
// To add a service:
//	name:		The name of the service
//	sname:		The scheduler name
//	addr:		The local IP address
//	port:		The local port
//	sslFlag:	Whether the service supports SSL (1) or not (0)
//
int aos_tcpvs_add(const char *name,
               const char *sname, 
               u32 addr, 
               u16 port, 
               u8 sslFlag,
               char *errmsg)
{
	int ret;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;
    	struct tcp_vs_config *conf = NULL;
	int svc_found = 0;

	errmsg[0] = 0;

	if (!name)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	if (!sname)
	{
		strcpy(errmsg, "Scheduler name is a null pointer");
		return -eAosRc_NullPointer;
	}

	if (strlen(sname) >= KTCPVS_SCHEDNAME_MAXLEN)
	{
		sprintf(errmsg, "Scheduler name too long. Maximum allowed: %d", 
			KTCPVS_SCHEDNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}

	// Create 'ident'
	if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, name);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
			
	if (svc)
	{
		svc_found = 1;
		if(svc->start)
		{
			strcpy(errmsg, "Service already defined and should be stopped first");
			ret = -eAosRc_ObjectDefined;
			goto out;
		}
	}	

	// 
	// Create a configure
	//
	if (!(conf = aos_malloc(sizeof(*conf)))) 
	{
		strcpy(errmsg, "Failed to allocate memory for config data");
		ret = -eAosRc_MemErr;
        	goto out;
    }
	memset(conf, 0, sizeof(*conf));
    conf->addr = addr; 
    conf->port = htons(port);
    conf->maxSpareServers = 18;
    conf->minSpareServers = 6;
    conf->startservers = 8;
    conf->maxClients = KTCPVS_CHILD_HARD_LIMIT;
    strcpy(conf->sched_name, sname);
		
	/*
	if (sslFlag == eAosAppProxy_SSL) 
		conf->front_ssl_flags = eAosSSLFlag_Server |eAosSSLFlag_Front;
	*/

	if(svc_found)
	{
		ret = tcp_vs_edit_service(svc, conf, errmsg);
	}
	else
	{
		// 
		// Create the virtual service
		//
		ret = tcp_vs_add_service(&ident, conf, errmsg);
	}
out:
	if (conf)
	{
		aos_free(conf);
	}

	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}


// 
// To remove a virtual service:
//	name:		The name of the service
//
int aos_tcpvs_remove(const char *name, char *errmsg)
{
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	errmsg[0] = 0;

	if (!name)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, name);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	
	if (atomic_read(&svc->running)) {
		strcpy(errmsg, "you'd better stop it first before deleting it.");
		ret = -eAosRc_ProgErr;
		goto out;
	}

    ret = tcp_vs_del_service(svc);

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}


// 
// To start a virtual service:
//	name:		The name of the service
//
int aos_tcpvs_start(const char *name, char *errmsg)
{
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	errmsg[0] = 0;

	if (!name)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, name);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	

	svc->start = 1;
	svc->stop = 0;

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}


// 
// To stop a virtual service:
//	name:		The name of the service
//
int aos_tcpvs_stop(const char *name, char *errmsg)
{
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	errmsg[0] = 0;

	if (!name)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, name);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	

	svc->start = 0;
	svc->stop = 1;
	
	while(atomic_read(&svc->running))
	{
		up(&__tcp_vs_mutex);
		
		__set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(100);
		__set_current_state(TASK_RUNNING);
    	
		down(&__tcp_vs_mutex);
		svc = tcp_vs_lookup_byident(&ident);
    	if (!svc)
    	{
        	strcpy(errmsg, "Service not found");
	        ret = -eAosRc_ObjectNotFound;
        	goto out;
    	}
	}

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}


// 
// To stop a virtual service:
//	name:		The name of the service
//
int aos_tcpvs_add_rs(char *vsname,
				__u32 addr,
				__u16 port,
				int   weight, 
				char *errmsg)
{
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	errmsg[0] = 0;

	if (!vsname)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(vsname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, vsname);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	

	ret = tcp_vs_add_dest(svc, addr, htons(port), weight, errmsg);

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}


int aos_ktcpvs_add_rs_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // app proxy attach rs <appname> <addr> <port> [<weight>]
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
		return -eAosRc_CliParmNumMismatch;
	}

	if (!appname)
	{
		strcpy(errmsg, "Missing virtual service name!");
		return -eAosRc_MissingVSName;
	}

	ret = aos_tcpvs_add_rs(appname, addr, port, weight, errmsg);
	return ret;
}


int aos_ktcpvs_del_rs_cli(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // app proxy rs remove <appname> <addr> <port>
    //
    char *appname = parms->mStrings[0];
    int addr = parms->mIntegers[0];
    int port = parms->mIntegers[1];
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	if (parms->mNumIntegers != 2 || parms->mNumStrings != 1)
	{
		strcpy(errmsg, "Incorrect parameters!");
		return -eAosRc_CliParmNumMismatch;
	}

	if (!appname)
	{
		strcpy(errmsg, "Missing virtual service name!");
		return -eAosRc_MissingVSName;
	}

	*length = 0;

	// Create 'ident'
	if (strlen(appname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, appname);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	

	ret = tcp_vs_del_dest(svc, addr, htons(port), errmsg);

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}

int aos_tcpvs_pmi(const char *name, int pmi, char *errmsg)
{
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	errmsg[0] = 0;

	if (!name)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(name) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	strcpy(ident.name, name);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	

	svc->conf.pmi = pmi;

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;
}

/*
int aos_tcpvs_ssl_clientauth(const char *name, int set, char *errmsg)
{
	int ret = 0;
	struct tcp_vs_service *svc = NULL;

	errmsg[0] = 0;

	down(&__tcp_vs_mutex);
    
	if (eAosRc_Success != (ret=aos_tcpvs_get_stoppedsvc_byname(name, errmsg, &svc, 1)))
    {
        up(&__tcp_vs_mutex);
        return ret;
    }

	if ((svc->conf.front_ssl_flags&(eAosSSLFlag_Server|eAosSSLFlag_Front)) != (__u32)(eAosSSLFlag_Server|eAosSSLFlag_Front)) 
	{
		strcpy(errmsg, "application proxy should be set ssl type first");
		up(&__tcp_vs_mutex);
		return -eAosRc_ObjectNotFound;
	}

	if (set)
		svc->conf.front_ssl_flags |= eAosSSLFlag_ClientAuth;
	else
		svc->conf.front_ssl_flags &= ~eAosSSLFlag_ClientAuth;
		
	up(&__tcp_vs_mutex);

	return ret;
}
*/

/*
int aos_tcpvs_ssl_front(const char *name, int set, char *errmsg)
{
	int ret = 0;
    struct tcp_vs_service *svc = NULL;

    errmsg[0] = 0;

    down(&__tcp_vs_mutex);

    if (eAosRc_Success != (ret=aos_tcpvs_get_stoppedsvc_byname(name, errmsg, &svc, 1)))
    {
        up(&__tcp_vs_mutex);
        return ret;
    }

    if (set)
        svc->conf.front_ssl_flags |= eAosSSLFlag_Server|eAosSSLFlag_Front;
    else
        svc->conf.front_ssl_flags &= ~(eAosSSLFlag_Server|eAosSSLFlag_Front);

    up(&__tcp_vs_mutex);

    return ret;
}

int aos_tcpvs_ssl_backend(const char *name, int set, char *errmsg)
{
	int ret = 0;
	struct tcp_vs_service *svc = NULL;

	errmsg[0] = 0;

	down(&__tcp_vs_mutex);

	if (eAosRc_Success != (ret=aos_tcpvs_get_stoppedsvc_byname(name, errmsg, &svc, 1)))
	{
		up(&__tcp_vs_mutex);
		return ret;
	}

	if (set)
		svc->conf.back_ssl_flags |= eAosSSLFlag_Client|eAosSSLFlag_Backend;
	else
		svc->conf.back_ssl_flags &= ~(eAosSSLFlag_Client|eAosSSLFlag_Backend);

	up(&__tcp_vs_mutex);

	return ret;
}

*/

// 
// To set pmi sysname on each virtual service
int aosAppProxy_pmiSysnameCli(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	char *appname = parms->mStrings[0];
	char *sysname = parms->mStrings[1];
	int ret = 0;
	struct tcp_vs_ident ident;
	struct tcp_vs_service *svc;

	*length = 0;

	if (!appname)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(appname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	
	if (!sysname)
    {
        strcpy(errmsg, "Service name is a null pointer");
        return -eAosRc_NullPointer;
    }

    if (strlen(sysname) >= PMI_SYSNAME_MAXLEN)
    {
        sprintf(errmsg, "PMI Sysname too long. Maximum allowed: %d",
            PMI_SYSNAME_MAXLEN);
        return -eAosRc_NameTooLong;
    }

	strcpy(ident.name, appname);

	// 
	// Lock
	//
	down(&__tcp_vs_mutex);

	// 
	// Check whether the service has already been defined
	//
    svc = tcp_vs_lookup_byident(&ident);
		
	if (!svc)
	{
		strcpy(errmsg, "Service not found");
		ret = -eAosRc_ObjectNotFound;
		goto out;
	}	

	strncpy(svc->conf.pmi_sysname, sysname, sizeof(svc->conf.pmi_sysname));

out:
	// 
	// Unlock
	//
	up(&__tcp_vs_mutex);

	return ret;		
}

// 
// To set forward table on or off for each virtual service
int aosAppProxy_forwardTableCli(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	char *appname = parms->mStrings[0];
	char *status = parms->mStrings[1];
	int ret = 0;
	struct tcp_vs_service *svc;

	*length = 0;

	if (!appname)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(appname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	
	// check whether the virtual service is exist and stopped
	down(&__tcp_vs_mutex);

	if (eAosRc_Success != (ret=aos_tcpvs_get_stoppedsvc_byname(appname, errmsg, &svc, 1)))
	{
		up(&__tcp_vs_mutex);
		return ret;
	}

	if (strcmp(status, "on") == 0)
	{
		svc->conf.dynamic_dst = 1;
	}
	else
	{
		svc->conf.dynamic_dst = 0;
	}


	up(&__tcp_vs_mutex);

	return ret;		
}

// 
// To set max supported clients for each virtual service
int aosAppProxy_maxClientsCli(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	char *appname = parms->mStrings[0];
	int  max_clients = parms->mIntegers[0];
	int ret = 0;
	struct tcp_vs_service *svc = NULL;

	*length = 0;
	if (!appname)
	{
		strcpy(errmsg, "Service name is a null pointer");
		return -eAosRc_NullPointer;
	}

	// Create 'ident'
	if (strlen(appname) >= KTCPVS_IDENTNAME_MAXLEN)
	{
		sprintf(errmsg, "Service name too long. Maximum allowed: %d", 
			KTCPVS_IDENTNAME_MAXLEN);
		return -eAosRc_NameTooLong;
	}
	
	// check whether the virtual service is exist and stopped
	down(&__tcp_vs_mutex);
	if (eAosRc_Success != (ret=aos_tcpvs_get_stoppedsvc_byname(appname, errmsg, &svc, 0)))
    {
        up(&__tcp_vs_mutex);
        return ret;
    }
	if ( max_clients > 0 && max_clients <= 10000 )
	{
		svc->conf.maxClients = max_clients;
	}

	up(&__tcp_vs_mutex);

	return ret;		
}

// 
// To set max supported concurrent connection for each proxy system
int aosAppProxy_maxConnsCli(
	char *data, 
	unsigned int *length, 
	struct aosKernelApiParms *parms,
	char *errmsg, 
	const int errlen)
{
	int  max_conns = parms->mIntegers[0];
	int ret = 0;

	*length = 0;
	
	if ( max_conns > 0 && max_conns <= 10000 )
	{
		aos_tcpvs_maxconns  = max_conns;
	}

	return ret;		
}


#define CONN_SESSION_ID_LENGTH	16
#define HEXTOU32( hexStr, uID ) sscanf( hexStr, "%x", &uID );
#define MEMNCPY( Src, Dst, len ) 	\
	for( i=0; i<len; i++ )\
		Src[i]=Dst[i];\
	Src[i]='\0';
	
#define ISHEXCHAR( cByte ) \
	((cByte>='0'&&cByte<='9')||\
	(cByte>='a'&&cByte<='f')||\
	(cByte>='A'&&cByte<='F'))

int aos_ktcpvs_replay_recv_data(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    //app proxy userland recv <session_id> <direct> <on/of>
    //
    char *sessionID = parms->mStrings[0];
    int onORoff=0,direct = parms->mIntegers[0];
    char buf[CONN_SESSION_ID_LENGTH], *switchStr = parms->mStrings[1];
    u64 session_id = 0;
    u32 high_id, low_id;
	int i, ret = 0;
	struct tcp_vs_conn *conn;

	*length = 0;
	
	if( strlen( sessionID ) != CONN_SESSION_ID_LENGTH )
	{
		strcpy(errmsg, "Session ID hex format is vaild (!=16)!");
		return -eAosRc_InvalidValue;
	}
	
	for( i=0; i<CONN_SESSION_ID_LENGTH; i++ )
	{
		if( ISHEXCHAR(sessionID[i]) )continue;
		sprintf(errmsg, "Session ID hex format(%c) is vaild!", sessionID[i] );
		return -eAosRc_InvalidValue;
	}
	MEMNCPY( buf, sessionID, 8 );
	HEXTOU32( buf, high_id );
	sessionID = &parms->mStrings[0][8];
	MEMNCPY( buf, sessionID, 8 );
	HEXTOU32( buf, low_id );
	session_id = (((u64)high_id)<<32)|low_id;

	AOS_DEBUG_PRINTK( "Get session id = %s from user land!\n",\
		aos_LogGetSessionID( session_id ) );
	
	conn = tcp_vs_conn_lookup_byid( session_id );

	if( conn == NULL )
	{
		strcpy(errmsg, "Conn lookup byid is a null pointer");
		return -eAosRc_NullPointer;
	}

	if( !strcmp(switchStr, "on" ) )onORoff = 1;
	else {
		if( !strcmp(switchStr, "off" ) )onORoff = 0;
		else return -eAosRc_InvalidValue;
	}
	switch( direct )
	{
	case 0:
		conn->cs_msg_to_daemon = onORoff;
		break;
	case 1:
		conn->sc_msg_to_daemon = onORoff;
		break;
	default:
		return -eAosRc_InvalidValue;
	}	

	return ret;
}


int aosAppProxy_registerCli(void)
{
	int ret;
	aos_min_log( eAosMD_TcpProxy, "Aos app proxy cli register" );

	ret = OmnKernelApi_addCliCmd("KtcpvsInit",		aos_ktcpvs_init_cli);
    ret |= OmnKernelApi_addCliCmd("app_prox_rs_add", 	aos_ktcpvs_add_rs_cli);
    ret |= OmnKernelApi_addCliCmd("app_prox_rs_del", 	aos_ktcpvs_del_rs_cli);
	
	ret |= OmnKernelApi_addCliCmd("AppProxyAdd",		aosAppProxy_addCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyDel",		aosAppProxy_delCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyShow",		aosAppProxy_showCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyClearAll",	aosAppProxy_clearAllCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySetStatus",	aosAppProxy_setStatusCli);

	/*
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaAdd",		aosAppProxy_aaaAddCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaDel",		aosAppProxy_aaaDelCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyAaaClear",	aosAppProxy_aaaClearCli);
*/
	ret |= OmnKernelApi_addCliCmd("AppProxySetSvrGrp",	aosAppProxy_setGroupCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyResetGroup",	aosAppProxy_resetGroupCli);
	/*
	ret |= OmnKernelApi_addCliCmd("AppProxyShowConn",	aosAppProxy_showConnCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyConnDurSet",	aosAppProxy_setMaxDurationCli);
	*/


	ret |= OmnKernelApi_addCliCmd("AppProxySaveConfig",	aosAppProxy_saveConfigCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyClearConfig",aosAppProxy_clearConfigCli);
//	ret |= OmnKernelApi_addCliCmd("AppProxySetPMI",		aosAppProxy_setPMICli);
	/*
	ret |= OmnKernelApi_addCliCmd("AppProxySslClientAuth",	aosAppProxy_sslClientAuthCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslFront",	aosAppProxy_sslFrontCli);
	ret |= OmnKernelApi_addCliCmd("AppProxySslBackend",	aosAppProxy_sslBackendCli);
	*/
	ret |= OmnKernelApi_addCliCmd("AppProxyPmiSysname",	aosAppProxy_pmiSysnameCli);

	ret |= OmnKernelApi_addCliCmd("AppProxyForwardTable",aosAppProxy_forwardTableCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyMaxClients",aosAppProxy_maxClientsCli);
	ret |= OmnKernelApi_addCliCmd("AppProxyMaxConns",	aosAppProxy_maxConnsCli);
	ret |= OmnKernelApi_addCliCmd("UserlandRecvData", 	aos_ktcpvs_replay_recv_data);
	return ret;
}

int aosAppProxy_unregisterCli(void)
{
	int ret;
	aos_min_log( eAosMD_TcpProxy, "Aos app proxy cli unregister" );

	ret = OmnKernelApi_delCliCmd("KtcpvsInit");
    ret |= OmnKernelApi_delCliCmd("app_prox_rs_add");
    ret |= OmnKernelApi_delCliCmd("app_prox_rs_del");
	
	ret = OmnKernelApi_delCliCmd("AppProxyAdd");
	ret |= OmnKernelApi_delCliCmd("AppProxyDel");
	ret |= OmnKernelApi_delCliCmd("AppProxyShow");
	ret |= OmnKernelApi_delCliCmd("AppProxyClearAll");
	ret |= OmnKernelApi_delCliCmd("AppProxySetStatus");
	/*
	ret |= OmnKernelApi_delCliCmd("AppProxyAaaAdd");
	ret |= OmnKernelApi_delCliCmd("AppProxyAaaDel");
	ret |= OmnKernelApi_delCliCmd("AppProxyAaaClear");
	*/
	ret |= OmnKernelApi_delCliCmd("AppProxySetSvrGrp");
	ret |= OmnKernelApi_delCliCmd("AppProxyResetGroup");

	/*
	ret |= OmnKernelApi_delCliCmd("AppProxyShowConn");
	ret |= OmnKernelApi_delCliCmd("AppProxyConnDurSet");
	*/

	ret |= OmnKernelApi_delCliCmd("AppProxySaveConfig");
	ret |= OmnKernelApi_delCliCmd("AppProxyClearConfig");
	ret |= OmnKernelApi_delCliCmd("AppProxySetPMI");
	/*
	ret |= OmnKernelApi_delCliCmd("AppProxySslClientAuth");
	ret |= OmnKernelApi_delCliCmd("AppProxySslFront");
	ret |= OmnKernelApi_delCliCmd("AppProxySslBackend");
	ret |= OmnKernelApi_delCliCmd("AppProxyPmiSysname");
	*/

	ret |= OmnKernelApi_delCliCmd("AppProxyForwardTable");
	ret |= OmnKernelApi_delCliCmd("AppProxyMaxClients");
	ret |= OmnKernelApi_delCliCmd("AppProxyMaxConns");
	return ret;
}
