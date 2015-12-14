////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: wrapper_ssl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __AOS_WRAPPER_SSL_H__
#define __AOS_WRAPPER_SSL_H__

#include "proxy.h"

typedef enum
{
	eAosSslWrapper_tcpRcvFin = 0x1,
	eAosSslWrapper_tcpSndFin = 0x2,
	eAosSslWrapper_tcpHasData = 0x4,
}AosSslWrapperTcpState;

#define WRAPPER_BUFFER_LEN 1024
typedef
struct AosSslWrapperObject
{
	AosSockWrapperObject_t	base;
	
	struct list_head		snd_buf_lst;
	
	//char 	buffer[16384];
	//char 	*buffer_start;
	//int		buffer_len;

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
int AosSslWrapper_sendRecordData( AosSockWrapperObject_t * wo, char * buffer, int len);
int AosSslWrapper_finReceived( AosSockWrapperObject_t *wo );
int AosSslWrapper_sendFin( AosSockWrapperObject_t * wo);
int AosSslWrapper_isWritable( AosSockWrapperObject_t * wo );
#endif // #ifndef __AOS_WRAPPER_SSL_H__

