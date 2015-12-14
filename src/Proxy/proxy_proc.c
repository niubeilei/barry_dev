////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_proc.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#define AOS_DEBUG

#include "proxy_proc.h"
#include "proxy_app.h"
#include "proxy_service.h"
#include "proxy_wrapper.h"
#include "proxy_scheduler.h"

static struct proc_dir_entry *proc_proxy;

static int
AosProxy_procReadSchedulers(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int	end=0;
	struct proxy_scheduler *p;

	if(offset > 0)
		return 0;

	list_for_each_entry(p,&proxy_schedulers,n_list)
	{
		sprintf(&buf[end],"%s\n",p->name);
		end=strlen(buf);
	}
	buf[end]=0;
	
	return strlen(buf);
}


static int
AosProxy_procReadTypes(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int	end=0;
	AppProxy_t *p;

	if(offset > 0)
		return 0;

	list_for_each_entry(p,&gAppProxyList,list)
	{
		sprintf(&buf[end],"%s %d\n",p->name,p->svcCount);
		end=strlen(buf);
	}
	buf[end]=0;
	
	return strlen(buf);
}

static int
AosProxy_procReadWrappers(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int	end=0;
	int	i;

	if(offset > 0)
		return 0;

	for( i=0; i<MAX_WRAPPER; i++)
	{
		if( gSockWrapperTypes[i] )
		{
			sprintf( &buf[end], "%s ", gSockWrapperTypes[i]->name );
			end = strlen(buf);
		}
	}

	sprintf( &buf[end], "\n" );
	end = strlen(buf);

	buf[end]=0;
	return strlen(buf);
}

static int
AosProxy_procReadPo(char *buf, char **start, off_t offset, int count, int *eof, void *data)
{
	int		end=0;
	
	ProxyService_t *svc;


#ifdef AOS_DEBUG
	ProxyObject_t * po;
	char * stateName[]={ "TCP_ESTABLISHED", "TCP_SYN_SENT", 
		"TCP_SYN_RECV", "TCP_FIN_WAIT1", "TCP_FIN_WAIT2", 
		"TCP_TIME_WAIT", "TCP_CLOSE", "TCP_CLOSE_WAIT", 
		"TCP_LAST_ACK", "TCP_LISTEN", "TCP_CLOSING",
		"TCP_MAX_STATES"};
#endif

	if(offset > 0)
		return 0;

	list_for_each_entry(svc,&gProxyServiceList,list)
	{
		sprintf(&buf[end],
				//"%s %s %u.%u.%u.%u:%d %u.%u.%u.%u:%d %s %s %s %d %d\n",
				"%s %s %u.%u.%u.%u:%d %s %s %d %s %s %d\n",
				svc->name,
				svc->app->name,
				NIPQUAD(svc->listenIp),
				svc->listenPort,
			//	NIPQUAD(svc->backIp),
			//	svc->backPort,
				svc->frontWt->name,
				svc->backWt->name,
				svc->clientauth,
				svc->scheduler->name,
				svc->status?"on":"off",
				atomic_read(&svc->poCount)
				);

		end=strlen(buf);
		
		int i;
		for(i=0;i<svc->destCount;i++)
		{
			sprintf(&buf[end]," Real Server: %u.%u.%u.%u:%d %d\n",
					NIPQUAD(svc->destinations[i]->addr),
					svc->destinations[i]->port,
					atomic_read(&svc->destinations[i]->weight)
					);
			end=strlen(buf);
		}

		/*
		ProxyDest_t	* dest;
		list_for_each_entry(dest,&svc->destinations,n_list)
		{
			sprintf(&buf[end]," Real Server: %u.%u.%u.%u:%d\n",
					NIPQUAD(dest->addr),
					dest->port
					);
			end=strlen(buf);
		}
		*/
	
#ifdef AOS_DEBUG
		list_for_each_entry(po,&svc->poList,list)
		{
			sprintf(&buf[end],"\t%s %s\n",
					stateName[po->frontSk->sk_state-1],
					stateName[po->backSk->sk_state-1]
					);
			end=strlen(buf);
		}
#endif

	
	}
	
	buf[end]=0;
	
	return strlen(buf);
}

int AosProxy_registerProc(void)
{
	proc_proxy = proc_mkdir("proxy",&proc_root);
	if(!proc_proxy)
	{
		aos_trace(KERN_ERR "proc_mkdir proxy failed!\n");
		goto failed;
	}

	if ( 0 == create_proc_read_entry("schedulers", 0444, proc_proxy,AosProxy_procReadSchedulers,0) )
	{
		aos_trace(KERN_ERR "create_proc_entry schedulers failed!\n");
		goto release_proxy;
	}

	if ( 0 == create_proc_read_entry("types", 0444, proc_proxy,AosProxy_procReadTypes,0) )
	{
		aos_trace(KERN_ERR "create_proc_entry types failed!\n");
		goto release_schedulers;
	}

	if ( 0 == create_proc_read_entry("po", 0444, proc_proxy,AosProxy_procReadPo,0) )
	{
		aos_trace(KERN_ERR "create_proc_entry po failed!\n");
		goto release_types;
	}

	if ( 0 == create_proc_read_entry("wrappers", 0444, proc_proxy, AosProxy_procReadWrappers,0) )
	{
		aos_trace(KERN_ERR "create_proc_entry wrappers failed!\n");
		goto release_po;
	}



	aos_trace(KERN_INFO "Proxy create proc ok\n");
	return 0;


release_po:
	remove_proc_entry("po",proc_proxy);
release_types:
	remove_proc_entry("types",proc_proxy);
release_schedulers:
	remove_proc_entry("schedulers",&proc_root);
release_proxy:
	remove_proc_entry("proxy",&proc_root);
failed:
	return -1;
}



void AosProxy_unregisterProc(void)
{
	remove_proc_entry("wrappers", proc_proxy);
	remove_proc_entry("services", proc_proxy);
	remove_proc_entry("po", proc_proxy);
	remove_proc_entry("types", proc_proxy);
	remove_proc_entry("Proxy", proc_proxy);
	remove_proc_entry("proxy",&proc_root);
}


