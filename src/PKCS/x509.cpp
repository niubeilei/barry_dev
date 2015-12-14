////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: x509.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/slab.h>
#else
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#endif

#include "aosUtil/Alarm.h"
#include "aosUtil/StringUtil.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "ASN1/AsnCodec.h"
#include "PKCS/ReturnCode.h"
#include "PKCS/asn1.h"
#include "PKCS/x509.h"
#include "PKCS/Base64.h"
#include "ssl/rsa.h"

/*#include "x509_attr.h"*/
/****************************************************
 Read a certificate serial number
	CertificateSerialNumber  ::=  INTEGER

 ****************************************************/
int
x509_decode_serial_number(DATA_OBJECT *object, 
						  asn_data_t *sn, 
						  const int tag ) 
{
	int length, ret;

	if (!sn)
		return -1;
	/* Read the integer component of the serial number */
	ret = asn_decode_generic_hole(object, &length, BER_INTEGER);
	if(ret < 0)
		return -1;

	if (length < 0 || length > ASN_SMALL_DATA_LEN)
		return -1;

	if(!length) {
		sn->len = 1;
		sn->data[0] = 0;
	} else {
		sn->len = length;
		ret = asn_read(object, sn->data, length);
	}

	return ret;
}


/***************************************************
 read X509 Validity, format as followed:
	Validity ::= SEQUENCE {
		notBefore	Time,
		notAfter	Time
	}

	Time ::= CHOICE {
		utcTime		UTCTime,
		generalTime	GeneralizedTime
	}
****************************************************/
static int
x509_decode_validity(DATA_OBJECT *object, x509_validity_t *validity) 
{
	int ret;

	ret = asn_decode_sequence(object, NULL);
	if (ret < 0)	return -1;
	//notBefore

	if (asn_peek_tag(object) == BER_TIME_UTC)
		ret = asn_decode_UTC_time(object, &validity->notbefore);
	else
		ret = asn_decode_generalized_time(object, &validity->notbefore);

	if (ret < 0)	return -1;

	//notAfter
	if (asn_peek_tag(object) == BER_TIME_UTC )
		ret = asn_decode_UTC_time(object, &validity->notafter);
	else
		ret = asn_decode_generalized_time(object, &validity->notbefore);
	if (ret < 0)	return -1;

	return 0;
}


typedef struct 
{
	const CRYPT_ALGO_TYPE algorithm;	/* The basic algorithm */
	const CRYPT_ALGO_TYPE sub_algorithm;	/* The algorithm subtype */
	const BYTE *oid;			/* The OID for this algorithm */
} ALGOID_INFO;


static const ALGOID_INFO algo_id_info_tbl[] = 
{
	/* RSA and <hash>WithRSA */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x01" ) },
	  /* rsaEncryption (1 2 840 113549 1 1 1) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_MD2,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x02" ) },
	  /* md2withRSAEncryption (1 2 840 113549 1 1 2) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_MD4,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x03" ) },
	  /* md4withRSAEncryption (1 2 840 113549 1 1 3) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_MD5,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x04" ) },
	  /* md5withRSAEncryption (1 2 840 113549 1 1 4) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x05" ) },
	  /* sha1withRSAEncryption (1 2 840 113549 1 1 5) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x06\x2B\x24\x03\x03\x01\x01" ) },
	  /* Another rsaSignatureWithsha1 (1 3 36 3 3 1 1) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_RIPEMD160,
	  MKOID( "\x06\x06\x2B\x24\x03\x03\x01\x02" ) },
	  /* rsaSignatureWithripemd160 (1 3 36 3 3 1 2) */

	{ CRYPT_ALGO_RSA, CRYPT_ALGO_SHA2,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x0B" ) },
	  /* sha256withRSAEncryption (1 2 840 113549 1 1 11) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_SHA2,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x0C" ) },
	  /* sha384withRSAEncryption (1 2 840 113549 1 1 12) */
	{ CRYPT_ALGO_RSA, CRYPT_ALGO_SHA2,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x01\x0D" ) },
	  /* sha512withRSAEncryption (1 2 840 113549 1 1 13) */

	/* DSA and dsaWith<hash> */
	{ CRYPT_ALGO_DSA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x07\x2A\x86\x48\xCE\x38\x04\x01" ) },
	  /* dsa (1 2 840 10040 4 1) */
	{ CRYPT_ALGO_DSA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x0C" ) },
	  /* Peculiar deprecated dsa (1 3 14 3 2 12), but used by CDSA and the
	     German PKI profile */
	{ CRYPT_ALGO_DSA, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x07\x2A\x86\x48\xCE\x38\x04\x03" ) },
	  /* dsaWithSha1 (1 2 840 10040 4 3) */
	{ CRYPT_ALGO_DSA, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x1B" ) },
	  /* Another dsaWithSHA1 (1 3 14 3 2 27) */
	{ CRYPT_ALGO_DSA, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x02\x01\x01\x02" ) },
	  /* Yet another dsaWithSHA-1 (2 16 840 1 101 2 1 1 2) */
	{ CRYPT_ALGO_DSA, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x0D" ) },
	  /* When they ran out of valid dsaWithSHA's, they started using invalid
	     ones.  This one is from JDK 1.1 and is actually dsaWithSHA, but it's
		 used as if it were dsaWithSHA-1 (1 3 14 3 2 13) */

	/* Elgamal and elgamalWith<hash>.  The latter will never actually be
	   used since we won't be doing Elgamal signing, only key exchange */
	{ CRYPT_ALGO_ELGAMAL, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x0A\x2B\x06\x01\x04\x01\x97\x55\x01\x02\x01" ) },
	  /* elgamal (1 3 6 1 4 1 3029 1 2 1) */
	{ CRYPT_ALGO_ELGAMAL, CRYPT_ALGO_SHA,
	  MKOID( "\x06\x0B\x2B\x06\x01\x04\x01\x97\x55\x01\x02\x01\x01" ) },
	  /* elgamalWithSHA-1 (1 3 6 1 4 1 3029 1 2 1 1) */
	{ CRYPT_ALGO_ELGAMAL, CRYPT_ALGO_RIPEMD160,
	  MKOID( "\x06\x0B\x2B\x06\x01\x04\x01\x97\x55\x01\x02\x01\x02" ) },
	  /* elgamalWithRIPEMD-160 (1 3 6 1 4 1 3029 1 2 1 2) */

	/* DH */
	{ CRYPT_ALGO_DH, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x07\x2A\x86\x48\xCE\x3E\x02\x01" ) },
	  /* dhPublicKey (1 2 840 10046 2 1) */

	/* KEA */
	{ CRYPT_ALGO_KEA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x02\x01\x01\x16" ) },
	  /* keyExchangeAlgorithm (2 16 840 1 101 2 1 1 22) */

	/* Hash algorithms */
	{ CRYPT_ALGO_MD2, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x02\x02" ) },
	  /* md2 (1 2 840 113549 2 2) */
	{ CRYPT_ALGO_MD2, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x0B\x60\x86\x48\x01\x86\xF8\x37\x01\x02\x08\x28" ) },
	  /* Another md2 (2 16 840 1 113719 1 2 8 40) */
	{ CRYPT_ALGO_MD4, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x02\x04" ) },
	  /* md4 (1 2 840 113549 2 4) */
	{ CRYPT_ALGO_MD4, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x02\x82\x06\x01\x0A\x01\x03\x01" ) },
	  /* Another md4 (0 2 262 1 10 1 3 1) */
	{ CRYPT_ALGO_MD4, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x0B\x60\x86\x48\x01\x86\xF8\x37\x01\x02\x08\x5F" ) },
	  /* Yet another md4 (2 16 840 1 113719 1 2 8 95) */
	{ CRYPT_ALGO_MD5, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x02\x05" ) },
	  /* md5 (1 2 840 113549 2 5) */
	{ CRYPT_ALGO_MD5, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x02\x82\x06\x01\x0A\x01\x03\x02" ) },
	  /* Another md5 (0 2 262 1 10 1 3 2) */
	{ CRYPT_ALGO_MD5, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x0B\x60\x86\x48\x01\x86\xF8\x37\x01\x02\x08\x32" ) },
	  /* Yet another md5 (2 16 840 1 113719 1 2 8 50) */
	{ CRYPT_ALGO_SHA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x1A" ) },
	  /* sha1 (1 3 14 3 2 26) */
	{ CRYPT_ALGO_SHA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x0B\x60\x86\x48\x01\x86\xF8\x37\x01\x02\x08\x52" ) },
	  /* Another sha1 (2 16 840 1 113719 1 2 8 82) */
	{ CRYPT_ALGO_RIPEMD160, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x05\x2B\x24\x03\x02\x01" ) },
	  /* ripemd160 (1 3 36 3 2 1) */
	{ CRYPT_ALGO_RIPEMD160, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x02\x82\x06\x01\x0A\x01\x03\x08" ) },
	  /* Another ripemd160 (0 2 262 1 10 1 3 8) */

	{ CRYPT_ALGO_SHA2, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x01" ) },
	  /* sha2-256 (2 16 840 1 101 3 4 2 1) */
	{ CRYPT_ALGO_SHA2, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x02" ) },
	  /* sha2-384 (2 16 840 1 101 3 4 2 2) */
	{ CRYPT_ALGO_SHA2, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x02\x03" ) },
	  /* sha2-512 (2 16 840 1 101 3 4 2 3) */

	/* MAC algorithms */
	{ CRYPT_ALGO_HMAC_MD5, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x2B\x06\x01\x05\x05\x08\x01\x01" ) },
	  /* hmac-MD5 (1 3 6 1 5 5 8 1 1) */
	{ CRYPT_ALGO_HMAC_SHA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x2B\x06\x01\x05\x05\x08\x01\x02" ) },
	  /* hmac-SHA (1 3 6 1 5 5 8 1 2) */
	{ CRYPT_ALGO_HMAC_SHA, CRYPT_ALGO_NONE,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x02\x07" ) },
	  /* Another hmacWithSHA1 (1 2 840 113549 2 7) */
#if 0
	/* Ciphers */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x01" ) },
	  /* aes128-ECB (2 16 840 1 101 3 4 1 1) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x15" ) },
	  /* aes192-ECB (2 16 840 1 101 3 4 1 21) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x29" ) },
	  /* aes256-ECB (2 16 840 1 101 3 4 1 41) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x02" ) },
	  /* aes128-CBC (2 16 840 1 101 3 4 1 2) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x16" ) },
	  /* aes192-CBC (2 16 840 1 101 3 4 1 22) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x2A" ) },
	  /* aes256-CBC (2 16 840 1 101 3 4 1 42) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x03" ) },
	  /* aes128-OFB (2 16 840 1 101 3 4 1 3) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x17" ) },
	  /* aes192-OFB (2 16 840 1 101 3 4 1 23) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x2B" ) },
	  /* aes256-OFB (2 16 840 1 101 3 4 1 43) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x04" ) },
	  /* aes128-CFB (2 16 840 1 101 3 4 1 4) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x18" ) },
	  /* aes192-CFB (2 16 840 1 101 3 4 1 24) */
	{ CRYPT_ALGO_AES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x03\x04\x01\x2C" ) },
	  /* aes256-CFB (2 16 840 1 101 3 4 1 44) */
	{ CRYPT_ALGO_BLOWFISH, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x0A\x2B\x06\x01\x04\x01\x97\x55\x01\x01\x01" ) },
	  /* blowfishECB (1 3 6 1 4 1 3029 1 1 1) */
	{ CRYPT_ALGO_BLOWFISH, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x0A\x2B\x06\x01\x04\x01\x97\x55\x01\x01\x02" ) },
	  /* blowfishCBC (1 3 6 1 4 1 3029 1 1 2) */
	{ CRYPT_ALGO_BLOWFISH, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x0A\x2B\x06\x01\x04\x01\x97\x55\x01\x01\x03" ) },
	  /* blowfishCFB (1 3 6 1 4 1 3029 1 1 3) */
	{ CRYPT_ALGO_BLOWFISH, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x0A\x2B\x06\x01\x04\x01\x97\x55\x01\x01\x04" ) },
	  /* blowfishOFB (1 3 6 1 4 1 3029 1 1 4) */
	{ CRYPT_ALGO_CAST, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x2A\x86\x48\x86\xF6\x7D\x07\x42\x0A" ) },
	  /* cast5CBC (1 2 840 113533 7 66 10) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x06" ) },
	  /* desECB (1 3 14 3 2 6) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x02\x01" ) },
	  /* Another desECB (0 2 262 1 10 1 2 2 1) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x07" ) },
	  /* desCBC (1 3 14 3 2 7) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x02\x02" ) },
	  /* Another desCBC (0 2 262 1 10 1 2 2 2) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x08" ) },
	  /* desOFB (1 3 14 3 2 8) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x02\x03" ) },
	  /* Another desOFB (0 2 262 1 10 1 2 2 3) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x05\x2B\x0E\x03\x02\x09" ) },
	  /* desCFB (1 3 14 3 2 9) */
	{ CRYPT_ALGO_DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x02\x05" ) },
	  /* Another desCFB (0 2 262 1 10 1 2 2 5) */
	{ CRYPT_ALGO_3DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x03\x07" ) },
	  /* des-EDE3-CBC (1 2 840 113549 3 7) */
	{ CRYPT_ALGO_3DES, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x03\x02" ) },
	  /* Another des3CBC (0 2 262 1 10 1 2 3 2) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x0B\x2B\x06\x01\x04\x01\x81\x3C\x07\x01\x01\x01" ) },
	  /* ideaECB (1 3 6 1 4 1 188 7 1 1 1) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x06\x2B\x24\x03\x01\x02\x01" ) },
	  /* Another ideaECB (1 3 36 3 1 2 1) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x05\x01" ) },
	  /* Yet another ideaECB (0 2 262 1 10 1 2 5 1) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x0B\x2B\x06\x01\x04\x01\x81\x3C\x07\x01\x01\x02" ) },
	  /* ideaCBC (1 3 6 1 4 1 188 7 1 1 2) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x06\x2B\x24\x03\x01\x02\x02" ) },
	  /* Another ideaCBC (1 3 36 3 1 2 2) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x05\x02" ) },
	  /* Yet another ideaCBC (0 2 262 1 10 1 2 5 2) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x0B\x2B\x06\x01\x04\x01\x81\x3C\x07\x01\x01\x04" ) },
	  /* ideaOFB (1 3 6 1 4 1 188 7 1 1 4) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x06\x2B\x24\x03\x01\x02\x03" ) },
	  /* Another ideaOFB (1 3 36 3 1 2 3) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x05\x03" ) },
	  /* Yet another ideaOFB (0 2 262 1 10 1 2 5 3) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x0B\x2B\x06\x01\x04\x01\x81\x3C\x07\x01\x01\x03" ) },
	  /* ideaCFB (1 3 6 1 4 1 188 7 1 1 3) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x06\x2B\x24\x03\x01\x02\x04" ) },
	  /* Another ideaCFB (1 3 36 3 1 2 4) */
	{ CRYPT_ALGO_IDEA, (CRYPT_ALGO_TYPE)CRYPT_MODE_CFB,
	  MKOID( "\x06\x09\x02\x82\x06\x01\x0A\x01\x02\x05\x05" ) },
	  /* Yet another ideaCFB (0 2 262 1 10 1 2 5 5) */
	{ CRYPT_ALGO_RC2, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x03\x02" ) },
	  /* rc2CBC (1 2 840 113549 3 2) */
	{ CRYPT_ALGO_RC2, (CRYPT_ALGO_TYPE)CRYPT_MODE_ECB,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x03\x03" ) },
	  /* rc2ECB (1 2 840 113549 3 3) */
	{ CRYPT_ALGO_RC4, (CRYPT_ALGO_TYPE)CRYPT_MODE_OFB,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x03\x04" ) },
	  /* rc4 (1 2 840 113549 3 4) */
	{ CRYPT_ALGO_RC5, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x03\x09" ) },
	  /* rC5-CBCPad (1 2 840 113549 3 9) */
	{ CRYPT_ALGO_RC5, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x08\x2A\x86\x48\x86\xF7\x0D\x03\x08" ) },
	  /* rc5CBC (sometimes used interchangeably with the above) (1 2 840 113549 3 8) */
	{ CRYPT_ALGO_SKIPJACK, (CRYPT_ALGO_TYPE)CRYPT_MODE_CBC,
	  MKOID( "\x06\x09\x60\x86\x48\x01\x65\x02\x01\x01\x04" ) },
	  /* fortezzaConfidentialityAlgorithm (2 16 840 1 101 2 1 1 4) */
#endif
	{ CRYPT_ALGO_NONE, (CRYPT_ALGO_TYPE)CRYPT_ALGO_NONE, NULL }

};

/* Map an OID to an algorithm type.  The subAlgorithm parameter can be
   NULL, in which case we don't return the sub-algorithm, but we return
   an error code if the OID has a sub-algorithm type */

CRYPT_ALGO_TYPE
x509_oid2algorithm(const BYTE *oid, int *sub_algorithm) 
{
	const BYTE oid_byte = oid[6];
	const int oid_size = sizeof_oid(oid);
	int i;

	if (oid_size < 7)
		return CRYPT_ALGO_NONE; /* it is an error */

	/* Clear the return value */
	if( sub_algorithm != NULL )
		*sub_algorithm = CRYPT_ALGO_NONE;

	/* Look for a matching OID.  For quick-reject matching we check the byte
	   furthest inside the OID that's likely to not match, this rejects the
	   majority of mismatches without requiring a full comparison */
	for (i = 0; algo_id_info_tbl[i].algorithm != CRYPT_ALGO_NONE; i++) {
		const ALGOID_INFO *algo_id_info_ptr = &algo_id_info_tbl[i];

		if(sizeof_oid(algo_id_info_ptr->oid) == oid_size &&
		   algo_id_info_ptr->oid[6] == oid_byte &&
		   !memcmp(algo_id_info_ptr->oid, oid, oid_size)) {
			/* If we're expecting a sub-algorithm, return the sub-algorithm
			   type alongside the main algorithm type */
			if (sub_algorithm != NULL) {
				*sub_algorithm = algo_id_info_ptr->sub_algorithm;
				return algo_id_info_ptr->algorithm;
			}

			/* If we're not expecting a sub-algorithm but there's one
			   present, mark it as an error */
			if (algo_id_info_ptr->sub_algorithm != CRYPT_ALGO_NONE)
				return CRYPT_ALGO_NONE;

			return algo_id_info_ptr->algorithm;
		}
	}

	return CRYPT_ALGO_NONE ;
}

/* Map an algorithm and optional sub-algorithm to an OID.  These functions
   are almost identical, the only difference is that the one used for
   checking only doesn't throw an exception when it encounters an algorithm
   value that it can't encode as an OID */

const BYTE *
x509_algorithm2oid(const CRYPT_ALGO_TYPE algorithm, 
				   const CRYPT_ALGO_TYPE sub_algorithm ) 
{
	int i;

	for (i = 0; algo_id_info_tbl[i].algorithm != CRYPT_ALGO_NONE; i++) {
		const ALGOID_INFO *algo_id_info_ptr = &algo_id_info_tbl[i];

		if (algo_id_info_ptr->algorithm == algorithm &&
		    algo_id_info_ptr->sub_algorithm == sub_algorithm)
			return algo_id_info_ptr->oid;
	}

	return NULL;
}


#if 0
static const BYTE *
x509_algorithm_to_oid_check(const CRYPT_ALGO_TYPE algorithm, const CRYPT_ALGO_TYPE sub_algorithm) {
	int i;

	for (i = 0; algo_id_info_tbl[i].algorithm != CRYPT_ALGO_NONE; i++)
		if (algo_id_info_tbl[i].algorithm == algorithm &&
			algo_id_info_tbl[i].sub_algorithm == sub_algorithm)
			return algo_id_info_tbl[i].oid;

	return NULL;
}
#endif


/* Read the start of an AlgorithmIdentifier record, used by a number of
   routines */
int
x509_decode_algo_id(DATA_OBJECT *object, 
					x509_algorithm_t *alg, 
					int *extra_length, 
					const int tag) 
{
	CRYPT_ALGO_TYPE crypt_algo;
	BYTE 		buffer[ASN_MAX_OID_SIZE + 8];
	int		buffer_length,
			crypt_sub_algo,
			length,
			ret;

	/* Clear the return values */
	if (alg != NULL) {
		alg->algorithm = CRYPT_ALGO_NONE;
		alg->sub_algorithm = CRYPT_ALGO_NONE;
	}

	if (extra_length)
		*extra_length = 0;
	/* Determine the algorithm information based on the AlgorithmIdentifier field */
	if (tag == DEFAULT_TAG)
		ret = asn_decode_sequence(object, &length);
	else
		ret = asn_decode_constructed(object, &length, tag);
	if (ret == 0)
		ret = asn_decode_raw_object(object, buffer, &buffer_length, ASN_MAX_OID_SIZE, BER_OBJECT_IDENTIFIER);
	if (ret < 0)	return -1;
	length -= buffer_length;
	if (buffer_length != sizeof_oid(buffer) || length < 0)
		/* It's a object-related error, make it persistent */
		return -1;

	if ((crypt_algo = x509_oid2algorithm(buffer, &crypt_sub_algo)) == CRYPT_ALGO_NONE)
		return -1;
	if (alg != NULL) {
		alg->algorithm = crypt_algo;
		alg->sub_algorithm = crypt_sub_algo;
	}
	/* If the user isn't interested in the algorithm parameters, skip them */
	if (extra_length == NULL)
		return (length > 0) ? asn_skip(object, length) : 0;

	/* Handle any remaining parameters */
	if (length == sizeof_null())
		return asn_decode_null(object);
	*extra_length = (int)length;
	return 0;
}


/*****************************************************************
 format:
	AlgorithmIdentifier  ::=  SEQUENCE  {
		algorithm	OBJECT IDENTIFIER,
		parameters	ANY DEFINED BY algorithm OPTIONAL
	}

 	--------now only support RSA with MD5 or SHA1--------
	List of expected (currently supported) OIDs
	algorithm			length		hex
	sha1				05		2b0e03021a
	md5				08		2a864886f70d0205
	rsaEncryption			09		2a864886f70d010101
	md5WithRSAEncryption		09		2a864886f70d010104
	sha-1WithRSAEncryption		09		2a864886f70d010105
*********************************************************************************************/
#if 0

static const OID_INFO x509_oid_info[] = {
	{(BYTE *)"\x06\x05\x2b\x0e\x03\x02\x1a", X509_SHA1, "sha1"},
	{(BYTE *)"\x06\x08\x2a\x86\x48\x86\xf7\x0d\x02\x05",X509_MD5, "md5"},
	{(BYTE *)"\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01", X509_RSA, "rsaEncryption"},
	{(BYTE *)"\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x04", X509_RSA_MD5, "md5WithRSAEncryption"},
	{(BYTE *)"\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x05", X509_RSA_SHA1, "shaWithRSAEncryption"},
	{NULL, X509_NULL, NULL}
};

static int x509_get_algorithm_identifier(DATA_OBJECT *object, int *algorithm_identifier) {
	int ret;

	ret = asn_decode_sequence(object, NULL);
	if (ret < 0) return -1;

	//if (asn_peek_tag(object) != BER_ID_OBJECT_IDENTIFIER)
	//	return -1;

	ret = asn_decode_oid(object, x509_oid_info, algorithm_identifier);

	/* Each of these cases might have a trailing NULL parameter.  Skip it */
	if (ret == 0 && asn_peek_tag(object) == BER_ID_NULL)
		ret = asn_decode_universal(object);

	return ret;
}
#endif


/* Read a uniqueID */

/**************************************************************************
   Name ::= CHOICE {
     RDNSequence
   }

   RDNSequence ::= SEQUENCE OF RelativeDistinguishedName

   RelativeDistinguishedName ::=
     SET OF AttributeTypeAndValue

   AttributeTypeAndValue ::= SEQUENCE {
     type     AttributeType,
     value    AttributeValue }

   AttributeType ::= OBJECT IDENTIFIER

   AttributeValue ::= ANY DEFINED BY AttributeType

   DirectoryString ::= CHOICE {
         teletexString           TeletexString (SIZE (1..MAX)),
         printableString         PrintableString (SIZE (1..MAX)),
         universalString         UniversalString (SIZE (1..MAX)),
         utf8String              UTF8String (SIZE (1..MAX)),
         bmpString               BMPString (SIZE (1..MAX)) }
****************************************************************************/


static int
x509_decode_unique_id(DATA_OBJECT *object, asn_bin_t *unique_id) 
{
	int length, ret;

	//aos_trace("decode_unique_id()");
	/* Read the length of the unique ID, allocate room for it, and read it
	   into the cert.  We ignore the tag since we've already checked it via
	   peekTag() before we got here */
	ret = asn_decode_bitstring_hole(object, &length, ANY_TAG);
	if (ret < 0 && (length < 1 || length > 1024))
		ret = -1;

	if (ret == 0) {
		/* clear it */
		//if (unique_id->data) 
		{
			unique_id->len  = length;
			unique_id->data = asn_object_cur_ptr(object);
		}

		ret = asn_skip(object, length);
	}
	return ret;
}

/* Read DN information and remember the encoded DN data so we can copy it
   (complete with any encoding errors) to the issuer DN field of anything
   we sign */

#define MKDNOID(value)	(( const BYTE * )"\x06\x03"value)

/* Type information for DN components */

typedef struct {
	const CRYPT_ATTRIBUTE_TYPE type;/* cryptlib type */
	const BYTE *oid;		/* OID for this type */
	const char *name, *altName;	/* Name for this type */
	const int max_length;		/* Maximum allowed length for this type */
	const BOOLEAN ia5OK;		/* Whether IA5 is allowed for this comp.*/
	const BOOLEAN wcsOK;		/* Whether widechar is allowed for comp.*/
} DN_COMPONENT_INFO;

static const DN_COMPONENT_INFO x509_certinfo_oids[] = {
	/* Useful components */
	{ CRYPT_CERTINFO_COMMONNAME, MKDNOID( "\x55\x04\x03" ), "cn", "oid.2.5.4.3", X509_MAX_TEXTSIZE, FALSE, TRUE },
	{ CRYPT_CERTINFO_COUNTRYNAME, MKDNOID( "\x55\x04\x06" ), "c", "oid.2.5.4.6", 2, FALSE, FALSE },
	{ CRYPT_CERTINFO_LOCALITYNAME, MKDNOID( "\x55\x04\x07" ), "l", "oid.2.5.4.7", 128, FALSE, TRUE },
	{ CRYPT_CERTINFO_STATEORPROVINCENAME, MKDNOID( "\x55\x04\x08" ), "sp", "oid.2.5.4.8", 128, FALSE, TRUE },
	{ CRYPT_CERTINFO_ORGANIZATIONNAME, MKDNOID( "\x55\x04\x0A" ), "o", "oid.2.5.4.10", X509_MAX_TEXTSIZE, FALSE, TRUE },
	{ CRYPT_CERTINFO_ORGANIZATIONALUNITNAME, MKDNOID( "\x55\x04\x0B" ), "ou", "oid.2.5.4.11", X509_MAX_TEXTSIZE, FALSE, TRUE },
	{ CRYPT_CERTINFO_EMAILADDR, (const BYTE *)"\x06\x09\x2A\x86\x48\x86\xF7\x0D\x01\x09\x01", "email", "oid.1.2.840.113549.1.9.1", X509_MAX_TEXTSIZE, TRUE, FALSE },
							/* emailAddress (1 2 840 113549 1 9 1). Deprecated, use an altName extension instead*/
#if 0
	/* Non-useful components */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x01" ), "oid.2.5.4.1", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* aliasObjectName (2 5 4 1) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x02" ), "oid.2.5.4.2", NULL, MAX_ATTRIBUTE_SIZE /*32768*/, FALSE, FALSE },
							/* knowledgeInformation (2 5 4 2) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x04" ), "s", "oid.2.5.4.4", X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* surname (2 5 4 4) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x05" ), "sn", "oid.2.5.4.5", X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* serialNumber (2 5 4 5) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x09" ), "st", "oid.2.5.4.9", 128, FALSE, FALSE },
							/* streetAddress (2 5 4 9) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x0C" ), "t", "oid.2.5.4.12", X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* title (2 5 4 12) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x0D" ), "d", "oid.2.5.4.13", 1024, FALSE, FALSE },
							/* description (2 5 4 13) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x0E" ), "oid.2.5.4.14", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* searchGuide (2 5 4 14) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x0F" ), "bc", "oid.2.5.4.15", 128, FALSE, FALSE },
							/* businessCategory (2 5 4 15) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x10" ), "oid.2.5.4.16", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* postalAddress (2 5 4 16) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x11" ), "oid.2.5.4.17", NULL, 40, FALSE, FALSE },
							/* postalCode (2 5 4 17) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x12" ), "oid.2.5.4.18", NULL, 40, FALSE, FALSE },
							/* postOfficeBox (2 5 4 18) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x13" ), "oid.2.5.4.19", NULL, 128, FALSE, FALSE },
							/* physicalDeliveryOfficeName (2 5 4 19) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x14" ), "oid.2.5.4.20", NULL, 32, FALSE, FALSE },
							/* telephoneNumber (2 5 4 20) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x15" ), "oid.2.5.4.21", NULL, 14, FALSE, FALSE },
							/* telexNumber (2 5 4 21) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x16" ), "oid.2.5.4.22", NULL, 24, FALSE, FALSE },
							/* teletexTerminalIdentifier (2 5 4 22) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x17" ), "oid.2.5.4.23", NULL, 32, FALSE, FALSE },
							/* facsimileTelephoneNumber (2 5 4 23) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x18" ), "oid.2.5.4.24", NULL, 15, FALSE, FALSE },
							/* x121Address (2 5 4 24) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x19" ), "isdn", "oid.2.5.4.25", 16, FALSE, FALSE },
							/* internationalISDNNumber (2 5 4 25) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x1A" ), "oid.2.5.4.26", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* registeredAddress (2 5 4 26) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x1B" ), "oid.2.5.4.27", NULL, 128, FALSE, FALSE },
							/* destinationIndicator (2 5 4 27) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x1C" ), "oid.2.5.4.28", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* preferredDeliveryMethod (2 5 4 28) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x1D" ), "oid.2.5.4.29", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* presentationAddress (2 5 4 29) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x1E" ), "oid.2.5.4.30", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* supportedApplicationContext (2 5 4 30) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x1F" ), "oid.2.5.4.31", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* member (2 5 4 31) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x20" ), "oid.2.5.4.32", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* owner (2 5 4 32) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x21" ), "oid.2.5.4.33", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* roleOccupant (2 5 4 33) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x22" ), "oid.2.5.4.34", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* seeAlso (2 5 4 34) */
							/* 0x23-0x28 are certs/CRLs and some weird encrypted directory components */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x29" ), "oid.2.5.4.41", NULL, MAX_ATTRIBUTE_SIZE /*32768*/, FALSE, FALSE },
							/* name (2 5 4 41) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x2A" ), "g", "oid.2.5.4.42", X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* givenName (2 5 4 42) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x2B" ), "i", "oid.2.5.4.43", X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* initials (2 5 4 43) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x2C" ), "oid.2.5.4.44", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* generationQualifier (2 5 4 44) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x2D" ), "oid.2.5.4.45", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* uniqueIdentifier (2 5 4 45) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x2E" ), "oid.2.5.4.46", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* dnQualifier (2 5 4 46) */
							/* 0x2F-0x30 are directory components */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x31" ), "oid.2.5.4.49", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* distinguishedName (2 5 4 49) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x32" ), "oid.2.5.4.50", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* uniqueMember (2 5 4 50) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x33" ), "oid.2.5.4.51", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* houseIdentifier (2 5 4 51) */
							/* 0x34-0x3A are more certs and weird encrypted directory components */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x41" ), "oid.2.5.4.65", NULL, 128, FALSE, FALSE },
							/* pseudonym (2 5 4 65) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x42" ), "oid.2.5.4.66", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* communicationsService (2 5 4 66) */
	{ CRYPT_ATTRIBUTE_NONE, MKDNOID( "\x55\x04\x43" ), "oid.2.5.4.67", NULL, X509_MAX_TEXTSIZE, FALSE, FALSE },
							/* communicationsNetwork (2 5 4 67) */
							/* 0x44-0x49 are more PKI-related attributes */
	{ CRYPT_ATTRIBUTE_NONE, (const BYTE *) "\x06\x0A\x09\x92\x26\x89\x93\xF2\x2C\x64\x01\x01", "uid", NULL, X509_MAX_TEXTSIZE, TRUE, FALSE },
							/* userid (0 9 2342 19200300 100 1 1) */
	{ CRYPT_ATTRIBUTE_NONE, (const BYTE *) "\x06\x0A\x09\x92\x26\x89\x93\xF2\x2C\x64\x01\x03", "oid.0.9.2342.19200300.100.1.3", NULL, X509_MAX_TEXTSIZE, TRUE, FALSE },
							/* rfc822Mailbox (0 9 2342 19200300 100 1 3) */
	{ CRYPT_ATTRIBUTE_NONE, (const BYTE *) "\x06\x0A\x09\x92\x26\x89\x93\xF2\x2C\x64\x01\x19", "dc", "oid.0.9.2342.19200300.100.1.25", X509_MAX_TEXTSIZE, TRUE, FALSE },
							/* domainComponent (0 9 2342 19200300 100 1 25) */
	{ CRYPT_ATTRIBUTE_NONE, (const BYTE *) "\x06\x07\x02\x82\x06\x01\x0A\x07\x14", "oid.0.2.262.1.10.7.20", NULL, X509_MAX_TEXTSIZE, TRUE, FALSE },
							/* nameDistinguisher (0 2 262 1 10 7 20) */
#endif
	{ CRYPT_ATTRIBUTE_NONE, NULL, NULL, NULL, X509_MAX_TEXTSIZE, TRUE, FALSE}
};


static int
x509_copy_from_asn1_string(DN_attributes_t *dn,	
						   int type, 
						   BYTE *buf, 
						   int len, 
						   int tag) 
{
	if (!dn || !buf)	return -1;

	if (type == CRYPT_CERTINFO_LOCALITYNAME ||
		type == CRYPT_CERTINFO_STATEORPROVINCENAME) {
			if (len > 2*X509_MAX_TEXTSIZE)	return -1;
	} else
			if (len > X509_MAX_TEXTSIZE)	return -1;
	switch(type) {
		case CRYPT_CERTINFO_COMMONNAME:
			memcpy(dn->CN, buf, len);
			dn->CN_len = len;
			dn->CN_tag = tag;
			break;
		case CRYPT_CERTINFO_COUNTRYNAME:
			memcpy(dn->C, buf, len);
			dn->C_len = len;
			dn->C_tag = tag;
			break;
		case CRYPT_CERTINFO_LOCALITYNAME:
			memcpy(dn->L, buf, len);
			dn->L_len = len;
			dn->L_tag = tag;
			break;
		case CRYPT_CERTINFO_STATEORPROVINCENAME:
			memcpy(dn->S, buf, len);
			dn->S_len = len;
			dn->S_tag = tag;
			break;
		case CRYPT_CERTINFO_ORGANIZATIONNAME:
			memcpy(dn->O, buf, len);
			dn->O_len = len;
			dn->O_tag = tag;
			break;
		case CRYPT_CERTINFO_ORGANIZATIONALUNITNAME:
			memcpy(dn->OU, buf, len);
			dn->OU_len = len;
			dn->OU_tag = tag;
			break;
		case CRYPT_CERTINFO_EMAILADDR:
			memcpy(dn->E, buf, len);
			dn->E_len = len;
			dn->E_tag = tag;
			break;
		default:
			ASN_ERROR("unrecognized type\n");
			return -1;
	}

	return 0;
}


/****************************************************************************
*                                                                           *
*                           Read a DN                                       *
*                                                                           *
*****************************************************************************/

/* Parse an AVA.   This determines the AVA type and leaves the object pointer
   at the start of the data value */

static int
x509_read_AVA(DATA_OBJECT *object, 
			  CRYPT_ATTRIBUTE_TYPE *type, 
			  int *length, 
			  int *string_tag) 
{
	BYTE oid[ASN_MAX_OID_SIZE];
	int oid_length, tag, i, ret;

	/* Clear return values */
	*type = CRYPT_ATTRIBUTE_NONE;
	*length = 0;
	*string_tag = 0;

	/* Read the start of the AVA and determine the type from the AttributeType
	   field.  If we find something which we don't recognise, we indicate it
	   as a non-component type which can be read or written but not directly
	   accessed by the user (although it can still be accessed using the
	   cursor functions) */
	asn_decode_sequence(object, NULL);

	ret = asn_decode_raw_object(object, oid, &oid_length, ASN_MAX_OID_SIZE, BER_OBJECT_IDENTIFIER );
	if(ret < 0)	return ret;

	for (i = 0; x509_certinfo_oids[i].oid != NULL; i++)
		if (sizeof_oid(x509_certinfo_oids[i].oid) == oid_length &&
		    !memcmp(x509_certinfo_oids[i].oid, oid, oid_length)) {
			*type = x509_certinfo_oids[i].type;
			break;
		}
	if (*type == CRYPT_ATTRIBUTE_NONE) {
		/* If we don't recognise the component type, skip it */
		asn_decode_universal(object);
		return OK_SPECIAL;
	}

	/* We've reached the data value */
	tag = asn_peek_tag(object);
	if (tag == BER_BITSTRING) {
		/* Bitstrings are used for uniqueIdentifiers, however these usually
		   encapsulate something else so we dig one level deeper to find the
		   encapsulated string */
		asn_decode_bitstring_hole(object, NULL, DEFAULT_TAG);
		tag = asn_peek_tag(object);
	}
	*string_tag = tag;

	return asn_decode_generic_hole(object, length, tag);
}



static int
x509_read_RDN(DATA_OBJECT *object, 
			  DN_attributes_t *dn, 
			  const int rdn_data_left) 
{
	CRYPT_ATTRIBUTE_TYPE type;
	BYTE  *value;
	const int rdn_start = object->pos;
	int value_length, string_tag;
	int ret;

	/* Read the type information for this AVA */
	ret = x509_read_AVA(object, &type, &value_length, &string_tag);
	if (ret < 0)	return ret;	 //error or OK_SPECIAL;

	value = object->data + object->pos;

	/* Skip broken AVAs with zero-length strings */
	if (value_length <= 0)	return 0;

	ret = asn_skip(object, value_length);
	if(ret < 0)	return -1;

	/* If there's room for another AVA, mark this one as being continued.  The
	   +10 is the minimum length for an AVA: SEQ { OID, value } (2-bytes SEQ +
	   5-bytes OID + 2-bytes tag + len + 1 byte min-length data).  We don't do
	   a simple =/!= check to get around incorrectly encoded lengths */
	if ((unsigned int)rdn_data_left >= (object->pos - rdn_start ) + 10)
		ASN_ERROR("rdn_data_left > 10\n, will continue it.");

	/* Convert the string into the local character set */
	ret = x509_copy_from_asn1_string(dn, type, value, value_length, string_tag);
	if (ret < 0)	return -1;

	return 0;
}


int
x509_read_DN(DATA_OBJECT *object, DN_attributes_t *dn) 
{
	int length, ret;
	int pos;
	HASH_STATE md;
	BYTE hash[SHA1_HASH_SIZE];

	pos = asn_object_pos(object);
	sha1_init(&md);

	ret = asn_decode_sequence(object, &length);
	if (ret < 0)
		return ret;

	while (length > 0) {
		const int spos = object->pos;
		int rdn_length;

		/* Read the start of the RDN */
		ret = asn_decode_set(object, &rdn_length );
		if (ret < 0)
			return ret;

		/* Read each RDN component */
		while(rdn_length > 0)	{
			const int rnd_start = object->pos;

			ret = x509_read_RDN(object, dn, rdn_length);
			if (ret < 0 && ret != OK_SPECIAL )
				return ret;

			rdn_length -= object->pos - rnd_start;
		}
		if (rdn_length < 0)
			return -1;

		length -= object->pos - spos;
	}
	if(length < 0)
		return -1;
	sha1_update(&md, object->data + pos, asn_object_pos(object) - pos);
	sha1_final(&md, hash);

	memcpy(dn->hash, hash, SHA1_HASH_SIZE);

	return 0;
}


/* Read public-key information
RFC3280
	SubjectPublicKeyInfo  ::=  SEQUENCE  {
		algorithm            AlgorithmIdentifier,
		subjectPublicKey     BIT STRING
	}

RFC3279 -- encoding for RSA public key

	RSAPublicKey ::= SEQUENCE {
		modulus            INTEGER,	-- n
		publicExponent     INTEGER	-- e
	}
*/

static int
x509_decode_public_key_info(DATA_OBJECT *object, rsa_pubkey_t *pubkey) 
{
	int length, ret;
	int pos;
	HASH_STATE md;
	BYTE hash[SHA1_HASH_SIZE];

	if (pubkey == NULL)
		return -1;

	ret = asn_decode_generic_hole(object, &length, DEFAULT_TAG);
	if (ret < 0)
		return ret;

	ret = x509_decode_algo_id(object, &pubkey->alg, NULL, DEFAULT_TAG);
	if (ret < 0)
		return ret;

	/* Read the BITSTRING encapsulation and the public key fields */
	ret = asn_decode_bitstring_hole(object, NULL, DEFAULT_TAG);
	pos = asn_object_pos(object);
	sha1_init(&md);

	if (ret == 0)
		ret = asn_decode_sequence(object, NULL);
	if (ret == 0)
		ret = asn_decode_bignum(object, &pubkey->modulus);
	if (ret == 0)
		ret = asn_decode_bignum(object, &pubkey->public_exponent);

	sha1_update(&md, object->data + pos, asn_object_pos(object) - pos);
	sha1_final(&md, hash);

	memcpy(pubkey->hash, hash, SHA1_HASH_SIZE);

	return ret;
}


/* decode RSA private */
int
x509_decode_private_key_info(DATA_OBJECT *object, 
							 rsa_privatekey_t *prvkey) 
{
	int length, ret;

	if (object == NULL || prvkey == NULL)
		return -1;

	/*
	RSAPrivateKey ::= SEQUENCE {
		version Version,
		modulus INTEGER, -- n
		publicExponent INTEGER, -- e
		privateExponent INTEGER, -- d
		prime1 INTEGER, -- p
		prime2 INTEGER, -- q
		exponent1 INTEGER, -- d mod (p-1)
		exponent2 INTEGER, -- d mod (q-1)
		coefficient INTEGER -- (inverse of q) mod p
		otherPrimeInfos   OtherPrimeInfos OPTIONAL
	}
	*/

	ret = asn_decode_generic_hole(object, &length, DEFAULT_TAG);
	if (ret < 0)
		return ret;

	ret = asn_decode_short_integer(object, &prvkey->version);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->modulus);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->public_exponent);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->private_exponent);

	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->prime1);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->prime2);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->exponent1);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->exponent2);
	if (ret == 0)
		ret = asn_decode_bignum(object, &prvkey->coefficient);

	if (ret == 0)
		prvkey->object = object;

	/* skip otherPrimeInfos */
	return ret;
}


int
x509_decode_signature(DATA_OBJECT *object, asn_bin_t *signature) 
{
	int  length;
	int  ret;

	if (signature) {
		signature->len  = 0;
		signature->data = NULL;
	};

	ret = asn_decode_bitstring_hole(object, &length, DEFAULT_TAG);
	if (ret == 0 && length > 0)	{
		signature->data = asn_object_cur_ptr(object);
		signature->len = length;
	}
	if (ret == 0 && length > 0)
		ret = asn_skip(object, length);

	return ret;
}


// 
// Description:
// 	The function decode a certificate. The input is the DER of the certificate.
// 	The caller should have created the certificate 'certinfo'. If failed, 
// 	the function returns -1. Otherwise, 'certinfo' is the decoded certificate
// 	and 0 is returned. 
// 
// Parameters:
//	'cert_der'
//		The DER of the certificate.
//
//	'cert_len'
//		The length of the certificate DER.
//
//	'certinfo'
//		The certificate object. If the decoding is successful, the results
//		are storedin 'certinfo'.
//
// Return Values
//	0 on success
//	-1 on failure
//	
int x509_decode_certinfo(const char *cert_der, 
				u32 cert_len, 
				X509_CERT_INFO *certinfo) 
{
	DATA_OBJECT object;
	int length, ret;
	UINT issuer_pos, subject_pos;
	
	//aos_trace("decode_certinfo()");

	object.data = (BYTE*)cert_der;
    	object.length = cert_len;
 	object.pos = 0;

	/* save the certificate data and length */
	// certinfo->object = &object;

	/*
		Certificate  ::=  SEQUENCE  {
		        tbsCertificate       TBSCertificate,
		        signatureAlgorithm   AlgorithmIdentifier,
		        signatureValue       BIT STRING
		}
		TBSCertificate  ::=  SEQUENCE  {
			version         [0]  EXPLICIT Version DEFAULT v1,
			serialNumber         CertificateSerialNumber,
			signature            AlgorithmIdentifier,
			issuer               Name,
			validity             Validity,
			subject              Name,
			subjectPublicKeyInfo SubjectPublicKeyInfo,
			issuerUniqueID  [1]  IMPLICIT UniqueIdentifier OPTIONAL,
			                     -- If present, version MUST be v2 or v3
			subjectUniqueID [2]  IMPLICIT UniqueIdentifier OPTIONAL,
			                     -- If present, version MUST be v2 or v3
			extensions      [3]  EXPLICIT Extensions OPTIONAL
			                     -- If present, version MUST be v3
		}
	*/

	/* Read the outer SEQUENCE and version number if it's present */
	ret = asn_decode_sequence(&object, &length);
	if (ret == 0) 
		ret = asn_decode_sequence(&object, &length);

	if (ret < 0 || (object.pos + length) > object.length) 
		return -1;

	/* is EXPLICIT version? if none, default 1 */
	if (asn_peek_tag(&object) == MAKE_CTAG(CTAG_CE_VERSION)) 
	{
		long version;
		asn_decode_constructed(&object, NULL, CTAG_CE_VERSION);
		asn_decode_short_integer(&object, &version);
		certinfo->version = version + 1;	/* Zero-based */
	} 
	else
	{
		certinfo->version = 1;
	}

	/* Read the serial number */
	ret = x509_decode_serial_number(&object, &certinfo->serial_number, DEFAULT_TAG );
	if(ret < 0) 
		return -1;

	/* Skip the signature algorithm information.  This was included to avert
	   a somewhat obscure attack that isn't possible anyway because of the
	   way the signature data is encoded in PKCS #1 sigs (although it's still
	   possible for some of the ISO sig.types) so there's no need to record
	   it */
	ret = x509_decode_algo_id(&object, &certinfo->sig_alg, NULL, DEFAULT_TAG);
	if (ret < 0)
		return -1;

	/* Read the issuer name, validity information, and subject name */
	issuer_pos = object.pos;
	aos_assert1(issuer_pos >= 0);
	ret = x509_read_DN(&object, &certinfo->issuer);
	if(ret < 0)
		return -1;
	// 
	// Need to store the issuer DER. 
	//
	aos_assert1(object.pos - issuer_pos > 0);

	certinfo->issuer_der = (char *)aos_zmalloc(object.pos - issuer_pos);
	aos_assert1(certinfo->issuer_der);
	memcpy(certinfo->issuer_der, &object.data[issuer_pos], object.pos - issuer_pos);
	certinfo->issuer_der_len = object.pos - issuer_pos;

	ret = x509_decode_validity(&object, &certinfo->validity);
	if(ret < 0)
		return -1;

	subject_pos = object.pos;
    aos_assert1(subject_pos >= 0);
	ret = x509_read_DN( &object, &certinfo->subject);
	if(ret < 0)
		return -1;
    // 
    // Need to store the subject DER. 
    //
    aos_assert1(object.pos - subject_pos > 0);

    certinfo->subject_der = (char *)aos_zmalloc(object.pos - subject_pos);
    aos_assert1(certinfo->subject_der);
    memcpy(certinfo->subject_der, &object.data[subject_pos], object.pos - subject_pos);
    certinfo->subject_der_len = object.pos - subject_pos;

	// Read the public key information
	ret = x509_decode_public_key_info(&object, &certinfo->pubkey);
	if (ret < 0)
		return ret;

	/* Read the issuer and subject unique ID's if there are any present */
	if (asn_peek_tag(&object) == MAKE_CTAG_PRIMITIVE(CTAG_CE_ISSUERUNIQUEID)) {
		ret = x509_decode_unique_id(&object, &certinfo->issuer_unique_id);
		if (ret < 0)
			return ret ;
	}

	if (asn_peek_tag(&object) == MAKE_CTAG_PRIMITIVE(CTAG_CE_SUBJECTUNIQUEID)) {
		ret = x509_decode_unique_id(&object, &certinfo->subject_unique_id);
		if (ret < 0)
			return ret ;
	}

	//skip certificate extensions now
	if (asn_peek_tag(&object) == MAKE_CTAG(CTAG_CE_EXTENSIONS))
		asn_decode_universal(&object);

	ret = x509_decode_algo_id(&object, &certinfo->signature_algorithm, NULL, DEFAULT_TAG);
	if (ret == 0)
		ret = x509_decode_signature(&object, &certinfo->signature);

	return ret ;
}


// 
// Description:
// 	The function decode a certificate. The input is the base64 DER of the certificate.
// 	The caller should have created the certificate 'certinfo'. If failed, 
// 	the function returns -1. Otherwise, 'certinfo' is the decoded certificate
// 	and 0 is returned. 
// 
// Parameters:
//	'cert_base64'
//		The base64 DER of the certificate.
//
//	'cert_len'
//		The length of cert_base64.
//
//	'certinfo'
//		The certificate object. If the decoding is successful, the results
//		are storedin 'certinfo'.
//
// Return Values
//	0 on success
//	-1 on failure
//	
int x509_decode_certinfo_base64(const char *cert_base64, X509_CERT_INFO *certinfo) 
{
	int der_len;
	char *cert_der = 0;
	int ret;

	der_len = AosBase64_decode(cert_base64, strlen(cert_base64), &cert_der);
	aos_assert1(cert_der);

	ret = x509_decode_certinfo(cert_der, der_len, certinfo);
	if (ret)
	{
		// 
		// Failed to decode the certificate. 
		// 
		aos_free(cert_der);
		certinfo->cert_der = 0;
		certinfo->cert_der_len = 0;
		return ret;
	}

	certinfo->cert_der = cert_der;
	certinfo->cert_der_len = der_len;
	return 0;
}


int AosCert_constructor(X509_CERT_INFO **cert)
{
	*cert = (X509_CERT_INFO *)aos_zmalloc(sizeof(X509_CERT_INFO));
	if (!(*cert))
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_MemErr, 
			"Failed to allocate memory");
		return 0;
	}

	memset(*cert, 0, sizeof(X509_CERT_INFO));
	atomic_set(&((*cert)->refcnt),1);
	return 0;
}


/*
SEQUENCE  {
	userCertificate         CertificateSerialNumber,
	revocationDate          Time,
	crlEntryExtensions      Extensions OPTIONAL
				-- if present, MUST be v2
}  OPTIONAL;
*/


int x509_dn2str(const char *title, 
				DN_attributes_t *dn, 
				char *buffer, 
				u32 buflen, 
				u32 *length)
{
	char local[200];

	sprintf(local, "%s" 
					"\n    C=%s" 
					"\n    ST=%s" 
					"\n    L=%s"
					"\n    O=%s"
					"\n    CN=%s",
		title, 
		dn->C, 
		dn->S, 
		dn->L, 
		dn->O,
		dn->CN);

	*length = strlen(local);
	aos_assert1(*length < 200);
	if (*length >= buflen)
	{
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr,
			"Buffer overflow: %d, %d", buflen, *length);
	}

	memcpy(buffer, local, *length);
	return 0;
}	
	

// 
// Description:
//	It converts the certificate 'cert' to a human-readable form. 
//  If successful, the function returns 0. Otherwise, a negative number is returned.
//
int x509_cert2str(X509_CERT_INFO *cert, char *buffer, u32 buflen)
{
	// 
	// Subject
	//	C=xxx
	//	ST=xxx
	//	...
	// Issuer
	//	C=xxx
	//	ST=xxx
	//	...
	// Serial Number
	// 
	int ret;
	u32 index = 0;
	u32 len = 0;
	uint8 sn[50];

	ret = x509_dn2str("Subject", &cert->subject, buffer+index, buflen-index, &len);
	index += len;
	ret = x509_dn2str("\nIssuer", &cert->issuer, buffer+index, buflen-index, &len);
	index += len;

	AosStrUtil_bin2HexStr(cert->serial_number.data, 
		cert->serial_number.len, 
		sn, 50);

	sprintf(buffer+index, "\nSerial Number:    %s", sn);
	
	return strlen(buffer);
}


int x509_cert_destructor(X509_CERT_INFO *cert)
{
	if (cert->cert_der)
		aos_free(cert->cert_der);
	if (cert->issuer_der)
		aos_free(cert->issuer_der);
	if (cert->subject_der)
		aos_free(cert->subject_der);
	if (cert->prikey)
	{
		aos_free(cert->prikey);
	}
	memset(cert, 0, sizeof(X509_CERT_INFO));
	aos_free(cert);
	return 0;
}


X509_CERT_INFO * x509_parse_cert_object(DATA_OBJECT *object)  
{
	X509_CERT_INFO * cert = NULL;
	int ret;

	if (!object) return NULL;

	AosCert_constructor(&cert);
	if (!cert) 
	{
		aos_alarm(eAosMD_PKCS, eAosAlarm_MemErr, "Failed to allocate memory");
		return 0;
	}

	ret = x509_decode_certinfo((char *)object->data, object->length, cert);
	if (ret < 0) goto error;

	return cert;

error:
	if (cert) aos_free(cert);
	return NULL;
}


int AosCert_decodePrivKey(
			const char *priv_key_base64, 
			AosRsaPrivateKey_t *priv_key)
{
	// 
	// Private key ASN.1 syntax:
	// RSAPrivateKey ::= SEQUENCE {
    //		version Version,
    //		 modulus INTEGER, -- n
    //		 publicExponent INTEGER, -- e
    //		 privateExponent INTEGER, -- d
    //		 prime1 INTEGER, -- p
    //		 prime2 INTEGER, -- q
    //		 exponent1 INTEGER, -- d mod (p-1)
    //		 exponent2 INTEGER, -- d mod (q-1)
    //		 coefficient INTEGER -- (inverse of q) mod p }
	// 
   	// Version ::= INTEGER
	// 
	// Below is an example:
	//
	// 30 82 01 3b 
    //		02 01       // Version
    //		    00  
    //		02 41       // modulus (65 bytes)
    //		    00 c1 f8 bf 09 b1 93 b4 10 49 8b 0c 72 eb c9 8e 
    //		    28 e8 5a 6a d8 7e bf 20 82 b3 7c 7e 55 76 55 9e 
    //		    07 d0 fe ee 58 80 0c f8 e0 8c 49 7b 5c 7c 51 41 
    //		    c3 ba 1b bb 82 8c 64 ed 87 a9 4b dc 22 59 f9 d6 
    //		    9d 
    //		02 03       // publicExponent
    //		    01 00 01 
    //		02 41       // privateExponent (65 bytes)
    //		    00 9f 70 c6 27 43 ae 33 7e 87 d5 22 20 7c ac 2a 
    //		    8c 79 a1 88 dd f8 21 46 a9 e8 bb 56 65 0d 66 07 
    //		    04 d9 d2 ab 3a cf db ce 97 cb 9a a9 3b e6 8a 7c 
    //		    5b 20 c5 ed 94 15 ea 42 88 61 52 cc df 2d 8e 1b 
    //		    01 
    //		02 21       // prime 1 (33 bytes)
    //		    00 f4 c8 40 1f 52 a1 aa d3 6e 21 82 33 1c f4 70 
    //		    13 4c 6e 19 30 8b a8 21 b6 88 ec aa ff 21 25 b5 
    //		    bd 
    //		02 21       // prime 2 (33 bytes)
    //		    00 ca dc 64 c4 3f e4 87 e7 fb ba ab 4c 98 c4 5f 
    //		    d6 97 b3 97 74 ef 0e c7 56 59 8b 62 e4 9d 5e 82 
    //		    61 
    //		02 20       // exponent 1
    //		    57 65 53 f7 9b 02 1b c2 c7 74 71 4f 1c 8c 6d a3 
    //		    86 4b 5c f5 07 37 ed 69 85 b3 b2 4d cb 20 f2 a5 
    //		02 20       // exponent 2
    //		    13 7b 73 19 fd e0 de c4 82 c2 7c ad 10 86 db 85 
    //		    6a 70 4e fd 65 f0 0f 68 d5 94 b3 5c ad 58 64 41 
    //		02 21       // coefficient 
    //		    00 ca 8e 66 14 92 62 1a 21 00 73 51 a9 b2 14 23 
    //		    3d 8f 49 91 a1 76 c8 dc 47 ad 61 d6 0d c8 be a7 
    //		    2e

	char *key_der = 0;
	u32 der_len;
	int modulus_len;
	int ret;
	unsigned int cursor = 0;
	unsigned int total_len;
	int version;

	aos_assert1(priv_key);
	aos_assert1(priv_key_base64);

	// 
	// Decode the base64
	//
	der_len = AosBase64_decode(priv_key_base64, 
		strlen(priv_key_base64), &key_der);
	aos_assert1(key_der);

 	ret = aosAsn1Codec_getSeqLen(key_der, &cursor, &total_len);

	// 
	// Decode version
	//
	ret = aosAsn1Codec_integer_decode(key_der, der_len, &cursor, &version);
	if (ret)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode the version: %d", ret);
	}

	// 
	// Decode modulus
	//
	modulus_len = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->modulus, MAX_RSA_MODULUS_LEN+1);
	if (modulus_len < 0)
	{
		aos_free(key_der);
		//aos_trace_hex("Private key", key_der, der_len);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode modulus: %d", ret);
	}

	// 
	// If the first byte is 0, we need to remove the first byte.
	//
	if (priv_key->modulus[0] == 0)
	{
		memmove(priv_key->modulus, &priv_key->modulus[1], MAX_RSA_MODULUS_LEN);
		modulus_len--;
	}

	aos_assert1(modulus_len <= MAX_RSA_MODULUS_LEN);
	priv_key->bits = (modulus_len << 3);

	// 
	// Decode Public Exponent
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->publicExponent, MAX_RSA_MODULUS_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode public exponent: %d", ret);
	}

	if (priv_key->publicExponent[0] == 0)
	{
		memmove(priv_key->publicExponent, &priv_key->publicExponent[1], MAX_RSA_MODULUS_LEN);
	}

	// 
	// Move the public exponent to the end of the buffer
	//
	memmove(&priv_key->publicExponent[modulus_len-ret], priv_key->publicExponent, ret);
	memset(priv_key->publicExponent, 0, modulus_len-ret);

	// 
	// Decode Private Exponent
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->privateExponent, MAX_RSA_MODULUS_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode private exponent: %d", ret);
	}

	if (priv_key->privateExponent[0] == 0)
	{
		memmove(priv_key->privateExponent, &priv_key->privateExponent[1], MAX_RSA_MODULUS_LEN);
	}

	// 
	// Decode Prime 1 
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->prime[0], MAX_RSA_PRIME_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode prime1: %d", ret);
	}

	if (priv_key->prime[0][0] == 0)
	{
		memmove(priv_key->prime[0], &priv_key->prime[0][1], MAX_RSA_PRIME_LEN);
	}

	// 
	// Decode Prime 2 
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->prime[1], MAX_RSA_PRIME_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode prime2: %d", ret);
	}

	if (priv_key->prime[1][0] == 0)
	{
		memmove(priv_key->prime[1], &priv_key->prime[1][1], MAX_RSA_PRIME_LEN);
	}

	// 
	// Decode Prime Exponent 1
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->primeExponent[0], MAX_RSA_PRIME_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode prime exponent 1: %d", ret);
	}

	if (priv_key->primeExponent[0][0] == 0)
	{
		memmove(priv_key->primeExponent[0], &priv_key->primeExponent[0][1], MAX_RSA_PRIME_LEN);
	}

	// 
	// Decode Prime Exponent 2
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->primeExponent[1], MAX_RSA_PRIME_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode prime exponent 2: %d", ret);
	}

	if (priv_key->primeExponent[1][0] == 0)
	{
		memmove(priv_key->primeExponent[1], &priv_key->primeExponent[1][1], MAX_RSA_PRIME_LEN);
	}

	// 
	// Decode Coefficient
	//
	ret = aosAsn1Codec_integer_decode1(key_der, der_len, 
			&cursor, (char *)priv_key->coefficient, MAX_RSA_PRIME_LEN+1);
	if (ret < 0)
	{
		aos_free(key_der);
		return aos_alarm(eAosMD_PKCS, eAosAlarm_PkcsErr, 
			"Failed to decode coefficient: %d", ret);
	}

	if (priv_key->coefficient[0] == 0)
	{
		memmove(priv_key->coefficient, &priv_key->coefficient[1], MAX_RSA_PRIME_LEN);
	}

	return 0;
}

#ifdef __KERNEL__
EXPORT_SYMBOL( x509_cert_destructor );
#endif

