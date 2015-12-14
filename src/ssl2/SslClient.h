////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslClient.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_SslClient_h
#define Aos_ssl_SslClient_h

#include "aosUtil/Types.h"
#include "ssl2/SslApi.h"

struct tcp_vs_conn;


struct AosSslRecord;
struct tcp_vs_conn;
struct aosSslContext;
extern int AosSslClient_serverHello(
			struct AosSslRecord *record, 
			struct tcp_vs_conn *conn,
			struct aosSslContext *context,
			SSL_PROC_CALLBACK callback, 
            u16 *errcode);
#endif

