////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: wrapper_plain.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#define AOS_DEBUG

#include "proxy_func.h"
#include "proxy_wrapper.h"


AosSockWrapperType_t	* g_plain_wt;

typedef
struct AosPlainWrapperObject
{
	AosSockWrapperObject_t	base;
	struct list_head		buffer_list;
	char					fin;
}
AosPlainWrapperObject_t;


static AosSockWrapperObject_t * AosPlainWrapper_create( void )
{
	AosPlainWrapperObject_t * pw = aos_malloc( sizeof(*pw) );
	if( !pw )
	{
		aos_trace( KERN_ERR "kmalloc plain wrapper object error\n");
		return 0;
	}
		
	memset( pw, 0, sizeof(*pw) );
	pw->base.wt = g_plain_wt;
	//pw->buffer_len=-1;
	INIT_LIST_HEAD( &pw->buffer_list );
	return (AosSockWrapperObject_t*)pw;
}


static int AosPlainWrapper_release( AosSockWrapperObject_t * wo )
{
	AosPlainWrapperObject_t * pw = (AosPlainWrapperObject_t*) wo;

	AosBuffer_destroy( &pw->buffer_list );
	kfree( wo );
	return 0;
}

static int
AosPlainWrapper_readyAcceptData( AosSockWrapperObject_t * wo )
{
	return app_of_wrapper(wo)->readyAcceptData( wo->po , wo );
}


static int
AosPlainWrapper_peerWritable( AosSockWrapperObject_t * wo )
{
	AosProxy_relayData( wo->sk );
	return 0;
}

static int
AosPlainWrapper_sendFin( AosSockWrapperObject_t * wo)
{
	AosPlainWrapperObject_t * pw = (AosPlainWrapperObject_t*) wo;
	aos_debug("plain wrapper, send fin\n");
	
	AosBuffer_send( wo->sk, &pw->buffer_list );
	if( AosBuffer_empty(&pw->buffer_list) )
	{
		AosProxy_sendFin( wo->sk );
	}
	else
	{
		pw->fin = 1;
	}
	
	return 0;
}

//
// buffer must be free
//
static int AosPlainWrapper_sendData( AosSockWrapperObject_t * wo, char * buffer, int len)
{
	int	len_sent;
	AosPlainWrapperObject_t * pw = (AosPlainWrapperObject_t*) wo;

	if(pw->fin)
	{
		kfree(buffer);
		return 0;
	}

	aos_debug( KERN_INFO "wrapper plain sendData(): send %d \n", len );	
	AosBuffer_add( &pw->buffer_list, buffer, len, 0 );
	len_sent = AosBuffer_send( wo->sk, &pw->buffer_list );
	aos_debug( KERN_INFO "wrapper plain sendData(): sent %d\n", len_sent );	
	return len_sent;
}

static int
AosPlainWrapper_dataAckReceived( AosSockWrapperObject_t * wo )
{
	AosPlainWrapperObject_t * pw = (AosPlainWrapperObject_t*) wo;
	aos_debug( KERN_INFO "plain wrapper dataAckReceived()\n" );	

	if( ! AosBuffer_empty(&pw->buffer_list) )
	{
		AosBuffer_send( pw->base.sk, &pw->buffer_list );
	}


	if( AosBuffer_empty(&pw->buffer_list) )
	{
		if ( pw->fin )
		{
			AosProxy_sendFin( wo->sk );
		}
		else
		{
			app_of_wrapper(wo)->dataAckReceived( wo->po, wo );
		}
	}

	return 0;
}

//
// buffer must be free
//
static int AosPlainWrapper_dataReceived( AosSockWrapperObject_t * wo, char * buffer, int len)
{
	/*
	kfree(buffer);
	return -1;
	*/

	aos_debug( KERN_INFO "plain wrapper dataReceived(): receive %d, call app->dataReceived() \n", len );	
	len = app_of_wrapper(wo)->dataReceived( wo->po, wo->sk, buffer, len );
	return len;
}

static int AosPlainWrapper_finReceived( AosSockWrapperObject_t * wo )
{

	// receive fin from the lower layer
	// we have no data to send to the upper layer
	// so, send fin to the upper layer
	
	aos_debug( KERN_INFO "plain wrapper finReceived()\n" );	
	app_of_wrapper(wo)->finReceived( wo->po, wo->sk );

	return 0;
}



static int
AosPlainWrapper_passiveConnected( AosSockWrapperObject_t * wo )
{

	return app_of_wrapper(wo)->passiveConnected( wo->po );

}

static int
AosPlainWrapper_connect( AosSockWrapperObject_t * wo , __u32 ip, __u16 port)
{
	int err;
	
	aos_debug("AosPlainWrapper_connect()\n");
	err = AosProxy_connect( &wo->sk, ip, port );
	if(err)
	{
		aos_debug("AosPlainWrapper_connect(): connect error \n");
		return err;
	}
	wo->sk->sk_proxy_data = wo;
	aos_debug("AosPlainWrapper_connect() ok\n");
	return 0;
}


static int AosPlainWrapper_isWritable( AosSockWrapperObject_t * wo )
{
	AosPlainWrapperObject_t * pw = (AosPlainWrapperObject_t*) wo;

#ifdef AOS_DEBUG
	if( list_empty(&pw->buffer_list) )
	{
		aos_debug("buffer_list empty\n");
	}
	if( AosProxy_sockWritable( wo->sk ) )
	{
		aos_debug("sock writable\n");
	}
	else
	{
		aos_debug("sock not writable\n");
	}

	aos_debug("sk_state=%d, sk_sndbuf=%d, sk_wmem_queued=%d\n",
			wo->sk->sk_state,
			wo->sk->sk_sndbuf,
			wo->sk->sk_wmem_queued
			);
#endif
	
	return ( list_empty(&pw->buffer_list) ) && AosProxy_sockWritable( wo->sk );
}

static int
AosPlainWrapper_initWt( AosSockWrapperType_t * wt )
{
	memset( wt, 0, sizeof(*wt) );
	WT_SETNAME(wt,"plain");
	wt->owner = THIS_MODULE;	

	INIT_LIST_HEAD(&wt->list);

	wt->create = AosPlainWrapper_create;
	wt->release = AosPlainWrapper_release;

	wt->readyAcceptData = AosPlainWrapper_readyAcceptData;
	wt->dataReceived = AosPlainWrapper_dataReceived;
	wt->dataAckReceived = AosPlainWrapper_dataAckReceived;
	wt->passiveConnected = AosPlainWrapper_passiveConnected;
	wt->finReceived = AosPlainWrapper_finReceived;
	wt->sendFin = AosPlainWrapper_sendFin;
	wt->peerWritable = AosPlainWrapper_peerWritable;


	wt->sendData = AosPlainWrapper_sendData;
	wt->connect = AosPlainWrapper_connect;
	wt->isWritable = AosPlainWrapper_isWritable;

	return 0;
}

static __init int 
AosPlainWrapper_initModule(void)
{
	int	ret;
	
	printk(KERN_INFO "loading plain wrapper ...\n");
	

	g_plain_wt = aos_malloc( sizeof(*g_plain_wt) );
	if ( !g_plain_wt )
	{
		printk( KERN_ERR "kmalloc AosSockWrapperType error\n" );
		return -ENOMEM;
	}

	AosPlainWrapper_initWt(g_plain_wt);
	ret = AosProxy_registerWrapperType(g_plain_wt);
	if( ret < 0 )
	{
		printk( KERN_ERR "register AosSockWrapperType error\n" );
		kfree( g_plain_wt );
	}

	return ret;
}

static __exit void 
AosPlainWrapper_exitModule(void)
{
	AosSockWrapperType_t	*wt;

	printk(KERN_INFO "unloading plain wrapper ...\n");
	wt = AosProxy_unregisterWrapperType(g_plain_wt);
	if( wt )
	{
		kfree( wt );
	}
}

module_init(AosPlainWrapper_initModule);
module_exit(AosPlainWrapper_exitModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zzh");




