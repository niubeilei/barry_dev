////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SslCipher.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifdef __KERNEL__
#include <asm/current.h>
#include <linux/sched.h>
#endif

#include "aos/aosReturnCode.h"
#include "aosUtil/Types.h"
#include "ssl/Ssl.h"
#include "ssl/SslCipher.h"
#include "ssl/SslCommon.h"
#include "ssl/SslMisc.h"
#include "ssl/des.h"
#include "ssl/rc4.h"
#include "ssl/rsa.h"
#include "ssl/sha1.h"
#include "ssl/md5.h"

#include "JNS9054Card/lib_9054jmk.h"


struct AosSslCipher aos_ssl_cipher[eAosEncryptType_Max];
struct AosSslHashCipher aos_ssl_hashcipher[eAosEncryptType_Max];
struct AosSslAsyCipher aos_ssl_asycipher;

static void AosSsl_nullHashInit(void *ctx)
{
	aos_trace("AosSsl_nullhashInit");
}

static void AosSsl_nullHashUpdate(void *ctx, const u8 *data, unsigned int len)
{
	aos_trace("AosSsl_nullHashUpdate");
}

static void AosSsl_nullHashFinal(void* ctx, u8 *out)
{
	aos_trace("AosSsl_nullHashFinal");
}

static int AosSsl_encryptNullFun(void *cipher_ctx, u8 *in, int in_len, u8 *out, int *out_len, u8 *key, int key_len)
{
	//aos_trace("null encrypt/decrypt");
	memcpy(out, in, in_len);
	*out_len = in_len;
	return eAosRc_Success;
}

static int AosSsl_setkeyNullFun(void *cipher_ctx, const u8 *in_key, int key_len)
{
	//aos_trace("this setkey function is not initialized");
	return eAosRc_Success;
}

static int AosSsl_desCBCEnc(void *cipher_ctx, uint8 *input, int input_len, uint8* output, int *output_len, uint8* key, int key_len)
{
	DES_CBC_CTX ctx;
	uint8 *iv;

	iv = (uint8*) cipher_ctx;

	DES_CBCInit(&ctx, key, iv, 1);
	DES_CBCUpdate(&ctx, output, input, input_len);

	return eAosRc_Success; 
}

static int AosSsl_desCBCDec(void *cipher_ctx, uint8 *in, int in_len, uint8* out, int *out_len, uint8* key, int key_len)
{
	DES_CBC_CTX ctx;
	uint8 *iv;

	iv = (uint8*) cipher_ctx;

	DES_CBCInit(&ctx, key, iv, 1);
	DES_CBCUpdate(&ctx, out, in, in_len);
	
	*out_len = in_len;
	return eAosRc_Success;
}

static int AosSsl_rc4Crypt(void *cipher_ctx, u8 *in, int in_len, u8 *out, int *out_len, u8 *key, int key_len)
{
	struct rc4_ctx *ctx = (struct rc4_ctx*)cipher_ctx;
	rc4_crypt((void*)ctx, out, in, in_len);
	*out_len = in_len;
	return 0;
}

static int AosSsl_hardwareRc4Enc(void *cipher_ctx, u8 *in, int in_len, u8 *out, int *out_len, u8 *key, int key_len)
{

#ifdef JNS_JMK
	int len;
	len = Seq_Encrypt(0, (unsigned char*)key, in_len, (unsigned char*)in, (unsigned char*)out);
	aos_assert1(len	> 0);
	*out_len = len;
#endif //ifdef __KERNEL__
	
	return eAosRc_Success;
}

static int AosSsl_hardwareRc4Dec(void *cipher_ctx, u8 *in, int in_len, u8 *out, int *out_len, u8 *key, int key_len)
{

#ifdef JNS_JMK
    int len;
    len = Seq_Decrypt(0, (unsigned char*)key, in_len, (unsigned char*)in, (unsigned char*)out);
    aos_assert1(len > 0);
    *out_len = len;
#endif //ifdef __KERNEL__

    return eAosRc_Success;
}

static int AosSsl_hardwareRsaPubEnc(
	unsigned char *out, 
	unsigned int *out_len, 
	unsigned char *in, 
	unsigned int in_len, 
	struct R_RSA_PUBLIC_KEY *key)
{
	int ret = eAosRc_Success;

#ifdef JNS_JMK
	*out_len = RSA_OutsidePub_Enc( key->modulus, in_len, in, out);
#endif //__KERNEL__

	return ret;
}

static int AosSsl_hardwareRsaPubDec(
	unsigned char *out, 
	unsigned int *out_len, 
	unsigned char *in, 
	unsigned int in_len, 
	struct R_RSA_PUBLIC_KEY *key)
{
	int ret = 0;

#ifdef JNS_JMK
	//aos_debug("hardware rsa pub dec");
	*out_len = RSA_OutsidePub_Dec(key->modulus, in_len, in, out);
#endif //__KERNEL__

	return ret;
}

static int AosSsl_hardwareRsaPriEnc(
	unsigned char *out, 
	unsigned int *out_len, 
	unsigned char *in, 
	unsigned int in_len, 
	struct AosRsaPrivateKey *key)
{
	int ret = 0;
	
#ifdef CONFIG_JNS_JMK
	//aos_debug("hardware rsa pri enc");	
	*out_len = RSA_IndexPri_Enc(PRIVATEKEY_INCARD_INDEX, in_len, in, out);
#endif //__KERNEL__

	return ret;
}

static int AosSsl_hardwareRsaPriDec(
	unsigned char *out, 
	unsigned int *out_len, 
	unsigned char *in, 
	unsigned int in_len, 
	struct AosRsaPrivateKey *key)
{
	int ret = 0;
		
#ifdef JNS_JMK
	
	//aos_debug("hardware rsa pri dec");		
	*out_len = RSA_IndexPri_Dec(PRIVATEKEY_INCARD_INDEX, in_len, in, out);
#endif //__KERNEL__
	
	return ret;
}

int AosSsl_cipherFunSet(AosSslAcceleratorType hardware_flag)
{

	aos_debug("set ssl cipher function");

	aos_ssl_hashcipher[eAosHashType_SHA1].dia_init = AosSsl_sha1Init;
	aos_ssl_hashcipher[eAosHashType_SHA1].dia_update = AosSsl_sha1Update;
	aos_ssl_hashcipher[eAosHashType_SHA1].dia_final = AosSsl_sha1Final;

	aos_ssl_hashcipher[eAosHashType_MD5].dia_init = AosSsl_md5Init;
	aos_ssl_hashcipher[eAosHashType_MD5].dia_update = AosSsl_md5Update;
	aos_ssl_hashcipher[eAosHashType_MD5].dia_final = AosSsl_md5Final;

	if ( hardware_flag != eAosSslAcceleratorType_517PM ) 
	{
		aos_ssl_cipher[eAosEncryptType_RC4_128].setkey = aos_rc4_setkey;		
		aos_ssl_cipher[eAosEncryptType_RC4_128].encrypt = AosSsl_rc4Crypt;
		aos_ssl_cipher[eAosEncryptType_RC4_128].decrypt = AosSsl_rc4Crypt;
		
		aos_ssl_cipher[eAosEncryptType_DES_CBC].encrypt = AosSsl_desCBCEnc;
		aos_ssl_cipher[eAosEncryptType_DES_CBC].decrypt = AosSsl_desCBCDec;

		aos_ssl_asycipher.pubenc = RSAPublicEncrypt;
		aos_ssl_asycipher.pubdec = RSAPublicDecrypt;		
		aos_ssl_asycipher.prienc = RSAPrivateEncrypt;
		aos_ssl_asycipher.pridec = RSAPrivateDecrypt;
	}
	else
	{
		aos_ssl_cipher[eAosEncryptType_RC4_128].setkey = AosSsl_setkeyNullFun;		
		aos_ssl_cipher[eAosEncryptType_RC4_128].encrypt = AosSsl_hardwareRc4Enc;
		aos_ssl_cipher[eAosEncryptType_RC4_128].decrypt = AosSsl_hardwareRc4Dec;

		
		aos_ssl_asycipher.pubenc = AosSsl_hardwareRsaPubEnc;
		aos_ssl_asycipher.pubdec = AosSsl_hardwareRsaPubDec;		
		aos_ssl_asycipher.prienc = AosSsl_hardwareRsaPriEnc;
		aos_ssl_asycipher.pridec = AosSsl_hardwareRsaPriDec;
	}
	return eAosRc_Success;	

}

int AosSsl_cipherFunInit(void)
{
	int i;
	
	aos_trace("AosSsl_cipherFun init");
	
	for(i = 0; i<eAosEncryptType_Max; i++)
	{
		aos_ssl_cipher[i].setkey = AosSsl_setkeyNullFun;
		aos_ssl_cipher[i].encrypt = AosSsl_encryptNullFun;
		aos_ssl_cipher[i].decrypt = AosSsl_encryptNullFun;
	}

	for (i = 0; i<eAosHashType_MAX; i++)
	{	
		aos_ssl_hashcipher[i].dia_init = AosSsl_nullHashInit;
		aos_ssl_hashcipher[i].dia_update = AosSsl_nullHashUpdate;
		aos_ssl_hashcipher[i].dia_final = AosSsl_nullHashFinal;
		aos_ssl_hashcipher[i].pad_len = (i==(int)eAosHashType_MD5)? MD5_PAD_LENGTH : SHA1_PAD_LENGTH;
		aos_ssl_hashcipher[i].hash_len = (i==(int)eAosHashType_MD5)? MD5_LEN : SHA1_LEN;
		aos_ssl_hashcipher[i].ctx_size = (i==(int)eAosHashType_MD5)? sizeof(struct MD5Context) : sizeof(struct sha1_ctx);
	}

	//AosSsl_cipherFunSet(aos_ssl_hardware_flag);
	AosSsl_cipherFunSet( gAosSslAcceleratorType );
	return eAosRc_Success;
	
}
	
