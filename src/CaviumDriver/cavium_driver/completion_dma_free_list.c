////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: completion_dma_free_list.c
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
#include "completion_dma_free_list.h"

volatile static Uint32 completion_dma_free_max=0;
volatile static Uint32 completion_dma_free_index=0;
volatile static Uint64 *completion_dma_buffer=NULL;
static Uint32 completion_dma_buffer_size=0; 
static ptrlong *completion_dma_free_list=NULL;
ptrlong completion_dma_bus_addr=0;
static cavium_spinlock_t completion_dma_free_lock;
static int  completion_dma_count;

int 
init_completion_dma_free_list(cavium_device *pkp_dev, Uint32 max)
{
   Uint32 i;
   Uint64 *p;

   /* initialize lock */
   cavium_spin_lock_init(&completion_dma_free_lock); 
   if (completion_dma_free_list) 
   {
      cavium_print(("completion_dma free list already exists\n"));
      return 1;
   }
   
   completion_dma_free_max = max;
   completion_dma_free_list = (ptrlong *)cavium_malloc_virt(completion_dma_free_max * sizeof(ptrlong));
   
   if (completion_dma_free_list == NULL) 
   {
      cavium_print(("Not enough memory in allocating completion_dma_free_list\n"));
      return 1;
   }
   
   /* allocate one contiguous block and divid 'em into pieces ;-)*/
   completion_dma_buffer_size = completion_dma_free_max * sizeof(Uint64);
   p = (Uint64 *)cavium_malloc_nc_dma(pkp_dev, completion_dma_buffer_size,
                 &completion_dma_bus_addr);
   completion_dma_buffer = (volatile Uint64 *)p;
   cavium_print("Completion Addr %p\n", completion_dma_buffer);
   cavium_print("Completion Bus Addr %lx\n", completion_dma_bus_addr);

   if(p == NULL) 
   {
      cavium_print(("Not enough memory in allocating completion_dma\n"));
      cavium_free_virt(completion_dma_free_list);
      completion_dma_free_list=NULL;
      return 1;
   }

   /* build the free list */
   for(i = 0; i < completion_dma_free_max; i++) 
   {
      completion_dma_free_list[i] = (ptrlong)(p);
      p++;
   }
    
   /* initialize lock */
   /* cavium_spin_lock_init(&completion_dma_free_lock); */

   completion_dma_count = completion_dma_free_max;

   completion_dma_free_index=0;
   return 0;
}


/*
 * Get next avilable entry
 */

Uint64 *
get_completion_dma(int *test)
{
    Uint64 *p;

    /* acquire lock */
   cavium_spin_lock_softirqsave(&completion_dma_free_lock);

   if(completion_dma_count == 0)
   {
      *test = -1;
      p = NULL;
      goto cleanup;
    }
    else
      *test=0;

   p = (Uint64 *)completion_dma_free_list[completion_dma_free_index];
   completion_dma_free_index++;
   completion_dma_count--;
 
cleanup:
   /* release lock*/
   cavium_spin_unlock_softirqrestore(&completion_dma_free_lock);
   return p;
}

/*
 * Returns bus address
 */
ptrlong 
get_completion_dma_bus_addr(volatile Uint64 *p)
{
   return (completion_dma_bus_addr +((ptrlong)p - (ptrlong)completion_dma_buffer));
}/*get_completion_dma_bus_addr*/

/*
 * Put entry back in the free list.
 */
int 
put_completion_dma(volatile Uint64 *p)
{
   
    int ret=0;
 
    cavium_spin_lock_softirqsave(&completion_dma_free_lock);

    completion_dma_free_index--;
    completion_dma_count++;
    completion_dma_free_list[completion_dma_free_index] = (ptrlong)p;
    ret=0;

    /* release lock*/
    cavium_spin_unlock_softirqrestore(&completion_dma_free_lock);   
   return ret;

}

/*
 * Cleanup things
 */
int 
cleanup_completion_dma_free_list(cavium_device *pkp_dev)
{

   cavium_spin_lock_destroy(&completion_dma_free_lock);
   if(completion_dma_free_list) 
   {
      if(completion_dma_buffer)
      {
         cavium_free_nc_dma(pkp_dev,completion_dma_buffer_size,(void *)completion_dma_buffer,completion_dma_bus_addr);
         completion_dma_buffer=NULL;
      }

      cavium_free_virt(completion_dma_free_list);
      completion_dma_free_list = NULL;
      completion_dma_free_index=0;
   }

   return 0;
}/*cleanup_completion_dma_free_list*/


/*
 * $Id: completion_dma_free_list.c,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: completion_dma_free_list.c,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:53  cding
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
 * Revision 1.1.2.1  2006/04/04 17:53:40  lijing
 * no message
 *
 * Revision 1.12  2005/10/13 09:21:15  ksnaren
 * changed cavium_malloc_dma to cavium_malloc_virt for the control structs
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
 * Revision 1.7  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.6  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.5  2004/06/23 19:52:23  bimran
 * included header file.
 *
 * Revision 1.4  2004/06/03 21:22:56  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.3  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.2  2004/04/30 00:00:33  bimran
 * Removed semaphoers from context memory in favour of just counts and a lock.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */





