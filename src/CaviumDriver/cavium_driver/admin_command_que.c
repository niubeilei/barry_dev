////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: admin_command_que.c
// Description:
// 
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
#if 0

#ifndef MC2
#include "cavium_sysdep.h"
#include "cavium_common.h"
#include "cavium.h"
#include "hw_lib.h"
#include "admin_command_que.h"


int 
init_admin_command_queue(cavium_device *pkp_dev)
{
 
 	pkp_dev->admin_command_queue_base = 
	 	(Uint8 *)cavium_malloc_nc_dma(pkp_dev,
				       	      ADMIN_COMMAND_BLOCK_SIZE*
					      MAX_CORES_NITROX,
				              &pkp_dev->admin_command_queue_bus_addr);

 	if(pkp_dev->admin_command_queue_base == NULL) {
   		cavium_print( "Not enough memory to intialize admin command que\n");
   		return 1;
  	}
 
	pkp_dev->admin_command_queue_end = 
	 	(Uint8 *)(pkp_dev->admin_command_queue_base + 
			  (ADMIN_COMMAND_BLOCK_SIZE * MAX_CORES_NITROX));

	cavium_spin_lock_init(&pkp_dev->admin_command_queue_lock);
	return 0;
}


int 
set_front_admin_command_queue(cavium_device *pkp_dev)
{
	Uint32 count;
 
	count = count_set_bits(pkp_dev->admin_core_mask, MAX_CORES_NITROX);
	if(count > 1) {
		cavium_print( "One admin core is supported.\n");
		return 1;
 	}

	if(count  == 0) {
		cavium_print( "Admin core mask not found.\n");
   		return 1;
 	}

	pkp_dev->admin_unit_id = get_unit_id(pkp_dev->admin_core_mask);
	if(pkp_dev->admin_unit_id > pkp_dev->exec_units) {
		cavium_print( 
			"Admin core id (%ld)greater than Max cores.\n",
			pkp_dev->admin_unit_id);
   		return 1;
 	}
 

	pkp_dev->admin_command_queue_front = pkp_dev->admin_command_queue_base
	       				    + (pkp_dev->admin_unit_id * ADMIN_COMMAND_BLOCK_SIZE);

	return 0;

} /* set front*/




int 
cleanup_admin_command_queue(cavium_device *pkp_dev)
{
	if(pkp_dev->admin_command_queue_base != NULL) {
		cavium_free_nc_dma(pkp_dev,
			   	   ADMIN_COMMAND_BLOCK_SIZE * MAX_CORES_NITROX,
				   pkp_dev->admin_command_queue_base,
				   pkp_dev->admin_command_queue_bus_addr);
  	}
	return 0;
}


Uint32 
get_size_admin_command_queue(cavium_device *pkp_dev)
{
	return (ADMIN_COMMAND_BLOCK_SIZE*MAX_CORES_NITROX);
}

/*
 * admin door bell ring
 */
void
ring_admin_door_bell(cavium_device *pkp_dev)
{
        disable_exec_units_from_mask(pkp_dev, pkp_dev->admin_core_mask);
        cavium_udelay(10);
        enable_exec_units_from_mask(pkp_dev, pkp_dev->admin_core_mask);
}


/*
 * sends request through admin queue mechanism
 */
int
send_admin_request(cavium_device *pkp_dev, Request request)
{
	Uint8 *command;

 cavium_dbgprint( "Request.cmd = %08lx%08lx\n",  (Uint32)(request.cmd >> 32),  (Uint32)request.cmd);
 cavium_dbgprint( "Request.dptr = %08lx%08lx\n", (Uint32)(request.dptr >> 32), (Uint32)request.dptr);
 cavium_dbgprint( "Request.rptr = %08lx%08lx\n", (Uint32)(request.rptr >> 32), (Uint32)request.rptr);
 cavium_dbgprint( "Request.cptr = %08lx%08lx\n", (Uint32)(request.cptr >> 32), (Uint32)request.cptr);

 	command = pkp_dev->admin_command_queue_front;
	cavium_memcpy(command, (Uint8 *)&request,32);

	ring_admin_door_bell(pkp_dev);

	return 0;
}
#endif
#endif /* if 0*/

/*
 * $Id: admin_command_que.c,v 1.4 2015/01/06 08:57:45 andy Exp $
 * $Log: admin_command_que.c,v $
 * Revision 1.4  2015/01/06 08:57:45  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:52  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:31  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:36  chen
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
 * Revision 1.3  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.2  2004/04/19 18:37:20  bimran
 * if 0'd out the whole file.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

