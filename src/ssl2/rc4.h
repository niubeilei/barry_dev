////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rc4.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __RC4_H__
#define __RC4_H__

#define RC4_MIN_KEY_SIZE	1
#define RC4_MAX_KEY_SIZE	256
#define RC4_BLOCK_SIZE		1

struct rc4_ctx {
	u8 S[256];
	u8 x, y;
};
extern void rc4_crypt(void *ctx_arg, u8 *out, const u8 *in, int len);
extern int aos_rc4_crypt(void *ctx, u8 *in, int in_len, u8 *out, u8 *key, int key_len);
extern int aos_rc4_setkey(void *ctx_arg, const u8 *in_key, int key_len);

#endif

