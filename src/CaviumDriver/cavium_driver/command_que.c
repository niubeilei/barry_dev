////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: command_que.c
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

#include "cavium_sysdep.h"
#include "cavium_common.h"
#include "cavium_list.h"
#include "cavium.h"
#include "command_que.h"

/*
 * Allocate memory for command queue and check bus address 
 * to be multiple of 32 bytes
 */
int 
init_command_queue(cavium_device *pkp_dev, Uint32 q_no)
{
  	/* size in bytes */
	pkp_dev->command_queue_size = 
		(pkp_dev->command_queue_max + 1)  * COMMAND_BLOCK_SIZE;

		pkp_dev->real_command_queue_base[q_no] =  
			(Uint8 *)cavium_malloc_nc_dma(pkp_dev,  
						      pkp_dev->command_queue_size, 
						      (Uint32 *)&pkp_dev->real_command_queue_bus_addr[q_no]);

	if( pkp_dev->real_command_queue_base[q_no] == NULL) {
   		cavium_print("Not enough memory to intialize command que\n");
   			return 1;
  	}
  	
	pkp_dev->command_queue_front[q_no] =
		pkp_dev->command_queue_base[q_no] =
			(Uint8 *) (((ptrlong)(pkp_dev->real_command_queue_base[q_no]) & (ptrlong)(~0x1fUL)) + COMMAND_BLOCK_SIZE);

    pkp_dev->command_queue_bus_addr[q_no] = 
		(pkp_dev->real_command_queue_bus_addr[q_no] & ~0x1fUL) + COMMAND_BLOCK_SIZE;

	pkp_dev->command_queue_end[q_no] = 
		(Uint8 *)(pkp_dev->command_queue_base[q_no] + 
			  (pkp_dev->command_queue_size - COMMAND_BLOCK_SIZE));

	
	/* cavium_spin_lock_init(&(pkp_dev->command_queue_lock[q_no])); */

	pkp_dev->door_bell_count[q_no]=0;
	pkp_dev->door_bell_threshold[q_no]=1;
 	
 	return 0;
}


/*
 * cleanup command queue. Free memory.
 */
int 
cleanup_command_queue(cavium_device * pkp_dev, int q_no)
{

	if(pkp_dev->real_command_queue_base[q_no] != NULL) {
		cavium_free_nc_dma(pkp_dev,
				   pkp_dev->command_queue_size,
				   pkp_dev->real_command_queue_base[q_no],
				   pkp_dev->real_command_queue_bus_addr[q_no]);
		pkp_dev->command_queue_base[q_no]=NULL;
		pkp_dev->command_queue_front[q_no]=NULL;
		pkp_dev->command_queue_end[q_no]=NULL;
		pkp_dev->door_bell_count[q_no]=0;
	    pkp_dev->door_bell_threshold[q_no]=1;
 	}
 
	return 0;
}

/*
 * Increment write pointer and handle wrap-around case.
 */
void   
inc_front_command_queue(cavium_device * pkp_dev, int q_no)
{
	
	pkp_dev->command_queue_front[q_no] = 
		(Uint8*)((ptrlong)pkp_dev->command_queue_front[q_no] + 
			 COMMAND_BLOCK_SIZE);

	if (pkp_dev->command_queue_front[q_no] == 
			pkp_dev->command_queue_end[q_no]) {
		pkp_dev->command_queue_front[q_no] = 
			pkp_dev->command_queue_base[q_no];
	}
	return;
}


void 
reset_command_queue(cavium_device * pkp_dev, int q_no)
{
	pkp_dev->command_queue_front[q_no] = 
		pkp_dev->command_queue_base[q_no];
	
	pkp_dev->door_bell_count[q_no]=0;
	pkp_dev->door_bell_threshold[q_no]=1;
}


/*
 * $Id: command_que.c,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: command_que.c,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:52  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:30  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:36  chen
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
 * Revision 1.1.2.1  2006/04/04 17:53:40  lijing
 * no message
 *
 * Revision 1.11  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.10  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.9  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.8  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.7  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.6  2004/06/23 19:48:33  bimran
 * Fixed command queue 32-byte alignment.
 *
 * Revision 1.4  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.3  2004/04/30 01:37:13  tsingh
 * chnaged inc_front_* function to inline
 *
 * Revision 1.2  2004/04/16 03:19:49  bimran
 * Added doorbell coalescing support.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

