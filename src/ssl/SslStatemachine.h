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
#include "ssl/SslCommon.h"
#include "ssl/SslApi.h"
#include "aosUtil/Tracer.h"

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
    eAosSSLState_handshakeFinished,
		
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
	eAosSSLState_Continuing,
	eAosSSLState_Error,
	eAosSSLState_ClientChangeCipherRcvd,
	eAosSSLState_ClientFinishedRcvd,
	eAosSSLState_ServerChangeCipher,
	eAosSSLState_ServerFinished,

} AosSSLState;

struct tcp_vs_conn;
struct AosSslRecord;
//struct AosSslContext;

extern int AosSslContext_release(struct AosSslContext *context);

static inline int AosSslContext_hold(struct AosSslContext *context)
{
	atomic_inc(&context->refcnt);
//aos_trace("context %x, context->refcnt %d", context, atomic_read(&context->refcnt) );
	return 0;
}

static inline int AosSslContext_put(struct AosSslContext *context)
{
    if (atomic_dec_and_test(&context->refcnt))
    {
//aos_trace("context %x, context->refcnt %d", context, atomic_read(&context->refcnt) );
        AosSslContext_release(context);
		return 0;
    }
//aos_trace("context %x, context->refcnt %d", context, atomic_read(&context->refcnt) );
    return 1;
}

extern int AosSslStm_skbInput(
	 struct AosSslContext *context,
	 struct sk_buff *skb, 
	 SSL_PROC_CALLBACK callback);

extern int AosSslStm_skbOutput(
	struct AosSslContext *context,	
	struct sk_buff *skb,
	SSL_PROC_CALLBACK callback);

extern int AosSslStm_startServer(
	 struct AosSslWrapperObject *wo, 
	 u16 flags,
	 int sock);

extern int AosSslStm_startClient(
	 struct AosSslWrapperObject *wo,
	 u16 flags,
	 const u16 serverId,
	 char *sessionId, 
	 u32 sessIdLen,
	 char *master_secret,
	 u8 ms_len,
	 char *client_random,
	 char *server_random,
	 int sock);

extern void AosSslStm_cardCallback(int result, void* ssldata);

extern int AosSslStm_ocspCallback(void *certinfo, void *contextin, int rc);

extern int AosSslStm_protocol(
	struct AosSslContext *context,
	struct AosSslRecord *record,
	SSL_PROC_CALLBACK callback,
	uint16 *error_code);

extern int AosSslStm_Established(
	struct AosSslContext *context,
	struct AosSslRecord *record,
	SSL_PROC_CALLBACK callback,
	uint16 *error_code);

#endif

