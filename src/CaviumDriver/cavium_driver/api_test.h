////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: api_test.h
// Description:
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, 
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice, 
//    this list of conditions and the following disclaimer in the documentation 
//    and/or other materials provided with the distribution.
// 
// 3. All manuals, brochures, user guides mentioning features or use of this software, 
//    and the About menu item in the software application must display the following
//    acknowledgement:
//
//   This product includes software developed by Cavium Networks
// 
// 4. Cavium Networks' name may not be used to endorse or promote products 
//    derived from this software without specific prior written permission.
// 
// 5. User agrees to enable and utilize only the features and performance 
//    purchased on the target hardware.
// 
// This Software,including technical data,may be subject to U.S. export control 
// laws, including the U.S. Export Administration Act and its associated 
// regulations, and may be subject to export or import regulations in other 
// countries.You warrant that You will comply strictly in all respects with all 
// such regulations and acknowledge that you have the responsibility to obtain 
// licenses to export, re-export or import the Software.

// TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" AND 
// WITH ALL FAULTS AND CAVIUM MAKES NO PROMISES, REPRESENTATIONS OR WARRANTIES, 
// EITHER EXPRESS,IMPLIED,STATUTORY, OR OTHERWISE, WITH RESPECT TO THE SOFTWARE,
// INCLUDING ITS CONDITION,ITS CONFORMITY TO ANY REPRESENTATION OR DESCRIPTION, 
// OR THE EXISTENCE OF ANY LATENT OR PATENT DEFECTS, AND CAVIUM SPECIFICALLY 
// DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES OF TITLE, MERCHANTABILITY, 
// NONINFRINGEMENT,FITNESS FOR A PARTICULAR PURPOSE,LACK OF VIRUSES, ACCURACY OR
// COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR CORRESPONDENCE TO 
// DESCRIPTION. THE ENTIRE RISK ARISING OUT OF USE OR PERFORMANCE OF THE 
// SOFTWARE LIES WITH YOU.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "cavium_sysdep.h"
#include "cavium_common.h"
#include "cavium_list.h"
#include "cavium.h"

#ifndef _API_TEST_H_
#define _API_TEST_H_

/* RANDOM */
#define RANDOM_OP 0x0101
#define RANDOM_DATA_LEN 100

/* DES */
#define DES_OP 0x040e
#define DES_DATA_LEN 80

static unsigned char des3_key[24] =
                { 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf };
static unsigned char des3_iv[8] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 };
static unsigned char des3_data[DES_DATA_LEN] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
static unsigned char des3_cmp_data[DES_DATA_LEN] =
		{0x49 ,0x2d ,0xfd ,0x47 ,0x22 ,0x51 ,0xc7 ,0x13
		,0x6e ,0x4c ,0x2e ,0xec ,0xe3 ,0x2b ,0x81 ,0x65
		,0xff ,0xa4 ,0xdb ,0xba ,0x0c ,0x63 ,0xe1 ,0x4e
		,0xb6 ,0x28 ,0xbf ,0xc4 ,0x5a ,0x7a ,0x4f ,0x38
		,0x56 ,0x36 ,0x9e ,0xb5 ,0x91 ,0x91 ,0x56 ,0x96
		,0x8d ,0x0d ,0xbd ,0x8c ,0xa2 ,0x81 ,0x48 ,0xbf
		,0xf0 ,0xad ,0xd7 ,0x4b ,0x76 ,0xc5 ,0x5a ,0xe6
		,0x71 ,0x7a ,0xb8 ,0xc2 ,0x79 ,0x31 ,0x56 ,0x3a
		,0x17 ,0x90 ,0x6b ,0x9f ,0x57 ,0x65 ,0xb1 ,0x87
		,0xc7 ,0xbe ,0x55 ,0x3c ,0x97 ,0x7f ,0x77 ,0x55};

/* AES */
#define AES_OP 0x060e
#define AES_DATA_LEN 80

static unsigned char aes_key[32] =
                { 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb};
static unsigned char aes_iv[16] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 };
static unsigned char aes_data[AES_DATA_LEN] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
static unsigned char aes_cmp_data[AES_DATA_LEN] =
         {0x7f ,0x29 ,0x7a ,0x5d ,0x34 ,0x2a ,0xc7 ,0xaa
         ,0x9e ,0x21 ,0xea ,0x17 ,0xfc ,0xb9 ,0xa9 ,0x87
         ,0x4d ,0x97 ,0x7b ,0x56 ,0xde ,0x85 ,0x5a ,0x32
         ,0x20 ,0x5d ,0x18 ,0xcf ,0x6e ,0xd9 ,0xf5 ,0x9f
         ,0x3d ,0x86 ,0x4c ,0x96 ,0x55 ,0x59 ,0xfa ,0x7e
         ,0xd4 ,0xe2 ,0x2a ,0x47 ,0x57 ,0x0e ,0x14 ,0x48
         ,0xef ,0x66 ,0x76 ,0x2d ,0xb5 ,0xe3 ,0x3d ,0x90
         ,0xe4 ,0xad ,0x0c ,0xd1 ,0x49 ,0xde ,0xe4 ,0x2a
         ,0xc4 ,0x40 ,0xbf ,0xe1 ,0xfc ,0x2f ,0xb5 ,0x99
         ,0x46 ,0x24 ,0x92 ,0x29 ,0x1c ,0x3c ,0x3f ,0x98 };


/* RC4 */
#define RC4_INITIALIZE_OP 0x0901
#define RC4_OP 0x00e
#define RC4_KEY_LEN 32
#define RC4_DATA_LEN 80

static unsigned char rc4_key[RC4_KEY_LEN] =
                { 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
                0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0xa, 0xb };
static unsigned char rc4_data[RC4_DATA_LEN] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
static unsigned char rc4_cmp_data[RC4_DATA_LEN] =
         {0x97 ,0x10 ,0x0c ,0xeb ,0xf5 ,0x0f ,0x30 ,0x97
         ,0xa7 ,0xcb ,0xed ,0xe6 ,0xed ,0xf5 ,0xdc ,0x5b
         ,0x9a ,0xee ,0x5d ,0xe4 ,0xf6 ,0x5b ,0x11 ,0x31
         ,0x8f ,0x69 ,0xaf ,0xfd ,0x88 ,0x30 ,0xa6 ,0x1c
         ,0xcb ,0xa9 ,0xdb ,0x75 ,0x95 ,0xfe ,0xc4 ,0xbf
         ,0x63 ,0xe9 ,0xe5 ,0xbc ,0x04 ,0x14 ,0x5c ,0xa6
         ,0x29 ,0x25 ,0x12 ,0x54 ,0x94 ,0x5f ,0x8e ,0x78
         ,0xea ,0x04 ,0x89 ,0x7f ,0xde ,0xe9 ,0xc5 ,0x0e
         ,0x11 ,0x67 ,0x5a ,0x1f ,0x70 ,0x98 ,0xd3 ,0x24
         ,0xd4 ,0x1c ,0x17 ,0x80 ,0x76 ,0xe8 ,0xf7 ,0x75};


/* MODEXP */
#define MODEXP_OP 0x004
#define MODEXP_LEN 128

static unsigned char modexp_data[MODEXP_LEN] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xc, 0x10,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0xb, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x8, 0xa, 0xb, 0x2, 0xd, 0xe, 0xf, 0x10,
                0x1, 0xaa, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xa, 0xe, 0xf, 0x10,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0xa, 0x7, 0x8,
                0x1, 0x23, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
static unsigned char modexp_exp[MODEXP_LEN] =
                { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xc, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0xd, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xcc, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0xff, 0x6, 0x7, 0x8,
                0x1, 0xbc, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 };
static unsigned char modexp_mod[MODEXP_LEN] =
                { 0x1 | 0x80, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10,
                0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8,
                0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf, 0x10 | 0x1 };
static unsigned char modexp_cmp_data[MODEXP_LEN] =
         {0x3a ,0xd6 ,0xfe ,0x8e ,0x7b ,0xdd ,0xc9 ,0xeb
         ,0xe6 ,0x82 ,0x62 ,0xfa ,0x0c ,0x08 ,0x96 ,0xfa
         ,0xc1 ,0x11 ,0xb1 ,0xe9 ,0x0e ,0x17 ,0x1f ,0xe2
         ,0x1b ,0x12 ,0x26 ,0xa1 ,0xd1 ,0xce ,0x05 ,0x01
         ,0xae ,0x48 ,0xc2 ,0xd6 ,0x53 ,0x0f ,0x62 ,0x41
         ,0x8c ,0x60 ,0xcb ,0x25 ,0x7d ,0x43 ,0xec ,0xfe
         ,0x5b ,0xe7 ,0xe3 ,0x17 ,0x05 ,0xd9 ,0x08 ,0xa4
         ,0x76 ,0x2e ,0x72 ,0x1e ,0x80 ,0x7b ,0x0d ,0xe7
         ,0xed ,0x70 ,0x02 ,0x63 ,0x8b ,0xf8 ,0xf3 ,0xbd
         ,0x13 ,0xbb ,0xb8 ,0xac ,0x1f ,0xa0 ,0xad ,0x0a
         ,0x4d ,0x2f ,0xd8 ,0xb0 ,0xde ,0x9f ,0x83 ,0xed
         ,0x46 ,0xca ,0x9b ,0x13 ,0x1e ,0x68 ,0xe8 ,0xdb
         ,0x11 ,0xe7 ,0xbb ,0xf9 ,0x51 ,0x87 ,0x19 ,0x24
         ,0xdb ,0xc0 ,0x7f ,0x30 ,0x6d ,0x86 ,0x1d ,0x72
         ,0xc6 ,0xc1 ,0xcb ,0x06 ,0xb1 ,0xc1 ,0x93 ,0x9d
         ,0x5c ,0xc7 ,0xbe ,0x5b ,0x16 ,0xc0 ,0xec ,0x91};

#endif /* _API_TEST_H_ */
