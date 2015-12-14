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
#include "ssl/SslApi.h"

struct AosSslRecord;
struct AosSslContext;
extern int AosSslClient_serverHello(
	struct AosSslContext *context,
	struct AosSslRecord *record, 
	SSL_PROC_CALLBACK callback, 
	u16 *errcode);
#endif

