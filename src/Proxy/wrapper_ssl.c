////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: wrapper_ssl.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//#define AOS_DEBUG
#include "wrapper_ssl.h"
#include "proxy_func.h"
#include "proxy_wrapper.h"
#include "aosUtil/Memory.h"
#include "PKCS/x509.h"
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "ssl/Ssl.h"
#include "ssl/SslStatemachine.h"
#include "ssl/SslMisc.h"
#include "Porting/TimeOfDay.h"

static AosSockWrapperType_t *g_ssl_wt = NULL;
//static u32 sgDiff = 2*HZ;

extern int AosSslWrapper_isWritable( AosSockWrapperObject_t * wo );
	
static inline short AosSslWrapper_getSockPort( struct sock * sk)
{
	struct sockaddr_in addr;
	int len;
	aos_inet_getname( sk,(struct sockaddr*)&addr,&len,0 );	
	return addr.sin_port;
}

static inline short AosSslWrapper_getSockPeerPort( struct sock * sk)
{
	struct sockaddr_in addr;
	int len;
	aos_inet_getname( sk,(struct sockaddr*)&addr,&len,1 );	
	return addr.sin_port;
}


static AosSockWrapperObject_t* 
AosSslWrapper_create( void )
{
	AosSslWrapperObject_t * swo = aos_malloc_atomic( sizeof(*swo) );	
aos_trace("AosSslWrapper_create");
	if ( !swo )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, "ssl wrapper object malloc failed\n");
		return 0;
	}

	memset( swo, 0, sizeof(*swo) );
	swo->base.wt = g_ssl_wt;
		
	INIT_LIST_HEAD( &swo->snd_buf_lst );
	
	swo->tt = jiffies;	// delete me
	
	return (AosSockWrapperObject_t*)swo;
}

static int 
AosSslWrapper_release( AosSockWrapperObject_t * wo )
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;
aos_trace("AosSslWrapper_release");
	if (swo->context)
	{
		swo->context->sock = 0;
		swo->context->wo = NULL;
		AosSslContext_put(swo->context);
	}

	if (swo->cert)
	{
		AosCert_put(swo->cert);
	}
	
	AosBuffer_destroy( &swo->snd_buf_lst );
		
	//aos_free( swo );
	kfree( swo );		// !!!!! aos_free()
	return 0;
}

static int 
AosSslWrapper_readyAcceptData( AosSockWrapperObject_t * wo )
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;

	if (!swo->context)
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, "context is null: %p", wo);
		return 0;
	}

/*aos_min_log( eAosMD_TcpProxy, "AosSslWrapper_readyAcceptData, context %p, wo %p, wo.sk %p, %d.%d\n", 
		swo->context, wo, wo->sk, 
		AosSslWrapper_getSockPort(wo->sk),  
		AosSslWrapper_getSockPeerPort(wo->sk));
*/
	//
	// If the SSL is established, we will ask the upper layer. Otherwise, 
	// the data is (most likely) the handshake data. We need to accept it.
	//
	if ( IS_SSL_ESTABLISHED(swo->context) )
	{
		return app_of_wrapper(wo)->readyAcceptData( wo->po , wo);
	}
	return 1;
}


//
// Call this function to send SSL data (messages) to the peer.
//
int
AosSslWrapper_sendRecordData( AosSockWrapperObject_t * wo, char * buffer, int len)
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;
	char *snd_buf;
	int	len_sent;
	
	aos_assert1( wo );
//aos_min_log( eAosMD_TcpProxy, "sendREcordData start, wo %p, usec %u", wo, AosGetUsec() );
	snd_buf = aos_malloc_atomic( len );		
	if ( !snd_buf )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, "send buffer malloc failed, buffer length is %d", len );
	}
	memcpy( snd_buf, buffer, len );
	
	AosBuffer_add( &swo->snd_buf_lst, snd_buf, len, 0 );
	len_sent = AosBuffer_send( wo->sk, &swo->snd_buf_lst );
	
	/*aos_t_trace( swo->tt, sgDiff, "TTT: wrapper ssl sendData(): sent %d:%d, context %p, wo %p, wo.sk %p, %d.%d\n", 
			len, len_sent, swo->context, wo, wo->sk,
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
	*/

	aos_debug( "wrapper ssl sendData(): sent %d:%d, context %p, wo %p, wo.sk %p, %d.%d\n", 
			len, len_sent, swo->context, wo, wo->sk,
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
	
//aos_min_log( eAosMD_TcpProxy, "sendREcordData end, wo %p, usec %u", wo, AosGetUsec() );
	
	return len_sent;
}


//
// Call this function when decryption is finished and ready to pass data to 
// upper layer.
//
static int  
AosSslWrapper_sslCallback(int rc, AosSslWrapperObject_t *swo, char *buffer, int buffer_len)
{
	char *rcv_buf;
	int  ret = eAosRc_Success;
	

	if ( !buffer || buffer_len <= 0 )
	{
		return aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, 
			"invalid buffer 0x%x, buffer_len %d", (__u32)buffer, buffer_len );
	}
	if ( !swo )
	{
		aos_trace( "ssl wrapper is released, call back failed, buffer %x, buffer_len %d", buffer, buffer_len );
		return -1;
	}

	/*aos_t_trace(swo->tt, sgDiff, 
			"TTT: AosSslWrapper_sslCallback, buffer_len %d, context %p, swo %p, swo->base.sk %p, %d.%d", 
			buffer_len, swo->context, swo, swo->base.sk,
			AosSslWrapper_getSockPort( swo->base.sk ),
			AosSslWrapper_getSockPeerPort( swo->base.sk ) );
	*/
	aos_debug( "AosSslWrapper_sslCallback, buffer_len %d, context %p, swo %p, swo->base.sk %p, %d.%d", 
			buffer_len, swo->context, swo, swo->base.sk,
			AosSslWrapper_getSockPort( swo->base.sk ),
			AosSslWrapper_getSockPeerPort( swo->base.sk ) );

	switch( rc )
	{
	case eAosContentType_AppData:
		rcv_buf = aos_malloc_atomic( buffer_len );
		if ( !rcv_buf ) 
		{
			return aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, "rcv buffer malloc failed, buffer_len %d", buffer_len );
		}
		memcpy( rcv_buf, buffer, buffer_len );
		ret = swo->base.po->svc->app->dataReceived( swo->base.po, swo->base.sk, rcv_buf, buffer_len );
		break;

	case eAosContentType_PlainData:
		ret = AosSslWrapper_sendRecordData( &(swo->base), buffer, buffer_len );
		break;
	default:
		aos_alarm(eAosMD_TcpProxy, eAosAlarmProgErr, "front ssl call back, unknown rc = %d", rc);
		return rc;
	}
	
	return ret;
}


//
// Upper layer calls this function to send data through the wrapper
//
static int
AosSslWrapper_sendAppData( AosSockWrapperObject_t * wo, char * buffer, int len)
{
	int ret = len;
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;

/*	aos_t_trace( swo->tt, sgDiff, "TTT: wrapper ssl send app data(): len is %d, context %p, wo %p, wo.sk %p, %d.%d\n", 
			len, swo->context, wo, wo->sk,
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
*/				
	aos_debug( "wrapper ssl send app data(): len is %d, context %p, wo %p, wo.sk %p, %d.%d\n", 
			len, swo->context, wo, wo->sk,
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));

//aos_min_log( eAosMD_TcpProxy, "sendAppData start, wo %p, usec %u", wo, AosGetUsec() );
	// if ( len <= 0 ) return len;
	if ( len > 0) 
	{
		ret = AosSslStm_output(
			swo->context, 
			buffer, 
			len, 
			AosSslWrapper_sslCallback);
	}
	kfree( buffer );	// TBD....
//aos_min_log( eAosMD_TcpProxy, "sendAppData end, wo %p, usec %u", wo, AosGetUsec() );
	return ret;
}


//
// This function is called from the lower layer when the connection receives
// an ack. This implies that the connection may have more room to send.
//
static int
AosSslWrapper_dataAckReceived( AosSockWrapperObject_t * wo )
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;
	
	if( !AosBuffer_empty( &swo->snd_buf_lst ) )
	{
		aos_trace( "ssl wrapper dataAckReceived" );	
		AosBuffer_send( swo->base.sk, &swo->snd_buf_lst );
	}

	if ( swo->tcp_state & eAosSslWrapper_tcpSndFin )
	{
		AosSslWrapper_sendFin( wo );
		return 0;
	}
	if ( AosSslWrapper_isWritable( wo ) )
	{
		swo->tcp_state &= ~eAosSslWrapper_tcpHasData; 
		app_of_wrapper(wo)->dataAckReceived( wo->po, wo );
	}
	return 0;
}


// 
// This is the function for the API function dataRecved(...)
//
static int
AosSslWrapper_recordReceived( AosSockWrapperObject_t * wo, char * buffer, int len)
{
	AosSslWrapperObject_t *swo = (AosSslWrapperObject_t*)wo;
	int ret;
	/*aos_t_trace( swo->tt, sgDiff, "TTT: ssl wrapper dataReceived(): receive %d, context %p, wo %p, wo.sk %p, %d.%d\n", 
			len, swo->context, wo, wo->sk,
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
	*/
//aos_min_log( eAosMD_TcpProxy, "recordReceived start, wo %p, usec %u", wo, AosGetUsec() );
	aos_debug( "ssl wrapper dataReceived(): receive %d, context %p, wo %p, wo.sk %p, %d.%d\n", 
			len, swo->context, wo, wo->sk,
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
	
	if ( len > 0 ) 
	{
		ret = AosSslStm_input(
			swo->context, 
			buffer, 
			len, 
			AosSslWrapper_sslCallback);
	}
	else
	{
		ret = len;
	}
	kfree( buffer ); // TBD....
//aos_min_log( eAosMD_TcpProxy, "recordReceived end, wo %p, usec %u", wo, AosGetUsec() );
	return ret;
}

static int
AosSslWrapper_serverInit( AosSockWrapperObject_t *wo )
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;

//aos_min_log( eAosMD_TcpProxy, "serverInit start, wo %p, usec %u", wo, AosGetUsec() );
	aos_trace("AosSslWrapper_serverInit entry");
	if ( swo->context )
	{
		aos_trace("server ssl context already allocated");
		return eAosRc_Success;
	}

	swo->cert = AosCertMgr_getSystemCert();
	if ( swo->cert ) 
	{
		AosCert_hold(swo->cert);
	}
	else
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no system cert");
		return -1;
	}
	swo->prikey = AosCertMgr_getSystemPrivKey();
	if ( !swo->prikey && gAosSslAcceleratorType != eAosSslAcceleratorType_517PM )
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no system pri key");
		return -1;
	}
	swo->authorities = AosCertChain_getByName("system");
	if ( !swo->authorities )
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no authorities certs");
		return -1;
	}
	
	if ( AosSsl_ClientAuth )
	{
		swo->ssl_flags |= eAosSSLFlag_ClientAuth;
	}

	if ( AosSsl_Start( swo, eAosSSLFlag_Server ) != 0 )
	{
	    aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start server failed");
	    return -1;
	}
//aos_min_log( eAosMD_TcpProxy, "serverInit end, wo %p, usec %u", wo, AosGetUsec() );
	return 0;
}

static int
AosSslWrapper_clientInit( AosSockWrapperObject_t *wo )
{
	struct sockaddr_in addr;
	int len;
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;

//aos_min_log( eAosMD_TcpProxy, "clientInit start, wo %p, usec %u", wo, AosGetUsec() );
	aos_trace("AosSslWrapper_clientInit entry");
	if ( swo->context )
	{
		aos_trace("client ssl context already allocated");
		return eAosRc_Success;
	}
	
	aos_inet_getname( wo->sk, (struct sockaddr*)&addr, &len, 0 );		
	swo->dest_addr  = addr.sin_addr.s_addr;	
	
	if ( AosSsl_Start( swo, eAosSSLFlag_Client ) < 0 )
	{
	    aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start client failed");
	    return -1;
	}
//aos_min_log( eAosMD_TcpProxy, "clientInit end, wo %p, usec %u", wo, AosGetUsec() );
	return 0;
}


static int
AosSslWrapper_connect( AosSockWrapperObject_t * wo , __u32 ip, __u16 port)
{
	int err;
	
	err = AosProxy_connect( &wo->sk, ip, port );
	if(err)
	{
		return err;
	}
	wo->sk->sk_proxy_data = wo;

	return 0;
}


static int
AosSslWrapper_passiveConnected( AosSockWrapperObject_t * wo )
{
	int ret;

	//AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;
	/*aos_t_trace( swo->tt, sgDiff, "TTT: passiveConnected: wo %p, wo.sk %p, %d.%d\n", 
			wo, wo->sk, 
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
	*/

	aos_debug( "passiveConnected: wo %p, wo.sk %p, %d.%d\n", 
			wo, wo->sk, 
			AosSslWrapper_getSockPort(wo->sk),  
			AosSslWrapper_getSockPeerPort(wo->sk));
	
	ret = AosSslWrapper_serverInit( wo );
	if(ret)
	{
		return ret;
	}

	ret = app_of_wrapper(wo)->passiveConnected( wo->po );
	return ret;
}

int AosSslWrapper_isWritable( AosSockWrapperObject_t * wo )
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;

	
	if ( !swo->context )
	{
		//aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, "context is null: %p\n", swo );
		swo->tcp_state |= eAosSslWrapper_tcpHasData; 
		return 0;
	}

	if ( swo->context )
	{

		// 
		// If the ssl is not established yet, not writable
		//
		if ( !IS_SSL_ESTABLISHED(swo->context) )
		{
			//aos_debug("TTT: isWritable return failed, ssl is not established yet\n");
			swo->tcp_state |= eAosSslWrapper_tcpHasData; 
			return 0;
		}

	
		// 
		// Check whether the card is busy. If it is busy, not writable
		//
		if ( atomic_read( &swo->context->out_to_card_cnt ) )
		{
			//aos_debug("TTT: isWritable return failed, out_to_card_cnt\n");
			swo->tcp_state |= eAosSslWrapper_tcpHasData; 
			return 0;
		}

		if ( atomic_read( &swo->context->in_to_card_cnt ) )
		{
			//aos_debug("TTT: isWritable return failed, in_to_card_cnt\n");
			swo->tcp_state |= eAosSslWrapper_tcpHasData; 
			return 0;
		}
	}

	// 
	// The card is not busy and the SSL is established. Now we need to check whether
	// the send buffer is empty. If it is not empty, not writable
	//
	if ( !list_empty( &swo->snd_buf_lst ) )
	{
		//aos_debug("TTT: isWritable return failed, snd_buf_lst\n");
		return 0;
	}
	
	// 
	// The card is not busy, the SSL is established, the send buff is empty.
	// We need to ask the lower layer whether it is writable
	//
	return AosProxy_sockWritable( wo->sk );
}

int 
AosSslWrapper_finReceived( AosSockWrapperObject_t * wo )
{

	// receive fin from the lower layer
	// we have no data to send to the upper layer
	// so, send fin to the upper layer
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;
	
	aos_trace( "ssl wrapper finReceived()" );
	if ( !swo->context )
	{
		aos_trace( "ssl context is null, receive fin" );
		app_of_wrapper(wo)->finReceived( wo->po, wo->sk );
		return 0;
	}
	
	if ( aos_list_empty( &swo->context->record_in_list ) &&
		 !atomic_read( &swo->context->in_to_card_cnt ) )
	{
		aos_trace("No received record processed in card");
		app_of_wrapper(wo)->finReceived( wo->po, wo->sk );
		return 0;
	}

	aos_trace("received records are still in cards, delay to send fin");

	swo->tcp_state |= eAosSslWrapper_tcpRcvFin;

	return 0;
}

int
AosSslWrapper_sendFin( AosSockWrapperObject_t * wo)
{
	AosSslWrapperObject_t * swo = (AosSslWrapperObject_t*) wo;
	aos_trace("ssl wrapper, send fin");
	
	if ( !swo->context )
	{
		aos_trace("ssl context is null, send fin");
		AosProxy_sendFin( wo->sk );
		return 0;
	}

	if ( AosBuffer_empty( &swo->snd_buf_lst ) &&
		 aos_list_empty( &swo->context->record_out_list ) &&
		 !atomic_read( &swo->context->out_to_card_cnt ) )
	{
		aos_trace("send list is null and No record processed in card");
		AosProxy_sendFin( wo->sk );
		return 0;
	}

	aos_trace("send list is not null, or records are still in cards, delay to send fin");

	swo->tcp_state |= eAosSslWrapper_tcpSndFin;
			
	return 0;
}

static int
AosSslWrapper_peerWritable( AosSockWrapperObject_t * wo )
{
	AosProxy_relayData( wo->sk );
	return 0;
}

static int
AosSslWrapper_initWt( AosSockWrapperType_t * wt )
{
	memset( wt, 0, sizeof(*wt) );
	WT_SETNAME(wt,"ssl");
	wt->owner = THIS_MODULE;	

	INIT_LIST_HEAD(&wt->list);

	wt->create = AosSslWrapper_create;
	wt->release = AosSslWrapper_release;
	wt->readyAcceptData = AosSslWrapper_readyAcceptData;
	wt->dataReceived = AosSslWrapper_recordReceived;
	wt->dataAckReceived = AosSslWrapper_dataAckReceived;
	wt->activeConnected = AosSslWrapper_clientInit;
	wt->passiveConnected = AosSslWrapper_passiveConnected;
	wt->finReceived = AosSslWrapper_finReceived;
	wt->sendFin = AosSslWrapper_sendFin;
		
	wt->sendData = AosSslWrapper_sendAppData;
	wt->connect = AosSslWrapper_connect;
	wt->isWritable = AosSslWrapper_isWritable;
	wt->peerWritable = AosSslWrapper_peerWritable;
	
	return 0;
}

static __init int 
AosSslWrapper_initModule(void)
{
	int	ret;
	
	aos_min_log(eAosMD_TcpProxy, "loading ssl wrapper ...\n");
	
	g_ssl_wt = aos_malloc_atomic( sizeof(*g_ssl_wt) );
	if ( !g_ssl_wt )
	{
		aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, "malloc AosSockWrapperType error\n" );
		return -ENOMEM;
	}

	AosSslWrapper_initWt( g_ssl_wt );
	ret = AosProxy_registerWrapperType( g_ssl_wt );
	if( ret < 0 )
	{
		aos_alarm( eAosMD_TcpProxy,  eAosAlarmProgErr, "register AosSockWrapperType error\n" );
		aos_free( g_ssl_wt );
	}

	return ret;
}

static __exit void 
AosSslWrapper_exitModule(void)
{
	AosSockWrapperType_t	*wt;

	aos_min_log( eAosMD_TcpProxy, "unloading ssl wrapper ...\n" );
	wt = AosProxy_unregisterWrapperType( g_ssl_wt );
	if ( wt )
	{
		aos_free( wt );
	}
}

module_init(AosSslWrapper_initModule);
module_exit(AosSslWrapper_exitModule);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liqin");




