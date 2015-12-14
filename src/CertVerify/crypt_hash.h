////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: crypt_hash.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef __SHA1__
#define __SHA1__

#include "common.h"

#define SHA1_HASH_SIZE	20
#define MD5_HASH_SIZE	16

#undef  MIN
#undef  MAX
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)>(y)?(x):(y))

#define LOAD32H(x, y)                            \
     { x = ((UINT32)((y)[0] & 255)<<24) | \
           ((UINT32)((y)[1] & 255)<<16) | \
           ((UINT32)((y)[2] & 255)<<8)  | \
           ((UINT32)((y)[3] & 255)); 	 \
     }
#define LOAD32L(x, y)                            \
     { x = ((UINT32)((y)[3] & 255)<<24) | \
           ((UINT32)((y)[2] & 255)<<16) | \
           ((UINT32)((y)[1] & 255)<<8)  | \
           ((UINT32)((y)[0] & 255)); }

#define STORE32H(x, y)                                                                     \
     { (y)[0] = (BYTE)(((x)>>24)&255); (y)[1] = (BYTE)(((x)>>16)&255);   \
       (y)[2] = (BYTE)(((x)>>8)&255); (y)[3] = (BYTE)((x)&255); }

#define STORE32L(x, y)                                                                     \
     { (y)[3] = (BYTE)(((x)>>24)&255); (y)[2] = (BYTE)(((x)>>16)&255);   \
       (y)[1] = (BYTE)(((x)>>8)&255); (y)[0] = (BYTE)((x)&255); }

#if defined(__GNUC__) && defined(__i386__)
	static inline UINT32
	ROL(UINT32 word, int i) {
		__asm__("roll %%cl,%0"
			:"=r" (word)
			:"0" (word),"c" (i));
		return word;
	}

	static inline UINT32
	ROR(UINT32 word, int i) {
		__asm__("rorl %%cl,%0"
			:"=r" (word)
			:"0" (word),"c" (i));
		return word;
	}
#else
	/* rotates the hard way  */
	#define ROL(x, y) ( (((UINT32)(x)<<(UINT32)((y)&31)) | (((UINT32)(x)&0xFFFFFFFFUL)>>(UINT32)(32-((y)&31)))) & 0xFFFFFFFFUL)
	#define ROR(x, y) ( ((((UINT32)(x)&0xFFFFFFFFUL)>>(UINT32)((y)&31)) | ((UINT32)(x)<<(UINT32)(32-((y)&31)))) & 0xFFFFFFFFUL)
#endif

struct sha1_state {
	UINT32	length_hi;
	UINT32	length_lo;

	UINT32 	state[5], curlen;
	BYTE	buf[64];
};

typedef struct sha1_state HASH_STATE;

extern void sha1_init(HASH_STATE * md);
extern void sha1_update(HASH_STATE * md, const BYTE *buf, UINT32 len);
extern int  sha1_final(HASH_STATE * md, BYTE *hash);

extern void md5_init(HASH_STATE * md);
extern void md5_update(HASH_STATE * md, const BYTE *buf, UINT32 len);
extern int  md5_final(HASH_STATE * md, BYTE *hash);

#endif
