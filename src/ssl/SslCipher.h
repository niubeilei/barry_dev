////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslCipher.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifndef __SSL_CIPHER_H__
#define __SSL_CIPHER_H__

#define PRIVATEKEY_INCARD_INDEX 9

typedef struct AosSslCipher{
	int (*encrypt)(void *cipher_ctx, u8 *in, int int_len, u8 *out, int *out_len, u8 *key, int key_len);
	int (*decrypt)(void *cipher_ctx, u8 *in, int int_len, u8 *out, int *out_len, u8 *key, int key_len);
	int (*setkey)(void *cipher_ctx, const u8 *in_key, int key_len);
}AosSslCipher_t;

typedef struct AosSslHashCipher{
	void (*dia_init)(void *ctx);
	void (*dia_update)(void *ctx, const u8 *data, unsigned int len);
	void (*dia_final)(void* ctx, u8 *out); 
	int pad_len;
	int hash_len;
	int ctx_size;
}AosSslHashCiper_t;

struct R_RSA_PUBLIC_KEY;
struct AosRsaPrivateKey;

typedef struct AosSslAsyCipher{
	int (*pubenc)(
		unsigned char *out, 
		unsigned int *out_len, 
		unsigned char *in, 
		unsigned int in_len, 
		struct R_RSA_PUBLIC_KEY *key);
	int (*pubdec)(
		unsigned char *out, 
		unsigned int *out_len, 
		unsigned char *in, 
		unsigned int in_len, 
		struct R_RSA_PUBLIC_KEY *key);
	int (*prienc) (
		unsigned char *out, 
		unsigned int *out_len, 
		unsigned char *in, 
		unsigned int in_len, 
		struct AosRsaPrivateKey *key);
	int (*pridec) (
		unsigned char *out, 
		unsigned int *out_len, 
		unsigned char *in, 
		unsigned int in_len, 
		struct AosRsaPrivateKey *key);	
}AosSslAsyCipher_t;

extern struct AosSslCipher aos_ssl_cipher[eAosEncryptType_Max];
extern struct AosSslHashCipher aos_ssl_hashcipher[eAosEncryptType_Max];
extern struct AosSslAsyCipher aos_ssl_asycipher;


extern int AosSsl_cipherFunInit(void);

extern int AosSsl_cipherFunSet(AosSslAcceleratorType hardware_flag);


#endif  //ifndef __SSL_CIPHER_H__

