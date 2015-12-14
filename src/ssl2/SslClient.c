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

#include "ssl2/SslClient.h"

#include "aos/aosKernelApi.h"
#include "aosUtil/Random.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/StringUtil.h"
#include "KernelSimu/string.h"
#include "PKCS/CertMgr.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/ReturnCode.h"
#include "ssl2/SslStatemachine.h"
#include "ssl2/SslSessionMgr.h"
#include "ssl2/SslMisc.h"
#include "ssl2/SslServer.h"
#include "ssl2/Ssl.h"
#include "ssl2/rsa.h"
#include "ssl2/SslRecord.h"
#include "ssl2/SslCipher.h"

#ifdef __KERNEL__
#include <asm/current.h>
#include <linux/sched.h>
#include "Ktcpvs/tcp_vs.h"
#endif

static X509_CERT_INFO * sg_system_cert = 0;
static AosRsaPrivateKey_t * sg_system_private_key = 0;


static int AosSslClient_serverHelloDone(
			AosSslRecord_t *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
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
			AosSslRecord_t *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
			SSL_PROC_CALLBACK callback, 
            u16 *errcode)	
{
	char *recved;
	int ret;

	aos_debug_log(eAosMD_SSL, "Enter server Finished");

	aos_assert1(record->record_type == eAosContentType_Handshake);
	context->flags &= ~eAosSSLFlag_ExpectFinished;
	record->msg_type = eAosSSLMsg_Finished;

	recved = AosSslRecord_getMsg(record);

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		if (0 != memcmp(context->EncryptServerFinishedMessage,	recved, record->record_body_len))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed server finished verification");
		}
	}
	else
	{
		if ((ret = AosSSL_verifyFinished(context, eAosSSLServer, 
			(uint8*)recved, record->record_body_len)))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed server finished verification");
		}
	}
		
	if (context->isResumed)
	{
		AosSslRecord_t msg_created;
		u32 msglen = 0;
		char *new_msg_body;

		// 
		// If it is resumed, we need to create:
		// 	ChangeCipherSpec
		//  Finished
		// 
		AosSslRecord_init(&msg_created, 5000);

		//
		// Create ChangeCipherSpec. Note this message shall not be
		// included in the Finished message.
		//
		AosSslRecord_createChangeCipherSpec(&msg_created);
		AosSslRecord_moveToNextMsg(&msg_created);

		//
		// Create the Finished Message
		//
		if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
		{
			int finished_msg_len;
			new_msg_body = AosSslRecord_getMsg(&msg_created);
			AosSsl_getFinishMsgLen(context, &finished_msg_len);
			memcpy(new_msg_body, context->EncryptClientFinishedMessage, finished_msg_len);
			AosSslRecord_createFinishedMsg(&msg_created, finished_msg_len);
			AosSslRecord_moveToNextMsg(&msg_created);
		}
		else
		{
			new_msg_body = AosSslRecord_getMsg(&msg_created);
			AosSSL_createFinishedMsg(context->hash_len, context,
				new_msg_body, &msglen, eAosSSLClient);

			aos_assert1(msglen > 4);
			AosSslRecord_createFinishedMsg(&msg_created, msglen);
			AosSslRecord_moveToNextMsg(&msg_created);
		}
		//
		// Send the message
		//
		ret = AosSslRecord_send(&msg_created, context->sock);
		AosSslRecord_release(&msg_created);
	}
	else
	{
		if (AosSsl_ClientSessionReuse)
		{
			// 
			// Need to save the session for future reuse
			//
			aos_assert1(context->conn);
			aos_assert1(context->conn->dest);
			AosSsl_setClientSession(
				context->conn->dest->addr,
				(char *)context->SessionId,
				(char *)context->MasterSecret,
				context->ClientRandom,
				context->ServerRandom);
		}
	}

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		if (context->EncryptServerFinishedMessage)
		{
			aos_free(context->EncryptServerFinishedMessage);
			context->EncryptServerFinishedMessage = NULL;
		}

		if (context->EncryptClientFinishedMessage)
		{
			aos_free(context->EncryptClientFinishedMessage);
			context->EncryptClientFinishedMessage = NULL;
		}
	}	

	if (context->Handshake)
	{
		aos_free(context->Handshake);
		context->Handshake = NULL;
		context->HandshakeLength = 0;
	}

	context->state = eAosSSLState_Established;
	context->stateFunc = AosSsl_Established;
	AosSsl_preEstablished(context);
	return 0;
}


// 
// State: ClientFinishedSent 
// Processing:
//	ClientFinished was sent and it is expecting ChangeCipherSpec 
//  message. If it is not ChangeCipherSpec, it is an error.
//		
static int AosSslClient_serverChangeCipherSpec(
			AosSslRecord_t *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
			SSL_PROC_CALLBACK callback, 
            u16 *errcode)	
{
	u32 key_block_length;
	aos_debug("Enter serverChangeCipherSpec");

	if (record->record_type != eAosContentType_ChangeCipherSpec)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Expecting ChangeCipherSpec but received is not: %d", 
			record->record_type);
	}

	if ((gAosSslAcceleratorType != eAosSslAcceleratorType_CaviumPro) && context->isResumed)
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
			context->MasterSecret,
			context->ServerRandom,
			context->ClientRandom,
			context,
			context->hash_len,
			key_block_length);
	}

	context->flags |= eAosSSLFlag_ExpectFinished;
	context->state = eAosSSLState_ServerChangeCipherSpecRcvd;
	context->stateFunc = AosSslClient_serverFinished;

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
			AosSslRecord_t *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
			SSL_PROC_CALLBACK callback, 
            u16 *errcode)	
{
	// 
	// If it is not a Certificate, return the error.
	//

	aos_debug("Enter AosSslClient_certificateRequest");

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
	AosSSL_addHandshake(context, record);

	context->state = eAosSSLState_ServerHelloDone;
	context->stateFunc = AosSslClient_serverHelloDone;
	return 0;
}

static int AosSslClient_csp1Finished_Finish(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	struct AosSslRecord record_out;
	char *msg_created;
	int finished_msg_len;
	int ret;

	aos_free(context->Handshake);
	context->Handshake = NULL;

	//
	// Create the ChangCipher Message
	//
	AosSslRecord_init(&record_out, 200);
	AosSslRecord_createChangeCipherSpec(&record_out);
	AosSslRecord_moveToNextMsg(&record_out);

	// 
	// Create the Client Finished Message
	//
	msg_created = AosSslRecord_getMsg(&record_out);
	AosSsl_getFinishMsgLen(context, &finished_msg_len);
	memcpy(msg_created, context->EncryptClientFinishedMessage, finished_msg_len);
	AosSslRecord_createFinishedMsg(&record_out, finished_msg_len);
	AosSslRecord_moveToNextMsg(&record_out);
	
	// 
	// Send the message
	//
	if ((ret = AosSslRecord_send(&record_out, context->sock)) < 0 )
	{
		AosSslRecord_release(&record_out);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send client cert verify message, %d", ret);
	}
	AosSslRecord_release(&record_out);

	context->state = eAosSSLState_ClientFinishedSent;
	context->stateFunc = AosSslClient_serverChangeCipherSpec;
	return 0;

	
}

static int AosSslClient_csp1Finished(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	struct AosSslRecord record_out;
	char *msg_created_body;
	uint32 request_id;
	int ret;

	aos_debug("Enter AosSslClient_csp1Finished");

	//
	// create and send client verify data message
	//
	aos_assert1(context->verify_data);
	AosSslRecord_init(&record_out, 200);
	msg_created_body = AosSslRecord_getMsgBody(&record_out);
	msg_created_body[0] = (MAX_ENCRYPTED_VERIFY_DATA_SIZE >> 8);
	msg_created_body[1] = MAX_ENCRYPTED_VERIFY_DATA_SIZE;
	memcpy(msg_created_body+2, context->verify_data, MAX_ENCRYPTED_VERIFY_DATA_SIZE);
	AosSslRecord_createHandshake(
		&record_out, 
		eAosSSLMsg_CertificateVerify, 
		MAX_ENCRYPTED_VERIFY_DATA_SIZE+2);
	AosSSL_addHandshake(context, &record_out);

	aos_free(context->verify_data);
	context->verify_data = NULL;
	// 
	// Send the message
	//
	if ((ret = AosSslRecord_send(&record_out, context->sock)) < 0 )
	{
		AosSslRecord_release(&record_out);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "failed to send client cert verify message, %d", ret);
	}
	AosSslRecord_release(&record_out);

	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128: 
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client/Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Csp1FinishedRc4Finish (
			gAosSslCaviumRequestType,
			context->cavium_context,
			(HashType)context->hash_type,
			context->ssl_version,
			context->HandshakeLength,
			context->Handshake,
			context->EncryptClientFinishedMessage,
			context->EncryptServerFinishedMessage, 
			&request_id);
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

	return AosSslClient_csp1Finished_Finish(record, conn, context, callback, errcode);

}

static int AosSslClient_csp1OtherVerify_Finish(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	uint32 request_id;
	int ret;

	aos_debug("Enter AosSslClient_csp1OtherVerify_Finish");

	//
	// Get Private Key
	//
	if (!sg_system_private_key &&
	     !(sg_system_private_key = AosCertMgr_getSystemPrivKey()))
	{
			*errcode = eAosSSLErr_NoCertificate;
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "No private key configured!");
	}

	//
	// Sign verify data
	//
	ret = Csp1Pkcs1v15CrtEnc(
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
		&request_id);
	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "sign client verify data failed!");
	}
	return AosSslClient_csp1Finished(record, conn, context, callback, errcode);
}


static int AosSslClient_csp1OtherVerify(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)
{
	uint32 request_id;
	int ret;

	aos_debug("Enter AosSslClient_csp1OtherVerify");

	ret = Csp1WriteContext(
			CAVIUM_BLOCKING,
		       context->cavium_context+128, 
		       PRE_MASTER_SECRET_LENGTH,
		       context->PreMasterSecret,
		       &request_id); // ?? should we do it , How many space pointed by cavium_context 

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"AosSslCient_csp1OtherVerify write premastersecret to context failed");
	}
	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128: 
		context->verify_data = (uint8*)aos_malloc(MAX_ENCRYPTED_VERIFY_DATA_SIZE);
		aos_assert1(context->verify_data);
		ret = Csp1OtherVerifyRc4 (
				gAosSslCaviumRequestType,
				context->cavium_context,
				(HashType)context->hash_type,
				context->ssl_version,
				RC4_128,
				RETURN_ENCRYPTED,
				PRE_MASTER_SECRET_LENGTH,
				context->ClientRandom,
				context->ServerRandom,
				context->HandshakeLength,
				context->Handshake,
				context->verify_data,
				context->MasterSecret, 
				&request_id);
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
	context->HandshakeLength = 0;
	return AosSslClient_csp1OtherVerify_Finish(record, conn, context, callback, errcode);

}

static int AosSslClient_csp1OtherFullFinish(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	AosSslRecord_t record_out;
	char *msg_created;
	int finished_msg_len;
	int ret;

	aos_debug("Enter AosSslClient_csp1OtherFullFinish");

	AosSslRecord_init(&record_out, eAosSSL_HandshakeMaxLen);
	
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
	AosSslRecord_createChangeCipherSpec(&record_out);
	AosSslRecord_moveToNextMsg(&record_out);

	
	// 
	// Create the Client Finished Message
	//
	msg_created = AosSslRecord_getMsg(&record_out);
	AosSsl_getFinishMsgLen(context, &finished_msg_len);
	memcpy(msg_created, context->EncryptClientFinishedMessage, finished_msg_len);
	AosSslRecord_createFinishedMsg(&record_out, finished_msg_len);
	AosSslRecord_moveToNextMsg(&record_out);
	
	// 
	// Send the message
	//
	ret = AosSslRecord_send(&record_out, context->sock);
	AosSslRecord_release(&record_out);

	context->state = eAosSSLState_ClientFinishedSent;
	context->stateFunc = AosSslClient_serverChangeCipherSpec;
	return 0;

}
static int AosSslClient_csp1OtherFull(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;

	aos_debug("Enter AosSslClient_csp1OtherFull");

	ret = Csp1WriteContext(
			CAVIUM_BLOCKING,
		       context->cavium_context+128, 
		       PRE_MASTER_SECRET_LENGTH,
		       context->PreMasterSecret,
		       &request_id);

	if (ret)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"AosSslCient_csp1OtherFull write premastersecret to context failed");
	}
	
	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128: 
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client/Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Csp1OtherFullRc4 (
				gAosSslCaviumRequestType,
				context->cavium_context,
				(HashType)context->hash_type,
				context->ssl_version,
				RC4_128,
				RETURN_ENCRYPTED,
				PRE_MASTER_SECRET_LENGTH,
				context->ClientRandom,
				context->ServerRandom,
				context->HandshakeLength,
				context->Handshake,
				context->EncryptClientFinishedMessage,
				context->EncryptServerFinishedMessage,
				context->MasterSecret, 
				&request_id);
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

	return AosSslClient_csp1OtherFullFinish(record, conn, context, callback, errcode);
		
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
			AosSslRecord_t *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
			SSL_PROC_CALLBACK callback, 
            u16 *errcode)	
{
	// 
	// If it is not a Certificate, return the error.
	//
	u32 msglen = 0;
	int ret;
	unsigned int outputLen;
	char *tmp;
	u32 key_block_length;
	char *new_msg_body;
	AosSslRecord_t msg_created;
	struct R_RSA_PUBLIC_KEY pubkey;

	aos_debug("Enter AosSslClient_serverHelloDone");

	if (AosSslRecord_getMsgType(record) == eAosSSLMsg_CertificateRequest)
	{
		// 
		// It is Certificate request. 
		// 
		return AosSslClient_certificateRequest(record, conn, 
			context, callback, errcode);
	}

	aos_assert1(context);
	aos_assert1(record->record_type == eAosContentType_Handshake);

	msglen = record->msg_body_len; 

	if (msglen != 0)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"ServerHelloDone length is not zero: %d", msglen);
	}

	// 
	// Accumulate the message for FINISHED
	//
	AosSSL_addHandshake(context, record);

	// 
	// Time to construct:
	//	Certificate;		if ClientAuth is on
	//	ClientKeyExchange; 
	//  CertificateVerify;	if ClientAuth is on
	//	ChangeCipherSpec;
	//	Finished
	//

	AosSslRecord_init(&msg_created, 5000);
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
			AosSslRecord_release(&msg_created);
			return aos_warn(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Server wants client authentication but no certificate");
		}
	
		// 
		// Create the certificate message
		//
		ret = AosSsl_createCertificateMsg(context, &msg_created, sg_system_cert);
		if (ret)
		{
			AosSslRecord_release(&msg_created);
			return aos_warn(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed to created Certificate message: %d", ret);
		}
			
		AosSslRecord_moveToNextMsg(&msg_created);
	}

	// 
	// Create ClientKeyExchange. Currently we support RSA only.
	//
	// bytes[0..n]		The RSA encrypted pre-master.
	//
	new_msg_body = AosSslRecord_getMsgBody(&msg_created);

	context->PreMasterSecret[0] = 3;
	context->PreMasterSecret[1] = 0;
	tmp = (char *)&context->PreMasterSecret[2];
	AosRandom_get(tmp, 46);

	// 
	// Encrypt it 
	//
	switch (context->key_exchange_type)
	{
	case eAosKeyExchange_RSA:
		aos_assert1(context->PeerCert);
		pubkey.bits = (context->PeerCert->pubkey.modulus.len << 3);
		memcpy(pubkey.modulus, context->PeerCert->pubkey.modulus.data, context->PeerCert->pubkey.modulus.len);
		memset(pubkey.exponent, 0, MAX_RSA_MODULUS_LEN);
		pubkey.exponent[MAX_RSA_MODULUS_LEN-1] = 1;
		pubkey.exponent[MAX_RSA_MODULUS_LEN-3] = 1;

		if ((ret = aos_ssl_asycipher.pubenc((unsigned char *)new_msg_body, 
				&outputLen, context->PreMasterSecret, 48, 
				&(pubkey))))
		 {
			 return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
				 "Failed to encrypt pre-master: %d", ret);
		 }
		 aos_assert1(outputLen < 256);
		 msglen = 9 + outputLen;
		 break;

	default:
		 return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
			"Key Exchange algorithm not supported: %d", 
			context->key_exchange_type);
	}

	AosSslRecord_createHandshake(&msg_created, 
		eAosSSLMsg_ClientKeyExchange, outputLen);
	AosSSL_addHandshake(context, &msg_created);
	AosSslRecord_moveToNextMsg(&msg_created);

	// 
	// Send ClientKeyExchange message
	//
	ret = AosSslRecord_send(&msg_created, context->sock);
	AosSslRecord_release(&msg_created);
	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		if (!(context->flags & eAosSSLFlag_ClientAuth))
			return AosSslClient_csp1OtherFull(record, conn, context, callback, errcode);
		else
			return AosSslClient_csp1OtherVerify(record, conn, context, callback, errcode);
	}

	// 
	// Create the MasterSecret
	//
	AosSsl_getMasterSecret(context->PreMasterSecret, PRE_MASTER_SECRET_LENGTH,
        context->MasterSecret, context->ClientRandom, 
		context->ServerRandom);


	AosSslRecord_init(&msg_created, 5000);
	
	//
	// Determine the hash length
	//
    	key_block_length=2*(context->hash_len + context->encrypt_key_len + context->iv_len);

	// 
	// Calculate all the keys
	//
    AosSsl_calculateKeyBlock(
    	context->MasterSecret, 
    	context->ServerRandom,
       	context->ClientRandom, 
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

		if (!aos_ssl_hardware_flag && !sg_system_private_key &&
			!(sg_system_private_key = AosCertMgr_getSystemPrivKey()))
		{
			*errcode = eAosSSLErr_NoCertificate;
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"No private key configured!");
		}

		msglen = MD5_LEN + SHA1_LEN;
		aos_assert1(msglen < 200);
		aos_assert1(msglen < AosSslRecord_getRemainLen(&msg_created));
		ret = AosSsl_createCltCertVeriMsg(context, (uint8*)local, msglen);

		if (ret)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed to create CertificateVerify messag: %d", ret);
		}

		// 
		// Encrypt the message using the certificate's private key
		//
		new_msg_body = AosSslRecord_getMsgBody(&msg_created);
		ret = aos_ssl_asycipher.prienc((unsigned char *)(new_msg_body+2), &new_msglen, 
			(unsigned char *)local, msglen, sg_system_private_key);
		if (ret)
		{
			*errcode = eAosSSLErr_InternalErr;
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Failed to encrypt the message: %d", ret);
		}

		new_msg_body[0] = (new_msglen >> 8);
		new_msg_body[1] = new_msglen;

		AosSslRecord_createHandshake(&msg_created, 
			eAosSSLMsg_CertificateVerify, new_msglen+2);
		AosSSL_addHandshake(context, &msg_created);
		AosSslRecord_moveToNextMsg(&msg_created);
	}

	// 
	// Create ChangeCipherSpec. Note this message shall not be 
	// included in the Finished message. 
	//
	AosSslRecord_createChangeCipherSpec(&msg_created);
	AosSslRecord_moveToNextMsg(&msg_created);

	
	// 
	// Create the Finished Message
	//
	new_msg_body = AosSslRecord_getMsg(&msg_created);
	AosSSL_createFinishedMsg(context->hash_len, context, 
		new_msg_body, &msglen, eAosSSLClient);

	aos_assert1(msglen > 4);
	AosSslRecord_createFinishedMsg(&msg_created, msglen);
	AosSslRecord_moveToNextMsg(&msg_created);

	// 
	// Send the message
	//
	ret = AosSslRecord_send(&msg_created, context->sock);
	AosSslRecord_release(&msg_created);

	context->state = eAosSSLState_ClientFinishedSent;
	context->stateFunc = AosSslClient_serverChangeCipherSpec;
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
			AosSslRecord_t *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
			SSL_PROC_CALLBACK callback, 
            u16 *errcode)	
{
	// 
	// If it is not a Certificate, return the error.
	//
	unsigned char *msg;
	u32 msglen = 0;
	int ret;
	u32 cert_len, totalLen;
	u32 idx = 0;
	u32 consumed = 0;
	u8 guard, first;

	aos_debug("Enter AosSslClient_serverCertificate");

	aos_assert1(record->record_type == eAosContentType_Handshake);
	aos_assert1(record->msg_type == eAosSSLMsg_Certificate);

	msg = (unsigned char *)AosSslRecord_getMsg(record);
	msglen = record->msg_body_len;
	totalLen = (msg[4] << 16) + (msg[5] << 8) + msg[6];

	//
	// Byte[4..6]		The total length of all certificates (3 bytes)
	// 
	// For each certificate, it is:
	// Byte[idx..+2]	The length of the certificate
	// Byte[...]		The certificate
	//
	idx = 7;
	guard = 0;
	first = 1;
	consumed = 3;
	while (guard++ < 5)
	{
		cert_len = (msg[idx] << 16) + (msg[idx+1] << 8) + msg[idx+2];
		consumed += cert_len + 3;
		idx += 3;
		if (consumed > msglen)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"Buffer out of bound: %d, %d", consumed, msglen);
		}

		if (first)
		{
			// if ((ret = AosCertMgr_decode(context->ServerId, (char *)&msg[idx], 
			if ((ret = AosCertMgr_decode((char *)&msg[idx], 
		 			cert_len, &context->PeerCert)))
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

		if (consumed >= msglen)
		{
			break;
		}
	}

	msglen += 4;
	context->state = eAosSSLState_CertificateRcvd;
	context->stateFunc = AosSslClient_serverHelloDone;

	// 
	// Append the message to 'context->messages'
	//
	AosSSL_addHandshake(context, record);

	return 0;
}


int AosSslClient_csp1ResumeFinish(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	context->state = eAosSSLState_ServerHelloRcvd;
	context->stateFunc = AosSslClient_serverChangeCipherSpec;
	return eAosRc_Success;
}

int AosSslClient_csp1Resume(
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	uint32 request_id;
	int ret;

	aos_debug("Enter AosSslClient_csp1Resume");

	switch(context->encrypt_type)
	{
	case eAosEncryptType_RC4_128:
		if (AosSsl_mallocFinishedMessage(context))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr, "Client/Server Finished Message allocate failed");
			return eAosAlarm_SSLSynErr;
		}
		ret = Csp1ResumeRc4 (
			gAosSslCaviumRequestType,
			context->cavium_context,
	       	(HashType)context->hash_type,
	       	context->ssl_version,
	       	RC4_128,
	       	INPUT_ENCRYPTED,
	       	context->ClientRandom,
	       	context->ServerRandom,
	       	context->MasterSecret,
	       	context->HandshakeLength,
	       	context->Handshake,
	       	context->EncryptClientFinishedMessage,
	       	context->EncryptServerFinishedMessage,
	       	&request_id);
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
	return AosSslClient_csp1ResumeFinish(record, conn, context, callback, errcode)	;	

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
//	'conn': IN
//		The connection through which the message was received.
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
	AosSslRecord_t *record, 
	struct tcp_vs_conn *conn,
	struct aosSslContext *context,
	SSL_PROC_CALLBACK callback, 
	u16 *errcode)	
{
	// 
	// If it is not a ServerHello, return the error.
	//
	unsigned char *msg;
	u32 msglen = 0;
	u16 cipher_suite, i;
	u16 sidLen;
	int ret;

	aos_debug("Enter AosSslClient_serverHello");

	aos_assert1(record->record_type == eAosContentType_Handshake);

	msg = (unsigned char *)AosSslRecord_getMsg(record);
	msglen = record->msg_body_len;

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
	if (msglen < eAosSSL_ServerHelloMinLen)
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
			"ServerHello too short: %d.", msglen);
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

		if ((u16)(eAosSSL_ServerHelloMinLen + sidLen) != msglen)
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLSynErr,
				"ServerHello too short: %d. Expected: %d", 
				msglen, 
				eAosSSL_ServerHelloMinLen + sidLen);
		}

		if (context->isResumed)
		{
			if (memcmp(context->SessionId, &msg[39], sidLen) != 0)
			{
				// 
				// We wanted to reuse the session but the server does
				// not like it.
				//
				context->isResumed = 0;
				memcpy(context->SessionId, &msg[39], sidLen);
			}
		}
		else
		{
			memcpy(context->SessionId, &msg[39], sidLen);
		}
	}
	if ( !(context->isResumed && ( gAosSslAcceleratorType == eAosSslAcceleratorType_517PM) ))
		memcpy(context->ServerRandom, &msg[6], 32);

	cipher_suite = (msg[39 + sidLen] << 8) + msg[40 + sidLen];
	for (i=0; i<AosNumClientCiphersSupported; i++)
	{
		if (AosClientCiphersSupported[i] == cipher_suite)
		{
			context->CipherSelected = cipher_suite;
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
	AosSSL_addHandshake(context, record);
	
	// 
	// If it reaches this point, the message is good and has been 
	// processed. Time to transit the state to the next state.
	//
	if (context->isResumed)
	{
		if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
		{
			return AosSslClient_csp1Resume(record, conn, context, callback, errcode);
		}
		context->state = eAosSSLState_ServerHelloRcvd;
		context->stateFunc = AosSslClient_serverChangeCipherSpec;
	}
	else
	{
		context->state = eAosSSLState_ServerHelloRcvd;
		context->stateFunc = AosSslClient_serverCertificate;
	}

	return 0;
}


