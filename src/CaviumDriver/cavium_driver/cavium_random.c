////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_random.c
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
#include "cavium_endian.h"
#include "cavium_list.h"
#include "cavium.h"
#include "cavium_random.h"
#include "request_manager.h"
#include "error_handler.h"
#include "init_cfg.h"
#include "buffer_pool.h"

/*
 * Random Number pool stuff
 */
int
init_rnd_buffer(cavium_device * pkp_dev)
{
	cavium_spin_lock_init(&pkp_dev->rnd_lock);
	cavium_spin_lock_init(&pkp_dev->id_lock);

	if((pkp_dev->rnd_buffer = 
		get_buffer_from_pool(pkp_dev,RND_MAX)) == NULL) {
	        cavium_print( "Not enough memory to create random pool.\n");
	        return 1;
	}
	pkp_dev->rnd_index=0;
	return 0;
}/* init_rnd_buffer*/

void
cleanup_rnd_buffer(cavium_device * pkp_dev)
{
	cavium_spin_lock_destroy(&pkp_dev->rnd_lock);
	cavium_spin_lock_destroy(&pkp_dev->id_lock);
	if(pkp_dev->rnd_buffer) {
	        put_buffer_in_pool(pkp_dev,(Uint8 *)pkp_dev->rnd_buffer);
	        pkp_dev->rnd_buffer = NULL;
	}
}

#ifdef NPLUS
int
fill_rnd_buffer(cavium_device * pkp_dev, int ucode_idx)
#else
int
fill_rnd_buffer(cavium_device * pkp_dev)
#endif
{
	int ret=0;
	Uint8 *out_buffer=NULL;
	Uint64 *completion_address;
	Cmd strcmd;
	Request request;
	Uint16 rnd_size = RND_MAX;
#ifdef NPLUS
	int srq_idx;
#endif

	strcmd.opcode= htobe16((0x1<<8) | MAJOR_OP_RANDOM_WRITE_CONTEXT);
#ifdef MC2
	strcmd.size  = htobe16(rnd_size);
#else
	strcmd.size  = htobe16(rnd_size>>3);
#endif
    	strcmd.param = htobe16(0);
    	strcmd.dlen  = htobe16(0);

	if(pkp_dev->rnd_buffer == NULL) {
		init_rnd_buffer(pkp_dev);
	}
	out_buffer = pkp_dev->rnd_buffer;

     	cavium_memcpy((Uint8 *)&request, (Uint8 *)&strcmd, 8);
	request.dptr = htobe64(0);
	request.rptr = htobe64(cavium_vtophys(out_buffer));
	request.cptr = htobe64(0);

	completion_address = (Uint64 *)(pkp_dev->rnd_buffer + RND_MAX);
	*completion_address = COMPLETION_CODE_INIT;

#ifdef NPLUS
	srq_idx = send_command(pkp_dev, &request, 0, ucode_idx, completion_address);
        ret = check_completion(pkp_dev, completion_address, 500, ucode_idx,srq_idx);
#else
	send_command(pkp_dev, &request, 0);
	ret = check_completion(completion_address, 500);
#endif
	if(ret) {
	      cavium_print( "Error: %x out while sending random request in fill_rnd_buffer.\n", ret);
	   goto fill_rnd_err;
	}
fill_rnd_err:
	return ret;

}


#ifdef NPLUS
int
get_rnd(cavium_device *pkp_dev, Uint8 *uaddr, Uint16 usize, int ucode_idx)
#else
int
get_rnd(cavium_device *pkp_dev, Uint8 *uaddr, Uint16 usize)
#endif
{
	int ret=0;
	Uint8 *p=NULL;

	cavium_spin_lock(&pkp_dev->rnd_lock);
	if((pkp_dev->rnd_index+usize) >= RND_MAX)
	{
#ifdef NPLUS
	        ret = fill_rnd_buffer(pkp_dev, ucode_idx);
#else
	        ret = fill_rnd_buffer(pkp_dev);
#endif
	        if(ret)
	                goto get_rnd_error;
	        else
	                pkp_dev->rnd_index=0;
	}
	p = &pkp_dev->rnd_buffer[pkp_dev->rnd_index];
	pkp_dev->rnd_index += usize;

get_rnd_error:
	cavium_spin_unlock(&pkp_dev->rnd_lock);
	if(p)
		if(cavium_copy_out(uaddr, p, usize))
		      ret =-1;
	return ret;
}

/*
 * $Id: cavium_random.c,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium_random.c,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:52  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:30  cding
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
 * Revision 1.14  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.13  2005/08/31 02:32:36  bimran
 * Fixed code to check for copy_in/out return values.
 *
 * Revision 1.12  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.11  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.10  2004/06/03 21:22:56  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.9  2004/06/02 02:07:45  tsingh
 * Removed get_id() (bimran)
 *
 * Revision 1.8  2004/06/01 17:44:26  bimran
 * made get_rnd() thread safe.
 *
 * Revision 1.7  2004/05/28 17:57:22  bimran
 * used id lock instead of rnd_lock
 *
 * Revision 1.6  2004/05/11 20:50:32  tsingh
 * Changed some arguments passed through a function
 *
 * Revision 1.5  2004/05/11 03:10:39  bimran
 * some performance opt.
 *
 * Revision 1.4  2004/05/05 06:45:06  bimran
 * Fixed initialization and cleanup for random pool.
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

