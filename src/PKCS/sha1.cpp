////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sha1.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
/*
 *	sha1.c
 ******************************************************************************/
#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#endif

#include "CertVerify/crypt_hash.h"

#define F0(x,y,z)	(z ^ (x & (y ^ z)))
#define F1(x,y,z)	(x ^ y ^ z)
#define F2(x,y,z)	((x & y) | (z & (x | y)))
#define F3(x,y,z)	(x ^ y ^ z)

static void
sha1_compress(HASH_STATE *md) {
	UINT32	a,b,c,d,e,W[80],i;
	UINT32	t;

	/* copy the state into 512-bits into W[0..15] */
	for (i = 0; i < 16; i++)
		LOAD32H(W[i], md->buf + (4*i));

	/* copy state */
	a = md->state[0];
	b = md->state[1];
	c = md->state[2];
	d = md->state[3];
	e = md->state[4];

	/* expand it */
	for (i = 16; i < 80; i++)
		W[i] = ROL(W[i-3] ^ W[i-8] ^ W[i-14] ^ W[i-16], 1);

	/* compress  */
	/* round one */
#define FFF0(a,b,c,d,e,i) e = (ROL(a, 5) + F0(b,c,d) + e + W[i] + 0x5a827999UL); b = ROL(b, 30);
#define FFF1(a,b,c,d,e,i) e = (ROL(a, 5) + F1(b,c,d) + e + W[i] + 0x6ed9eba1UL); b = ROL(b, 30);
#define FFF2(a,b,c,d,e,i) e = (ROL(a, 5) + F2(b,c,d) + e + W[i] + 0x8f1bbcdcUL); b = ROL(b, 30);
#define FFF3(a,b,c,d,e,i) e = (ROL(a, 5) + F3(b,c,d) + e + W[i] + 0xca62c1d6UL); b = ROL(b, 30);

	for (i = 0; i < 20; ) {
		FFF0(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 40; ) {
		FFF1(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 60; ) {
		FFF2(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}

	for (; i < 80; ) {
		FFF3(a,b,c,d,e,i++); t = e; e = d; d = c; c = b; b = a; a = t;
	}


#undef FFF0
#undef FFF1
#undef FFF2
#undef FFF3

	/*  store  */
	md->state[0] = md->state[0] + a;
	md->state[1] = md->state[1] + b;
	md->state[2] = md->state[2] + c;
	md->state[3] = md->state[3] + d;
	md->state[4] = md->state[4] + e;
}


void
sha1_init(HASH_STATE * md) {
	md->state[0] = 0x67452301UL;
	md->state[1] = 0xefcdab89UL;
	md->state[2] = 0x98badcfeUL;
	md->state[3] = 0x10325476UL;
	md->state[4] = 0xc3d2e1f0UL;
	md->curlen = 0;

	md->length_hi = 0;
	md->length_lo = 0;
}

void
sha1_update(HASH_STATE * md, const BYTE *buf, UINT32 len) {
	UINT32 n;

	while (len > 0) {
		n = MIN(len, (64 - md->curlen));
		memcpy(md->buf + md->curlen, buf, (size_t)n);
		md->curlen		+= n;
		buf					+= n;
		len					-= n;

		/* is 64 bytes full? */
		if (md->curlen == 64) {
			sha1_compress(md);
			n = (md->length_lo + 512) & 0xFFFFFFFFL;
			if (n < md->length_lo) {
				md->length_hi++;
			}
			md->length_lo = n;
			md->curlen = 0;
		}
	}
}

int
sha1_final(HASH_STATE * md, BYTE *hash) {
	int i;
	UINT32	n;

	if (md->curlen >= sizeof(md->buf) || hash == NULL)
		return -1;

	/* increase the length of the message */
	n = (md->length_lo + (md->curlen << 3)) & 0xFFFFFFFFL;
	if (n < md->length_lo) {
		md->length_hi++;
	}
	md->length_hi += (md->curlen >> 29);
	md->length_lo = n;

	/* append the '1' bit */
	md->buf[md->curlen++] = (BYTE)0x80;

	/* if the length is currently above 56 bytes we append zeros then compress.
	  Then we can fall back to padding zeros and length encoding like normal.
 	*/
	if (md->curlen > 56) {
		while (md->curlen < 64) {
			md->buf[md->curlen++] = (BYTE)0;
		}
		sha1_compress(md);
		md->curlen = 0;
	}

	/* pad upto 56 bytes of zeroes  */
	while (md->curlen < 56) {
		md->buf[md->curlen++] = (BYTE)0;
	}

	/* store length */
	STORE32H(md->length_hi, md->buf+56);
	STORE32H(md->length_lo, md->buf+60);
	sha1_compress(md);

/*
	copy output
 */
	for (i = 0; i < 5; i++) {
		STORE32H(md->state[i], hash+(4*i));
	}

	return 20;
}

#ifdef SHA1_DEBUG
int  sha1_test() {
	static const struct {
		char *msg;
		BYTE hash[SHA1_HASH_SIZE];
	} tests[] = {
		{ "abc",
		{ 0xa9, 0x99, 0x3e, 0x36, 0x47, 0x06, 0x81, 0x6a,
		  0xba, 0x3e, 0x25, 0x71, 0x78, 0x50, 0xc2, 0x6c,
		  0x9c, 0xd0, 0xd8, 0x9d }
		},
		{ "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		{ 0x84, 0x98, 0x3E, 0x44, 0x1C, 0x3B, 0xD2, 0x6E,
		  0xBA, 0xAE, 0x4A, 0xA1, 0xF9, 0x51, 0x29, 0xE5,
		  0xE5, 0x46, 0x70, 0xF1 }
		}
	};

	int i;
	BYTE tmp[20];
	HASH_STATE md;

	for (i = 0; i < (int)(sizeof(tests) / sizeof(tests[0]));  i++) {
		sha1_init(&md);
		sha1_update(&md, (BYTE*)tests[i].msg, (UINT32)strlen(tests[i].msg));
		sha1_final(&md, tmp);
		if (memcmp(tmp, tests[i].hash, SHA1_HASH_SIZE) != 0) {
			return -1;
		}
	}
	return 0;
}

int main() {
	int ret;

	ret = sha1_test();

	if (ret == 0)
		printf("sha1 test ok\n");
	else
		printf("sha1 test failed\n");

	return ret;
}

#endif
