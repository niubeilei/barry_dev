////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: app_http.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//#define AOS_DEBUG

#include "proxy_app.h"
#include "proxy_service.h"
#include "aosUtil/Memory.h"

#include <net/sock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zzh");


static int
Aoshttp_readyAcceptData( ProxyObject_t * po, AosSockWrapperObject_t *wo )
{
	wo = ( wo == po->frontWo ) ? po->backWo : po->frontWo ; 

	return wo->wt->isWritable( wo ); 
}


//
// buffer must be free
//
static int Aoshttp_dataReceived( ProxyObject_t * po, struct sock * sk, char * buffer, int len )
{
	AosSockWrapperObject_t *wo;

	aos_debug( KERN_INFO "app http dataReceived(): receive %d, call the other end wo->wt->sendData() \n", len );	
	wo = ( sk==po->frontSk ) ? po->backWo : po->frontWo ;
	len = wo->wt->sendData( wo, buffer, len );
	aos_debug( KERN_INFO "app http dataReceived(): sent %d \n", len );	
	return len;
}


static int Aoshttp_dataAckReceived( ProxyObject_t * po, AosSockWrapperObject_t * wo )
{
	AosSockWrapperObject_t * other_wo;

	aos_debug( KERN_INFO "app http dataAckReceived()\n" );	
	other_wo = ( wo==po->frontWo ) ? po->backWo : po->frontWo ;
	other_wo->wt->peerWritable( other_wo );
	return 0;
}


static int
Aoshttp_finReceived( ProxyObject_t * po, struct sock * sk )
{
	AosSockWrapperObject_t *wo;

	// received a fin from one side(sock)
	// we have no data to send to the other side
	// send fin to the other side
	aos_debug( KERN_INFO "app http finReceived()\n" );	
	wo = ( sk==po->frontSk ) ? po->backWo : po->frontWo ;
	wo->wt->sendFin( wo );

	return 0;
}


static int
Aoshttp_passiveConnected( ProxyObject_t * po )
{
	int		err;

	po->dest = po->svc->scheduler->schedule( po->svc, po->frontSk );
	if(!po->dest)
	{
		proxy_alarm("schedule failed\n");
		return -1;
	}

	err = po->backWo->wt->connect( po->backWo, po->dest->addr, po->dest->port);
	if( err )
	{
		return err;
	}
	po->backSk = po->backWo->sk;
	return 0;
}


static int
Aoshttp_resetConnection( ProxyObject_t * po )
{
	struct sock *sk1, *sk2;
	sk1=po->frontSk;
	sk2=po->backSk;
	AosProxy_poRelease( po );
	tcp_close_aos( sk1,0 );
	tcp_close_aos( sk2,0 );
	return 0;
}

static int
Aoshttp_inithttp( AppProxy_t * app )
{

	memset( app, 0, sizeof(*app) );
	PROXY_SETNAME( app, "http" );
	app->owner = THIS_MODULE;	

	app->readyAcceptData= Aoshttp_readyAcceptData;
	app->dataReceived = Aoshttp_dataReceived;
	app->dataAckReceived = Aoshttp_dataAckReceived;
	app->finReceived = Aoshttp_finReceived;
	app->passiveConnected = Aoshttp_passiveConnected;
	app->resetConnection = Aoshttp_resetConnection;
	return 0;
}

static __init int 
Aoshttp_initModule( void )
{
	AppProxy_t	* app;

	printk( KERN_INFO "loading http proxy ...\n" );
	
	app = aos_malloc( sizeof(*app) );
	if ( !app )
	{
		printk( KERN_ERR "kmalloc http error\n" );
		return -ENOMEM;
	}

	Aoshttp_inithttp( app );
	AosProxy_registerAppProxy( app );
	return 0;
}

static __exit void 
Aoshttp_exitModule( void )
{
	AppProxy_t	* app;
	printk(KERN_INFO "unloading http proxy ...\n");
	app = AosProxy_unregisterAppProxy("http");
	if ( app )
	{
		kfree( app );
	}

}

module_init(Aoshttp_initModule);
module_exit(Aoshttp_exitModule);


