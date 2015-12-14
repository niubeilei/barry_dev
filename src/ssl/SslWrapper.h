////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2007
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslWrapper.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __AOS_SSL_WRAPPER_H__
#define __AOS_SSL_WRAPPER_H__
#include "KernelSimu/list.h"
#include "KernelSimu/types.h"

typedef enum
{
	eAosSslWrapper_tcpRcvFin = 0x1,
	eAosSslWrapper_tcpSndFin = 0x2,
	eAosSslWrapper_tcpHasData = 0x4,
}AosSslWrapperTcpState;

typedef
struct AosSslWrapperObject
{
	// AosSockWrapperObject_t	base;
	void*                   sk;
	struct list_head        snd_buf_lst;

	struct AosSslContext 	*context;
	u32 					ssl_flags;
	//struct tcp_vs_dest      *dest;   		// destination server
	u32					dest_addr;		// destination ip addr
	struct _X509_CERT_INFO	*cert;			// ssl certificate 
	struct AosRsaPrivateKey	*prikey;		// ssl PrivateKey;
	struct AosCertChain     *authorities;	// authorites trusted by server
	unsigned char			isSecureUser;
	u8						tcp_state;		// bit0 = rcvFin, bit1 = sendFin, bit2 = isWritable
	u32						tt;
}
AosSslWrapperObject_t;

extern int AosSslTcp_sendRecord( AosSslWrapperObject_t * swo, struct AosSslRecord *record);
extern int AosSslWrapper_serverInit(void *swo);
extern int AosSslWrapper_clientInit(void *swo);
extern int AosSslWrapper_rcv(void *swo, struct sk_buff *skb);
extern int AosSslWrapper_release(void *wrapper);
extern int AosSslWrapper_skbCallback(int rc, AosSslWrapperObject_t *swo, char *record, int buffer_len);
extern int AosSslWrapper_sendAppData(AosSslWrapperObject_t *swo, struct sk_buff *skb);

#endif // #ifndef __AOS_SSL_WRAPPER_H__

