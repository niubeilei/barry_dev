////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sha1.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "KernelSimu/string.h"
#include "aosUtil/Memory.h"

#if KERNEL
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#else
#include <stdio.h>
#endif

#include "ssl/SslCommon.h"
#include "ssl/sha1.h"

void  SHA(unsigned char *s,unsigned int nl,unsigned char *md)

{
	int i=0;
	int j=0;
	unsigned int ucCount=0;
	

	 unsigned int nll=nl*8;
	 unsigned char *temp;
	 int m1, u, l, lm;
	 unsigned int *m;
	 unsigned int w[80+0x80],A,B,C,D,E,a,b,c,d,e,t;
	 unsigned int G[10];


	temp=(unsigned char *)aos_malloc_atomic(nl+128);
	memcpy(temp, s, nl);
//	if (!CountFlag)
//	{
		m1=nl%64;
		u=(m1< 56)?63-m1:127-m1;
		temp[nl]=0x80;
		nl++;
		for( i=0;i<u;i++) 
			temp[nl++]=0;
		ucCount+=nll;
		temp[nl-4]=(ucCount>>24)&0xff;
		temp[nl-3]=(ucCount>>16)&0xff;
		temp[nl-2]=(ucCount>>8)&0xff;
		temp[nl-1]=ucCount&0xff;
//		ucCount=0;
//	}
//	else 	
//		ucCount+=nll;

	l=nl/4;

#ifdef RGDEBUG
        printf("temp :\n");
        for(i=0;i<(nl/64+1)*64;i++)
		printf (" %02X ",*(temp+i));
#endif

	m=(unsigned int *)aos_malloc_atomic(l*sizeof(int));
	lm=0;
	for(i=0;i<(int)nl;i+=4)
	{
		m[lm]=temp[i+3]|(temp[i+2]<<8)|(temp[i+1]<<16)|(temp[i]<<24);
		lm++;
	}

	A=0x67452301;
	B=0xefcdab89;
	C=0x98badcfe;
	D=0x10325476;
	E=0xc3d2e1f0;

/*
	A=(*(IV)<<24)|(*(IV+1)<<16)|(*(IV+2)<<8)|(*(IV+3));
	B=(*(IV+4)<<24)|(*(IV+5)<<16)|(*(IV+6)<<8)|(*(IV+7));
	C=(*(IV+8)<<24)|(*(IV+9)<<16)|(*(IV+10)<<8)|(*(IV+11));
	D=(*(IV+12)<<24)|(*(IV+13)<<16)|(*(IV+14)<<8)|(*(IV+15));
	E=(*(IV+16)<<24)|(*(IV+17)<<16)|(*(IV+18)<<8)|(*(IV+19));*/
	
	for( i=0;i<l;i+=16)
	{
		a=A;b=B;c=C;d=D;e=E;
		//16个32位字变成80个
		for(j=0;j<16;j++) w[j]=m[j+i];
		for(j=16;j<80;j++)
		{
		w[j]=w[j-3]^w[j-8]^w[j-14]^w[j-16];
		w[j]=(w[j]<<1)|((w[j]>>31)&1);    //旧版不循环左移，新版需要循环左移1位。
		}
		for(j=0;j<20;j++)
		{
			t=(a<<5)|((a>>27)&0x1f);
			t=t+((b&c)|((b^0xffffffff)&d))+e+w[j]+0x5a827999;
			e=d;
			d=c;
			c=(b<<30)|((b>>2)&0x3fffffff);
			b=a;
			a=t;
		}
		for(j=20;j<40;j++)
		{
			t=(a<<5)|((a>>27)&0x1f);
			t=t+(b^c^d)+e+w[j]+0x6ed9eba1;
			e=d;
			d=c;
			c=(b<<30)|((b>>2)&0x3fffffff);
			b=a;
			a=t;
		}
		for(j=40;j<60;j++)
		{
			t=(a<<5)|((a>>27)&0x1f);
			t=t+((b&c)|(b&d)|(c&d))+e+w[j]+0x8f1bbcdc;
			e=d;
			 d=c;
			c=(b<<30)|((b>>2)&0x3fffffff);
			b=a;
			a=t;
		}
		for(j=60;j<80;j++)
		{
			t=(a<<5)|((a>>27)&0x1f);
			t=t+(b^c^d)+e+w[j]+0xca62c1d6;
			e=d;
			d=c;
			c=(b<<30)|((b>>2)&0x3fffffff);
			b=a;
			a=t;
		}
		A+=a;
		B+=b;
		C+=c;
		D+=d;
		E+=e;
	}
	

//A、B、C、D、E为最终结果
	G[0]=A;
	G[1]=B;
	G[2]=C;
	G[3]=D;
	G[4]=E;

	for(j=0;j<5;j++)
	{
		*md++=(G[j]>>24)&0xff;
		*md++=(G[j]>>16)&0xff;
		*md++=(G[j]>>8)&0xff;
		*md++=(G[j])&0xff;
	}

	aos_free(m);
	aos_free(temp);
}

#undef  MIN
#undef  MAX
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define STORE32H(x, y)                                                                     \
     { (y)[0] = (uint8)(((x)>>24)&255); (y)[1] = (uint8)(((x)>>16)&255);   \
       (y)[2] = (uint8)(((x)>>8)&255); (y)[3] = (uint8)((x)&255); }

#define LOAD32H(x, y)                            \
     { x = ((uint32)((y)[0] & 255)<<24) | \
           ((uint32)((y)[1] & 255)<<16) | \
           ((uint32)((y)[2] & 255)<<8)  | \
           ((uint32)((y)[3] & 255)); 	 \
     }

#if defined(__GNUC__) && defined(__i386__)
	static inline uint32
	ROL(uint32 word, int i) {
		__asm__("roll %%cl,%0"
			:"=r" (word)
			:"0" (word),"c" (i));
		return word;
	}

	static inline uint32
	ROR(uint32 word, int i) {
		__asm__("rorl %%cl,%0"
			:"=r" (word)
			:"0" (word),"c" (i));
		return word;
	}
#else
	/* rotates the hard way  */
	#define ROL(x, y) ( (((uint32)(x)<<(uint32)((y)&31)) | (((uint32)(x)&0xFFFFFFFFUL)>>(uint32)(32-((y)&31)))) & 0xFFFFFFFFUL)
	#define ROR(x, y) ( ((((uint32)(x)&0xFFFFFFFFUL)>>(uint32)((y)&31)) | ((uint32)(x)<<(uint32)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

#define F0(x,y,z)	(z ^ (x & (y ^ z)))
#define F1(x,y,z)	(x ^ y ^ z)
#define F2(x,y,z)	((x & y) | (z & (x | y)))
#define F3(x,y,z)	(x ^ y ^ z)

static void
sha1_compress(struct sha1_ctx *md) {
	uint32	a,b,c,d,e,W[80],i;
	uint32	t;

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


void AosSsl_sha1Init(void *ctx) {
	struct sha1_ctx *md = (struct sha1_ctx*)ctx;
	md->state[0] = 0x67452301UL;
	md->state[1] = 0xefcdab89UL;
	md->state[2] = 0x98badcfeUL;
	md->state[3] = 0x10325476UL;
	md->state[4] = 0xc3d2e1f0UL;
	md->curlen = 0;

	md->length_hi = 0;
	md->length_lo = 0;
}

void AosSsl_sha1Update(void *ctx, const unsigned char *buf, unsigned int len) {
	uint32 n;
	struct sha1_ctx *md = (struct sha1_ctx*)ctx;

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

void AosSsl_sha1Final(void *ctx, unsigned char *hash) {
	int i;
	uint32	n;
	struct sha1_ctx *md = (struct sha1_ctx*)ctx;

	if (md->curlen >= sizeof(md->buf) || hash == NULL)
		return;

	/* increase the length of the message */
	n = (md->length_lo + (md->curlen << 3)) & 0xFFFFFFFFL;
	if (n < md->length_lo) {
		md->length_hi++;
	}
	md->length_hi += (md->curlen >> 29);
	md->length_lo = n;

	/* append the '1' bit */
	md->buf[md->curlen++] = (uint8)0x80;

	/* if the length is currently above 56 bytes we append zeros then compress.
	  Then we can fall back to padding zeros and length encoding like normal.
 	*/
	if (md->curlen > 56) {
		while (md->curlen < 64) {
			md->buf[md->curlen++] = (uint8)0;
		}
		sha1_compress(md);
		md->curlen = 0;
	}

	/* pad upto 56 bytes of zeroes  */
	while (md->curlen < 56) {
		md->buf[md->curlen++] = (uint8)0;
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

	return;
}
