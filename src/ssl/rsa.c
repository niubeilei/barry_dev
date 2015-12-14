////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: rsa.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
//aosSslProxy RSA arth.
#if KERNEL
#include <linux/kernel.h>
#include <linux/string.h>
#else
#include <stdio.h>
#endif

#include "aosUtil/Tracer.h"
#include "rsa.h"
#include "PKCS/x509.h"
#include "ssl/ReturnCode.h"
#include "aosUtil/Alarm.h"


/* RSA public-key encryption, according to PKCS #1.
 */
int RSAPublicEncrypt(unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					struct R_RSA_PUBLIC_KEY *publicKey)
{
  	int status;
  	unsigned char byte;
  	//unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
  	unsigned char *pkcsBlock = NULL;
  	unsigned int i, modulusLen;
  
  	pkcsBlock = (unsigned char*)aos_malloc_atomic(MAX_RSA_MODULUS_LEN);
  	if(!pkcsBlock)
  	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA pkcsblock memory alloc failed");
  	}
	
  	modulusLen = (publicKey->bits + 7) / 8;
  	if (inputLen + 11 > modulusLen)
  	{
		aos_free(pkcsBlock);
	  	return (RE_LEN);
	}
  
 	 pkcsBlock[0] = 0;
  	// block type 2 
  	pkcsBlock[1] = 2;

  	for (i = 2; i < modulusLen - inputLen - 1; i++) {
    	// Find nonzero random byte.
     	
 		// do {
      	//	R_GenerateBytes (&byte, 1, randomStruct);
    	// } while (byte == 0);
		//
		byte=(unsigned char)0xff;
    	pkcsBlock[i] = byte;
  	}
 
  	// separator 
  	pkcsBlock[i++] = 0;
  
  	R_memcpy ((POINTER)&pkcsBlock[i], (POINTER)input, inputLen);
  
 	status = RSAPublicBlock(output, outputLen, pkcsBlock, modulusLen, publicKey);
  
  	// Zeroize sensitive information.
   
  	byte = 0;
  	//R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));
  	
	aos_free(pkcsBlock);
  	
	return (status);
}


int RSAPublicEncrypt1(unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					struct rsa_pubkey *publicKey)
{
	struct R_RSA_PUBLIC_KEY key;
	char exponent[128];
	memset(exponent, 0, 128);
	exponent[125] = 1;
	exponent[127] = 1;
	
    key.bits = (publicKey->modulus.len << 3);
	memcpy(key.modulus, publicKey->modulus.data, MAX_RSA_MODULUS_LEN);
    // memcpy(key.exponent, publicKey->public_exponent.data, MAX_RSA_MODULUS_LEN);
    memcpy(key.exponent, exponent, MAX_RSA_MODULUS_LEN);
	
	return RSAPublicEncrypt(output, outputLen, input, inputLen, &key);
}
	

// RSA public-key decryption, according to PKCS #1.
int RSAPublicDecrypt (unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					struct R_RSA_PUBLIC_KEY *publicKey)
{
  	int status;
  	//unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
	unsigned char *pkcsBlock = NULL;
  	unsigned int i, modulusLen, pkcsBlockLen;
  
	pkcsBlock = (unsigned char*)aos_malloc_atomic(MAX_RSA_MODULUS_LEN);
  	if(!pkcsBlock)
  	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA pkcsblock memory alloc failed");
  	}

  	modulusLen = (publicKey->bits + 7) / 8;
  	if (inputLen > modulusLen)
   	{
		aos_free(pkcsBlock);
		return (RE_LEN);
	}
  	status=RSAPublicBlock(pkcsBlock, &pkcsBlockLen, input, inputLen, publicKey);
  	if (status)
    {
		aos_free(pkcsBlock);
		return (status);
	}
  
  	if (pkcsBlockLen != modulusLen)
    {
		aos_free(pkcsBlock);
		return (RE_LEN);
	}
  
  	// Require block type 1.
  	if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 1))
   	{
		aos_free(pkcsBlock);
		return (RE_DATA);
	}

  	for (i = 2; i < modulusLen-1; i++)
    	if (pkcsBlock[i] != 0xff)
      		break;
    
  	/* separator */
  	if (pkcsBlock[i++] != 0)
    {
		aos_free(pkcsBlock);
		return (RE_DATA);
	}
  
  	*outputLen = modulusLen - i;
  
  	if (*outputLen + 11 > modulusLen)
    {
		aos_free(pkcsBlock);
		return (RE_DATA);
	}

  	R_memcpy ((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);
  
  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));
  
	aos_free(pkcsBlock);
  	return (0);
}



// RSA private-key encryption, according to PKCS #1.
int RSAPrivateEncrypt (unsigned char *output, 
					unsigned int *outputLen, 
					unsigned char *input, 
					unsigned int inputLen, 
					AosRsaPrivateKey_t *privateKey)
{
  	int status;
  	//unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
  	unsigned char *pkcsBlock = NULL;
  	unsigned int i, modulusLen;
  
	pkcsBlock = (unsigned char*)aos_malloc_atomic(MAX_RSA_MODULUS_LEN);
  	if(!pkcsBlock)
  	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA pkcsblock memory alloc failed");
  	}

  	modulusLen = (privateKey->bits + 7) / 8;
  	if (inputLen + 11 > modulusLen)
    {
		
		aos_free(pkcsBlock);
		return (RE_LEN);
  	}
  	pkcsBlock[0] = 0;
  	// block type 1 
  	pkcsBlock[1] = 1;

  	for (i = 2; i < modulusLen - inputLen - 1; i++)
    	pkcsBlock[i] = 0xff;

  	// separator
  	pkcsBlock[i++] = 0;
  
  	R_memcpy ((POINTER)&pkcsBlock[i], (POINTER)input, inputLen);
 	// aos_trace_hex("pkcsBlock", (char*)pkcsBlock, modulusLen); 
  	status = RSAPrivateBlock(output, outputLen, pkcsBlock, modulusLen, privateKey);
 	// aos_trace_hex("output", (char*)output, *outputLen);

  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));

	aos_free(pkcsBlock);

  	return (status);
}


// RSA private-key decryption, according to PKCS #1.
int RSAPrivateDecrypt (unsigned char *output, 
						unsigned int *outputLen, 
						unsigned char *input, 
						unsigned int inputLen, 
						AosRsaPrivateKey_t *privateKey)
{
  	int status;
  	unsigned char *pkcsBlock = NULL;
  	unsigned int i, modulusLen, pkcsBlockLen;
	
	pkcsBlock = (unsigned char*)aos_malloc_atomic(MAX_RSA_MODULUS_LEN);
  	if(!pkcsBlock)
  	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA pkcsblock memory alloc failed");
  	}

  	modulusLen = (privateKey->bits + 7) / 8;

  	if (inputLen > modulusLen)
    {
		aos_free(pkcsBlock);
		return (RE_LEN);
	}
  	status=RSAPrivateBlock(pkcsBlock, &pkcsBlockLen, input, inputLen, privateKey);
  	if (status)
    {
		aos_free(pkcsBlock);
		return (status);
	}
  
  	if (pkcsBlockLen != modulusLen)
    {
		aos_free(pkcsBlock);
		return (RE_LEN);
	}
  
  	// Require block type 2.
  	if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 2))
   	{
		aos_free(pkcsBlock);
		return (RE_DATA);
	}

  	for (i = 2; i < modulusLen-1; i++)
    	// separator 
    	if (pkcsBlock[i] == 0)
      		break;
    
  	i++;
  	if (i >= modulusLen)
    {
		aos_free(pkcsBlock);
		return (RE_DATA);
	}
    
  	*outputLen = modulusLen - i;
  
  	if (*outputLen + 11 > modulusLen)
    {
		aos_free(pkcsBlock);
		return (RE_DATA);
	}

  	R_memcpy ((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);
  
  	// Zeroize sensitive information.
  	//R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));
	
	aos_free(pkcsBlock);
  	
	return (0);
}


// Raw RSA public-key operation. Output has same length as modulus.
//
//   Assumes inputLen < length of modulus.
//   Requires input < modulus.
// 
int RSAPublicBlock (unsigned char *output, 
						unsigned int *outputLen, 
						unsigned char *input,
						unsigned int inputLen, 
						struct R_RSA_PUBLIC_KEY *publicKey)
{
  	//NN_DIGIT c[MAX_NN_DIGITS], e[MAX_NN_DIGITS], m[MAX_NN_DIGITS],
  	//  n[MAX_NN_DIGITS];
  	NN_DIGIT *c = NULL, *e = NULL, *m = NULL, *n = NULL;
  	unsigned int eDigits, nDigits;

	c = (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!c)
  	{
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA c memory alloc failed");
  	}
	e = (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!e)
  	{
		aos_free(c);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA e memory alloc failed");
  	}
	m = (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!m)
  	{
		aos_free(c);
		aos_free(e);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA m memory alloc failed");
  	}
	n = (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!n)
  	{
		aos_free(c);
		aos_free(e);
		aos_free(m);
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA n memory alloc failed");
  	}

  	NN_Decode (m, MAX_NN_DIGITS, input, inputLen);
 	NN_Decode (n, MAX_NN_DIGITS, publicKey->modulus, MAX_RSA_MODULUS_LEN);
  	NN_Decode (e, MAX_NN_DIGITS, publicKey->exponent, MAX_RSA_MODULUS_LEN);
  	nDigits = NN_Digits (n, MAX_NN_DIGITS);
  	eDigits = NN_Digits (e, MAX_NN_DIGITS);
  
  	if (NN_Cmp (m, n, nDigits) >= 0)
    {
		aos_free(c);
		aos_free(e);
		aos_free(m);
		aos_free(n);
		return (RE_DATA);
 	} 
  	// Compute c = m^e mod n.
  	NN_ModExp (c, m, e, eDigits, n, nDigits);

  	*outputLen = (publicKey->bits + 7) / 8;
  	NN_Encode (output, *outputLen, c, nDigits);
  
  	// Zeroize sensitive information.
  	//R_memset ((POINTER)c, 0, sizeof (c));
  	//R_memset ((POINTER)m, 0, sizeof (m));

	aos_free(c);
	aos_free(e);
	aos_free(m);
	aos_free(n);
  	
	return (0);
}


// Raw RSA private-key operation. Output has same length as modulus.
//
// Assumes inputLen < length of modulus.
// Requires input < modulus.
//
int RSAPrivateBlock (unsigned char *output, 
						unsigned int *outputLen, 
						unsigned char *input, 
						unsigned int inputLen, 
						AosRsaPrivateKey_t *privateKey)
{
  	//NN_DIGIT c[MAX_NN_DIGITS], cP[MAX_NN_DIGITS], cQ[MAX_NN_DIGITS],
    //	dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS], mP[MAX_NN_DIGITS],
    //	mQ[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], q[MAX_NN_DIGITS],
    //	qInv[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
  	NN_DIGIT *c = NULL, *cP = NULL, *cQ = NULL, *dP = NULL, *dQ = NULL, *mP = NULL,
    		 *mQ = NULL, *n = NULL, *p = NULL, *q = NULL, *qInv = NULL, *t = NULL;
  	unsigned int cDigits, nDigits, pDigits;
	int ret = 0;
  
	c = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	cP = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	cQ = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	dP = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	dQ = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	mP = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	mQ = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	n = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	p = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	q = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	qInv =  (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
	t = 	(NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!c || !cP || !cQ || !dP || !dQ || !mP || !mQ || !n || !p || !q || !qInv || !t)
  	{
		ret = -1;
		aos_trace("memory alloc error");
		goto alloc_err;
  	}

  	NN_Decode (c, MAX_NN_DIGITS, input, inputLen);
  	NN_Decode (n, MAX_NN_DIGITS, privateKey->modulus, MAX_RSA_MODULUS_LEN);
  	NN_Decode (p, MAX_NN_DIGITS, privateKey->prime[0], MAX_RSA_PRIME_LEN);
  	NN_Decode (q, MAX_NN_DIGITS, privateKey->prime[1], MAX_RSA_PRIME_LEN);
  	NN_Decode (dP, MAX_NN_DIGITS, privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
  	NN_Decode (dQ, MAX_NN_DIGITS, privateKey->primeExponent[1], MAX_RSA_PRIME_LEN);
  	NN_Decode (qInv, MAX_NN_DIGITS, privateKey->coefficient, MAX_RSA_PRIME_LEN);
  	cDigits = NN_Digits (c, MAX_NN_DIGITS);
  	nDigits = NN_Digits (n, MAX_NN_DIGITS);
  	pDigits = NN_Digits (p, MAX_NN_DIGITS);

  	if (NN_Cmp (c, n, nDigits) >= 0)
    {
		ret = RE_DATA;
		goto alloc_err;
	}
  
  	// Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has
    // length at most pDigits, i.e., p > q.)
  	NN_Mod (cP, c, cDigits, p, pDigits);
  	NN_Mod (cQ, c, cDigits, q, pDigits);
  	NN_ModExp (mP, cP, dP, pDigits, p, pDigits);
  	NN_AssignZero (mQ, nDigits);
  	NN_ModExp (mQ, cQ, dQ, pDigits, q, pDigits);
  
  	// Chinese Remainder Theorem:
    // m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.
  	if (NN_Cmp (mP, mQ, pDigits) >= 0)
    	NN_Sub (t, mP, mQ, pDigits);
  	else {
    	NN_Sub (t, mQ, mP, pDigits);
    	NN_Sub (t, p, t, pDigits);
  	}
  	NN_ModMult (t, t, qInv, p, pDigits);
  	NN_Mult (t, t, q, pDigits);
  	NN_Add (t, t, mQ, nDigits);

  	*outputLen = (privateKey->bits + 7) / 8;
  	NN_Encode (output, *outputLen, t, nDigits);

  	// Zeroize sensitive information.
  /*	R_memset ((POINTER)c, 0, sizeof (c));
  	R_memset ((POINTER)cP, 0, sizeof (cP));
  	R_memset ((POINTER)cQ, 0, sizeof (cQ));
  	R_memset ((POINTER)dP, 0, sizeof (dP));
  	R_memset ((POINTER)dQ, 0, sizeof (dQ));
  	R_memset ((POINTER)mP, 0, sizeof (mP));
  	R_memset ((POINTER)mQ, 0, sizeof (mQ));
  	R_memset ((POINTER)p, 0, sizeof (p));
  	R_memset ((POINTER)q, 0, sizeof (q));
  	R_memset ((POINTER)qInv, 0, sizeof (qInv));
  	R_memset ((POINTER)t, 0, sizeof (t));
*/
alloc_err:

	if(c)	aos_free(c);
	if(cP)	aos_free(cP);
	if(cQ)	aos_free(cQ);
	if(dP)	aos_free(dP);
	if(dQ)	aos_free(dQ);
	if(mP)	aos_free(mP);
	if(mQ)	aos_free(mQ);
	if(n)	aos_free(n);
	if(p)	aos_free(p);
	if(q)	aos_free(q);
	if(qInv)	aos_free(qInv);
	if(t)	aos_free(t);
	if( ret == -1 )
		return aos_alarm(eAosMD_SSL, eAosAlarm_SSLProcErr, "RSA memory alloc failed");
  	return (ret);
}

void R_memcpy (POINTER output, POINTER input, unsigned int len)
{
  if (len)
    memcpy (output, input, len);
}

void R_memset (POINTER output, int value, unsigned int len)
{
  //if (len)
  //  memset (output, value, len);
}


/* Decodes character string b into a, where character string is ordered
   from most to least significant.

   Lengths: a[digits], b[len].
   Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
   significant bytes are truncated.)
 */
void NN_Decode (NN_DIGIT *a, unsigned int digits, unsigned char *b, unsigned int len)
{
  NN_DIGIT t;
  int j;
  unsigned int i, u;
  
  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = 0;
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
      t |= ((NN_DIGIT)b[j]) << u;
    a[i] = t;
  }
  
  for (; i < digits; i++)
    a[i] = 0;
}


/* Returns the significant length of a in digits.

   Lengths: a[digits].
 */
unsigned int NN_Digits (NN_DIGIT *a, unsigned int digits)
{
  int i;
  
  for (i = digits - 1; i >= 0; i--)
    if (a[i])
      break;

  return (i + 1);
}


// Computes a = b mod c.
//
// Lengths: a[cDigits], b[bDigits], c[cDigits].
// Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
//
void NN_Mod (NN_DIGIT *a,NN_DIGIT * b, unsigned int bDigits, NN_DIGIT *c, unsigned int cDigits)
{
  	//NN_DIGIT t[2 * MAX_NN_DIGITS];
  	NN_DIGIT *t = NULL;
	
	t = (NN_DIGIT*) aos_malloc_atomic(2*MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!t)
  	{
		aos_trace("RSA t memory alloc failed");
		return;
  	}
  
  	NN_Div (t, a, b, bDigits, c, cDigits);
  
  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)t, 0, sizeof (t));

	aos_free(t);	
}

/* Returns sign of a - b.

   Lengths: a[digits], b[digits].
 */
int NN_Cmp (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits)
{
  int i;
  
  for (i = digits - 1; i >= 0; i--) {
    if (a[i] > b[i])
      return (1);
    if (a[i] < b[i])
      return (-1);
  }

  return (0);
}


// Computes a = c div d and b = c mod d.
//
// Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
// Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
//         dDigits < MAX_NN_DIGITS.
//
void NN_Div (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits)
{
  	// NN_DIGIT ai, cc[2*MAX_NN_DIGITS+1], dd[MAX_NN_DIGITS], t;
  	NN_DIGIT ai, t;
  	NN_DIGIT *cc = NULL, *dd = NULL;
  	int i;
  	unsigned int ddDigits, shift;
  
	cc = (NN_DIGIT*) aos_malloc_atomic((2*MAX_NN_DIGITS+1)*sizeof(NN_DIGIT));
  	if (!cc)
  	{
		aos_trace("RSA cc memory alloc failed");
		return;
	}
	dd = (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!dd)
  	{
		aos_free(cc);
		aos_trace("RSA dd memory alloc failed");
		return;
	}

  	ddDigits = NN_Digits (d, dDigits);
  	if (ddDigits == 0)
    {
		aos_free(cc);
		aos_free(dd);
		return;
 	} 
  	// Normalize operands.
  	shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
  	NN_AssignZero (cc, ddDigits);
  	cc[cDigits] = NN_LShift (cc, c, shift, cDigits);
  	NN_LShift (dd, d, shift, ddDigits);
  	t = dd[ddDigits-1];
  
  	NN_AssignZero (a, cDigits);

  	for (i = cDigits-ddDigits; i >= 0; i--) {
    	// Underestimate quotient digit and subtract.
    	if (t == MAX_NN_DIGIT)
      		ai = cc[i+ddDigits];
    	else
      		NN_DigitDiv (&ai, &cc[i+ddDigits-1], t + 1);
    	cc[i+ddDigits] -= NN_SubDigitMult (&cc[i], &cc[i], ai, dd, ddDigits);

    	// Correct estimate.
    	while (cc[i+ddDigits] || (NN_Cmp (&cc[i], dd, ddDigits) >= 0)) {
      		ai++;
      		cc[i+ddDigits] -= NN_Sub (&cc[i], &cc[i], dd, ddDigits);
    	}
    
    	a[i] = ai;
  	}
  
  	// Restore result.
  	NN_AssignZero (b, dDigits);
  	NN_RShift (b, cc, shift, ddDigits);

  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)cc, 0, sizeof (cc));
  	//R_memset ((POINTER)dd, 0, sizeof (dd));
	
	aos_free(cc);
	aos_free(dd);
}


/* Returns the significant length of a in bits, where a is a digit.
 */
unsigned int NN_DigitBits (NN_DIGIT a)
{
  unsigned int i;
  
  for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
    if (a == 0)
      break;
    
  return (i);
}


/* Sets a = b / c, where a and c are digits.

   Lengths: b[2].
   Assumes b[1] < c and HIGH_HALF (c) > 0. For efficiency, c should be
   normalized.
 */
void NN_DigitDiv (NN_DIGIT *a, NN_DIGIT b[2], NN_DIGIT c)
{
  NN_DIGIT t[2], u, v;
  NN_HALF_DIGIT aHigh, aLow, cHigh, cLow;

  cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
  cLow = (NN_HALF_DIGIT)LOW_HALF (c);

  t[0] = b[0];
  t[1] = b[1];

  /* Underestimate high half of quotient and subtract.
   */
  if (cHigh == MAX_NN_HALF_DIGIT)
    aHigh = (NN_HALF_DIGIT)HIGH_HALF (t[1]);
  else
    aHigh = (NN_HALF_DIGIT)(t[1] / (cHigh + 1));
  u = (NN_DIGIT)aHigh * (NN_DIGIT)cLow;
  v = (NN_DIGIT)aHigh * (NN_DIGIT)cHigh;
  if ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
    t[1]--;
  t[1] -= HIGH_HALF (u);
  t[1] -= v;

  /* Correct estimate.
   */
  while ((t[1] > cHigh) ||
         ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
    if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow))
      t[1]--;
    t[1] -= cHigh;
    aHigh++;
  }

  /* Underestimate low half of quotient and subtract.
   */
  if (cHigh == MAX_NN_HALF_DIGIT)
    aLow = (NN_HALF_DIGIT)LOW_HALF (t[1]);
  else
    aLow = 
      (NN_HALF_DIGIT)((TO_HIGH_HALF (t[1]) + HIGH_HALF (t[0])) / (cHigh + 1));
  u = (NN_DIGIT)aLow * (NN_DIGIT)cLow;
  v = (NN_DIGIT)aLow * (NN_DIGIT)cHigh;
  if ((t[0] -= u) > (MAX_NN_DIGIT - u))
    t[1]--;
  if ((t[0] -= TO_HIGH_HALF (v)) > (MAX_NN_DIGIT - TO_HIGH_HALF (v)))
    t[1]--;
  t[1] -= HIGH_HALF (v);

  /* Correct estimate.
   */
  while ((t[1] > 0) || ((t[1] == 0) && t[0] >= c)) {
    if ((t[0] -= c) > (MAX_NN_DIGIT - c))
      t[1]--;
    aLow++;
  }
  
  *a = TO_HIGH_HALF (aHigh) + aLow;
}


/* Assigns a = 0.

   Lengths: a[digits].
 */
void NN_AssignZero (NN_DIGIT *a, unsigned int digits)
{
  unsigned int i;

  for (i = 0; i < digits; i++)
    a[i] = 0;
}


/* Computes a = c div 2^c (i.e., shifts right c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires: c < NN_DIGIT_BITS.
 */
NN_DIGIT NN_RShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits)
{
  NN_DIGIT bi, carry;
  int i;
  unsigned int t;
  
  if (c >= NN_DIGIT_BITS)
    return (0);
  
  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = digits - 1; i >= 0; i--) {
    bi = b[i];
    a[i] = (bi >> c) | carry;
    carry = c ? (bi << t) : 0;
  }
  
  return (carry);
}


/* Computes a = b - c*d, where c is a digit. Returns borrow.

   Lengths: a[digits], b[digits], d[digits].
 */
NN_DIGIT NN_SubDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, unsigned int digits)
{
  NN_DIGIT borrow, t[2];
  unsigned int i;

  if (c == 0)
    return (0);

  borrow = 0;
  for (i = 0; i < digits; i++) {
    NN_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
      borrow = 1;
    else
      borrow = 0;
    if ((a[i] -= t[0]) > (MAX_NN_DIGIT - t[0]))
      borrow++;
    borrow += t[1];
  }
  
  return (borrow);
}


/* Computes a = b * c, where b and c are digits.

   Lengths: a[2].
 */
void NN_DigitMult (NN_DIGIT a[2], NN_DIGIT b, NN_DIGIT c)
{
  NN_DIGIT t, u;
  NN_HALF_DIGIT bHigh, bLow, cHigh, cLow;

  bHigh = (NN_HALF_DIGIT)HIGH_HALF (b);
  bLow = (NN_HALF_DIGIT)LOW_HALF (b);
  cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
  cLow = (NN_HALF_DIGIT)LOW_HALF (c);

  a[0] = (NN_DIGIT)bLow * (NN_DIGIT)cLow;
  t = (NN_DIGIT)bLow * (NN_DIGIT)cHigh;
  u = (NN_DIGIT)bHigh * (NN_DIGIT)cLow;
  a[1] = (NN_DIGIT)bHigh * (NN_DIGIT)cHigh;
  
  if ((t += u) < u)
    a[1] += TO_HIGH_HALF (1);
  u = TO_HIGH_HALF (t);
  
  if ((a[0] += u) < u)
    a[1]++;
  a[1] += HIGH_HALF (t);
}


/* Computes a = b - c. Returns borrow.

   Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT NN_Sub (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
  NN_DIGIT ai, borrow;
  unsigned int i;

  borrow = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
      ai = MAX_NN_DIGIT - c[i];
    else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
      borrow = 1;
    else
      borrow = 0;
    a[i] = ai;
  }

  return (borrow);
}


/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires c < NN_DIGIT_BITS.
 */
NN_DIGIT NN_LShift (NN_DIGIT *a, NN_DIGIT *b, unsigned int c, unsigned int digits)
{
  NN_DIGIT bi, carry;
  unsigned int i, t;
  
  if (c >= NN_DIGIT_BITS)
    return (0);
  
  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = 0; i < digits; i++) {
    bi = b[i];
    a[i] = (bi << c) | carry;
    carry = c ? (bi >> t) : 0;
  }
  
  return (carry);
}

void NN_ModExp (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d, unsigned int dDigits)
{
  	//NN_DIGIT bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
  	NN_DIGIT ci;
  	NN_DIGIT *bPower = NULL, *t = NULL;
  	int i;
  	unsigned int ciBits, j, s;

	bPower = (NN_DIGIT*) aos_malloc_atomic(3*MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!bPower)
  	{
		aos_trace("RSA bPower memory alloc failed");
		return;
	}
	t = (NN_DIGIT*) aos_malloc_atomic(MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!t)
  	{
		aos_free(bPower);
		aos_trace("RSA t memory alloc failed");
		return;
	}
  	
	// Store b, b^2 mod d, and b^3 mod d.
  	//NN_Assign (bPower[0], b, dDigits);
  	//NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
  	//NN_ModMult (bPower[2], bPower[1], b, d, dDigits);
  	
	NN_Assign (bPower, b, dDigits);
  	NN_ModMult (bPower+MAX_NN_DIGITS, bPower, b, d, dDigits);
  	NN_ModMult (bPower+2*MAX_NN_DIGITS, bPower+MAX_NN_DIGITS, b, d, dDigits);
  
  	NN_ASSIGN_DIGIT (t, 1, dDigits);

  	cDigits = NN_Digits (c, cDigits);
  	for (i = cDigits - 1; i >= 0; i--) {
    	ci = c[i];
    	ciBits = NN_DIGIT_BITS;
    
    	// Scan past leading zero bits of most significant digit.
    	if (i == (int)(cDigits - 1)) {
      		while (! DIGIT_2MSB (ci)) {
        		ci <<= 2;
        		ciBits -= 2;
      		}
    	}

    	for (j = 0; j < ciBits; j += 2, ci <<= 2) {
      		// Compute t = t^4 * b^s mod d, where s = two MSB's of ci.
      		NN_ModMult (t, t, t, d, dDigits);
      		NN_ModMult (t, t, t, d, dDigits);
      		if ((s = DIGIT_2MSB (ci)) != 0)
        		NN_ModMult (t, t, bPower+(s-1)*MAX_NN_DIGITS, d, dDigits);
    	}
  	}
  
  	NN_Assign (a, t, dDigits);
  
  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)bPower, 0, sizeof (bPower));
  	//R_memset ((POINTER)t, 0, sizeof (t));
	
	aos_free(bPower);
	aos_free(t);
}


/* Assigns a = b.

   Lengths: a[digits], b[digits].
 */
void NN_Assign (NN_DIGIT *a, NN_DIGIT *b, unsigned int digits)
{
  unsigned int i;

  for (i = 0; i < digits; i++)
    a[i] = b[i];
}


// Computes a = b * c mod d.
//
// Lengths: a[digits], b[digits], c[digits], d[digits].
// Assumes d > 0, digits < MAX_NN_DIGITS.
//
void NN_ModMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d, unsigned int digits)
{
  	//NN_DIGIT t[2*MAX_NN_DIGITS];
  	NN_DIGIT *t = NULL;

	t = (NN_DIGIT*) aos_malloc_atomic(2*MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!t)
  	{
		aos_trace("RSA t memory alloc failed");
		return;
	}
	
  	NN_Mult (t, b, c, digits);
  	NN_Mod (a, t, 2 * digits, d, digits);
  
  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)t, 0, sizeof (t));

	aos_free(t);
}


// Computes a = b * c.
//
// Lengths: a[2*digits], b[digits], c[digits].
// Assumes digits < MAX_NN_DIGITS.
//
void NN_Mult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
  	//NN_DIGIT t[2*MAX_NN_DIGITS];
  	NN_DIGIT *t = NULL;
  	unsigned int bDigits, cDigits, i;

	t = (NN_DIGIT*) aos_malloc_atomic(2*MAX_NN_DIGITS*sizeof(NN_DIGIT));
  	if (!t)
  	{
		aos_trace("RSA t memory alloc failed");
		return;
	}

  	NN_AssignZero (t, 2 * digits);
  
  	bDigits = NN_Digits (b, digits);
  	cDigits = NN_Digits (c, digits);

  	for (i = 0; i < bDigits; i++)
    	t[i+cDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], c, cDigits);
  
  	NN_Assign (a, t, 2 * digits);
  
  	// Zeroize potentially sensitive information.
  	//R_memset ((POINTER)t, 0, sizeof (t));

	aos_free(t);
}


/* Computes a = b + c*d, where c is a digit. Returns carry.

   Lengths: a[digits], b[digits], d[digits].
 */
NN_DIGIT NN_AddDigitMult (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d, unsigned int digits)
{
  NN_DIGIT carry, t[2];
  unsigned int i;

  if (c == 0)
    return (0);

  carry = 0;
  for (i = 0; i < digits; i++) {
    NN_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] + carry) < carry)
      carry = 1;
    else
      carry = 0;
    if ((a[i] += t[0]) < t[0])
      carry++;
    carry += t[1];
  }
  
  return (carry);
}


/* Computes a = b + c. Returns carry.

   Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT NN_Add (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int digits)
{
  NN_DIGIT ai, carry;
  unsigned int i;

  carry = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] + carry) < carry)
      ai = c[i];
    else if ((ai += c[i]) < c[i])
      carry = 1;
    else
      carry = 0;
    a[i] = ai;
  }

  return (carry);
}


/* Encodes b into character string a, where character string is ordered
   from most to least significant.

   Lengths: a[len], b[digits].
   Assumes NN_Bits (b, digits) <= 8 * len. (Otherwise most significant
   digits are truncated.)
 */
void NN_Encode (unsigned char *a, unsigned int len, NN_DIGIT *b, unsigned int digits)
{
  NN_DIGIT t;
  int j;
  unsigned int i, u;

  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = b[i];
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
      a[j] = (unsigned char)(t >> u);
  }

  for (; j >= 0; j--)
    a[j] = 0;
}



