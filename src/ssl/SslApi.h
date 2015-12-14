////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslApi.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_ssl_SslApi_h
#define Aos_ssl_SslApi_h

struct AosSslWrapperObject;

typedef int (*SSL_PROC_CALLBACK)(int rc, struct AosSslWrapperObject *wo, char *rslt_data, int rslt_data_len);

#ifndef __KERNEL__
typedef int (*SSL_REQUEST_CALLBACK)(void *certinfo, void *context, int rc);
int inline aos_verify_cert(void *certinfo, void *context, SSL_REQUEST_CALLBACK callback)
{
	return -1;
};
#endif 
 
#endif
