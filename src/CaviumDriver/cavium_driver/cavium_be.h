////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_be.h
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
// 3. All manuals,brochures,user guides mentioning features or use of this software 
//    must display the following acknowledgement:
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

#ifndef _CAVIUM_ENDIAN_H
#define _CAVIUM_ENDIAN_H

#define CAVIUM_BIG_ENDIAN 4321
#define CAVIUM_ENDIAN	CAVIUM_BIG_ENDIAN

#if 0
#define COMPLETION_CODE_VALUE((p))	(Uint8 *)((p) >> COMPLETION_CODE_SHIFT)
#endif

#define ENDIAN_SWAP_8_BYTE(_i) \
  ((((((Uint64)(_i)) >>  0) & (Uint64)0xff) << 56) | \
   (((((Uint64)(_i)) >>  8) & (Uint64)0xff) << 48) | \
   (((((Uint64)(_i)) >> 16) & (Uint64)0xff) << 40) | \
   (((((Uint64)(_i)) >> 24) & (Uint64)0xff) << 32) | \
   (((((Uint64)(_i)) >> 32) & (Uint64)0xff) << 24) | \
   (((((Uint64)(_i)) >> 40) & (Uint64)0xff) << 16) | \
   (((((Uint64)(_i)) >> 48) & (Uint64)0xff) <<  8) | \
   (((((Uint64)(_i)) >> 56) & (Uint64)0xff) <<  0))

#define htobe64(_i)	(_i)
#define htole64(_i) ENDIAN_SWAP_8_BYTE((_i))
#define betoh64(_i)	(_i)
#define letoh64(_i) ENDIAN_SWAP_8_BYTE((_i))
#define htobe32(_i)	(_i)
#define betoh32(_i)	(_i)
#define htobe16(_i)	(_i)
#define betoh16(_i)	(_i)

#endif

/*
 * $Id: cavium_be.h,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium_be.h,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:35  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:28  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:33  chen
 * AOS
 *
 * Revision 1.1.1.1  2007/09/27 06:28:06  jeff
 * Rebuild AOS trunk
 *
 * Revision 1.4  2006/06/23 09:57:50  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.3  2006/04/07 00:24:13  lijing
 * *** empty log message ***
 *
 * Revision 1.2  2006/04/06 02:24:59  lijing
 * add cavium_driver support by lijing
 *
 * Revision 1.1.2.1  2006/04/04 17:53:40  lijing
 * no message
 *
 * Revision 1.2  2005/02/01 04:12:05  bimran
 * copyright fix
 *
 * Revision 1.1  2004/06/10 05:14:41  bimran
 * big endian macros
 *
 */

