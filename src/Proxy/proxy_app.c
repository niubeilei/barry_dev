////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_app.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "proxy_app.h"
#include "proxy_service.h"



struct list_head	gAppProxyList;

AppProxy_t * 
AosProxy_hasAppType(char *type)
{
	AppProxy_t	*p;

	list_for_each_entry(p,&gAppProxyList,list)
	{
		if( 0==strcmp(p->name,type) )
		{
			return p;
		}
	}
	return 0;
}

int
AosProxy_registerAppProxy(AppProxy_t *app)
{
	if(AosProxy_hasAppType(app->name))
	{
		aos_trace(KERN_ERR "app proxy %s already exists\n",app->name);
		return -1;
	}

	if( !app->readyAcceptData
			|| !app->dataReceived
			|| !app->dataAckReceived
			|| !app->passiveConnected
			|| !app->finReceived
			|| !app->resetConnection
	  )
	{
		aos_trace( "app proxy %s register error\n",app->name);
		return -1;
	}
			

	app->svcCount=0;
	list_add(&app->list, &gAppProxyList);
	
	aos_trace(KERN_INFO "app proxy %s added\n",app->name);
	return 0;
}

EXPORT_SYMBOL(AosProxy_registerAppProxy);

AppProxy_t *
AosProxy_unregisterAppProxy(char *name)
{
	AppProxy_t *p=AosProxy_hasAppType(name);
	if(!p)
	{
		aos_trace(KERN_EMERG "aosproxy unregister appproxy: can not find app type:%s",name);
		return p;
	}

	if(p->svcCount>0)
	{
		aos_trace(KERN_INFO "aosproxy unregister appproxy: svc_count>0, app type:%s",name);
		return p;
	}
	list_del(&p->list);
	
	return p;
}

EXPORT_SYMBOL(AosProxy_unregisterAppProxy);

