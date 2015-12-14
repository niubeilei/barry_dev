////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: md5.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "crypt_hash.h"

#define F(x,y,z)	(z ^ (x & (y ^ z)))
#define G(x,y,z)	(y ^ (z & (y ^ x)))
#define H(x,y,z)	(x^y^z)
#define I(x,y,z)	(y^(x|(~z)))

#define FF(a,b,c,d,M,s,t) \
	a = (a + F(b,c,d) + M + t); a = ROL(a, s) + b;

#define GG(a,b,c,d,M,s,t) \
	a = (a + G(b,c,d) + M + t); a = ROL(a, s) + b;

#define HH(a,b,c,d,M,s,t) \
	a = (a + H(b,c,d) + M + t); a = ROL(a, s) + b;

#define II(a,b,c,d,M,s,t) \
	a = (a + I(b,c,d) + M + t); a = ROL(a, s) + b;

static const BYTE Worder[64] = {
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
	1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12,
	5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2,
	0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9
};

static const BYTE Rorder[64] = {
	7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
	5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
	4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
	6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21
};

static const UINT32 Korder[] = {
	0xd76aa478UL, 0xe8c7b756UL, 0x242070dbUL, 0xc1bdceeeUL,
	0xf57c0fafUL, 0x4787c62aUL, 0xa8304613UL, 0xfd469501UL,
	0x698098d8UL, 0x8b44f7afUL, 0xffff5bb1UL, 0x895cd7beUL,
	0x6b901122UL, 0xfd987193UL, 0xa679438eUL, 0x49b40821UL,
	0xf61e2562UL, 0xc040b340UL, 0x265e5a51UL, 0xe9b6c7aaUL,
	0xd62f105dUL, 0x02441453UL, 0xd8a1e681UL, 0xe7d3fbc8UL,
	0x21e1cde6UL, 0xc33707d6UL, 0xf4d50d87UL, 0x455a14edUL,
	0xa9e3e905UL, 0xfcefa3f8UL, 0x676f02d9UL, 0x8d2a4c8aUL,
	0xfffa3942UL, 0x8771f681UL, 0x6d9d6122UL, 0xfde5380cUL,
	0xa4beea44UL, 0x4bdecfa9UL, 0xf6bb4b60UL, 0xbebfbc70UL,
	0x289b7ec6UL, 0xeaa127faUL, 0xd4ef3085UL, 0x04881d05UL,
	0xd9d4d039UL, 0xe6db99e5UL, 0x1fa27cf8UL, 0xc4ac5665UL,
	0xf4292244UL, 0x432aff97UL, 0xab9423a7UL, 0xfc93a039UL,
	0x655b59c3UL, 0x8f0ccc92UL, 0xffeff47dUL, 0x85845dd1UL,
	0x6fa87e4fUL, 0xfe2ce6e0UL, 0xa3014314UL, 0x4e0811a1UL,
	0xf7537e82UL, 0xbd3af235UL, 0x2ad7d2bbUL, 0xeb86d391UL,
	0xe1f27f3aUL, 0xf5710fb0UL, 0xada0e5c4UL, 0x98e4c919UL
 };

static void
md5_compress(HASH_STATE *md) {
	UINT32	i, W[16], a, b, c, d;
	UINT32	t;


	/* copy the state into 512-bits into W[0..15] */
	for (i = 0; i < 16; i++)
		LOAD32L(W[i], md->buf + (4*i));

	/* copy state */
	a = md->state[0];
	b = md->state[1];
	c = md->state[2];
	d = md->state[3];

	for (i = 0; i < 16; ++i) {
		FF(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
		t = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 32; ++i) {
		GG(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
		t = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 48; ++i) {
		HH(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
		t = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 64; ++i) {
		II(a,b,c,d,W[Worder[i]],Rorder[i],Korder[i]);
		t = d; d = c; c = b; b = a; a = t;
	}

	md->state[0] = md->state[0] + a;
	md->state[1] = md->state[1] + b;
	md->state[2] = md->state[2] + c;
	md->state[3] = md->state[3] + d;
}


void
md5_init(HASH_STATE * md) {
	md->state[0] = 0x67452301UL;
	md->state[1] = 0xefcdab89UL;
	md->state[2] = 0x98badcfeUL;
	md->state[3] = 0x10325476UL;
	md->curlen = 0;
	md->length_hi = 0;
	md->length_lo = 0;
}

void
md5_update(HASH_STATE * md, const BYTE *buf, UINT32 len) {
	UINT32 n;

	while (len > 0) {
		n = MIN(len, (64 - md->curlen));
		memcpy(md->buf + md->curlen, buf, (size_t)n);
		md->curlen	+= n;
		buf		+= n;
		len		-= n;

/*
		is 64 bytes full?
 */
		if (md->curlen == 64) {
			md5_compress(md);
			n = (md->length_lo + 512) & 0xFFFFFFFFL;
			if (n < md->length_lo) {
				md->length_hi++;
			}
			md->length_lo = n;
			md->curlen = 0;
		}
	}
}

int md5_final(HASH_STATE * md, BYTE *hash) {
	int i;
	UINT32	n;

	if (hash == NULL)
		return -1;

	/* increase the length of the message  */
	n = (md->length_lo + (md->curlen << 3)) & 0xFFFFFFFFL;
	if (n < md->length_lo) {
		md->length_hi++;
	}
	md->length_hi += (md->curlen >> 29);
	md->length_lo = n;

	/* append the '1' bit  */
	md->buf[md->curlen++] = (BYTE)0x80;

	/*
	if the length is currently above 56 bytes we append zeros then compress.
	Then we can fall back to padding zeros and length encoding like normal.
 	*/
	if (md->curlen > 56) {
		while (md->curlen < 64)
			md->buf[md->curlen++] = (BYTE)0;
		md5_compress(md);
		md->curlen = 0;
	}

	/* pad upto 56 bytes of zeroes  */
	while (md->curlen < 56)
		md->buf[md->curlen++] = (BYTE)0;
	/* store length  */
	STORE32L(md->length_lo, md->buf+56);
	STORE32L(md->length_hi, md->buf+60);

	md5_compress(md);

	/* copy output */
	for (i = 0; i < 4; i++) {
		STORE32L(md->state[i], hash+(4*i));
	}

	return 16;
}

#ifdef MD5_DEBUG

int md5_test() {
	static const struct {
		char *msg;
		BYTE hash[MD5_HASH_SIZE];
	} tests[] = {
		{ "",
		{ 0xd4, 0x1d, 0x8c, 0xd9, 0x8f, 0x00, 0xb2, 0x04,
			0xe9, 0x80, 0x09, 0x98, 0xec, 0xf8, 0x42, 0x7e } },
		{ "a",
		{0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
			0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61 } },
		{ "abc",
		{ 0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
			0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72 } },
		{ "message digest",
		{ 0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
			0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0 } },
		{ "abcdefghijklmnopqrstuvwxyz",
		{ 0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
			0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b } },
		{ "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		{ 0xd1, 0x74, 0xab, 0x98, 0xd2, 0x77, 0xd9, 0xf5,
			0xa5, 0x61, 0x1c, 0x2c, 0x9f, 0x41, 0x9d, 0x9f } },
		{ "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		{ 0x57, 0xed, 0xf4, 0xa2, 0x2b, 0xe3, 0xc9, 0x55,
			0xac, 0x49, 0xda, 0x2e, 0x21, 0x07, 0xb6, 0x7a } },
		{ NULL, { 0 } }
	};

	int i;
	BYTE tmp[16];
	HASH_STATE md;

	for (i = 0; tests[i].msg != NULL; i++) {
		md5_init(&md);
		md5_update(&md, (BYTE *)tests[i].msg, (UINT32)strlen(tests[i].msg));
		md5_final(&md, tmp);

		if (memcmp(tmp, tests[i].hash, MD5_HASH_SIZE))
			return -1;
	}
	return 0;
}

int main() {
	int ret;

	ret = md5_test();

	if (ret == 0)
		printf("MD5 test ok\n");
	else
		printf("MD5 test failed\n");

	return ret;
}

#endif
