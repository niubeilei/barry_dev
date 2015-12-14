////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: random.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if KERNEL
#include <linux/string.h>
#else
#include <string.h>
#include <stdio.h>
#endif

#include "ssl2/md5.h"
#include "ssl2/random.h"
#include "ssl2/rsa.h"



 int R_RandomInit (R_RANDOM_STRUCT *randomStruct)
{
  randomStruct->bytesNeeded = RANDOM_BYTES_NEEDED;
  R_memset ((POINTER)randomStruct->state, 0, sizeof (randomStruct->state));
  randomStruct->outputAvailable = 0;
  
  return (0);
}

int R_RandomUpdate (R_RANDOM_STRUCT *randomStruct, unsigned char *block, unsigned int blockLen)
{
  struct MD5Context context;
  unsigned char digest[16];
  unsigned int i, x;
  
  AosSsl_md5Init (&context);
  AosSsl_md5Update (&context, block, blockLen);
  AosSsl_md5Final (&context, digest);

  /* add digest to state */
  x = 0;
  for (i = 0; i < 16; i++) {
    x += randomStruct->state[15-i] + digest[15-i];
    randomStruct->state[15-i] = (unsigned char)x;
    x >>= 8;
  }
  
  if (randomStruct->bytesNeeded < blockLen)
    randomStruct->bytesNeeded = 0;
  else
    randomStruct->bytesNeeded -= blockLen;
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)digest, 0, sizeof (digest));
  x = 0;
  
  return (0);
}

int R_GetRandomBytesNeeded (unsigned int *bytesNeeded, R_RANDOM_STRUCT *randomStruct)
{
  *bytesNeeded = randomStruct->bytesNeeded;
  
  return (0);
}

int R_GenerateBytes (unsigned char *block, unsigned int blockLen, R_RANDOM_STRUCT *randomStruct)
{
  struct MD5Context context;
  unsigned int available, i;
  
  if (randomStruct->bytesNeeded)
    return (RE_NEED_RANDOM);
  
  available = randomStruct->outputAvailable;
  
  while (blockLen > available) {
    R_memcpy
      ((POINTER)block, (POINTER)&randomStruct->output[16-available],
       available);
    block += available;
    blockLen -= available;

    /* generate new output */
    AosSsl_md5Init (&context);
    AosSsl_md5Update (&context, randomStruct->state, 16);
    AosSsl_md5Final (&context, randomStruct->output);
    available = 16;

    /* increment state */
    for (i = 0; i < 16; i++)
      if (randomStruct->state[15-i]++)
        break;
  }

  R_memcpy 
    ((POINTER)block, (POINTER)&randomStruct->output[16-available], blockLen);
  randomStruct->outputAvailable = available - blockLen;

  return (0);
}

void R_RandomFinal (R_RANDOM_STRUCT *randomStruct)
{
  R_memset ((POINTER)randomStruct, 0, sizeof (*randomStruct));
}

void InitRandomStruct (R_RANDOM_STRUCT *randomStruct)
{
  static unsigned char seedByte = 0;
  unsigned int bytesNeeded;
  
  R_RandomInit (randomStruct);
  
  /* Initialize with all zero seed bytes, which will not yield an actual
       random number output.
   */
  while (1) {
    R_GetRandomBytesNeeded (&bytesNeeded, randomStruct);
    if (bytesNeeded == 0)
      break;
    
    R_RandomUpdate (randomStruct, &seedByte, 1);
  }
}
