////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: des.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SSL_DES_HEAD_H
#define AOS_SSL_DES_HEAD_H

typedef struct {
  unsigned int subkeys[32];                                             /* subkeys */
  unsigned int iv[2];                                       /* initializing vector */
  unsigned int originalIV[2];                        /* for restarting the context */
  int encrypt;                                               /* encrypt flag */
} DES_CBC_CTX;

typedef struct {
  unsigned int subkeys[3][32];                     /* subkeys for three operations */
  unsigned int iv[2];                                       /* initializing vector */
  unsigned int originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DES3_CBC_CTX;


void DES_CBCInit (DES_CBC_CTX *context, unsigned char key[8], unsigned char iv[8], int encrypt);
int DES_CBCUpdate (DES_CBC_CTX *context, 
					unsigned char *output, 
					unsigned char *input, 
					unsigned int len);
void DES_CBCRestart (DES_CBC_CTX *context);
void DES3_CBCInit(DES3_CBC_CTX *context, unsigned char key[24], unsigned char iv[8], int encrypt);
int DES3_CBCUpdate (DES3_CBC_CTX *context, unsigned char *output, unsigned char *input, unsigned int len);
void DES3_CBCRestart (DES3_CBC_CTX *context);
void Pack (unsigned int *into, unsigned char *outof);
void Unpack (unsigned char *into, unsigned int *outof);
void DESKey (unsigned int subkeys[32], unsigned char key[8], int encrypt);
void DESFunction (unsigned int *block, unsigned int *subkeys);
void CookKey (unsigned int *subkeys, unsigned int *kn, int encrypt);


#endif
