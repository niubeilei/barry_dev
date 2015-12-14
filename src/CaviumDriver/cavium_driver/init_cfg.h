////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: init_cfg.h
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

#ifndef _INIT_CFG_H
#define _INIT_CFG_H

typedef struct _CAVIUM_CONFIG
{
	void *dev;	/* platfomr dependent device pointer */
	Uint32 bus_number;
	Uint32 dev_number;
	Uint32 func_number;
	Uint32 bar_0;
	Uint32 bar_2;
	Uint32 command_queue_max;
	/* context memory size to be pre-allocated,
	 * if DDR memory is not found.
	 * Otherwise, actual size will be used */
	Uint32 context_max; 
}cavium_config;


typedef struct _CAVIUM_GENERAL_CONFIG
{
	Uint32 pending_max; /* number of pending response structures to be pre-allocated. */
	Uint32 direct_max;	/* number of operation structures to be pre-allocated. */
	Uint32 sg_max;		/* number of operation structures to be pre-allocated. */
	Uint32 sg_dma_list_max; /* number of scatter/gather lists to be pre-allocated. */
	Uint32 huge_buffer_max; /* number in huge 32K buffer pool */
	Uint32 large_buffer_max; /* number in large 16K buffer pool */
	Uint32 medium_buffer_max; /* number in medium 8K buffer pool */
	Uint32 small_buffer_max; /* number in small 4K buffer pool */
	Uint32 tiny_buffer_max; /* number in tiny 2K buffer pool */
	Uint32 ex_tiny_buffer_max; /* number in ex tiny 1K buffer pool */
}cavium_general_config;

/* Initialization and cleanup functions */
int cavium_init(cavium_config *config);
int cavium_general_init(cavium_general_config *gconfig);
void cavium_cleanup(void *pdev);
int cavium_general_cleanup(void);
int do_init(cavium_device * pkp_dev);
int load_microcode(cavium_device * pkp_dev, int type);
int pkp_init_board (cavium_device * pkp_dev);
#ifdef NPLUS
int
init_ms_key(cavium_device *pkp_dev, int ucode_idx);
#else
int
init_ms_key(cavium_device *pkp_dev);
#endif

#endif


/*
 * $Id: init_cfg.h,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: init_cfg.h,v $
 * Revision 1.4  2015/01/06 08:57:47  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:36  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:29  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:34  chen
 * AOS
 *
 * Revision 1.1.1.1  2007/09/27 06:28:06  jeff
 * Rebuild AOS trunk
 *
 * Revision 1.4  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.3  2006/04/07 00:24:14  lijing
 * *** empty log message ***
 *
 * Revision 1.2  2006/04/06 02:24:59  lijing
 * add cavium_driver support by lijing
 *
 * Revision 1.1.2.1  2006/04/04 17:53:41  lijing
 * no message
 *
 * Revision 1.6  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.5  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.4  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.3  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.2  2004/04/20 02:26:41  bimran
 * Removed context chunk size field from cavium_config structure.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

