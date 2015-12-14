////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: sg_dma_free_list.c
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
#include "sg_dma_free_list.h"

volatile static Uint32 sg_dma_free_max=0;
volatile static Uint32 sg_dma_free_index=0;
static ptrlong *sg_dma_real_free_list=NULL;
static ptrlong *sg_dma_free_list=NULL;
static cavium_spinlock_t sg_dma_free_lock;

int 
init_sg_dma_free_list(Uint32 max)
{
  Uint32 i,j, size;
  Uint8 *p;

  /* initialzie lock */
  cavium_spin_lock_init(&sg_dma_free_lock); 
   
  if(sg_dma_free_list)
  {
     cavium_print(("sg_dma free list already exists.\n"));
     return 1;
  }
  
  sg_dma_free_max = max;
  sg_dma_free_list = (ptrlong *)cavium_malloc_virt(sg_dma_free_max*sizeof(ptrlong));
  sg_dma_real_free_list = (ptrlong *)cavium_malloc_virt(sg_dma_free_max*sizeof(ptrlong));
  
  if(sg_dma_free_list == NULL || sg_dma_real_free_list == NULL)
     {
        cavium_print(("Not enough memory in allocating sg_dma_free_list\n"));
        if (sg_dma_free_list)
           cavium_free_virt (sg_dma_free_list);
        if (sg_dma_real_free_list)
           cavium_free_virt (sg_dma_real_free_list);
        return 1;
     }
  
  
  cavium_memset(sg_dma_free_list, 0, sizeof(ptrlong)*sg_dma_free_max);
  cavium_memset(sg_dma_real_free_list, 0, sizeof(ptrlong)*sg_dma_free_max);
  size = 8 + (((MAX_OUTCNT +3)/4) + ((MAX_INCNT +3)/4))*40;

  /* 8 byte alignment is needed for mc 0.96 */
  size += 8;
  for(i=0; i<sg_dma_free_max; i++)
  {
     p = (Uint8 *)cavium_malloc_dma(size, NULL);
     if(p) 
     {
        sg_dma_real_free_list[i] = (ptrlong)p;
        sg_dma_free_list[i] = (ptrlong) (((ptrlong) p + 0x7) & ~0x7L);
     }
     else
     {
        for(j=0; j<i; j++)
        {
           p = (Uint8 *)sg_dma_real_free_list[j];
           cavium_free_dma(p);
        }
       goto sg_fail;
     }
  }    
      
  /* initialzie lock */

 /* cavium_spin_lock_init(&sg_dma_free_lock); */

 sg_dma_free_index=0;
 return 0;
sg_fail:
 cavium_free_virt(sg_dma_free_list);
 cavium_free_virt(sg_dma_real_free_list);
 sg_dma_free_list = NULL;
 sg_dma_real_free_list = NULL;
 sg_dma_free_max = 0;
 return 1;
}


Uint8 *
get_sg_dma(int *test)
{
   Uint8 *p;

   /* acquire lock */
   cavium_spin_lock_softirqsave(&sg_dma_free_lock);

   if(sg_dma_free_index >= sg_dma_free_max)
   {
      *test = -1;
      p=NULL;
      goto cleanup;
   }
   else
      *test=0;

   p = (Uint8 *)sg_dma_free_list[sg_dma_free_index];
   sg_dma_free_index++;

cleanup:
   /* release lock*/
   cavium_spin_unlock_softirqrestore(&sg_dma_free_lock);
   return p;
}


int 
put_sg_dma(volatile Uint8 *p)
{
   
   int ret=0;
 
   cavium_spin_lock_softirqsave(&sg_dma_free_lock);

   sg_dma_free_index--;
   sg_dma_free_list[sg_dma_free_index] = (ptrlong)p;
   ret=0;

   /* release lock*/
   cavium_spin_unlock_softirqrestore(&sg_dma_free_lock);   
   return ret;

}

int 
cleanup_sg_dma_free_list(void)
{
   Uint32 i;
   Uint8 *p;

   cavium_spin_lock_destroy(&sg_dma_free_lock);
   if(sg_dma_free_list)
   {
      for(i=0; i<sg_dma_free_max; i++)
      {
         p = (Uint8 *)sg_dma_real_free_list[i];
         cavium_free_dma(p);
      }

      cavium_free_virt(sg_dma_free_list);
      cavium_free_virt(sg_dma_real_free_list);
      sg_dma_free_list = sg_dma_real_free_list = NULL;
      sg_dma_free_index=0;
   }

   return 0;
}


/*
 * $Id: sg_dma_free_list.c,v 1.4 2015/01/06 08:57:48 andy Exp $
 * $Log: sg_dma_free_list.c,v $
 * Revision 1.4  2015/01/06 08:57:48  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:54  cding
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
 * Revision 1.3  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.2  2006/04/06 02:25:00  lijing
 * add cavium_driver support by lijing
 *
 * Revision 1.1.2.1  2006/04/04 17:53:41  lijing
 * no message
 *
 * Revision 1.14  2005/10/13 09:26:33  ksnaren
 * changed cavium_malloc_dma to cavium_malloc_virt for the control structs
 *
 * Revision 1.13  2005/09/29 04:43:05  ksadasivuni
 * - Mistake in CVS Merge corrected
 *
 * Revision 1.12  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.11  2005/09/21 06:54:49  lpathy
 * Merging windows server 2003 release with CVS head
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
 * Revision 1.5  2004/06/23 20:58:30  bimran
 * compiler warning on NetBSD
 *
 * Revision 1.4  2004/06/03 21:22:57  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.3  2004/05/02 19:45:31  bimran
 * Added Copyright notice.
 *
 * Revision 1.2  2004/04/30 00:00:34  bimran
 * Removed semaphoers from context memory in favour of just counts and a lock.
 *
 * Revision 1.1  2004/04/15 22:40:50  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

