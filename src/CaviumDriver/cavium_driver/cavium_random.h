////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_random.h
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

#ifndef _CAV_RANDOM_H
#define _CAV_RANDOM_H

#define RND_MAX		(32768-16)

int init_rnd_buffer(cavium_device *);
void cleanup_rnd_buffer(cavium_device *);

#ifdef NPLUS
int  fill_rnd_buffer(cavium_device *, int);
int get_rnd(cavium_device *, Uint8 *, Uint16, int);
#else
int  fill_rnd_buffer(cavium_device *);
int get_rnd(cavium_device *, Uint8 *, Uint16);
#endif

#endif 

/*
 * $Id: cavium_random.h,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium_random.h,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:36  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:29  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:35  chen
 * AOS
 *
 * Revision 1.1.1.1  2007/09/27 06:28:06  jeff
 * Rebuild AOS trunk
 *
 * Revision 1.4  2006/06/23 09:57:51  lixiaox
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
 * Revision 1.8  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.7  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.6  2004/06/02 02:08:02  tsingh
 * removed get_id() (bimran).
 *
 * Revision 1.5  2004/05/11 03:10:39  bimran
 * some performance opt.
 *
 * Revision 1.4  2004/05/05 06:45:30  bimran
 * Added another function to get request ids from random pool.
 *
 * Revision 1.3  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.2  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

