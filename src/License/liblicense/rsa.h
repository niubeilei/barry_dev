////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rsa.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SSL_RSA_HEAD_H
#define AOS_SSL_RSA_HEAD_H
#ifndef RSA_HEAD_H
#define RSA_HEAD_H

#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

#endif

/* Error codes.
 */
#define RE_CONTENT_ENCODING 0x0400
#define RE_DATA 0x0401
#define RE_DIGEST_ALGORITHM 0x0402
#define RE_ENCODING 0x0403
#define RE_KEY 0x0404
#define RE_KEY_ENCODING 0x0405
#define RE_LEN 0x0406
#define RE_MODULUS_LEN 0x0407
#define RE_NEED_RANDOM 0x0408
#define RE_PRIVATE_KEY 0x0409
#define RE_PUBLIC_KEY 0x040a
#define RE_SIGNATURE 0x040b
#define RE_SIGNATURE_ENCODING 0x040c
#define RE_ENCRYPTION_ALGORITHM 0x040d
#define RE_SUCCESS 0x0000

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;
/* Type definitions.
 */
typedef unsigned int NN_DIGIT;
typedef unsigned short NN_HALF_DIGIT;
/* Constants.

   Note: MAX_NN_DIGITS is long enough to hold any RSA modulus, plus
   one more digit as required by R_GeneratePEMKeys (for n and phiN,
   whose lengths must be even). All natural numbers have at most
   MAX_NN_DIGITS digits, except for double-length intermediate values
   in NN_Mult (t), NN_ModMult (t), NN_ModInv (w), and NN_Div (c).
 */
/* Length of digit in bits */
#define NN_DIGIT_BITS 32
#define NN_HALF_DIGIT_BITS 16
/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)
/* Maximum length in digits */
#define MAX_NN_DIGITS \
  ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)
/* Maximum digits */
#define MAX_NN_DIGIT 0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

/* Macros.
 */
#define LOW_HALF(x) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x) (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((NN_DIGIT)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x) (unsigned int)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (unsigned int)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero (a, digits); a[0] = b;}
#define NN_EQUAL(a, b, digits) (! NN_Cmp (a, b, digits))
#define NN_EVEN(a, digits) (((digits) == 0) || ! (a[0] & 1))

/* RSA public and private key.
 */
typedef struct R_RSA_PUBLIC_KEY{
  unsigned int bits;                           /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN+1];  /* modulus */
  unsigned char exponent[MAX_RSA_MODULUS_LEN+1]; /* public exponent */
} R_RSA_PUBLIC_KEY_t;

typedef struct AosRsaPrivateKey 
{
  unsigned int bits;                           			/* leng in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN+1];           /* modulus */
  unsigned char publicExponent[MAX_RSA_MODULUS_LEN+1];    /* public exponent */
  unsigned char privateExponent[MAX_RSA_MODULUS_LEN+1];   /* private exponent */
  unsigned char prime[2][MAX_RSA_PRIME_LEN+1];            /* prime factors */
  unsigned char primeExponent[2][MAX_RSA_PRIME_LEN+1];    /* exponents for CRT */
  unsigned char coefficient[MAX_RSA_PRIME_LEN+1];         /* CRT coefficient */
} AosRsaPrivateKey_t;



struct rsa_pubkey;
int RSAPublicEncrypt1(unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					struct rsa_pubkey *publicKey);
int RSAPublicEncrypt(unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					struct R_RSA_PUBLIC_KEY *publicKey);
int RSAPublicDecrypt (unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					struct R_RSA_PUBLIC_KEY *publicKey);
int RSAPrivateEncrypt (unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					AosRsaPrivateKey_t *privateKey);
int RSAPrivateDecrypt (unsigned char *output, 
						unsigned int *outputLen, 
						unsigned char *input, 
						unsigned int inputLen, 
						AosRsaPrivateKey_t *privateKey);
int RSAPublicBlock (unsigned char *output, 
						unsigned int *outputLen, 
						unsigned char *input,
						unsigned int inputLen, 
						struct R_RSA_PUBLIC_KEY *publicKey);
int RSAPrivateBlock (unsigned char *output, 
						unsigned int *outputLen, 
						unsigned char *input, 
						unsigned int inputLen, 
						AosRsaPrivateKey_t *privateKey);
void R_memcpy (POINTER output, POINTER input, unsigned int len);
void R_memset (POINTER output, int value, unsigned int len);
void NN_Decode (NN_DIGIT *a, unsigned int digits, unsigned char *b, unsigned int len);
unsigned int NN_Digits (NN_DIGIT *a, unsigned int digits);
void NN_Mod (NN_DIGIT *a,NN_DIGIT * b, unsigned int bDigits, NN_DIGIT *c, unsigned int cDigits);
int NN_Cmp (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits);
void NN_Div (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits);
unsigned int NN_DigitBits (NN_DIGIT a);
void NN_DigitDiv (NN_DIGIT *a, NN_DIGIT b[2], NN_DIGIT c);
void NN_AssignZero (NN_DIGIT *a, unsigned int digits);
NN_DIGIT NN_RShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits);
NN_DIGIT NN_SubDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, unsigned int digits);
NN_DIGIT NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits);
void NN_DigitMult (NN_DIGIT a[2], NN_DIGIT b, NN_DIGIT c);
NN_DIGIT NN_LShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits);
void NN_Assign (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits);
void NN_ModExp (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits);
void NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits);
NN_DIGIT NN_AddDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, unsigned int digits);
void NN_Encode (unsigned char *a, unsigned int len, NN_DIGIT *b, unsigned int digits);
NN_DIGIT NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits);
void NN_ModMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, unsigned int digits);


#endif





