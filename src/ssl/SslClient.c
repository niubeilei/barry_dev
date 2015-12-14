////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslClient.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "ssl/SslClient.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/Random.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/string.h"
#include "PKCS/CertMgr.h"
#include "ssl/SslCommon.h"
#include "ssl/ReturnCode.h"
#include "ssl/SslStatemachine.h"
#include "ssl/SslSessionMgr.h"
#include "ssl/SslMisc.h"
#include "ssl/SslServer.h"
#include "ssl/Ssl.h"
#include "ssl/rsa.h"
#include "ssl/SslRecord.h"
#include "ssl/SslCipher.h"
#include "ssl/SslWrapper.h"
#include "Porting/TimeOfDay.h"

#ifdef __KERNEL__
#include <asm/current.h>
#include <linux/sched.h>
#include "Ktcpvs/tcp_vs.h"
#include "ssl/cavium.h"
#endif

static X509_CERT_INFO * sg_system_cert = 0;
static AosRsaPrivateKey_t * sg_system_private_key = 0;


static int AosSslClient_serverHelloDone(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode);

// 
// Ciphers supported as a client. It is defaulted to 
// the following values. This array can be modified through 
// the CLI commands.
//
u32 AosClientCiphersSupported[eAosMaxCipher] =
{
	TLS_RSA_WITH_RC4_128_MD5,
	TLS_RSA_WITH_RC4_128_SHA,
	TLS_RSA_WITH_DES_CBC_SHA,
	JNSTLS_RSA_WITH_NULL_MD5,
	JNSTLS_RSA_WITH_NULL_SHA,
	//	TLS_RSA_WITH_3DES_EDE_CBC_SHA,
};
u8 AosNumClientCiphersSupported = 5;

// 
// State: ServerChangeCipherSpec Received 
// Processing:
//  The Server ChangeCipherSpec was received and it is expecting Server 
//  Finished. If it is not finished, it is an error.
//		
static int AosSslClient_serverFinished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	char *recved;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "serverFinished start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log(eAosMD_SSL, "Enter server Finished");

	aos_assert1(record->record_cur_type == eAosContentType_Handshake);
	context->flags &= ~eAosSSLFlag_ExpectFinished;
	record->msg_cur_type = eAosSSLMsg_Finished;

	recved = AosSslRecord_getMsg(record);

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		if (0 != memcmp(context->enc_svr_fin_msg,	recved, record->record_cur_body_len))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed server finished verification");
		}
	}
	else
	{
		if ((ret = AosSSL_verifyFinished(context, eAosSSLServer, 
			(uint8*)recved, record->record_cur_body_len)))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed server finished verification");
		}
	}
		
	if (context->is_resumed)
	{
		AosSslRecord_t *record_out;
		u32 msglen = 0;
		char *new_msg_body;

		// 
		// If it is resumed, we need to create:
		// 	ChangeCipherSpec
		//  Finished
		// 
		record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output );
		aos_assert1( record_out );

		//
		// Create ChangeCipherSpec. Note this message shall not be
		// included in the Finished message.
		//
		AosSslRecord_createChangeCipherSpecRecord( record_out );
		AosSslRecord_moveToNextRecord( record_out );

		//
		// Create the Finished Message
		//
		if ( gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro )
		{
			int finished_msg_len;
			new_msg_body = AosSslRecord_getMsg( record_out );
			AosSsl_getFinishMsgLen( context, &finished_msg_len );
			memcpy( new_msg_body, context->enc_clt_fin_msg, finished_msg_len );
			AosSslRecord_createFinishedRecord( record_out, finished_msg_len );
		}
		else
		{
			new_msg_body = AosSslRecord_getMsg( record_out );
			AosSSL_createFinishedMsg( context->hash_len, context,
				new_msg_body, &msglen, eAosSSLClient );

			aos_assert1(msglen > 4);
			AosSslRecord_createFinishedRecord( record_out, msglen );
		}
		//
		// Send the message
		//
		ret = AosSslRecord_send( record_out, context );
		if (ret == eAosRc_Success)
		AosSslRecord_release( record_out );
	}
	else
	{
		if (AosSsl_ClientSessionReuse)
		{
			// 
			// Need to save the session for future reuse
			//
			aos_assert1(context->wo);
			AosSsl_setClientSession(
				context->wo->dest_addr, 
				(char *)context->session_id,
				(char *)context->master_secret,
				context->client_random,
				context->server_random);
		}
	}

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		if (context->enc_svr_fin_msg)
		{
			aos_free(context->enc_svr_fin_msg);
			context->enc_svr_fin_msg = NULL;
		}

		if (context->enc_clt_fin_msg)
		{
			aos_free(context->enc_clt_fin_msg);
			context->enc_clt_fin_msg = NULL;
		}
	}	

	if (context->handshake)
	{
		aos_free(context->handshake);
		context->handshake = NULL;
		context->handshake_len = 0;
	}

	context->state = eAosSSLState_Established;
	context->state_func = AosSslPoto_Established;
	AosSsl_preEstablished(context);
//aos_min_log( eAosMD_TcpProxy, "serverFinished end, wo %p, usec %u", context->wo, AosGetUsec() );
	return 0;
}


// 
// State: ClientFinishedSent 
// Processing:
//	ClientFinished was sent and it is expecting ChangeCipherSpec 
//  message. If it is not ChangeCipherSpec, it is an error.
//		
static int AosSslClient_serverChangeCipherSpec(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	u32 key_block_length;
	aos_debug_log( eAosMD_SSL, "Enter serverChangeCipherSpec");

	if (record->record_cur_type != eAosContentType_ChangeCipherSpec)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Expecting ChangeCipherSpec but received is not: %d", 
			record->record_cur_type);
	}

	if ((gAosSslAcceleratorType != eAosSslAcceleratorType_CaviumPro) && context->is_resumed)
	{
		//
		// Determine the hash length
		//
		key_block_length=2*(context->hash_len + 
			context->encrypt_key_len + context->iv_len);
		//
		// Calculate all the keys
		//
		AosSsl_calculateKeyBlock(
			context->master_secret,
			context->server_random,
			context->client_random,
			context,
			context->hash_len,
			key_block_length);
	}

	context->flags |= eAosSSLFlag_ExpectFinished;
	context->state = eAosSSLState_ServerChangeCipherSpecRcvd;
	context->state_func = AosSslClient_serverFinished;

	return 0;
}
	

// 
// State: CertificateRequest
// Processing:
//		The Server Certificate was received and the server
//		wants to perform client authentication. A 
//		CertificateRequest is received. This state function
//		processes the message. 
//		
static int AosSslClient_certificateRequest(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	// 
	// If it is not a Certificate, return the error.
	//

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_certificateRequest");

	aos_assert1(AosSslRecord_getMsgType(record) == 
		eAosSSLMsg_CertificateRequest);

	// 
	// Currently a client has only one certificate. We are not going
	// to analyze the CertificateRequest message.
	// 	
	context->flags |= eAosSSLFlag_ClientAuth;

	// 
	// Accumulate the message for FINISHED
	//
	AosSSL_addhandshake(context, record);

	context->state = eAosSSLState_ServerHelloDone;
	context->state_func = AosSslClient_serverHelloDone;
	return 0;
}

static int AosSslClient_csp1Finished_Finish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	struct AosSslRecord *record_out;
	char *msg_created;
	int finished_msg_len;
	int ret;

	aos_free(context->handshake);
	context->handshake = NULL;

	//
	// Create the ChangCipher Message
	//
	record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output );
	aos_assert1(record_out);
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );

	// 
	// Create the Client Finished Message
	//
	msg_created = AosSslRecord_getMsg( record_out );
	AosSsl_getFinishMsgLen( context, &finished_msg_len );
	memcpy( msg_created, context->enc_clt_fin_msg, finished_msg_len );
	AosSslRecord_createFinishedRecord( record_out, finished_msg_len );
	AosSslRecord_moveToNextRecord( record_out );
	
	// 
	// Send the message
	//
	if ( ( ret = AosSslRecord_send( record_out, context ) ) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send client cert verify message, %d", ret);
	}
	if (ret == eAosRc_Success)
	AosSslRecord_release( record_out );

	context->state = eAosSSLState_ClientFinishedSent;
	context->state_func = AosSslClient_serverChangeCipherSpec;
	return 0;

	
}

static int AosSslClient_csp1Finished(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	struct AosSslRecord *record_out;
	char *msg_created_body;
	uint32 request_id;
	int ret;

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1Finished");
	
	//
	// create and send client verify data message
	//
	aos_assert1(context->verify_data);
	record_out = AosSslRecord_create( NULL, 200, eAosSslRecord_Output );
	aos_assert1(record_out);
	msg_created_body = AosSslRecord_getMsgBody( record_out );
	msg_created_body[0] = (MAX_ENCRYPTED_VERIFY_DATA_SIZE >> 8);
	msg_created_body[1] = MAX_ENCRYPTED_VERIFY_DATA_SIZE;
	memcpy(msg_created_body+2, context->verify_data, MAX_ENCRYPTED_VERIFY_DATA_SIZE);
	AosSslRecord_createHandshake(
		record_out, 
		eAosSSLMsg_CertificateVerify, 
		MAX_ENCRYPTED_VERIFY_DATA_SIZE+2);
	AosSSL_addhandshake( context,  record_out );

	aos_free(context->verify_data);
	context->verify_data = NULL;
	// 
	// Send the message
	//
	if ( ( ret = AosSslRecord_send( record_out, context ) ) < 0 )
	{
		AosSslRecord_release( record_out );
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send client cert verify message, %d", ret);
	}
	if (ret == eAosRc_Success)
	AosSslRecord_release( record_out );

	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128: 
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client/Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Kernel_Csp1FinishedRc4Finish (
			gAosSslCaviumRequestType,
			context->cavium_context,
			(HashType)context->hash_type,
			context->ssl_version,
			context->handshake_len,
			context->handshake,
			context->enc_clt_fin_msg,
			context->enc_svr_fin_msg, 
			&request_id,
			AosSslStm_cardCallback,
			(void*)record);
		break;
	default:
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "AosSslCient_csp1Finished cipher %d is not supported", context->encrypt_type);
		return eAosAlarm_SSLSynErr;
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1Finished*Finish failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}

	//return AosSslClient_csp1Finished_Finish(record, context, callback, errcode);
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->state_func = AosSslClient_csp1Finished_Finish;
	context->callback = callback;
	context->state = eAosSSLState_Pending;

	return eAosRc_Pending;

}

static int AosSslClient_csp1OtherVerify_Finish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	uint32 request_id;
	int ret;

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1OtherVerify_Finish");

	//
	// Get Private Key
	//
	if ( !sg_system_private_key &&
	      !( sg_system_private_key = AosCertMgr_getSystemPrivKey() ) )
	{
		*errcode = eAosSSLErr_NoCertificate;
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "No private key configured!");
	}

	//
	// Sign verify data
	//
	ret = Kernel_Csp1Pkcs1v15CrtEnc(
		gAosSslCaviumRequestType,
		BT1,
		sg_system_private_key->bits/8, 
		MAX_VERIFY_DATA_SIZE, 
		sg_system_private_key->prime[1], 		// Q
		sg_system_private_key->primeExponent[1],// Eq 
		sg_system_private_key->prime[0], 		// P
		sg_system_private_key->primeExponent[0],// Ep
		sg_system_private_key->coefficient, 		// iqmp, 
		context->verify_data,	// plain
		context->verify_data,	// encrypted
		&request_id,
		AosSslStm_cardCallback,
		(void*)record);

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "sign client verify data failed!");
	}
//	return AosSslClient_csp1Finished(record, context, callback, errcode);

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->state_func = AosSslClient_csp1Finished;
	context->callback = callback;
	context->state = eAosSSLState_Pending;

	return eAosRc_Pending;
}


static int AosSslClient_csp1OtherVerify(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	uint32 request_id;
	int ret;

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1OtherVerify");

/*	ret = Kernel_Csp1WriteContext(
			CAVIUM_BLOCKING,
		       context->cavium_context+128, 
		       PRE_MASTER_SECRET_LENGTH,
		       context->pre_master_secret,
		       &request_id,
		       NULL,
		       NULL); // ?? should we do it , How many space pointed by cavium_context 

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"AosSslCient_csp1OtherVerify write premastersecret to context failed");
	}
*/	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128: 
		context->verify_data = (uint8*)aos_malloc_atomic(MAX_ENCRYPTED_VERIFY_DATA_SIZE);
		aos_assert1(context->verify_data);
		ret = Kernel_Csp1OtherVerifyRc4 (
				gAosSslCaviumRequestType,
				context->cavium_context,
				(HashType)context->hash_type,
				context->ssl_version,
				RC4_128,
				RETURN_ENCRYPTED,
				PRE_MASTER_SECRET_LENGTH,
				context->client_random,
				context->server_random,
				context->handshake_len,
				context->handshake,
				context->verify_data,
				context->master_secret, 
				&request_id,
				AosSslStm_cardCallback,
				(void*)record);
		break;
	default:
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "AosSslCient_csp1OtherVerify cipher %d is not supported", context->encrypt_type);
		return eAosAlarm_SSLSynErr;
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1OtherVerify* failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}
	context->handshake_len = 0;

	// return AosSslClient_csp1OtherVerify_Finish(record, context, callback, errcode);
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state_func = AosSslClient_csp1OtherVerify_Finish;
	context->state = eAosSSLState_Pending;

	return eAosRc_Pending;

}

static int AosSslClient_csp1WriteContextCltAuth(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	uint32 request_id;
	int ret;

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1WriteContextCltAuth");
	ret = Kernel_Csp1WriteContext(
			CAVIUM_BLOCKING,
		    context->cavium_context+128, 
		    PRE_MASTER_SECRET_LENGTH,
		    context->pre_master_secret,
		    &request_id,
			AosSslStm_cardCallback,
			(void*)record);

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"AosSslCient_csp1WriteContext wiht CltAuth write premastersecret to context failed");
	}
	
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslClient_csp1OtherVerify;

	return eAosRc_Pending;

}

static int AosSslClient_csp1OtherFullFinish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	char *msg_created;
	int finished_msg_len;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "csp1OtherFull_Finish start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1OtherFullFinish");

	record_out = AosSslRecord_create( NULL, 128, eAosSslRecord_Output );
	aos_assert1(record_out);
	// 
	// Create the CertificateVerify message, if needed
	//
	if (context->flags & eAosSSLFlag_ClientAuth)
	{
	}

	// 
	// Create ChangeCipherSpec. Note this message shall not be 
	// included in the Finished message. 
	//
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );

	
	// 
	// Create the Client Finished Message
	//
	msg_created = AosSslRecord_getMsg( record_out );
	AosSsl_getFinishMsgLen( context, &finished_msg_len );
	memcpy( msg_created, context->enc_clt_fin_msg, finished_msg_len );
	AosSslRecord_createFinishedRecord( record_out, finished_msg_len );
	
	// 
	// Send the message
	//
	ret = AosSslRecord_send( record_out, context );
	if (ret == eAosRc_Success)
	AosSslRecord_release( record_out );

	context->state = eAosSSLState_ClientFinishedSent;
	context->state_func = AosSslClient_serverChangeCipherSpec;
//aos_min_log( eAosMD_TcpProxy, "csp1OtherFull_Finish end, wo %p, usec %u", context->wo, AosGetUsec() );
	return 0;

}
static int AosSslClient_csp1OtherFull(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1OtherFull");

//aos_min_log( eAosMD_TcpProxy, "csp1OtherFull start, wo %p, usec %u", context->wo, AosGetUsec() );
/*	ret = Kernel_Csp1WriteContext(
			CAVIUM_BLOCKING,
		       context->cavium_context+128, 
		       PRE_MASTER_SECRET_LENGTH,
		       context->pre_master_secret,
		       &request_id,
		       NULL,
		       NULL);

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"AosSslCient_csp1OtherFull write premastersecret to context failed");
	}
*/	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128: 
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client/Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Kernel_Csp1OtherFullRc4 (
				gAosSslCaviumRequestType,
				context->cavium_context,
				(HashType)context->hash_type,
				context->ssl_version,
				RC4_128,
				RETURN_ENCRYPTED,
				PRE_MASTER_SECRET_LENGTH,
				context->client_random,
				context->server_random,
				context->handshake_len,
				context->handshake,
				context->enc_clt_fin_msg,
				context->enc_svr_fin_msg,
				context->master_secret, 
				&request_id,
				AosSslStm_cardCallback,
				(void*)record);
		break;
	default:
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "AosSslCient_csp1OtherFull cipher %d is not supported", context->encrypt_type);
		return eAosAlarm_SSLSynErr;
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Csp1OtherFull* failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}

//	return AosSslClient_csp1OtherFullFinish(record, context, callback, errcode);

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslClient_csp1OtherFullFinish;

//aos_min_log( eAosMD_TcpProxy, "csp1OtherFull end, wo %p, usec %u", context->wo, AosGetUsec() );
	return eAosRc_Pending;
		
}

static int AosSslClient_csp1WriteContext(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "csp1WriteContext start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1WriteContext");

	ret = Kernel_Csp1WriteContext(
			CAVIUM_BLOCKING,
			context->cavium_context+128, 
			PRE_MASTER_SECRET_LENGTH,
			context->pre_master_secret,
			&request_id,
			AosSslStm_cardCallback,
			(void*)record);

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"AosSslCient_csp1WriteContext write premastersecret to context failed");
	}
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslClient_csp1OtherFull;

//aos_min_log( eAosMD_TcpProxy, "csp1WriteContext end, wo %p, usec %u", context->wo, AosGetUsec() );
	return eAosRc_Pending;
}

static int AosSslClient_csp1ServerHelloDoneFinish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{	
	AosSslRecord_t *record_out;
	char *new_msg_body;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "csp1 serverHelloDoneFinish start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_trace("AosSslClient_csp1ServerHelloDoneFinish");

	// 
	// Time to construct:
	//	Certificate;		if ClientAuth is on
	//	ClientKeyExchange; 
	//  CertificateVerify;	if ClientAuth is on
	//	ChangeCipherSpec;
	//	Finished
	//

	record_out = AosSslRecord_create( NULL, eAosSSL_HandshakeMaxLen, eAosSslRecord_Output );
	aos_assert1( record_out );
	
	if ( context->flags & eAosSSLFlag_ClientAuth )
	{
		// 
		// Client Authentication is requested. 
		//

		if (!sg_system_cert && 
			!(sg_system_cert = AosCertMgr_getSystemCert()))
		{
			// 
			// The client does not have a certificate. 
			//
			*errcode = eAosSSLErr_NoCertificate;
			AosSslRecord_release( record_out );
			return aos_warn(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Server wants client authentication but no certificate");
		}
	
		// 
		// Create the certificate message
		//
		ret = AosSsl_createCertificateMsg( context, record_out, sg_system_cert );
		if (ret)
		{
			AosSslRecord_release( record_out );
			return aos_warn( eAosMD_SSL, eAosAlarm_SSLProcErr, "Failed to created Certificate message: %d", ret );
		}
			
		AosSslRecord_moveToNextMsg( record_out );
	}

	// 
	// Create ClientKeyExchange. Currently we support RSA only.
	//
	// bytes[0..n]		The RSA encrypted pre-master.
	//
	new_msg_body = AosSslRecord_getMsgBody( record_out );
	memcpy( new_msg_body, context->key_exchange, context->peer_cert->pubkey.modulus.len);
	aos_free( context->key_exchange );
	context->key_exchange = NULL;
	
	AosSslRecord_createHandshake( record_out, eAosSSLMsg_ClientKeyExchange, context->peer_cert->pubkey.modulus.len );
	AosSSL_addhandshake( context, record_out );
	// 
	// Send ClientKeyExchange message
	//
	ret = AosSslRecord_send( record_out, context );

	if (ret == eAosRc_Success)
	AosSslRecord_release( record_out );
	if ( !( context->flags & eAosSSLFlag_ClientAuth ) )
		return AosSslClient_csp1WriteContext( context, record, callback, errcode );
	else
		return AosSslClient_csp1WriteContextCltAuth( context, record, callback, errcode );
}

// 
// State: ServerCertReceived 
// Processing:
//		The Server Certificate was received and it expects 
//		ServerHelloDone or CertificateRequest. If the server
//		wants to perform client authentication, this state 
//		should receive "CertificateRequest". Otherwise, 
// 		this state should receive ServerHelloDone.
//		
static int AosSslClient_csp1ServerHelloDone(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	u32 msg_body_len = 0;
	uint32 request_id;
	char *tmp;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "Csp1serverHelloDone start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1ServerHelloDone");

	// 
	// If it is not a Certificate, return the error.
	//

	if (AosSslRecord_getMsgType(record) == eAosSSLMsg_CertificateRequest)
	{
		// 
		// It is Certificate request. 
		// 
		return AosSslClient_certificateRequest( context, record, callback, errcode);
	}
	
	aos_assert1( record->record_cur_type == eAosContentType_Handshake );
	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 
	if ( msg_body_len != 0 )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ServerHelloDone length is not zero: %d", msg_body_len);
	}

	// 
	// Accumulate the message for FINISHED
	//
	AosSSL_addhandshake( context, record );


	context->pre_master_secret[0] = 3;
	context->pre_master_secret[1] = 0;
	tmp = (char *)&context->pre_master_secret[2];
	AosRandom_get(tmp, 46);

	// 
	// Encrypt it 
	//
	switch (context->key_exchange_type)
	{
	case eAosKeyExchange_RSA:
		{
			//uint8 exponent[128] ;
			//memset( exponent, 0, 128 );
			//exponent[MAX_RSA_MODULUS_LEN-1] = 1;
			//exponent[MAX_RSA_MODULUS_LEN-3] = 1;
		
			uint8 exponent[3] = {1, 0, 1};
		
			aos_assert1(context->peer_cert);
			context->key_exchange = (uint8*) aos_malloc_atomic( context->peer_cert->pubkey.modulus.len );
			aos_assert1(context->key_exchange);

			ret = Kernel_Csp1Pkcs1v15Enc(
				gAosSslCaviumRequestType,
			    BT2,
				context->peer_cert->pubkey.modulus.len, 
				3,  
				48, 
				context->peer_cert->pubkey.modulus.data, 
				exponent, 
				context->pre_master_secret, 
				context->key_exchange,
				&request_id,
				AosSslStm_cardCallback,
				(void*) record );
			if ( ret )
			{
			 	return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Failed to encrypt pre-master: %d", ret);
			}
			break;		
		}
	default:
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Key Exchange algorithm not supported: %d", 
				context->key_exchange_type);
	}

//aos_min_log( eAosMD_TcpProxy, "Csp1serverHelloDone start, wo %p, usec %u", context->wo, AosGetUsec() );
	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslClient_csp1ServerHelloDoneFinish;
	
	return eAosRc_Pending;
}

// 
// State: ServerCertReceived 
// Processing:
//		The Server Certificate was received and it expects 
//		ServerHelloDone or CertificateRequest. If the server
//		wants to perform client authentication, this state 
//		should receive "CertificateRequest". Otherwise, 
// 		this state should receive ServerHelloDone.
//		
static int AosSslClient_serverHelloDone(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t *record_out;
	struct R_RSA_PUBLIC_KEY pubkey;
	unsigned int outputLen;
	u32 key_block_length;
	char *new_msg_body;
	char *tmp;
	u32 msg_body_len = 0;
	int ret;

	if ( gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro )
	{
		return AosSslClient_csp1ServerHelloDone( context, record, callback, errcode );
	}
//aos_min_log( eAosMD_TcpProxy, "serverHelloDone start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_serverHelloDone");

	// 
	// If it is not a Certificate, return the error.
	//

	if (AosSslRecord_getMsgType(record) == eAosSSLMsg_CertificateRequest)
	{
		// 
		// It is Certificate request. 
		// 
		return AosSslClient_certificateRequest( context, record, callback, errcode);
	}

	aos_assert1(context);
	aos_assert1(record->record_cur_type == eAosContentType_Handshake);

	msg_body_len = AosSslRecord_getMsgBodyLen( record ); 

	if (msg_body_len != 0)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"ServerHelloDone length is not zero: %d", msg_body_len);
	}

	// 
	// Accumulate the message for FINISHED
	//
	AosSSL_addhandshake(context, record);

	// 
	// Time to construct:
	//	Certificate;		if ClientAuth is on
	//	ClientKeyExchange; 
	//  CertificateVerify;	if ClientAuth is on
	//	ChangeCipherSpec;
	//	Finished
	//

	record_out = AosSslRecord_create( NULL, eAosSSL_HandshakeMaxLen, eAosSslRecord_Output );
	aos_assert1( record_out );
	
	if (context->flags & eAosSSLFlag_ClientAuth)
	{
		// 
		// Client Authentication is requested. 
		//

		if (!sg_system_cert && 
			!(sg_system_cert = AosCertMgr_getSystemCert()))
		{
			// 
			// The client does not have a certificate. 
			//
			*errcode = eAosSSLErr_NoCertificate;
			AosSslRecord_release( record_out );
			return aos_warn(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Server wants client authentication but no certificate");
		}
	
		// 
		// Create the certificate message
		//
		ret = AosSsl_createCertificateMsg( context, record_out, sg_system_cert );
		if (ret)
		{
			AosSslRecord_release( record_out );
			return aos_warn( eAosMD_SSL, eAosAlarm_SSLProcErr, "Failed to created Certificate message: %d", ret );
		}
			
		AosSslRecord_moveToNextMsg( record_out );
	}

	// 
	// Create ClientKeyExchange. Currently we support RSA only.
	//
	// bytes[0..n]		The RSA encrypted pre-master.
	//
	new_msg_body = AosSslRecord_getMsgBody( record_out );

	context->pre_master_secret[0] = 3;
	context->pre_master_secret[1] = 0;
	tmp = (char *)&context->pre_master_secret[2];
	AosRandom_get(tmp, 46);

	// 
	// Encrypt it 
	//
	switch (context->key_exchange_type)
	{
	case eAosKeyExchange_RSA:
		aos_assert1(context->peer_cert);
		pubkey.bits = (context->peer_cert->pubkey.modulus.len << 3);
		memcpy(pubkey.modulus, context->peer_cert->pubkey.modulus.data, context->peer_cert->pubkey.modulus.len);
		memset(pubkey.exponent, 0, MAX_RSA_MODULUS_LEN);
		pubkey.exponent[MAX_RSA_MODULUS_LEN-1] = 1;
		pubkey.exponent[MAX_RSA_MODULUS_LEN-3] = 1;

		if ((ret = aos_ssl_asycipher.pubenc((unsigned char *)new_msg_body, 
				&outputLen, context->pre_master_secret, 48, 
				&(pubkey))))
		 {
			 return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
				 "Failed to encrypt pre-master: %d", ret);
		 }
		 aos_assert1(outputLen < 256);
		 msg_body_len = 9 + outputLen;
		 break;

	default:
		 return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
			"Key Exchange algorithm not supported: %d", 
			context->key_exchange_type);
	}

	AosSslRecord_createHandshake( record_out, eAosSSLMsg_ClientKeyExchange, outputLen );
	AosSSL_addhandshake( context, record_out );

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		// 
		// Send ClientKeyExchange message
		//
		ret = AosSslRecord_send( record_out, context );
		if (ret == eAosRc_Success)
		AosSslRecord_release( record_out );
		
//aos_min_log( eAosMD_TcpProxy, "serverHelloDone end, wo %p, usec %u", context->wo, AosGetUsec() );
		if ( !( context->flags & eAosSSLFlag_ClientAuth ) )
			return AosSslClient_csp1WriteContext( context, record, callback, errcode );
		else
			return AosSslClient_csp1WriteContextCltAuth( context, record, callback, errcode );
	}

	// 
	// Create the master_secret
	//
	AosSsl_getmaster_secret(
		context->pre_master_secret, 
		PRE_MASTER_SECRET_LENGTH,
        	context->master_secret, 
        	context->client_random, 
		context->server_random);

	//
	// Determine the hash length
	//
    	key_block_length=2*(context->hash_len + context->encrypt_key_len + context->iv_len);

	// 
	// Calculate all the keys
	//
	AosSsl_calculateKeyBlock(
		context->master_secret, 
		context->server_random,
		context->client_random, 
		context, 
		context->hash_len, 
		key_block_length);

	// 
	// Create the CertificateVerify message, if needed
	//
	if (context->flags & eAosSSLFlag_ClientAuth)
	{
		// 
		// ClientAuth is on. Create CertificateVerify message
		//
		char local[200];
		unsigned int new_msglen;

		//if (!aos_ssl_hardware_flag && !sg_system_private_key &&
		//	!(sg_system_private_key = AosCertMgr_getSystemPrivKey()))
		if ((gAosSslAcceleratorType != eAosSslAcceleratorType_517PM) && !sg_system_private_key &&
			!(sg_system_private_key = AosCertMgr_getSystemPrivKey()))
		{
			*errcode = eAosSSLErr_NoCertificate;
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"No private key configured!");
		}

		msg_body_len = MD5_LEN + SHA1_LEN;
		aos_assert1(msg_body_len < 200);
		AosSslRecord_moveToNextMsg( record_out );
		aos_assert1(msg_body_len < AosSslRecord_getRemainBufferLen( record_out ));
		ret = AosSsl_createCltCertVeriMsg(context, (uint8*)local, msg_body_len);

		if (ret)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed to create CertificateVerify messag: %d", ret);
		}

		// 
		// Encrypt the message using the certificate's private key
		//
		new_msg_body = AosSslRecord_getMsgBody( record_out );
		ret = aos_ssl_asycipher.prienc((unsigned char *)(new_msg_body+2), &new_msglen, 
			(unsigned char *)local, msg_body_len, sg_system_private_key);
		if (ret)
		{
			*errcode = eAosSSLErr_InternalErr;
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed to encrypt the message: %d", ret);
		}

		new_msg_body[0] = (new_msglen >> 8);
		new_msg_body[1] = new_msglen;

		AosSslRecord_createHandshake( record_out, eAosSSLMsg_CertificateVerify, new_msglen+2 );
		AosSSL_addhandshake( context, record_out );
	}

	AosSslRecord_moveToNextRecord( record_out );
	// 
	// Create ChangeCipherSpec. Note this message shall not be 
	// included in the Finished message. 
	//
	AosSslRecord_createChangeCipherSpecRecord( record_out );
	AosSslRecord_moveToNextRecord( record_out );

	
	// 
	// Create the Finished Message
	//
	new_msg_body = AosSslRecord_getMsg( record_out );
	AosSSL_createFinishedMsg(context->hash_len, context, 
		new_msg_body, &msg_body_len, eAosSSLClient);

	aos_assert1(msg_body_len > 4);
	AosSslRecord_createFinishedRecord( record_out , msg_body_len );

	// 
	// Send the message
	//
	ret = AosSslRecord_send( record_out, context );
	if (ret == eAosRc_Success)
	AosSslRecord_release( record_out );

	context->state = eAosSSLState_ClientFinishedSent;
	context->state_func = AosSslClient_serverChangeCipherSpec;
	return 0;
}


// 
// State: ServerHelloReceived
// Processing:
//		The ServerHello was received and it expects Certificate. 
//		If it is not a Certificate, it is an error. It returns
//		the error. The caller should close the connection.
//		
static int AosSslClient_serverCertificate(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	// 
	// If it is not a Certificate, return the error.
	//
	unsigned char *msg_body;
	u32 msg_body_len = 0;
	int ret;
	u32 cert_len, total_len;
	u32 idx = 0;
	u32 consumed = 0;
	u8 guard, first;

//aos_min_log( eAosMD_TcpProxy, "serverCertificate start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_serverCertificate");

	aos_assert1(record->record_cur_type == eAosContentType_Handshake);
	aos_assert1(record->msg_cur_type == eAosSSLMsg_Certificate);

	msg_body = (unsigned char *)AosSslRecord_getMsgBody( record );
	msg_body_len = AosSslRecord_getMsgBodyLen( record );
	total_len = (msg_body[0] << 16) + (msg_body[1] << 8) + msg_body[2];
	//
	// Byte[4..6]		The total length of all certificates (3 bytes)
	// 
	// For each certificate, it is:
	// Byte[idx..+2]	The length of the certificate
	// Byte[...]		The certificate
	//
	idx = 3;
	guard = 0;
	first = 1;
	consumed = 3;
	while (guard++ < 5)
	{
		cert_len = (msg_body[idx] << 16) + (msg_body[idx+1] << 8) + msg_body[idx+2];
		consumed += cert_len + 3;
		idx += 3;
		if (consumed > msg_body_len)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Buffer out of bound: %d, %d", consumed, msg_body_len);
		}

		if (first)
		{
			if ((ret = AosCertMgr_decode((char *)&msg_body[idx], 
		 			cert_len, &context->peer_cert)))
			{
				// 
				// Should not happen
				//
				return aos_alarm(eAosMD_SSL, eAosAlarm_VerifyCertFailed, 
			 		"Failed to verify certificate: %d", ret);
			}

			first = 0;
		}

		idx += cert_len;

		if (consumed >= msg_body_len)
		{
			break;
		}
	}

	context->state = eAosSSLState_CertificateRcvd;
	context->state_func = AosSslClient_serverHelloDone;

	// 
	// Append the message to 'context->messages'
	//
	AosSSL_addhandshake(context, record);

//aos_min_log( eAosMD_TcpProxy, "serverCertificate end, wo %p, usec %u", context->wo, AosGetUsec() );
	return 0;
}


int AosSslClient_csp1ResumeFinish(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	context->state = eAosSSLState_ServerHelloRcvd;
	context->state_func = AosSslClient_serverChangeCipherSpec;
	return eAosRc_Success;
}

int AosSslClient_csp1Resume(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;

	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_csp1Resume");

	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client/Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Kernel_Csp1ResumeRc4 (
			gAosSslCaviumRequestType,
			context->cavium_context,
	       	(HashType)context->hash_type,
	       	context->ssl_version,
	       	RC4_128,
	       	INPUT_ENCRYPTED,
	       	context->client_random,
	       	context->server_random,
	       	context->master_secret,
	       	context->handshake_len,
	       	context->handshake,
	       	context->enc_clt_fin_msg,
	       	context->enc_svr_fin_msg,
	       	&request_id,
	       	AosSslStm_cardCallback,
	       	(void*)record);
		break;
	default:
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "AosSslClient_Csp1Resume cipher %d is not supported", context->encrypt_type);
		return eAosAlarm_SSLSynErr;
	}
	
	if(ret)
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "AosSslClient_csp1Resume Csp1Resume* failed, ret is %d", ret);
		return eAosAlarm_SSLSynErr;
	}
//	return AosSslClient_csp1ResumeFinish(record, context, callback, errcode)	;	

	AosSslContext_hold( record->context );
	atomic_inc( &context->in_to_card_cnt );
	context->callback = callback;
	context->state = eAosSSLState_Pending;
	context->state_func = AosSslClient_csp1ResumeFinish;

	return eAosRc_Pending;
}

// 
// State: ClientHelloSent
// Processing:
//		The ClientHello was sent and it expects ServerHello. 
//		If it is not a ServerHello, it is an error. It returns
//		the error. The caller should close the connection.
//		
//		If the message contains 'Certificate' message, this function
//		will transit to 'ServerHelloReceived' and let that state
//		process the message. 
//
// Parameters:
//	'record': IN
//		The message (may contain multiple messages).
//
//	'buffer': IN/OUT
//		If this function generates messages, the messages are 
//		stored in 'buffer'. The caller should have allocate 
//		large enough memory to hold the returned messages.
//
//	'bufflen': IN
//		The length of the memory 'buffer'.
//
//	'index': OUT
//		If this function generates messages, 'index' is the length
//		of the messages.
//
int AosSslClient_serverHello(
	struct AosSslContext *context,
	AosSslRecord_t *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	// 
	// If it is not a ServerHello, return the error.
	//
	unsigned char *msg;
	u32 msg_body_len = 0;
	u16 cipher_suite, i;
	u16 sidLen;
	int ret;

//aos_min_log( eAosMD_TcpProxy, "serverHello start, wo %p, usec %u", context->wo, AosGetUsec() );
	aos_debug_log( eAosMD_SSL, "Enter AosSslClient_serverHello");

	aos_assert1(record->record_cur_type == eAosContentType_Handshake);

	msg = (unsigned char *)AosSslRecord_getMsg( record );
	msg_body_len = AosSslRecord_getMsgBodyLen( record );

	//
	// Process the message. If the message syntax error, return the
	// error. 
	//
	// Byte[4]			Protocol Version, Major
	// Byte[5]			protocol Version, Minor
	// Byte[6..38]		Random (32 bytes)
	// Byte[39]			Session ID length
	// Byte[...n]		The session ID
	// Byte[n+1, n+2]	CipherSuite
	// Byte[n+3]		Compression Method
	//
	// ServerHello should be at least 38 bytes long.
	//
	if ( msg_body_len < eAosSSL_ServerHelloMinLen )
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"ServerHello too short: %d.", msg_body_len);
	}
	sidLen = (unsigned int)msg[38];
	if (sidLen)
	{
		// 
		// Session ID is not null.
		//
		if (sidLen > eAosSSL_SessionIdLen)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Session ID too long: %d", sidLen);
		}

		if ((u16)(eAosSSL_ServerHelloMinLen + sidLen) != msg_body_len)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"ServerHello too short: %d. Expected: %d", 
				msg_body_len, 
				eAosSSL_ServerHelloMinLen + sidLen);
		}

		if (context->is_resumed)
		{
			if (memcmp(context->session_id, &msg[39], sidLen) != 0)
			{
				// 
				// We wanted to reuse the session but the server does
				// not like it.
				//
				context->is_resumed = 0;
				memcpy(context->session_id, &msg[39], sidLen);
			}
		}
		else
		{
			memcpy(context->session_id, &msg[39], sidLen);
		}
	}
	if ( !(context->is_resumed && ( gAosSslAcceleratorType == eAosSslAcceleratorType_517PM) ))
		memcpy(context->server_random, &msg[6], 32);

	cipher_suite = (msg[39 + sidLen] << 8) + msg[40 + sidLen];
	for (i=0; i<AosNumClientCiphersSupported; i++)
	{
		if (AosClientCiphersSupported[i] == cipher_suite)
		{
			context->cipher_selected = cipher_suite;
			if ((ret = AosSSL_parseCipher(context)))
			{
				return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
					"Invalid CipherSuite: %d", cipher_suite);
			}
			break;
		}
	}

	if (i >= AosNumClientCiphersSupported)
	{
		// 
		// The server returned cipher suite is not one of the ones
		// the client sent.
		//
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
			"Cipher Suites not supported: %d", cipher_suite);
	}

	// 
	// Append the message to 'context->messages'
	//
	AosSSL_addhandshake(context, record);
	
	// 
	// If it reaches this point, the message is good and has been 
	// processed. Time to transit the state to the next state.
	//
	if ( context->is_resumed )
	{
		if ( gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro )
		{
			return AosSslClient_csp1Resume( context, record, callback, errcode );
		}
		context->state = eAosSSLState_ServerHelloRcvd;
		context->state_func = AosSslClient_serverChangeCipherSpec;
	}
	else
	{
		context->state = eAosSSLState_ServerHelloRcvd;
		context->state_func = AosSslClient_serverCertificate;
	}

//aos_min_log( eAosMD_TcpProxy, "serverHello end, wo %p, usec %u", context->wo, AosGetUsec() );
	return 0;
}


