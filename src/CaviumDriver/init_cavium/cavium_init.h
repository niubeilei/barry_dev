////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_init.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef CAVIUM_INIT_HEAD_H
#define CAVIUM_INIT_HEAD_H

#include <linux/ioctl.h>

#define MICROCODE_MAX   2
#define MAX_INIT MICROCODE_MAX


#define VERSION_LEN 	32
#define SRAM_ADDRESS_LEN 	8

#ifndef ROUNDUP16
#define ROUNDUP16(val) (((val) + 15)&0xfffffff0)
#endif

typedef unsigned char		Uint8;
typedef unsigned short		Uint16;
typedef unsigned long		Uint32;
typedef unsigned long long	Uint64;

typedef struct 
{
   Uint8 size;         /* number of init buffers */
   Uint8 version_info[MAX_INIT][32];
   Uint32 code_length[MAX_INIT];
   Uint8* code[MAX_INIT];
   Uint32 data_length[MAX_INIT];
   Uint8 sram_address[MAX_INIT][8];
   Uint8* data[MAX_INIT];
   Uint8 signature[MAX_INIT][256];
#ifdef NPLUS
   Uint8 ucode_idx[MAX_INIT];
#endif /*NPLUS*/
} Csp1InitBuffer;
#define N1_MAGIC	0xC0
#define N1_INIT_CODE      14
#define IOCTL_N1_INIT_CODE		_IOWR(N1_MAGIC, N1_INIT_CODE, Csp1InitBuffer)


#endif
