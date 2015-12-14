////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: random.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SSL_RANDOM_HEAD_H
#define AOS_SSL_RANDOM_HEAD_H

#define RANDOM_BYTES_NEEDED 256


 /* Random structure.
 */
typedef struct {
  unsigned int bytesNeeded;
  unsigned char state[16];
  unsigned int outputAvailable;
  unsigned char output[16];
} R_RANDOM_STRUCT;

 int R_RandomInit (R_RANDOM_STRUCT *randomStruct);
 int R_RandomUpdate (R_RANDOM_STRUCT *randomStruct, unsigned char *block, unsigned int blockLen);
 int R_GetRandomBytesNeeded (unsigned int *bytesNeeded, R_RANDOM_STRUCT *randomStruct);
 void R_RandomFinal (R_RANDOM_STRUCT *randomStruct);
void InitRandomStruct (R_RANDOM_STRUCT *randomStruct);
 int R_GenerateBytes (unsigned char *block, unsigned int blockLen, R_RANDOM_STRUCT *randomStruct);
#endif
