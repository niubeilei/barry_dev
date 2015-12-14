////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslWrapper.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "SslWrapper.h"
#include "aosUtil/Alarm.h"
#include "ssl/SslRecord.h"
#include "ssl/Ssl.h"
#include "ssl/SslStatemachine.h"
#include "ssl/ssl_sk_api.h"
#include "ssl/Tester/vpn/app_event.h"

#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"

//int app_func_run(int layer,struct sock *sk,int type,void *input,void **output);
//
// Call this function to send SSL data (messages) to the peer.
//
int AosSslTcp_sendRecord( AosSslWrapperObject_t * swo, struct AosSslRecord *record)
{
	struct sk_buff *skb;
	//int len_sent;

	aos_assert1( swo );

	while((skb = skb_dequeue(&record->rskb_queue)))
		app_func_run(EVENT_LAYER_SSL, (sock*)swo->sk, FUNC_TYPE_SEND, skb,NULL);

	return 0;
}

int AosSslWrapper_serverInit(void *wrapper)
{
	AosSslWrapperObject_t *swo =(AosSslWrapperObject_t *)wrapper;
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

int AosSslWrapper_clientInit(void *wrapper)
{
	AosSslWrapperObject_t *swo =(AosSslWrapperObject_t *)wrapper;
	//struct sockaddr_in addr;
	//int len;

	//aos_min_log( eAosMD_TcpProxy, "clientInit start, wo %p, usec %u", wo, AosGetUsec() );
	aos_trace("AosSslWrapper_clientInit entry");
	if ( swo->context )
	{
		aos_trace("client ssl context already allocated");
		return eAosRc_Success;
	}

	//aos_inet_getname(swo->sk, (struct sockaddr*)&addr, &len, 0 );		
	//swo->dest_addr  = addr.sin_addr.s_addr;	

	if ( AosSsl_Start(swo, eAosSSLFlag_Client) < 0 )
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start client failed");
		return -1;
	}
	//aos_min_log( eAosMD_TcpProxy, "clientInit end, wo %p, usec %u", wo, AosGetUsec() );
	return 0;
}

int AosSslWrapper_rcv(void *wrapper, struct sk_buff *skb)
{
	AosSslWrapperObject_t *swo =(AosSslWrapperObject_t *)wrapper;
	int ret;
	aos_assert1(swo);
	aos_trace("%s", __FUNCTION__);
	ret = AosSslStm_skbInput(swo->context, skb, AosSslWrapper_skbCallback);
	return ret;
}

int AosSslWrapper_release(void *wrapper)
{
	AosSslWrapperObject_t *swo = (AosSslWrapperObject_t *)wrapper;
	aos_assert1(swo);

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

	aos_free(swo);

	return 0;
}

int AosSslWrapper_skbCallback(int rc, AosSslWrapperObject_t *swo, char *record, int buffer_len)
{
	int  ret = eAosRc_Success;

	aos_trace("%s", __FUNCTION__);

	if (!record)
	{
		return aos_alarm( eAosMD_TcpProxy, eAosAlarmProgErr, 
				"invalid buffer 0x%x, buffer_len %d", (__u32)record, buffer_len );
	}
	if ( !swo )
	{
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "ssl wrapper is released, call back failed, buffer %x, buffer_len %d", record, buffer_len );
		return -1;
	}

	/*aos_debug( "%s, buffer_len %d, context %p, swo %p, swo->sk %p, %d.%d", __FUNCTION__,
			buffer_len, swo->context, swo, swo->sk,
			AosSslWrapper_getSockPort( swo->sk ),
			AosSslWrapper_getSockPeerPort( swo->sk ) );
	*/
	switch( rc )
	{
		case eAosContentType_AppData:
			aos_trace("plain data received");
			ret = ssl_app_data_rcv((struct sock*)(swo->sk), &(((AosSslRecord_t*)record)->rskb_queue));
			break;

		case eAosContentType_PlainData:
			break;
		default:
			aos_alarm(eAosMD_TcpProxy, eAosAlarmProgErr, "ssl skb call back, unknown rc = %d", rc);
			return rc;
	}

	return ret;
}

int AosSslWrapper_sendAppData(AosSslWrapperObject_t *swo, struct sk_buff *skb)
{
	aos_assert1(swo);
	return AosSslStm_skbOutput(swo->context, skb, NULL);
}
