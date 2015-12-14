////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sha1.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __AOS_SSL_SHA1_H__
#define __AOS_SSL_SHA1_H__

struct sha1_ctx{
	uint32	length_hi;
	uint32	length_lo;

	uint32 	state[5], curlen;
	uint8	buf[64];
};
extern void  SHA(unsigned char *s,unsigned int nl,unsigned char *md);

extern void AosSsl_sha1Init(void *ctx);

extern void AosSsl_sha1Update(void *ctx, const u8 *data, unsigned int len);

extern void AosSsl_sha1Final(void* ctx, u8 *out);


#endif

