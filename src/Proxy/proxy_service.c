////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_service.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//#define AOS_DEBUG


#include "aosUtil/Memory.h"

#include "proxy_scheduler.h"
#include "proxy_service.h"
#include "proxy_app.h"

#include <net/tcp.h>

struct list_head 	gProxyServiceList;
kmem_cache_t		*gProxyObjectCachep;

inline int AosProxy_poRelease1( struct ProxyObject * po )
{
	ProxyService_t * svc = po->svc;

	aos_list_del( &po->timer.entry );

	if ( po->frontWo )
	{
		po->frontWo->wt->release( po->frontWo );
		po->frontWo = 0;
	}

	if ( po->backWo )
	{
		po->backWo->wt->release( po->backWo );
		po->backWo = 0;
	}

	atomic_dec(&po->svc->poCount);
	list_del(&po->list);
	kmem_cache_free( gProxyObjectCachep, po );

	if( svc->app->poReleased )
	{
		svc->app->poReleased( svc );
	}
	return 0;
}


inline int AosProxy_poRelease( struct ProxyObject * po )
{
	ProxyService_t * svc = po->svc;

	aos_list_del( &po->timer.entry );

	po->frontSk->sk_proxy_data = po->backSk->sk_proxy_data = 0;
	if ( po->frontWo )
	{
		po->frontWo->wt->release( po->frontWo );
		po->frontWo = 0;
	}

	if ( po->backWo )
	{
		po->backWo->wt->release( po->backWo );
		po->backWo = 0;
	}

	atomic_dec(&po->svc->poCount);
	list_del(&po->list);
	kmem_cache_free( gProxyObjectCachep, po );

	if( svc->app->poReleased )
	{
		svc->app->poReleased( svc );
	}
	return 0;
}


ProxyService_t *AosProxy_getServiceByName(char *name)
{
	ProxyService_t * svc;	
	list_for_each_entry(svc,&gProxyServiceList,list)
	{
		if(0==strcmp(name,svc->name))
			return svc;
	}
	return 0;
}

static void AosProxy_poUpdate( struct aos_list_head * list )
{
	AosPackTimer_t	*ptr, *tmpptr;
	ProxyObject_t	*po;

	aos_list_for_each_entry_safe(ptr,tmpptr,list,entry)
	{
		po=(ProxyObject_t*)ptr->data;
		if ( jiffies - po->lut > PROXY_TIMER_TIMEOUT )
		{
			aos_trace("This po expired\n");
			//sock_hold(po->frontSk);
			if(po->frontSk)
			{
				po->frontSk->sk_proxy_data = 0;
				//tcp_reset( po->frontSk );
				mb();
				tcp_close_aos( po->frontSk,0 );
			}
			//sock_put(po->frontSk);
			//sock_hold(po->backSk);
			if(po->backSk)
			{
				po->backSk->sk_proxy_data = 0;
				//tcp_reset( po->backSk );
				mb();
				tcp_close_aos( po->backSk,0 );
			}
			//sock_put(po->backSk);
			AosProxy_poRelease1( po );
			/*
			AosProxy_poRelease( po );
			tcp_close_aos(po->frontSk,0);
			tcp_close_aos(po->backSk,0);
			*/

		}
		else
		{
			aos_list_del( & ptr->entry );
			aos_debug("This po does not expired\n");
			po->timer.expires = po->svc->pack.pack_tick + PROXY_TIMER_TIMEOUT / po->svc->pack.freq_msec;
			AosTimerPack_addInternal( &po->svc->pack, &po->timer );
		}
	}
}


int AosProxy_serviceAdd(char * appname, char * type, int addr, int port, 
		AosSockWrapperType_t * fwt,
		AosSockWrapperType_t * bwt,
		struct proxy_scheduler * scheduler,
		char *errmsg)
{
	ProxyService_t * svc;
	
	AppProxy_t	*p;
	p=AosProxy_hasAppType(type);
	if(!p)
	{
		if(errmsg)
			sprintf(errmsg,"no such service type:%s",type);
		return 1;
	}
	svc=AosProxy_getServiceByName(appname);
	if(svc)
	{
		if(errmsg)
			sprintf(errmsg,"service %s exist",appname);
		return 1;
	}

	svc=aos_malloc(sizeof(*svc));
	if(!svc)
	{
		if(errmsg)
			sprintf(errmsg,"malloc service struct error");
		aos_trace(KERN_EMERG "kmalloc svc error\n");
		return 1;
	}
	memset(svc,0,sizeof(*svc));
	INIT_LIST_HEAD( &svc->poList );
	//INIT_LIST_HEAD( &svc->destinations );
	svc->destCount=0;

	AosTimerPack_init( &svc->pack, PROXY_TIMER_ACCURATE, AosProxy_poUpdate );

	list_add(&svc->list,&gProxyServiceList);
//	svc->poCount=0;
	atomic_set(&svc->poCount, 0);
	svc->status=0;
	svc->listenSock=0;
	strncpy(svc->name,appname,PROXY_SERVICE_NAME_LEN);
	svc->listenIp=addr;
	svc->listenPort=port;
	svc->app=p;
	try_module_get(p->owner);
	p->svcCount++;
	svc->frontWt = fwt;
	svc->backWt = bwt;
	proxy_bind_scheduler(svc, scheduler);
	return 0;
}

int AosProxy_getDest( ProxyService_t * svc, __u32 ip, __u16 port )
{

	ProxyDest_t * dest;
	int	i;

	for( i=0; i<svc->destCount; i++ )
	{
		dest = svc->destinations[i];

		if(dest->addr==ip && dest->port==port)
		{
			return i;
		}

	}

	/*

	list_for_each_entry( dest, &svc->destinations, n_list )
	{
		if(dest->addr==ip && dest->port==port)
		{
			return dest;
		}
	}
*/
	return -1;
}

int AosProxy_rsAdd(char *name, u32 addr, u16 port, int weight, char *errmsg)
{
	ProxyService_t	*svc;
	ProxyDest_t		*dest;
	int	ret;

	svc=AosProxy_getServiceByName(name);
	if(!svc)
	{
		if(errmsg)
			sprintf(errmsg,"service %s not found",name);
		return -1;
	}

	if( svc->destCount >= MAX_REAL_SERVER )
	{
		if(errmsg)
			sprintf(errmsg,"the number of real servers reach %d",MAX_REAL_SERVER);
		return -1;
	}

	ret = AosProxy_getDest( svc, addr, port );
	if( ret>=0 )
	{
		if(errmsg)
			sprintf(errmsg,"real server %u.%u.%u.%u:%d already exist",NIPQUAD(addr),port);
		return -1;
	}

	dest = aos_malloc( sizeof(*dest) );
	if(!dest)
	{
		proxy_alarm( "aos_malloc dest error\n" );
		return -1;
	}
	dest->addr = addr;
	dest->port = port;
	if(weight==0)
	{
		weight=1;
	}
	atomic_set(&dest->weight,weight);
	//list_add( &dest->n_list, &svc->destinations );
	svc->destinations[svc->destCount++] = dest;	
	svc->scheduler->update_service(svc);
	return 0;
}

int AosProxy_rsRemove(char *name, u32 addr, u16 port, char *errmsg)
{
	ProxyService_t	*svc;
	int	dest, i;
	
	svc=AosProxy_getServiceByName(name);
	if(!svc)
	{
		if(errmsg)
			sprintf(errmsg,"service %s not found",name);
		return -1;
	}
	dest = AosProxy_getDest( svc, addr, port );
	if( dest<0 )
	{
		if(errmsg)
			sprintf(errmsg,"real server %u.%u.%u.%u:%d not found", NIPQUAD(addr), port );
		return -1;
	}
	aos_free( svc->destinations[dest] );
	for( i=dest; i<svc->destCount-1; i++)
	{
		svc->destinations[i]=svc->destinations[i+1];
	}
	svc->destCount--;

	return 0;
}

int __AosProxy_serviceStop(ProxyService_t *svc)
{
	ProxyObject_t	*po;

	if(svc->status==SERVICE_STATUS_OFF)
		return 0;

	svc->listenSock->sk->sk_user_data = 0;
	sock_release(svc->listenSock);

	AosTimerPack_stop( &svc->pack );

	while(!list_empty(&svc->poList))
	{
		po=list_entry(svc->poList.next,ProxyObject_t,list);
		//list_del(&po->list);
		if(po->frontSk)
		{
			po->frontSk->sk_proxy_data = 0;
			tcp_close_aos( po->frontSk,0 );
		}

		if(po->backSk)
		{	
			po->backSk->sk_proxy_data = 0;
			tcp_close_aos( po->backSk,0 );
		}
		AosProxy_poRelease(po);
		//tcp_close_aos(po->frontSk,0);
		//tcp_close_aos(po->backSk,0);

	}
	svc->status=SERVICE_STATUS_OFF;
	return 0;
}



int __AosProxy_serviceStart(ProxyService_t *svc)
{
	if(svc->status==SERVICE_STATUS_ON)
		return 0;

	int error;
	struct sockaddr_in sin;
	
	atomic_set(&svc->poCount,0);
	error = sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&svc->listenSock);
	if ( error < 0 )
	{
		aos_trace(KERN_ERR "Proxy can not create a sock\n");
		return error;
	}

	sin.sin_family=AF_INET;
	sin.sin_addr.s_addr = svc->listenIp;
	sin.sin_port = htons(svc->listenPort);
	svc->listenSock->sk->sk_reuse=1;

	error = svc->listenSock->ops->bind(svc->listenSock,(struct sockaddr *)&sin,sizeof(sin));
	if(error<0)
	{
		aos_trace(KERN_ERR "%d Proxy can not bind sock: %u.%u.%u.%u:%d\n",
				error, NIPQUAD(svc->listenIp), svc->listenPort);
		sock_release(svc->listenSock);
		return error;
	}

	error = svc->listenSock->ops->listen(svc->listenSock, 10);
	if(error<0)
	{
		aos_trace(KERN_ERR "Proxy can not listen sock\n");
		sock_release(svc->listenSock);
		return error;
	}
	
	AosTimerPack_start( &svc->pack );

	svc->listenSock->sk->sk_user_data = svc;
	aos_trace(KERN_INFO "Proxy listen on: %u.%u.%u.%u:%d\n",
				NIPQUAD(svc->listenIp), svc->listenPort);

	svc->status=SERVICE_STATUS_ON;
	return 0;
}

int AosProxy_serviceStart(char * name, char *errmsg)
{
	ProxyService_t	*svc;
	int	ret;

	svc=AosProxy_getServiceByName(name);
	if(!svc)
	{
		if(errmsg)
			sprintf(errmsg,"service %s not found",name);
		return 1;
	}
	
	if( !svc->backWt )
	{
		if(errmsg)
			sprintf(errmsg,"service %s has no real server",name);
		return -1;
	}
	
	ret = __AosProxy_serviceStart(svc);	
	if(ret)
	{
		if(errmsg)
			sprintf(errmsg,"can not start service %s",name);
	}
	else
	{
		if(errmsg)
			sprintf(errmsg,"start service %s success",name);
	}
	return ret;
}


int AosProxy_serviceStop(char * name, char *errmsg)
{
	ProxyService_t	*svc;

	svc=AosProxy_getServiceByName(name);
	if(!svc)
	{
		if(errmsg)
		{
			sprintf(errmsg,"service %s not found",name);
		}

		return 1;
	}
	return __AosProxy_serviceStop(svc);	

}

EXPORT_SYMBOL(AosProxy_serviceRemove);
int AosProxy_serviceRemove( ProxyService_t * svc )
{
	svc->app->svcCount--;
	list_del(&svc->list);
	module_put(svc->app->owner);
	kfree(svc);

	return 0;
}



EXPORT_SYMBOL(__AosProxy_serviceStop);
EXPORT_SYMBOL(AosProxy_serviceStart);
EXPORT_SYMBOL(AosProxy_rsAdd);
EXPORT_SYMBOL(AosProxy_serviceAdd);
EXPORT_SYMBOL(AosProxy_getServiceByName);
EXPORT_SYMBOL(AosProxy_poRelease);

