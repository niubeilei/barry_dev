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

#include "KernelSimu/atomic.h"

#include "ssl2/SslStatemachine.h"

#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tcp.h"
#include "aosUtil/Random.h"
#include "KernelSimu/string.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/ReturnCode.h"
#include "ssl2/SslServer.h"
#include "ssl2/SslClient.h"
#include "ssl2/SslRecord.h"
#include "ssl2/SslMisc.h"
#include "PKCS/x509.h"

#include "Porting/TimeOfDay.h"
#include "ssl2/cavium.h"

//Ciphers supported as the Server
u32 sgServerCiphersSupported[eAosMaxCipher] =
{
	TLS_RSA_WITH_RC4_128_MD5,	// 4
	TLS_RSA_WITH_RC4_128_SHA,	// 5
    TLS_RSA_WITH_DES_CBC_SHA,  	// 9
	JNSTLS_RSA_WITH_NULL_MD5,  	// 0x81,
    JNSTLS_RSA_WITH_NULL_SHA,	// 0x82,
};
u8 sgNumServerCiphersSupported = 5;

extern u32 AosClientCiphersSupported[eAosMaxCipher];
extern u8 AosNumClientCiphersSupported;
extern int gAosSslAcceleratorType;
extern int aos_ssl_hardware_flag;
extern ContextType gAosCaviumContextType;
extern KeyMaterialLocation gAosCaviumKeyMaterialLocation;

static int AosSslContext_constructor(struct aosSslContext **context)
{
	u32 ret;
	(*context) = (struct aosSslContext *)
	aos_malloc(sizeof(struct aosSslContext));
	aos_assert1(*context);

	memset((*context), 0, sizeof(struct aosSslContext));

	(*context)->Handshake = (uint8*)aos_malloc(eAosSSL_HandshakeMaxLen);
	if (!(*context)->Handshake)
	{
		aos_free(*context);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Handshake malloc failed");
	}

	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro ||
	    gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumGen)
	{
		if ((ret = Csp1AllocContext(gAosCaviumContextType, &(*context)->cavium_context)))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1AllocContext Failed, ret is %d", ret);
			aos_free(*context);
			*context = NULL;
			return eAosRc_Error;
		}
		if ((ret = Csp1AllocKeyMem(gAosCaviumKeyMaterialLocation, &(*context)->cavium_keyhandle)))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1AllocContext Failed, ret is %d", ret);
			Csp1FreeContext(gAosCaviumContextType, (*context)->cavium_context);
			aos_free(*context);
			*context = NULL;
			return eAosRc_Error;
		}
		(*context)->ssl_version = VER3_0;
	}

	atomic_set(&((*context)->refcnt), 1);
	AosSslRecord_init(&((*context)->record_plain), eAosSSL_RecordMaxLen);
	
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
int AosSslContext_release(struct aosSslContext *context)
{
	if (context->bufferedData)
	{
		aos_free(context->bufferedData);
		context->bufferedData = NULL;
	}
	
	if (context->PeerCert)
	{
		AosCert_put(context->PeerCert);
		context->PeerCert = NULL;
	}

	if (context->self_cert)
	{
	    AosCert_put(context->self_cert);	
		context->self_cert = NULL;
	}

	if (context->plain_buffer)
		aos_free(context->plain_buffer);
	
	if (context->EncryptClientFinishedMessage)
	{
		aos_free(context->EncryptClientFinishedMessage);
	}
	if (context->EncryptServerFinishedMessage)
	{
		aos_free(context->EncryptServerFinishedMessage);
	}	
	if (context->verify_data)
	{
		aos_free(context->verify_data);
	}	
	if (context->Handshake)
	{
		aos_free(context->Handshake);
	}
	
	AosSslRecord_release(&context->record);
	AosSslRecord_release(&context->record_plain);

	if (context->PrivateKey)
	{
		aos_free(context->PrivateKey);
	}
	if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro ||
	    gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumGen)
	{
		Csp1FreeKeyMem(context->cavium_keyhandle);
		Csp1FreeContext(gAosCaviumContextType, context->cavium_context);
	}

	aos_free(context);

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
	struct tcp_vs_conn *conn, 
	u16 flags,
	int sock)
{
	//
	// create the context
	//
	struct aosSslContext *context;
	AosRsaPrivateKey_t *privatekey;
	struct _X509_CERT_INFO *cert;
	int  ret;

	//aos_trace("AosSslStm_startServer entry");
	if (flags & eAosSSLFlag_Front)
	{
		AosSslContext_constructor(&conn->front_context);
		context = conn->front_context;
		cert = conn->front_cert;
		privatekey = conn->front_prikey;
	}	
	else if (flags & eAosSSLFlag_Backend)
	{
		AosSslContext_constructor(&conn->backend_context);
		context = conn->backend_context;
		cert = conn->backend_cert;
		privatekey = conn->backend_prikey;
	}
	else
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Invalid SSL side: %d", flags);
	}

	aos_assert1(context);

	// cert 
	aos_assert1(cert);
	context->self_cert = cert;
	if (context->self_cert) AosCert_hold(context->self_cert);

	// cert chain
	if (flags & eAosSSLFlag_Front)
	{
		aos_assert1(conn->front_authorities);
		context->authorities = conn->front_authorities;
	}

	// private key
	if (gAosSslAcceleratorType == eAosSslAcceleratorType_Soft)
	{
		context->PrivateKey = (AosRsaPrivateKey_t*)aos_malloc(sizeof(AosRsaPrivateKey_t));
		aos_assert1(context->PrivateKey);
		aos_assert1(privatekey);
		memcpy(context->PrivateKey, privatekey, sizeof(AosRsaPrivateKey_t));
	}
	else if (gAosSslAcceleratorType == eAosSslAcceleratorType_CaviumPro)
	{
		uint8 *keycrt;
		int pos, prime_len;
		keycrt = (uint8*)aos_malloc(MAX_CAVIUM_CRT_LEN);
		aos_assert1(keycrt);
		aos_assert1(privatekey);
		prime_len = privatekey->bits/16;
		aos_assert1(prime_len*5 <= MAX_CAVIUM_CRT_LEN);
		// copy prime2
		memcpy(keycrt, (char*)privatekey->prime[1], prime_len);
		pos = prime_len;
		// copy exponent2
		memcpy(keycrt+pos, (char*)privatekey->primeExponent[1], prime_len);
		pos += prime_len;
		// copy prime1
		memcpy(keycrt+pos, (char*)privatekey->prime[0], prime_len);
		pos += prime_len;
		// copy exponent1
		memcpy(keycrt+pos, (char*)privatekey->primeExponent[0], prime_len);
		pos += prime_len;
		// copy coefficeint
		memcpy(keycrt+pos, (char*)privatekey->coefficient, prime_len);
		pos += prime_len;		
		if (ret = Csp1StoreKey(&context->cavium_keyhandle, pos, keycrt, CRT_MOD_EX))
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "Csp1StoreKey failed: %d", ret);
			aos_free(keycrt);
			return eAosRc_Error;
		}
		aos_free(keycrt);
	}

	// others
	context->isClient = 0;
	context->sock = sock;
	context->flags = flags;
	context->state = eAosSSLState_ServerStart;
	context->stateFunc = AosSslServer_ClientHello;
	context->conn = conn;	
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
			 struct tcp_vs_conn *conn, 
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
	struct aosSslContext *context;
	char buffer[eAosSSL_ClientHelloMaxLen];
	u16 msglen = 0;

	aos_assert1(sessIdLen < 33);
	if (flags & eAosSSLFlag_Front)
    {
        AosSslContext_constructor(&conn->front_context);
        context = conn->front_context;
    }
    else if (flags & eAosSSLFlag_Backend)
    {
        AosSslContext_constructor(&conn->backend_context);
        context = conn->backend_context;
    }
    else
    {
        return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Invalid SSL side: %d", flags);
    }

	context->isClient = 1;
	context->sock = sock;
	context->conn = conn;
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

	// Record Layer
	index = 0;
	buffer[index++] = eAosContentType_Handshake;
	buffer[index++] = eAosSSL_VersionMajor;
	buffer[index++] = eAosSSL_VersionMinor;

	// Handshake Layer
	index = 5;
	buffer[index++] = eAosSSLMsg_ClientHello;
	buffer[index++] = msglen >> 16;
	buffer[index++] = msglen >> 8;
	buffer[index++] = msglen;
	
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

	memcpy(context->ClientRandom, &buffer[index-32], 32);

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

		memcpy(context->SessionId, sessionId, sessIdLen);
		memcpy(context->MasterSecret, master_secret, ms_len);
		if (gAosSslAcceleratorType == eAosSslAcceleratorType_517PM)
		{
			if (client_random)
				memcpy(context->ClientRandom, client_random, RANDOM_LENGTH);
			if (server_random)
				memcpy(context->ServerRandom, server_random, RANDOM_LENGTH);
		}
		context->isResumed = 1;
	}
	else
	{
		context->isResumed = 0;
	}

	// CipherSuites
	cipherSuiteLen = (AosNumClientCiphersSupported << 1);
	buffer[index++] = (cipherSuiteLen >> 8);
	buffer[index++] = cipherSuiteLen;
	for (i=0; i<AosNumClientCiphersSupported; i++)
	{
		buffer[index++] = (AosClientCiphersSupported[i] >> 8);
		buffer[index++] = AosClientCiphersSupported[i];
//		context->CipherSuitesSupported[i] = AosClientCiphersSupported[i];
	}

//	context->NumCipherSuitesSupported = AosNumClientCiphersSupported;

	// Compression
	buffer[index++] = 1;
	buffer[index++] = 0;

	// 
	// Set Record Length
	//
	msglen += 4;
	buffer[3] = (msglen >> 8);
	buffer[4] = msglen;
	
	msglen += 5;

	// 
	// Add the message to 'context->messages'
	//
	memcpy(context->Handshake, &buffer[5], msglen-5);
	context->HandshakeLength = msglen - 5;

	AosTcp_write(sock, buffer, msglen);

	// 
	// Message constructed. Prepare the state machine
	//
	context->state = eAosSSLState_ClientHelloSent;
	context->stateFunc = AosSslClient_serverHello;

	return 0;
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
//	'conn': IN
//		The connection for the SSL.
//
//	'rslt_data': OUT
//		The messages, if any, to be sent back to the remote site.
//
//	'rslt_data_len": OUT
//		The length of 'rslt_data_len'.
//
//	'callback': IN
//		If the call is asynchronous, this is the function to be called back.
//
// Function Return Values:
//	eAosRc_DoNothing:
//		The caller should do nothing.
//
//	eAosRc_ForwardToSender:
//		There are some messages to be sent to the sender. 
//	
int AosSslStm_procState(
			 char *data, 
			 unsigned int data_len,
			 u32 flags,
			 struct tcp_vs_conn *conn, 
			 struct aosSslContext *context,
			 SSL_PROC_CALLBACK callback)
{
	int ret = 0;
	int guard = 0;
	int guard1;
	char *buffer = 0;
	int remainlen;
	u16 errcode;

	aos_assert1(context);
	// when ssl established, send plain from ssl, then do processing below 
	if (flags &	eAosSSLFlag_Plain)
	{

		//struct AosSslRecord record_plain;
		if (context->stateFunc != AosSsl_Established)
		{
			aos_eng_log(eAosMD_SSL,
			"app want to send plain data to ssl tunnel, but ssl is not established yet");
			// buffered this data
			if (context->plain_buffer)
				aos_free(context->plain_buffer);
			context->plain_buffer = (uint8*)aos_malloc(data_len);
			aos_assert1(context->plain_buffer);
			memcpy(context->plain_buffer, data, data_len);
			context->plain_len = data_len;
			return eAosRc_Success;
		}

		//AosSslRecord_init(&record_plain, data_len+100);

		ret = AosSslRecord_getOneRecord(data, data_len, flags, &context->record_plain, 
				(unsigned int*)(&remainlen), 0);
		if (ret != eAosRc_Success)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl process plain data error");
			//AosSslRecord_release(&record_plain);
			return ret;
		}

		ret = context->stateFunc(&context->record_plain, conn, context, callback, &errcode);
		if (ret != eAosRc_Success)
        {
            aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "ssl process plain data error");
            //AosSslRecord_release(&record_plain);
            return ret;
        }
		//AosSslRecord_release(&record_plain);
		return ret;
	}

	while (guard++ < 500 && context->stateFunc)
	{
		if(context->state==eAosSSLState_Error)
			return -1;

		ret = AosSslRecord_getNextRecord(data, data_len, flags, context);
		aos_assert1(ret == eAosRc_NoMsg || ret <= 0);

		// 
		// IMPORTANT: After calling the above function, the input data was
		// stored in 'conn'. Do not use 'data' and 'data_len' anymore.
		//
		//aos_trace("state");
		data = 0;
		data_len = 0;

		if (ret == eAosRc_NoMsg) 
		{
			if (buffer) aos_free(buffer);
			return eAosRc_DoNothing;
		}

		if (ret < 0)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"AosSslRecord_getNextRecord return error %d", ret);
			goto error;
		}

		// 
		// Retrieved a record. Process it.
		//
		guard1 = 0;
		while (guard1++ < 10)
		{
			ret = context->stateFunc(&context->record, conn, context, callback, &errcode);
			switch (ret)
			{
			case eAosRc_Pending:
				 return eAosRc_Success;

			case eAosRc_Success:
			case eAosRc_DoNothing:
				 break;

			default:
				 ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
					"SSL Process failed: %d", ret);
				 goto error;
			}

			ret = AosSslRecord_nextMsg(&context->record, 
				(context->flags & eAosSSLFlag_ExpectFinished)?1:0);

			if (ret)
			{
				break;
			}
		}

		if (guard1 >= 10)
		{
			// 
			// Guard1 failed.
			//
			ret = aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Guard failed: %d, %d", (context->record).msg_start, 
				(context->record).record_body_len);
			errcode = 0x0250;	// Internal error
			goto error;
		}
	}

	// 
	// Should never come to this point.
	//
	if (!context->stateFunc)
	{
		ret = aos_progerr(eAosMD_SSL, eAosAlarm_ProgErr, "State Function is null");
	}
	else
	{
		ret = aos_progerr(eAosMD_SSL, eAosAlarm_ProgErr, 
			"Guard overflowed: %d", guard);
	}

error:
	if (ret == -eAosRc_SSLAbort)
	{
		//
		// Something serious wrong. The caller should disconnect the connection
		// immediately.
		//
		if (buffer) aos_free(buffer);
		return ret;	
	}

	// 
	// Errors were encountered. We will construct an alert message.
	//
	if (!buffer)
	{
		buffer = (char *)aos_malloc(100);
		aos_assert1(buffer);
	}

	ret = AosSsl_createAndSendAlertRecord(context, errcode);
	aos_assert1(ret == 0);
	return -eAosAlarm_SSLProcErr; 
}


// Dscription:
// SSL API Function. This function is called whenever an SSL (TCP) connection is
// established or data is received. 
//
// Prms:
//  'data': IN
//   The input data. It can be NULL. 
//
//  'data_len': IN
//    The length of 'data'. If 'data' is null, 'data_len' is 0.
//
//  'flags': IN
//   The flags. It is OR of the following flags:
//   	eAosSSLFlag_VirtualService: It is the virtual service side
//   	eAosSSLFlag_Backend:        It is the backend side 
//   	eAosSSLFlag_Client:			It is an SSL client
//   	eAosSSLFlag_Server:			It is an SSL server
//
//  'conn': IN
//    The connection for the SSL.
// 
//  'rslt_data': OUT
//    The messages, if any, to be sent back to the remote site.
// 
//  'rslt_data_len": OUT
//    The length of 'rslt_data_len'.
// 
//  'callback': IN
//    If the call is asynchronous, this is the function to be called back.
// 
int AosSslStm_procRequest(
   	 		char *data, 
   	 		unsigned int data_len,
   	 		u32 flags,
   	 		struct tcp_vs_conn *conn, 
   	 		SSL_PROC_CALLBACK callback)
{
	int ret;
	struct aosSslContext *context = 0;

	// Determine the side and context
	if (flags & eAosSSLFlag_Front)
	{
		context = conn->front_context;
	}
	else if (flags & eAosSSLFlag_Backend)
	{
		context = conn->backend_context;
	}
	else
	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
			"Side is not set: %d", flags);
	}
	
	//
	// If context is null, try to create it
	//
	if (!context)
	{
		// Context has not been set yet. Create it.
		if ((flags & eAosSSLFlag_Client) || (flags & eAosSSLFlag_Server))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, 
				"Context is null");
		}

		if (!(flags & eAosSSLFlag_Server))
		{
			return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr,
				"None of SSL Server and Client is set: %d", flags);
		}

		ret = AosSslStm_startServer(conn, flags, 0);
		aos_assert1(ret == 0);

		context = (flags & eAosSSLFlag_Front) ? 
			conn->front_context : conn->backend_context;
	
		context->flags = (flags & eAosSSLFlag_Front)?conn->front_ssl_flags:conn->backend_ssl_flags;

		aos_assert1(context);
	}

	return AosSslStm_procState(data, data_len, flags, conn, 
		context, callback);
}

