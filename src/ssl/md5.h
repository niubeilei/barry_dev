////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: md5.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SSL_MD5_HEAD_H
#define AOS_SSL_MD5_HEAD_H

#if KERNEL
#else
typedef unsigned int __u32;
#endif

struct MD5Context {
	__u32 buf[4];
	__u32 bits[2];
	unsigned char in[64];
};

struct HMACMD5Context {
	struct MD5Context ctx;
	unsigned char k_ipad[65];
	unsigned char k_opad[65];
};

extern void AosSsl_md5Init(void *context);
extern void AosSsl_md5Update(void *context, const unsigned char *buf,unsigned int len);
extern void AosSsl_md5Final(void *context, unsigned char *digest);

#endif

