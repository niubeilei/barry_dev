////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslStatemachine.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_SslStatemachine_h
#define Aos_ssl_SslStatemachine_h

#include "aosUtil/Types.h"
#include "ssl2/aosSslProc.h"
#include "ssl2/SslApi.h"

typedef enum
{
    eAosSSLState_Invalid,
    eAosSSLState_Idle,
    
    //
    // Client States
    // 
    eAosSSLState_ClientHelloSent,
    eAosSSLState_ServerHelloRcvd,
    eAosSSLState_CertificateRcvd,
    eAosSSLState_ServerHelloDoneRcvd,
    eAosSSLState_ClientKeyExchSent,
    eAosSSLState_ClientChangeCipherSent,
    eAosSSLState_ClientFinishedSent,
    eAosSSLState_ServerChangeCipherSpecRcvd,
    eAosSSLState_Established,
    eAosSSLState_HandshakeFinished,
		
	//
	// Server States
	//
	eAosSSLState_ServerStart,
	eAosSSLState_ClientHelloRcvd,
	eAosSSLState_ServerHello,
	eAosSSLState_ServerCert,
	eAosSSLState_ServerCertRequest,
	eAosSSLState_ServerHelloDone,
	eAosSSLState_ClientCertRcvd,
	eAosSSLState_ClientCertOCSP,
	eAosSSLState_ClientKeyExchangeRcvd,
	eAosSSLState_ClientCertVerifyRcvd,
	eAosSSLState_Pending,
	eAosSSLState_Error,
	eAosSSLState_ClientChangeCipherRcvd,
	eAosSSLState_ClientFinishedRcvd,
	eAosSSLState_ServerChangeCipher,
	eAosSSLState_ServerFinished,

} AosSSLState;

struct tcp_vs_conn;
struct AosSslRecord;
struct aosSslContext;

extern int AosSslContext_release(struct aosSslContext *context);

static inline int AosSslContext_hold(struct aosSslContext *context)
{
	atomic_inc(&context->refcnt);
	return 0;
}

static inline int AosSslContext_put(struct aosSslContext *context)
{
    if (atomic_dec_and_test(&context->refcnt))
    {
        AosSslContext_release(context);
    }
    return 0;
}

/* typedef int (*AosSSLStateFunc)(
            AosSslRecord_t *record,
            struct tcp_vs_conn *conn,
            struct aosSslContext *context,
            SSL_PROC_CALLBACK callback,
            u16 *errcode);
*/

extern int AosSslStm_procState(
			 char *data, 
			 unsigned int data_len,
			 u32 flags,
			 struct tcp_vs_conn *conn, 
			 struct aosSslContext *context,
			 SSL_PROC_CALLBACK callback);
extern int AosSslStm_startServer(
			 struct tcp_vs_conn *conn, 
//			 u32 *cipherSuites,
//			 u8 numCipherSuites,
			 u16 flags,
			 int sock);
extern int AosSslStm_startClient(
			 struct tcp_vs_conn *conn, 
			 u16 flags,
			 const u16 serverId,
			 char *sessionId, 
			 u32 sessIdLen,
			 char *master_secret,
			 u8 ms_len,
             		 char *client_random,
			 char *server_random,
			 int sock);
extern int AosSslStm_procRequest(
   	 		char *data, 
   	 		unsigned int data_len,
   	 		u32 flags,
   	 		struct tcp_vs_conn *conn, 
   	 		SSL_PROC_CALLBACK callback);

#endif

