////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: proxy_main.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//#define AOS_DEBUG

#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "proxy_service.h"
#include "proxy_wrapper.h"
#include "proxy_cli.h"
#include "proxy_proc.h"
#include "proxy_hook.h"
#include "aosUtil/TimerPack.h"
#include <net/tcp.h>

int g_proxy_concurrent=2000;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zzh");

//#define RECEIVE_BUFFER_LEN (1448)
#define RECEIVE_BUFFER_LEN (1024*2)


static int AosProxy_receive(struct sock * sk, char * buffer, int len)
{
	struct msghdr 	msg;
	struct iovec	iov;
	mm_segment_t	oldfs;

	memset(&msg,0,sizeof(msg));
	msg.msg_iovlen=1;
	msg.msg_iov=&iov;
	iov.iov_len=len;
	iov.iov_base=buffer;
	oldfs=get_fs();
	set_fs(KERNEL_DS);
//	len = sock_recvmsg(sock, &msg, len, MSG_DONTWAIT);
	len = tcp_recvmsg(
			0, // iocb
			sk, 
			&msg,
			len,
			1,// int nonblock.
			0, //flags.
			0 //int * addr_len, because our flags is 0, so we need not this parameter.
			);
	
	set_fs(oldfs);

	return len;
	
}


static int
AosProxy_transferData(struct sock * sk)
{
	int		len=RECEIVE_BUFFER_LEN;
	char	*buffer;

	AosSockWrapperObject_t	* wo = (AosSockWrapperObject_t*) sk->sk_proxy_data;
	//struct sock *other_sk = (sk==wo->po->frontSk) ? wo->po->backSk : wo->po->frontSk ;
	aos_debug("AosProxy_transferData()\n");

	BUG_TRAP( sk );
	BUG_TRAP( wo->wt );

	// receive and send
	while( !skb_queue_empty(&sk->sk_receive_queue) )
	{
		
		buffer = aos_malloc_atomic( RECEIVE_BUFFER_LEN );
		if( !buffer )
		{
			printk("malloc error\n");
			BUG();	
		}

		len=AosProxy_receive(sk,buffer,RECEIVE_BUFFER_LEN);
		aos_debug("AosProxy_transferData() proxy receive %d bytes\n",len);

		if(len>0)
		{
			BUG_TRAP( wo->wt );
			len = wo->wt->dataReceived( wo, buffer, len );
/*
			if( len<0 )
			{
				proxy_alarm("wo->wt->dataReceived() return <0, close the connections");
				AosProxy_poRelease( wo->po );
				tcp_close_aos(sk,0);
				tcp_close_aos(other_sk,0);
			}
*/
		}
		else
		{
			aos_free( buffer );
		}
	}
	return 0;
}

int AosProxy_relayData(struct sock * sk)
{
	int		len=RECEIVE_BUFFER_LEN;
	int		len_sent;
	char	*buffer;

	AosSockWrapperObject_t	* wo = (AosSockWrapperObject_t*) sk->sk_proxy_data;
	struct sock *other_sk = (sk==wo->po->frontSk) ? wo->po->backSk : wo->po->frontSk ;
	aos_debug("AosProxy_relayData(),wo=%p, sk=%p\n",wo,sk);

	wo->po->lut = jiffies;
	// receive and send
	while( !skb_queue_empty(&sk->sk_receive_queue) && wo->wt->readyAcceptData(wo) )
	{
		
		aos_debug("wrapper is ready to receive data, wo=%p, sk=%p\n",wo,sk);
		buffer = aos_malloc_atomic( RECEIVE_BUFFER_LEN );
		if( !buffer )
		{
			proxy_alarm("malloc receive buffer error\n");
			BUG();	
		}

		len=AosProxy_receive(sk,buffer,RECEIVE_BUFFER_LEN);
		aos_debug("send %d bytes from hook to wrapper, wo=%p, sk=%p\n",len,wo,sk);

		if(len>=0)
		{
			len_sent = wo->wt->dataReceived( wo, buffer, len );
			if( len_sent<0 )
			{
				proxy_alarm("wo->wt->dataReceived() return <0, close the connections");
				AosProxy_poRelease( wo->po );
				tcp_close_aos(sk,0);
				tcp_close_aos(other_sk,0);
			}
	
		}
	}
	return 0;
}

static int
AosProxyHook_dataAckReceived(struct sock * sk)
{
	AosSockWrapperObject_t	* wo = (AosSockWrapperObject_t*) sk->sk_proxy_data;
	
	aos_debug("AosProxyHook_dataAckReceived()\n");
	wo->wt->dataAckReceived( wo );


	/*
	if( sk == wo->po->frontSk )
	{
		AosProxy_relayData( wo->po->backSk );
	}
	else 
	{
		AosProxy_relayData( wo->po->frontSk );
	}
	*/

	return 0;
}


static int
AosProxyHook_dataReceived(struct sock * sk)
{
	/*
	if( !sk->sk_socket )
	{
		printk(KERN_ERR " sk->sk_state = %d \n", sk->sk_state );
	}
	else
	*/

	{
		aos_debug(KERN_ERR "Proxy_data_received hook\n");
		AosProxy_relayData( sk );
	}

	return 0;
}

/*
 * when a client sends SYN, it's state become SYN_SENT
 * later, the client receives SYN/ACK or RESET.
 * then this function will be fired AFTER linux's process
 * This will only occure on the backend connection
 */
static int
AosProxyHook_backConnectionCreated(struct sock * sk)
{
	AosSockWrapperObject_t	* wo = (AosSockWrapperObject_t *)sk->sk_proxy_data;
	struct sock * other_sk = (sk==wo->po->frontSk) ? wo->po->backSk : wo->po->frontSk ;

	if(sk->sk_state!=TCP_ESTABLISHED)
	{
		aos_debug(KERN_ERR "Proxy_back_connection_created hook, back send reset\n");
	}
	else
	{
		// now sk->sk_state == TCP_ESTABLISHED
		aos_debug(KERN_INFO "Proxy_back_connection_created hook, connection success\n");
		
		// tell the wrapper that the backend connection is established
		// the wrapper may do nothing
		if( wo->wt->activeConnected )
		{
			if( wo->wt->activeConnected(wo)<0 )
			{
				AosProxy_poRelease(wo->po);
				tcp_close_aos(sk,0);
				tcp_close_aos(other_sk,0);
			}
		}
		
		if( wo->po->frontSk->sk_state == TCP_ESTABLISHED )
		{
			// if the frontend connection was established before.
			// we must check to see if there is data in the front receive queue and tranfer it
			aos_debug(KERN_INFO "Proxy_back_connection_created hook, try to relay from front to backend\n");
			AosProxy_relayData( wo->po->frontSk );
		}

	}
	return 0;
}

static int
AosProxyHook_allowFrontConnect(struct sock * listen_sk)
{
	ProxyService_t	* svc;

	aos_debug("AosProxyHook_allowFrontConnect()\n");
	svc=(ProxyService_t*)listen_sk->sk_user_data;

	return atomic_read(&svc->poCount) < g_proxy_concurrent;
}

static int
AosProxyHook_frontConnectionCreated(struct sock * listen_sk)
{
	ProxyObject_t 	* po;
	ProxyService_t	* svc;
	int 			err;

	aos_debug("AosProxyHook_frontConnectionCreated()\n");
	svc=(ProxyService_t*)listen_sk->sk_user_data;
	
	po=kmem_cache_alloc(gProxyObjectCachep,GFP_ATOMIC);
	if(!po)
	{
		aos_trace(KERN_EMERG "Proxy allocate po failed\n");
		goto out;
	}
	memset( po, 0, sizeof(*po) );

	po->frontSk = tcp_accept( listen_sk, O_NONBLOCK, &err );
	if( !po->frontSk )
	{
		aos_trace(KERN_ERR "Proxy accept failed\n");
		goto release_po;
	}

	po->svc=svc;

	// Create front Wrapper
	po->frontWo = svc->frontWt->create();
	if (!po->frontWo)
	{
		aos_trace(KERN_ERR "Proxy front wrapper create failed\n");
		goto release_front_sock;
	}
	po->frontWo->po = po;
	po->frontWo->sk = po->frontSk;
	po->frontSk->sk_proxy_data = po->frontWo;
	po->frontSk->sk_allocation = GFP_ATOMIC;

	// create back wrapper
	po->backWo = svc->backWt->create();
	if(!po->backWo)
	{
		aos_trace(KERN_ERR "Proxy back wrapper create failed\n");
		goto release_front_wo;
	}
	po->backWo->po = po;

	// call front wrapper
	if ( po->frontWo->wt->passiveConnected(po->frontWo) )
	{
		goto release_back_wo;
	}
	po->backSk->sk_allocation = GFP_ATOMIC;
	sock_put( po->backSk );

	aos_debug("connect bo backend\n");

	// the backSock is created
	//po->backSock = po->backWo->sock;
	//po->backSock->sk->sk_proxy_data = po->backWo;

	// add po to the list of svc
	atomic_inc(&svc->poCount);
//	svc->poCount++;
	list_add(&po->list,&svc->poList);

	po->lut = jiffies;

	AosPackTimer_init( &svc->pack, &po->timer, PROXY_TIMER_TIMEOUT, (__u32)po );
	AosTimerPack_addInternal( &svc->pack, &po->timer );

	return 0;

release_back_wo:
	po->backWo->wt->release( po->backWo );
release_front_wo:
	po->frontWo->wt->release( po->frontWo );
release_front_sock:
//	sock_release(po->frontSk);
//	kmem_cache_free(po->frontSk->sk_slab, po->frontSk);
	po->frontSk->sk_proxy_data = 0;
	tcp_close_aos( po->frontSk, 0 );
release_po:
	kmem_cache_free(gProxyObjectCachep,po);
out:
	return 0;
}

/* 
 * If a TCP packet with a reset flag in the TCP header
 * Then this function will be fired AFTER linux tcp stack's processing
 * Either frontend or backend
 */
static int
AosProxyHook_resetReceived(struct sock * sk)
{
	AosSockWrapperObject_t	*wo = (AosSockWrapperObject_t*) sk->sk_proxy_data;
	ProxyObject_t * po = wo->po;
	struct sock	* other_sk;
	
	aos_debug("AosProxyHook_resetReceived\n");
	
	other_sk = (sk==po->frontSk) ? po->backSk : po->frontSk ;
	// testing
	AosProxy_poRelease( wo->po );
	tcp_close_aos( other_sk, 0);
	tcp_close_aos( sk,0 );
	return 0;
	
}


/*
 * If a TCP packet with a FIN flag in the header, either front or backend
 * this function will be fired AFTER linux tcp stack's processing
 *
 */ 
static int
AosProxyHook_finReceived(struct sock * sk)
{

	BUG_TRAP( sk );
	BUG_TRAP( sk->sk_proxy_data );

	AosSockWrapperObject_t	*wo = (AosSockWrapperObject_t*) sk->sk_proxy_data;
	struct sock * other_sk = ( wo->po->frontSk == sk ) ? wo->po->backSk : wo->po->frontSk ;
	//AosSockWrapperObject_t	*other_wo;

	AosProxy_transferData( sk );

	aos_debug("AosProxyHook_finReceived, %d\n",sk->sk_state);
	aos_debug("state: %d,%d\n",sk->sk_state,other_sk->sk_state);

	wo->wt->finReceived( wo );

//	other_wo = (AosSockWrapperObject_t*) other_sk->sk_proxy_data;
//	other_wo->wt->sendFin( other_wo );


	if( sk->sk_state==TCP_CLOSING || other_sk->sk_state==TCP_CLOSING)
	{
		AosProxy_poRelease( wo->po );
		tcp_close_aos(sk,0);
		tcp_close_aos(other_sk,0);
	}


	if( (sk->sk_state==TCP_CLOSE 
			|| sk->sk_state==TCP_LAST_ACK 
			|| sk->sk_state==TCP_TIME_WAIT)
			&& 
			(other_sk->sk_state==TCP_CLOSE 
			 || other_sk->sk_state==TCP_LAST_ACK 
			 || other_sk->sk_state==TCP_TIME_WAIT ) 
			)
	{
		AosProxy_poRelease( wo->po );
		tcp_close_aos(sk,0);
		tcp_close_aos(other_sk,0);
	}

	return 0;
}

/*
 * When TCP retransmit timer timeout
 * this function will be fired BEFORE linux's processing
 */
static int
AosProxyHook_connectionTimeout(struct sock *sk)
{
	aos_trace("AosProxyHook_connectionTimeout\n");
	AosSockWrapperObject_t	*wo = (AosSockWrapperObject_t*) sk->sk_proxy_data;
	if( wo->wt->connectionTimeout )
	{
		wo->wt->connectionTimeout( wo );
	}
	wo->po->frontSk->sk_proxy_data=0;
	tcp_close_aos(wo->po->frontSk,0);
	wo->po->backSk->sk_proxy_data=0;
	tcp_close_aos(wo->po->backSk,0);
	AosProxy_poRelease1( wo->po );
	return 0;
}

static int 
AosProxy_unregisterProxy(void)
{
	memset(&gProxyHook,0,sizeof(gProxyHook));
	return 0;
}


static int 
AosProxy_registerProxy(void)
{
	sysctl_tcp_tw_recycle=1;
	sysctl_tcp_tw_reuse=1;

	gProxyHook.proxyAllowFrontConnect		= AosProxyHook_allowFrontConnect;
	gProxyHook.proxyFrontConnectionCreated	= AosProxyHook_frontConnectionCreated;
	gProxyHook.proxyBackConnectionCreated	= AosProxyHook_backConnectionCreated;
	gProxyHook.proxyDataReceived			= AosProxyHook_dataReceived;
	gProxyHook.proxyDataAckReceived			= AosProxyHook_dataAckReceived;
	gProxyHook.proxyFinReceived				= AosProxyHook_finReceived;
	gProxyHook.proxyResetReceived			= AosProxyHook_resetReceived;
	gProxyHook.proxyConnectionTimeout		= AosProxyHook_connectionTimeout;
	
	return 0;
}


static __init int 
AosProxy_initModule(void)
{
	int ret;

	aos_trace(KERN_INFO "loading Proxy ...\n");
	INIT_LIST_HEAD(&gProxyServiceList);
	INIT_LIST_HEAD(&gAppProxyList);


	for( ret=0; ret<MAX_WRAPPER; ret++)
	{
		gSockWrapperTypes[ret] = 0;
	}
	
	gProxyObjectCachep = kmem_cache_create( "proxy_object", sizeof(ProxyObject_t), 0,0,0,0);
			
	ret=AosProxy_registerProc();
	if(ret)
		return ret;

	AosProxy_registerProxy();
	AosProxy_registerCli();
	//AosProxy_registerSockWrapperCli();
	return 0;

}

static __exit void 
AosProxy_exitModule(void)
{
	//AosProxy_unregisterSockWrapperCli();
	AosProxy_unregisterCli();
	AosProxy_unregisterProxy();
	AosProxy_unregisterProc();
	kmem_cache_destroy(gProxyObjectCachep);
}

module_init(AosProxy_initModule);
module_exit(AosProxy_exitModule);
     
EXPORT_SYMBOL( AosProxy_relayData );

