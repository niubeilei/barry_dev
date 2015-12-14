////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rc4.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelSimu/types.h"
#include "ssl2/rc4.h"

int aos_rc4_setkey(void *ctx_arg, const u8 *in_key, int key_len)
{
	struct rc4_ctx *ctx = (struct rc4_ctx*)ctx_arg;
	int i, j = 0, k = 0;

	ctx->x = 1;
	ctx->y = 0;

	for(i = 0; i < 256; i++)
		ctx->S[i] = i;

	for(i = 0; i < 256; i++)
	{
		u8 a = ctx->S[i];
		j = (j + in_key[k] + a) & 0xff;
		ctx->S[i] = ctx->S[j];
		ctx->S[j] = a;
		if(++k >= key_len)
			k = 0;
	}

	return 0;
}

void rc4_crypt(void *ctx_arg, u8 *out, const u8 *in, int len)
{
	struct rc4_ctx *ctx = (struct rc4_ctx*)ctx_arg;

	u8 *const S = ctx->S;
	u8 x = ctx->x;
	u8 y = ctx->y;
	u8 a, b;
	int i;

	for( i=0; i<len; i++)
	{
		a = S[x];
		y = (y + a) & 0xff;
		b = S[y];
		S[x] = b;
		S[y] = a;
		x = (x + 1) & 0xff;
		*(out+i) = *(in+i) ^ S[(a + b) & 0xff];

		ctx->x = x;
		ctx->y = y;
	}
}

int aos_rc4_crypt(void *cipher_ctx, u8 *in, int in_len, u8 *out, u8 *key, int key_len)
{
	struct rc4_ctx *ctx = (struct rc4_ctx*)cipher_ctx;
	rc4_crypt((void*)ctx, out, in, in_len);
	return 0;
}

