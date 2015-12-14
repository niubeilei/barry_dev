////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslStatemachine.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#define AOS_DEBUG
#include "KernelSimu/atomic.h"
#include "KernelSimu/string.h"

#include "ssl/SslStatemachine.h"

#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tcp.h"
#include "aosUtil/Random.h"
#include "aosUtil/Slab.h"
#include "aosUtil/List.h"

#include "ssl/SslCommon.h"
#include "ssl/ReturnCode.h"
#include "ssl/SslServer.h"
#include "ssl/SslClient.h"
#include "ssl/SslRecord.h"
#include "ssl/SslMisc.h"
#include "ssl/cavium.h"
#include "ssl/SslWrapper.h"

#include "PKCS/x509.h"
#include "Porting/TimeOfDay.h"
//#include "Proxy/wrapper_ssl.h"
#include "CertVerify/aosCertVerify.h"
// #include "Proxy/proxy_func.h"

#include "KernelSimu/module.h"

//Ciphers supported as the Server
u32 sgServerCiphersSupported[eAosMaxCipher] =
{
	TLS_RSA_WITH_RC4_128_MD5,	// 4
	TLS_RSA_WITH_RC4_128_SHA,	// 5
	TLS_RSA_WITH_DES_CBC_SHA,  // 9
	JNSTLS_RSA_WITH_NULL_MD5,  	// 0x81,
	JNSTLS_RSA_WITH_NULL_SHA,	// 0x82,
};
u8 sgNumServerCiphersSupported = 5;

extern u32 AosClientCiphersSupported[eAosMaxCipher];
extern u8 AosNumClientCiphersSupported;
extern int gAosSslAcceleratorType;
extern ContextType gAosCaviumContextType;
extern KeyMaterialLocation gAosCaviumKeyMaterialLocation;
extern struct AosSlab *gAosSlabContext;

extern volatile u32 allocated_context_count; // liqin debug only , delete me

static int AosSslContext_constructor(struct AosSslContext **context)
{
	u32 ret;

	(*context) = (struct AosSslContext *) AosSlab_get( gAosSlabContext );
	aos_assert1(*context);

	memset((*context), 0, sizeof(struct AosSslContext));

	(*context)->handshake = (uint8*)aos_malloc_atomic(eAosSSL_HandshakeMaxLen);
	if (!(*context)->handshake)
	{
		AosSlab_release( gAosSlabContext, (void*)(*context) );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "handshake malloc failed");
	}

	if ( gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro ||
	     gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumGen )
	{
		if ( ( ret = Csp1AllocContext( gAosCaviumContextType, &(*context)->cavium_context ) ) )
		{
			aos_alarm( eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1AllocContext Failed, ret is %d", ret );
			AosSlab_release( gAosSlabContext, (void*)(*context) );
			*context = NULL;
			return eAosRc_Error;
		}

aos_debug( "context alloc num %d, cavium_context %llu, context %p\n", 
		allocated_context_count, (*context)->cavium_context, (*context));
		
		if ( ( ret = Csp1AllocKeyMem(gAosCaviumKeyMaterialLocation, &(*context)->cavium_keyhandle ) ) )
		{
			aos_alarm( eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1AllocKeyMem Failed, ret is %d", ret );
			Csp1FreeContext( gAosCaviumContextType, (*context)->cavium_context );
			AosSlab_release( gAosSlabContext, (void*)(*context) );
			*context = NULL;
			return eAosRc_Error;
		}
		(*context)->ssl_version = VER3_0;
	}

/*	
	(*context)->client_random = aos_malloc_atomic( RANDOM_LENGTH );
	aos_assert1( (*context)->client_random );
	(*context)->server_random = aos_malloc_atomic( RANDOM_LENGTH );
	aos_assert1( (*context)->server_random );
	(*context)->pre_master_secret = aos_malloc_atomic( PRE_MASTER_SECRET_LENGTH );
	aos_assert1( (*context)->pre_master_secret );
	(*context)->master_secret = aos_malloc_atomic ( MASTER_SECRET_LENGTH );
	aos_assert1( (*context)->master_secret );
	(*context)->key_block = aos_malloc_atomic( KEY_BLOCK_MAX_LENGTH );
	aos_assert1( (*context)->key_block );
*/	

	atomic_set(&((*context)->refcnt), 1);
	atomic_set(&((*context)->out_to_card_cnt), 0);
	atomic_set(&((*context)->in_to_card_cnt), 0);

	AOS_INIT_LIST_HEAD( &((*context)->record_in_list) );
	AOS_INIT_LIST_HEAD( &((*context)->record_out_list) );
	AOS_INIT_LIST_HEAD( &((*context)->app_out_list) );
	skb_queue_head_init(&(*context)->rskb_in_queue);
	
	return 0;
}

//
// Description:
//  This function release the allocated context and its contect memory.
// Arguments:
//  Input:
//  context: the context to be released.
// Return value:
//   0: success
//   else: failed 
//
int AosSslContext_release(struct AosSslContext *context)
{
	struct AosSslRecord *record_this, *record_next;
	if (context->peer_cert)
	{
		AosCert_put(context->peer_cert);
		context->peer_cert = NULL;
	}
	if (context->self_cert)
	{
	    AosCert_put(context->self_cert);	
		context->self_cert = NULL;
	}
	if (context->enc_clt_fin_msg)
	{
		aos_free(context->enc_clt_fin_msg);
	}
	if (context->enc_svr_fin_msg)
	{
		aos_free(context->enc_svr_fin_msg);
	}	
	if (context->verify_data)
	{
		aos_free(context->verify_data);
	}
	if ( context->veri_rcvd )
	{
		aos_free( context->veri_rcvd );
	}

	if ( context->key_exchange )
	{
		aos_free( context->key_exchange );
	}

/*
	if ( context->client_random ) aos_free( context->client_random );
	if ( context->server_random ) aos_free( context->server_random );
	if ( context->pre_master_secret ) aos_free( context->pre_master_secret );
	if ( context->master_secret ) aos_free( context->master_secret );
	if ( context->key_block ) aos_free( context->key_block );
*/

	if (context->handshake)
	{
		aos_free(context->handshake);
	}
	
	if (context->private_key)
	{
		aos_free(context->private_key);
	}
	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro ||
	    gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumGen)
	{
		Csp1FreeKeyMem(context->cavium_keyhandle);

aos_debug("context release num %d, cavium_context %llu, context %p\n", 
		allocated_context_count, context->cavium_context, context);

		Csp1FreeContext(gAosCaviumContextType, context->cavium_context);
	}

	// release record in list
	aos_list_for_each_entry_safe( record_this, record_next, &(context->record_in_list), list ) 
	{
		aos_list_del( &(record_this->list) );
		AosSslRecord_release( record_this );
	}
	// release record out list
	aos_list_for_each_entry_safe( record_this, record_next, &(context->record_out_list), list ) 
	{
		aos_list_del( &(record_this->list) );
		AosSslRecord_release( record_this );
	}
	// release all receiving skb
	skb_queue_purge(&context->rskb_in_queue);
	
	AosSlab_release( gAosSlabContext, (void*)(context) );

	return 0;
}

// 
// This function should be called to start an SSL Server statemachine.
// It creates a context and changes its state to eAosSSLState_ServerIdle, 
// sets the state function to ???.
//
// Parameters:
//	'conn': IN
//		The connection on which the SSL session is created
//	
//	'cipherSuites': IN
//		The list of cipher suites the server is to support.
//
//	'numCipherSuites': IN
//		The number of cipher suites.
//
int AosSslStm_startServer(
	struct AosSslWrapperObject *wo, 
	u16 flags,
	int sock)
{
	//
	// create the context
	//
	struct AosSslContext *context;
	int  ret;

	aos_trace("AosSslStm_startServer entry");
	AosSslContext_constructor(&wo->context);
	context = wo->context;

	aos_assert1(context);

	// cert 
	aos_assert1(wo->cert);
	context->self_cert = wo->cert;
	AosCert_hold(context->self_cert);

	// cert chain
	aos_assert1(wo->authorities);
	context->authorities = wo->authorities;

	// private key
	if (gAosSslAcceleratorType == eAosSslAcceleratorType_Soft)
	{
		context->private_key = (AosRsaPrivateKey_t*)aos_malloc_atomic(sizeof(AosRsaPrivateKey_t));
		aos_assert1(context->private_key);
		aos_assert1(wo->prikey);
		memcpy(context->private_key, wo->prikey, sizeof(AosRsaPrivateKey_t));
	}
	else if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		uint8 *keycrt;
		int pos, prime_len;
		keycrt = (uint8*)aos_malloc_atomic(MAX_CAVIUM_CRT_LEN);
		aos_assert1(keycrt);
		aos_assert1(wo->prikey);
		prime_len = wo->prikey->bits/16;
		aos_assert1(prime_len*5 <= MAX_CAVIUM_CRT_LEN);
		// copy prime2
		memcpy(keycrt, (char*)wo->prikey->prime[1], prime_len);
		pos = prime_len;
		// copy exponent2
		memcpy(keycrt+pos, (char*)wo->prikey->primeExponent[1], prime_len);
		pos += prime_len;
		// copy prime1
		memcpy(keycrt+pos, (char*)wo->prikey->prime[0], prime_len);
		pos += prime_len;
		// copy exponent1
		memcpy(keycrt+pos, (char*)wo->prikey->primeExponent[0], prime_len);
		pos += prime_len;
		// copy coefficeint
		memcpy(keycrt+pos, (char*)wo->prikey->coefficient, prime_len);
		pos += prime_len;		
		ret = Csp1StoreKey(&context->cavium_keyhandle, pos, keycrt, CRT_MOD_EX);
		if (ret)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1StoreKey failed: %d", ret);
			aos_free(keycrt);
			return eAosRc_Error;
		}
		aos_free(keycrt);
	}

	// others
	context->sock = sock;
	context->flags = flags;
	context->state = eAosSSLState_ServerStart;
	context->state_func = AosSslServer_ClientHello;
	context->wo = wo;	
	return 0;
}	


// 
// This function should be called to initiate a client SSL handshake.
// It creates a context, constructs a ClientHello and returns that
// message to the caller. 
//
// Parameters:
//	'rslt_data': OUT
//		It is the ClientHello message constructed by this function.
//		It is important that the caller should have allocated enough
//		space for ClientHello.
//
//	'rslt_data_len': IN/OUT
//		The length of 'rslt_data'. After the processing, it is the
//		length of message constructed by this function.
//
int AosSslStm_startClient(
	struct AosSslWrapperObject *wo, 
	u16 flags,
	const u16 serverId,
	char *sessionId, 
	u32 sessIdLen,
	char *master_secret,
	u8 ms_len,
	 char *client_random,
	char *server_random,
	int sock)
{
	// create the context
	u16 index, i;
	u16 cipherSuiteLen = 0;
	struct timeval tv;
	struct AosSslContext *context;
	struct AosSslRecord	*record_out;
	char *buffer;
	u16 msglen = 0;
	int ret;

	aos_assert1(sessIdLen < 33);

	AosSslContext_constructor(&wo->context);
	context = wo->context;
	
	if ( !context )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Start client, context is null");
		return eAosRc_Error;
	}
	
	context->is_client = 1;
	context->sock = sock;
	context->wo = wo;
	//
	// Create ClientHello
	// Byte[0]			Version, Major
	// Byte[1]			Version, Minor
	// Byte[2..33]		Random (32 bytes)
	// Byte[34]			Session ID length
	// Byte[35..n]		Session ID
	// Byte[n+1, n+2]	CipherSuite Length
	// Byte[...]		CipherSuites
	// Byte[+1]			Compression Method Length
	// Byte[+2]			Compression Methods
	//
	msglen = 39 + sessIdLen + (AosNumClientCiphersSupported << 1);
	aos_assert1(msglen <= eAosSSL_ClientHelloMaxLen);


	record_out = AosSslRecord_create( NULL, eAosSSL_ClientHelloMaxLen, eAosSslRecord_Output );
	aos_assert1( record_out );

	buffer = AosSslRecord_getMsgBody( record_out );
	
	// Record Layer
	index = 0;
	
	// ClientHello Body
	buffer[index++] = eAosSSL_VersionMajor;
	buffer[index++] = eAosSSL_VersionMinor;

	// Random
	OmnGetTimeOfDay(&tv);	
	buffer[index++] = (tv.tv_sec >> 24);
	buffer[index++] = (tv.tv_sec >> 16);
	buffer[index++] = (tv.tv_sec >> 8);
	buffer[index++] = (tv.tv_sec);
	AosRandom_get(&buffer[index], 28);
	index += 28;

	memcpy(context->client_random, &buffer[index-32], 32);

	// Session ID
	buffer[index++] = sessIdLen;
	if (sessIdLen)
	{
		// 
		// The caller wants to reuse a session ID. In this case,
		// master_secret cannot be null.
		//
		if (!master_secret)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"To reuse a session ID but master secret is null");
		}

		memcpy(&buffer[index], sessionId, sessIdLen);
		index += sessIdLen;

		memcpy(context->session_id, sessionId, sessIdLen);
		memcpy(context->master_secret, master_secret, ms_len);
		if (gAosSslAcceleratorType == eAosSslAcceleratorType_517PM)
		{
			if (client_random)
				memcpy(context->client_random, client_random, RANDOM_LENGTH);
			if (server_random)
				memcpy(context->server_random, server_random, RANDOM_LENGTH);
		}
		context->is_resumed = 1;
	}
	else
	{
		context->is_resumed = 0;
	}

	// CipherSuites
	cipherSuiteLen = (AosNumClientCiphersSupported << 1);
	buffer[index++] = (cipherSuiteLen >> 8);
	buffer[index++] = cipherSuiteLen;
	for (i=0; i<AosNumClientCiphersSupported; i++)
	{
		buffer[index++] = (AosClientCiphersSupported[i] >> 8);
		buffer[index++] = AosClientCiphersSupported[i];
	}

	// Compression
	buffer[index++] = 1;
	buffer[index++] = 0;

	// create record
	AosSslRecord_createHandshake( record_out, eAosSSLMsg_ClientHello, index);
	// store ClientHello message into context
	AosSSL_addhandshake(context, record_out);
	
	// Send the message
	if ( (ret = AosSslRecord_send( record_out, context )) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send clienthello message, %d", ret);
	}
	AosSslRecord_release( record_out );
	 			
	// 
	// Message constructed. Prepare the state machine
	//
	context->state = eAosSSLState_ClientHelloSent;
	context->state_func = AosSslClient_serverHello;

	return 0;
}

// 
// Description:
//	It processes incoming record queue, record_in_list
//  
// Parms:
//	'context': IN
//		SSL context. 
//
// Function Return Values:
//	
int AosSslStm_skbRecordIn(struct AosSslContext *context, SSL_PROC_CALLBACK callback)
{
	struct AosSslRecord *record_this, *record_next;
	int ret;
	
	aos_list_for_each_entry_safe( record_this, record_next, &context->record_in_list, list ) 
	{
		if ( atomic_read( &context->in_to_card_cnt ) ) break;

		aos_list_del( &record_this->list );
		ret = AosSslRecord_stm(record_this, callback);

		if ( ret == eAosRc_Pending ) return eAosRc_Success;

		AosSslRecord_release( record_this );

		if ( ret != eAosRc_Success ) goto error;
	}

	return eAosRc_Success;

error:

	// if error found, clear all following inputted record.
	aos_list_for_each_entry_safe( record_this, record_next, &context->record_in_list, list ) 
	{
		aos_list_del( &record_this->list );
		AosSslRecord_release( record_this );
	}	
	return -eAosAlarm_SSLProcErr; 
}

// 
// Description:
//	It processes output record queue, record_out_list
//  
// Parms:
//	'context': IN
//		SSL context. 
//
// Function Return Values:
//	
int AosSslStm_skbRecordOut(struct AosSslContext *context, SSL_PROC_CALLBACK callback)
{
	struct AosSslRecord *record_this, *record_next;
	// u16 error_code;
	int ret;
	
	aos_trace("%s", __FUNCTION__);	

	aos_list_for_each_entry_safe( record_this, record_next, &context->record_out_list, list ) 
	{
		if ( atomic_read( &context->out_to_card_cnt ) ) break;

		aos_list_del( &record_this->list );
		ret = AosSslRecord_stm(record_this, callback);

		if ( ret == eAosRc_Pending ) return eAosRc_Success;

		// Send record out
		aos_trace("record out len %d", record_this->record_total_len);
		AosSslTcp_sendRecord(context->wo, record_this);
		AosSslRecord_release( record_this );

		if ( ret != eAosRc_Success ) goto error;
	}

	return eAosRc_Success;

error:

	// if error found, clear all following output record.
	aos_list_for_each_entry_safe( record_this, record_next, &context->record_out_list, list ) 
	{
		aos_list_del( &record_this->list );
		AosSslRecord_release( record_this );
	}	
	return -eAosAlarm_SSLProcErr; 
}

// 
// Description:
//	It processes output record queue, record_out_list
//  
// Parms:
//	'context': IN
//		SSL context. 
//
// Function Return Values:
//	
int AosSslStm_skbAppDataOut(struct AosSslContext *context, SSL_PROC_CALLBACK callback)
{
	struct AosSslRecord *record_this, *record_next;
	// u16 error_code;
	int ret;
	
	aos_trace("%s", __FUNCTION__);	

	aos_list_for_each_entry_safe( record_this, record_next, &context->app_out_list, list ) 
	{
		if ( atomic_read( &context->out_to_card_cnt ) ) break;

		aos_list_del( &record_this->list );
		ret = AosSslRecord_stm(record_this, callback);

		if ( ret == eAosRc_Pending ) return eAosRc_Success;

		if ( ret != eAosRc_Success ) {
			AosSslRecord_release( record_this );
			goto error;
		}
	}

	return eAosRc_Success;

error:

	// if error found, clear all following output record.
	aos_list_for_each_entry_safe( record_this, record_next, &context->app_out_list, list ) 
	{
		aos_list_del( &record_this->list );
		AosSslRecord_release( record_this );
	}	
	return -eAosAlarm_SSLProcErr; 
}


// 
// Description:
//	SSL Statemachine driver. It processes the input data until all input data
//  are processed or the statemachine reaches a pending state. 
//
// Parms:
//	'data': IN
//		The input data. It can be NULL. 
//
//	'data_len': IN
//		The length of 'data'. If 'data' is null, 'data_len' is 0.
//
//	'callback': IN
//		If the call is asynchronous, this is the function to be called back.
//
// Function Return Values:
//	
int AosSslStm_skbInput(
	struct AosSslContext *context,
	struct sk_buff *skb,
	SSL_PROC_CALLBACK callback)
{
	//int ret = 0;

	aos_trace("%s enter", __FUNCTION__);
	aos_assert1(context);

	// accumulate record, parse it, and store it  into list
	if ( eAosRc_Success != AosSslRecord_skbCollectRecord( context, skb) )
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "received ssl record collection failed" );
		return eAosRc_Error;
	}

	if (eAosRc_Success != AosSslStm_skbRecordIn(context, callback))
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "record receiving queue processing failed" );
		return eAosRc_Error;
	}
	
	if (eAosRc_Success != AosSslStm_skbRecordOut(context, callback))
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "record sending queue processing failed" );
		return eAosRc_Error;
	}
	
	return eAosRc_Success; 
}

// 
// Description:
//	SSL Statemachine driver. It processes the input data until all input data
//  are processed or the statemachine reaches a pending state. 
//
// Parms:
//	'data': IN
//		The input data. It can be NULL. 
//
//	'data_len': IN
//		The length of 'data'. If 'data' is null, 'data_len' is 0.
//
//	'callback': IN
//		If the call is asynchronous, this is the function to be called back.
//
// Function Return Values:
//	
int AosSslStm_protocol(
	struct AosSslContext *context,
	struct AosSslRecord *record,
	SSL_PROC_CALLBACK callback,
	uint16 *error_code)
{
	int safe_guard = 0;
	int ret = 0;

	aos_trace( "AosSslStm_protocol" );

	aos_assert1( context );
	aos_assert1( context->state_func );

	if ( context->state == eAosSSLState_Pending )
	{
		return eAosRc_Pending;
	}
	ret = AosSslRecord_setMsg( record, (context->flags & eAosSSLFlag_ExpectFinished)?1:0 );

	if ( ret != eAosRc_Success )
	{
		*error_code = eAosSSL_AlertInternalError;
	 	aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "SSL protocol stm, parse protocol message failed: %d", ret);
		return -eAosAlarm_SSLProcErr;
	}
	while ( safe_guard ++ < 10 )
	{
		ret = context->state_func( context, record, callback, error_code );
		switch (ret)
		{
		case eAosRc_Pending:
			 return eAosRc_Pending;

		case eAosRc_Success:
			 break;

		default:
			*error_code = eAosSSL_AlertInternalError;
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "SSL input process failed: %d", ret);
			return -eAosAlarm_SSLProcErr; 
		}

		ret = AosSslRecord_nextMsg(record, (context->flags & eAosSSLFlag_ExpectFinished)?1:0 );
		if (ret ) // no more message
		{
			break;
		}
			
	}

	return eAosRc_Success;

}

// 
// Description:
//	SSL Statemachine driver. It processes the applicaton output data. 
//
// Parms:
//	'context': IN
//		SSL context. 
//
//	'skb': IN
//		Application output data
//
//	'callback': IN
//		If the call is asynchronous, this is the function to be called back.
//
// Function Return Values:
//	
int AosSslStm_skbOutput(
	struct AosSslContext *context,	
	struct sk_buff *skb, 
	SSL_PROC_CALLBACK callback)
{
	struct AosSslRecord *record_out;

	aos_trace("%s", __FUNCTION__);
	aos_assert1(context);

	if (context->state != eAosSSLState_Established)
	{
		aos_trace("%s ssl is not established yet, failed to send");
		return -eAosSslRc_NotEstablished;
	}

	if (atomic_read(&context->out_to_card_cnt))
	{
		aos_trace("%s multiple application data in sending queue");
		return -eAosSslRc_MultipleAppData;
	}

	if (skb)
	{
		record_out = AosSslRecord_skbCreate(
					 context, 
					 skb, 
					 SSL_RECORD_HEADER_SIZE, 
					 SSL_RECORD_EXTEND_BYTES, 
					 eAosSslRecord_Output);
		if ( !record_out )
		{
			aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "record out creation failed" );
			return -1;
		}
	
		AosSslRecord_skbCreatePlainAppRecord(record_out, SSL_RECORD_HEADER_SIZE, SSL_RECORD_EXTEND_BYTES);
		aos_list_add_tail(&(record_out->list), &(context->app_out_list));
	}
	
	if (eAosRc_Success != AosSslStm_skbAppDataOut(context, callback))
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "record sending queue processing failed" );
		return -1;
	}

	if (eAosRc_Success != AosSslStm_skbRecordOut(context, callback))
	{
		aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "record sending queue processing failed" );
		return -1;
	}

	if (skb)
		return skb->len;
	return 0;
}

void AosSslStm_cardCallback(int result, void* ssldata)
{
	struct AosSslRecord *record = (struct AosSslRecord*) ssldata;
	struct AosSslContext *context;
	struct AosSslRecord *record_this, *record_next;
	uint16 error_code;
	int ret = 0;

	aos_trace("AosSslStm_cardCallback ret %d, ssldata %x", result, ssldata);

	if ( !record )
	{
		aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "record is null");
		return;
	}

	context = record->context;
	if ( !context )
	{
		aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "context is null");
		return;
	}
	
	if ( !AosSslContext_put( record->context ) )
	{
		aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "context refcnt is zero");
		return;
	}
	
	if ( record->direct_type == eAosSslRecord_Input )
	{
		atomic_dec( &context->in_to_card_cnt );
	}
	else
	{
		atomic_dec( &context->out_to_card_cnt );
	}

aos_trace("out refcnt %d, in refcnt %d", 
	
	atomic_read( &context->out_to_card_cnt ),
	atomic_read( &context->in_to_card_cnt ) );

//aos_debug("record %p, callback seq num %d, record->buffer %p, record[5][6][7][8] %x %x %x %x\n", 
//		record, (int)(record->seq), record->buffer,
//		record->buffer[5]&0xff, record->buffer[6]&0xff, record->buffer[7]&0xff, record->buffer[8]&0xff);

	if ( result != 0 )
	{
		//aos_trace("ssl card operation error, may caused by wrong input parameters");
		aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "ssl card callback ret %d error, card operation error.", result );
		error_code = 0x0250;
		goto error;
	}
	
	if ( context->state == eAosSSLState_Pending )
	{
		context->state = eAosSSLState_Continuing;
	}

	if ( record->state_func )
	{
		record->state_func( context, record, context->callback, &error_code ); 
	}
	
	if ( !context->state_func )
	{
		aos_alarm( eAosMD_SSL, eAosAlarmProgErr, "context %x state_fun is null", context );
		return;
	}

	while ( 1 )
	{
		ret = context->state_func( context, record, context->callback, &error_code );
		switch (ret)
		{
		case eAosRc_Pending:
			 return;

		case eAosRc_Success:
			 break;

		default:
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "SSL input process failed: %d", ret);
			goto error;
		}

		ret = AosSslRecord_nextMsg(record, (context->flags & eAosSSLFlag_ExpectFinished)?1:0 );
		if (ret ) // no more message
		{
			break;
		}
	}
	
	if ( record->direct_type == eAosSslRecord_Input )
	{
		AosSslStm_skbInput( context, NULL, context->callback );
		if ( !atomic_read( &context->in_to_card_cnt ) )
		{
			if ( context->wo && (context->wo->tcp_state & eAosSslWrapper_tcpRcvFin ) )
		   	{
				// AosSslWrapper_finReceived( &context->wo->sk );
			}
		}
	}
	else
	{
		AosSslStm_skbOutput(context, NULL, context->callback);
		if ( !atomic_read( &context->out_to_card_cnt ) )
		{	
			//if ( context->wo && (context->wo->tcp_state & eAosSslWrapper_tcpSndFin ) )
		    	// AosSslWrapper_sendFin( &context->wo->sk);
		}
	}
	

	AosSslRecord_release( record );
	
	if ( context->wo && ( context->wo->tcp_state & eAosSslWrapper_tcpHasData ) )
		
	{
		/*
		if ( AosSslWrapper_isWritable( &context->wo->base ) )
		{
			context->wo->tcp_state &= ~eAosSslWrapper_tcpHasData;
			context->wo->base.po->svc->app->dataAckReceived( context->wo->base.po, &context->wo->base );
		}*/
	}
	
	
	return;
error:
	aos_list_for_each_entry_safe( record_this, record_next, &context->record_in_list, list ) 
	{
		aos_list_del( &record_this->list );
		AosSslRecord_release( record_this );
	}
	aos_list_for_each_entry_safe( record_this, record_next, &context->record_out_list, list ) 
	{
		aos_list_del( &record_this->list );
		AosSslRecord_release( record_this );
	}
	ret = AosSsl_createAndSendAlertRecord(context, error_code);
	AosSslRecord_release( record );
	return;
}

int AosSslStm_ocspCallback(void *certinfo, void *contextin, int rc)
{
	struct AosSslContext *context = (struct AosSslContext*)contextin;
	int ret;

	aos_trace("AosSslStm_ocspCallback");
	aos_assert1(context);
	aos_assert1(certinfo);

	AosCert_put((struct _X509_CERT_INFO*)certinfo);


	if ( rc == eAosRc_CertificateValid )
	{
		aos_trace("OCSP result: Certificate is verified, context %x", context);
		context->state = eAosSSLState_ClientCertOCSP;
		ret = AosSslStm_skbInput(
				context,
				0, //data, 
				context->callback);
		//		AosAppProxy_connPut(context->conn);
		return ret;
	}

	if (rc == eAosRc_CertificateInvalid || rc == eAosRc_Error)
	{
		if (rc == eAosRc_CertificateInvalid)
		        aos_trace("OCSP result: Certificate is invalid, context %x", context);
		else
		        aos_trace("OCSP result: error in OCSP");
		AosSsl_createAndSendAlertRecord(context, eAosSSLErr_LevelFatal|eAosSSLErr_CertificateRevoked);
		context->state = eAosSSLState_Error;
		ret = AosSslStm_skbInput(
				context,
				0, //data,
				context->callback);
		// AosAppProxy_connPut(context->conn);
		return ret;
	}

	return rc;
}

int AosSslStm_Established(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	uint16 *error_code)
{
	int ret;
	aos_trace(" AosSslStm_Established ");
	ret = AosSslRecord_setMsg( record, (context->flags & eAosSSLFlag_ExpectFinished)?1:0 );

	if ( ret != eAosRc_Success )
	{
		*error_code = eAosSSL_AlertInternalError;
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "SSL established stm, parse record message failed: %d", ret);
		return -eAosAlarm_SSLProcErr;
	}
	if ( gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro )
	{
		return AosSslRecord_stateCsp1Established( context, record, callback, error_code);
	}
	return AosSslRecord_stateEstablished( context, record, callback, error_code);
}

int AosSslStm_inputCallback(int rc, struct AosSslWrapperObject *wo, char *rslt_data, int rslt_data_len)
{
	return 0;	
}


EXPORT_SYMBOL( AosSslStm_skbInput );
EXPORT_SYMBOL( AosSslStm_skbOutput);
EXPORT_SYMBOL( AosSslContext_release );

