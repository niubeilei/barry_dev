////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_wrapper.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "proxy_wrapper.h"
#include "proxy_service.h"
#include "aos/aosKernelApi.h"

AosSockWrapperType_t	* gSockWrapperTypes[MAX_WRAPPER];

int
find_sw_by_pointer( AosSockWrapperType_t * wt )
{
	int i;
	for(i=0;i<MAX_WRAPPER;i++)
	{
		if( gSockWrapperTypes[i] == wt )
		{
			return i;
		}
	}
	return -1;
}


int
find_sw_by_name( char * name )
{
	int i;
	for(i=0;i<MAX_WRAPPER;i++)
	{
		if( gSockWrapperTypes[i] &&  0 == strcmp(name, gSockWrapperTypes[i]->name) )
		{
			return i;
		}
	}
	return -1;
}


/*
static inline int
in_list( AosSockWrapperType_t * list[], AosSockWrapperType_t * wt )
{
	int i;
	for( i=0; i<MAX_WRAPPER; i++)
	{
		if( list[i] == wt )
			return i;
	}

	return -1;
}

static inline int
add_list( AosSockWrapperType_t * list[], AosSockWrapperType_t * wt )
{
	int i;

	for( i=0; i<MAX_WRAPPER; i++)
	{
		if( list[i] == 0 )
		{
			list[i] = wt;
			return i;
		}
	}

	return -1;
}

static inline int
remove_list( AosSockWrapperType_t * list[], int pos )
{
	int i;
	for( i=pos; i<MAX_WRAPPER-1; i++)
	{
		list[pos] = list[pos+1];
	}

	list[MAX_WRAPPER-1]=0;

	return 0;
}
*/



/*
static int
AosProxy_setWrapper( ProxyService_t * svc, AosSockWrapperType_t * wt, char * direction, char *onoff)
{

	if( 0 == strcmp("front",direction) )
	{
		svc->frontWt = wt;
	}
	else
	{
		svc->backWt = wt;
	}
	return 0;
}
*/

/*
static int
AosProxy_cliAttr(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //
    // proxy attr <appname> front/backend <attrname> on/off
    //
    char *appname 	= parms->mStrings[0];
    char *direction = parms->mStrings[1];
    char *attrname 	= parms->mStrings[2];
    char *onoff 	= parms->mStrings[3];
    int ret;

	ProxyService_t	* svc;

	*length = 0;

	aos_trace(KERN_INFO "proxy attr %s %s %s %s\n", appname,direction,attrname,onoff);
	svc = AosProxy_getServiceByName( appname );
	if (!svc)
	{
		strcpy(errmsg, "Application proxy not found");
		return 1;
	}
	if ( svc->status == SERVICE_STATUS_ON )
	{
		strcpy(errmsg, "Application proxy is running");
		return 1;
	}

	if ( strcmp(direction,"front") && strcmp(direction,"backend") )
	{
		strcpy(errmsg, "front/backend error");
		return 1;
	}
	if ( strcmp(onoff,"on") && strcmp(onoff,"off") )
	{
		strcpy(errmsg, "on/off error");
		return 1;
	}

	ret = find_sw_by_name( attrname );
	if( ret < 0 )
	{
		strcpy(errmsg, "attribute not found");
		return 1;
	}

	ret = AosProxy_setWrapper(svc,gSockWrapperTypes[ret],direction,onoff);
	return ret;
}
*/

int AosProxy_registerWrapperType( AosSockWrapperType_t * wt)
{
	int	i;

	i = find_sw_by_name( wt->name );
	if( i >= 0 )
	{
		aos_trace( KERN_ERR "sock wrapper type %s already exists\n", wt->name );
		return -EEXIST;
	}

	if( !wt->create 
			|| !wt->release 
			|| !wt->readyAcceptData 
			|| !wt->dataReceived 
			|| !wt->dataAckReceived 
			|| !wt->sendData 
			|| !wt->sendFin 
			|| !wt->passiveConnected
			|| !wt->finReceived
			|| !wt->peerWritable
			|| !wt->connect
			|| !wt->isWritable
			)
	{
		return -1;
	}

	for( i=0; i<MAX_WRAPPER; i++ )
	{
		if( 0 == gSockWrapperTypes[i] )
		{
			gSockWrapperTypes[i] = wt;
			return 0;
		}
	}
	return -1;
}

AosSockWrapperType_t * 
AosProxy_unregisterWrapperType( AosSockWrapperType_t * wt )
{
	int	i;

	i = find_sw_by_pointer( wt );
	if( i < 0 )
	{
		return 0;
	}

	gSockWrapperTypes[i] = 0;
	return wt;
}

EXPORT_SYMBOL( AosProxy_registerWrapperType );
EXPORT_SYMBOL( AosProxy_unregisterWrapperType );

/*
int
AosProxy_registerSockWrapperCli(void)
{
	int ret=0;
	
	ret |= OmnKernelApi_addCliCmd("ProxyAttr", AosProxy_cliAttr);
	
	return ret;
}

int
AosProxy_unregisterSockWrapperCli(void)
{
	
	OmnKernelApi_delCliCmd("ProxyAttr");
	return 0;
}

*/
