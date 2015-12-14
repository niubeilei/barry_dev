////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: context_memory.c
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
#include "context_memory.h"
#include "linux_main.h" 	// added by liqin 2006/06/09 for CAVIUM_CONTEXT_MAX


/* context memory */
extern int dev_count;
volatile Uint32 ipsec_chunk_count[MAX_DEV];
volatile Uint32 ssl_chunk_count[MAX_DEV];
volatile Uint32 allocated_context_count = 0;

/*
 * Initialize context buffers
 */
int 
init_context(cavium_device *pkp_dev)
{

   Uint32 i;
   ptrlong  ipsec_context_memory_base, ssl_context_memory_base;
        Uint32 ipsec_context_memory_size=0;
        Uint32 ssl_context_memory_size=0;
   Uint8 *p;

   if(pkp_dev->ctx_free_list) 
   {
      cavium_print(("context memory free list already exists\n"));
      return 1;
   }
     cavium_spin_lock_init(&pkp_dev->ctx_lock); 

   /* calculate the total amount of context memory that IPsec contexts will use*/
   ipsec_context_memory_size = (pkp_dev->dram_max-pkp_dev->dram_base)/2;

   /* well same goes for SSL */
   ssl_context_memory_size = ipsec_context_memory_size;

   /* calcualte base addresses for IPsec and SSL blcoks */
   ipsec_context_memory_base = pkp_dev->dram_base;
   ssl_context_memory_base = pkp_dev->dram_base + ipsec_context_memory_size;

   /* calculate chunk counts */
   ipsec_chunk_count[dev_count] = ipsec_context_memory_size/IPSEC_CONTEXT_SIZE;
   //ssl_chunk_count[dev_count] = ssl_context_memory_size/SSL_CONTEXT_SIZE;		// commented by liqin 
   ssl_chunk_count[dev_count] = CAVIUM_CONTEXT_MAX/SSL_CONTEXT_SIZE;			// added by liqin
   //ssl_chunk_count[dev_count] = 4*ssl_context_memory_size/SSL_CONTEXT_SIZE;	// added by liqin

   pkp_dev->dram_chunk_count = ipsec_chunk_count[dev_count] + ssl_chunk_count[dev_count];

   cavium_dbgprint("dram_chunk count: %ld\n",pkp_dev->dram_chunk_count);
   
   pkp_dev->ctx_free_list = 
      (ptrlong *)cavium_malloc_virt(pkp_dev->dram_chunk_count*sizeof(ptrlong));
   pkp_dev->org_ctx_free_list = 
      (ptrlong *)cavium_malloc_virt(pkp_dev->dram_chunk_count*sizeof(ptrlong));
   if(pkp_dev->ctx_free_list == NULL || pkp_dev->org_ctx_free_list == NULL) 
   {
      if(pkp_dev->ctx_free_list)
       {
         cavium_free_virt(pkp_dev->ctx_free_list);
         pkp_dev->ctx_free_list = NULL;
      }
      if(pkp_dev->org_ctx_free_list)
      {
         cavium_free_virt(pkp_dev->org_ctx_free_list);
         pkp_dev->org_ctx_free_list = NULL;
      }
     cavium_print("Not enough memory in allocating ctx_free_list\n");
     return 1;
   }
      
   /* Allocate IPSEC */
   for (i=0; i < ipsec_chunk_count[dev_count] ; i++) 
   {

      /* DRAM present */
      if (pkp_dev->dram_present) 
      {
         pkp_dev->ctx_free_list[i] = (ptrlong)((i*IPSEC_CONTEXT_SIZE)+ipsec_context_memory_base);
      } 
      else 
      {
      /* NO DRAM Using host memory*/
#ifdef N1_SANITY
         p = cavium_malloc_dma(IPSEC_CONTEXT_SIZE+ALIGNMENT,NULL);
#else
         p = cavium_malloc_dma(IPSEC_CONTEXT_SIZE,NULL);
#endif
         if(p)
         {
            pkp_dev->org_ctx_free_list[i] = (ptrlong)p;
#ifdef N1_SANITY
            p = (Uint8 *)((ptrlong)((Uint8 *)p+ALIGNMENT) & ALIGNMENT_MASK );
#endif
            /*pkp_dev->ctx_free_list[i] = (ptrlong)p;*/
            pkp_dev->ctx_free_list[i] = (ptrlong)cavium_vtophys(p); 
         } 
         else 
         {
            Uint32 j;

            for(j=0; j<i; j++) 
            {
               p = (Uint8 *)pkp_dev->org_ctx_free_list[j];
               cavium_free_dma(p);
            }
            cavium_print("Not enough memory in allocating context memory\n");
            cavium_free_virt(pkp_dev->ctx_free_list);
            cavium_free_virt(pkp_dev->org_ctx_free_list);
            pkp_dev->org_ctx_free_list=NULL;
            pkp_dev->ctx_free_list=NULL;
            return 1;
         }
      }
   }
      
   /* Allocate SSL*/
   for (i=ipsec_chunk_count[dev_count]; i<pkp_dev->dram_chunk_count; i++) 
   {
      /* DRAM present */
      if (pkp_dev->dram_present) 
      {
         pkp_dev->ctx_free_list[i] = (ptrlong)(((i-ipsec_chunk_count[dev_count])*SSL_CONTEXT_SIZE)+ssl_context_memory_base);
      } 
      else
      {
         /* NO DRAM Using host memory*/
#ifdef N1_SANITY
         p = cavium_malloc_dma(SSL_CONTEXT_SIZE+ALIGNMENT,NULL);
#else
         p = cavium_malloc_dma(SSL_CONTEXT_SIZE,NULL);
#endif
         if(p)
         {
            pkp_dev->org_ctx_free_list[i] = (ptrlong)p;
#ifdef N1_SANITY
            p = (Uint8 *)((ptrlong)((Uint8 *)p+ALIGNMENT) & ALIGNMENT_MASK );
#endif
            /*pkp_dev->ctx_free_list[i] = (ptrlong)p;*/
            pkp_dev->ctx_free_list[i] = (ptrlong)cavium_vtophys(p);
         
          }
          else 
          {
             Uint32 j;

             for(j=0; j<i; j++) 
             {
                p = (Uint8*)pkp_dev->org_ctx_free_list[j];
                cavium_free_dma(p);
             }
             cavium_print("Not enough memory in allocating context memory\n");
             cavium_free_virt(pkp_dev->ctx_free_list);
             cavium_free_virt(pkp_dev->org_ctx_free_list);
             pkp_dev->org_ctx_free_list=NULL;
             pkp_dev->ctx_free_list=NULL;
             return 1;
          }
      }
   }
   
 /* initialize lock */
 /* cavium_spin_lock_init(&pkp_dev->ctx_lock); */

 pkp_dev->ctx_ipsec_count = ipsec_chunk_count[dev_count];
 pkp_dev->ctx_ssl_count = ssl_chunk_count[dev_count];

 pkp_dev->ctx_ipsec_free_index = 0;
 pkp_dev->ctx_ssl_free_index = ipsec_chunk_count[dev_count];
 return 0;
}/*init_context*/


/*
 * Get next available context ID
 */
int
alloc_context_id(cavium_device *pkp_dev, ContextType c, ptrlong *cid)
{
   int ret=0;
   ptrlong   cm=0;

   cavium_spin_lock_softirqsave(&pkp_dev->ctx_lock);
   if ( c ==  CONTEXT_IPSEC) 
   {
      if(pkp_dev->ctx_ipsec_count==0)
      {
         ret = -1;
         goto cleanup;
      }
         
      cm = pkp_dev->ctx_free_list[pkp_dev->ctx_ipsec_free_index];
      pkp_dev->ctx_ipsec_free_index++;
      pkp_dev->ctx_ipsec_count--;
   } 
   else 
   {
      if(pkp_dev->ctx_ssl_count == 0)
      {
         ret = -1;
         goto cleanup;
      }
      
      cm = pkp_dev->ctx_free_list[pkp_dev->ctx_ssl_free_index];
      pkp_dev->ctx_ssl_free_index++;
      pkp_dev->ctx_ssl_count--;
   }

cleanup:
   cavium_spin_unlock_softirqrestore(&pkp_dev->ctx_lock);
   *cid = cm;
   return ret;

} /* alloc_context */


/*
 * Put back
 */
int 
dealloc_context_id(cavium_device * pkp_dev, ContextType c, ptrlong cid)
{
   int ret=0;

    /* acquire lock */   
    cavium_spin_lock_softirqsave(&pkp_dev->ctx_lock);
   
    if (c == CONTEXT_IPSEC)  
    {
       pkp_dev->ctx_ipsec_free_index--;
      pkp_dev->ctx_ipsec_count++;
       pkp_dev->ctx_free_list[pkp_dev->ctx_ipsec_free_index] = cid;   
    } 
    else 
    {
      /* SSL */
       pkp_dev->ctx_ssl_free_index--;
       pkp_dev->ctx_ssl_count++;
       pkp_dev->ctx_free_list[pkp_dev->ctx_ssl_free_index] = cid;   
    }
   
    ret = 0;

    /* release lock*/
    cavium_spin_unlock_softirqrestore(&pkp_dev->ctx_lock);

    return ret;
}/* dealloc context */

Uint64
alloc_context(cavium_device *pkp_dev, ContextType c)
{
   ptrlong cid;

   if(alloc_context_id(pkp_dev, c, &cid))
   {
      cavium_error("alloc_context: context memory allocation failed\n");
      return 0;
   }
   else
   {
      /* Increment the allocated context counter */
      allocated_context_count++;
      if (pkp_dev->dram_present)
      {
         return (Uint64)cid | (Uint64)(0x8000000000000000ULL);
      }
      else
         return (Uint64)cid;
   }
}
   
#ifdef CAVIUM_RESOURCE_CHECK
int
insert_ctx_entry(struct cavium_list_head *ctx_head, ContextType c, Uint64 addr)
{
   struct CTX_ENTRY *entry;
   
   entry = cavium_malloc(sizeof(struct CTX_ENTRY), NULL);
   if (entry == NULL) {
      cavium_error("Insert-ctx-entry: Not enough memory\n");
      return -1;
   }

   entry->ctx = addr;
   entry->ctx_type = c;
   cavium_list_add_tail(&entry->list, ctx_head);   
   
   return 0;
}
#endif

void 
dealloc_context(cavium_device *pkp_dev, ContextType c, Uint64 addr)
{
   ptrlong cid = 0;

   if(pkp_dev->dram_present)
   {
      cid = (ptrlong)(addr & (Uint64)0x7fffffffffffffffULL);
   }
   else
      cid = (ptrlong)addr;

   dealloc_context_id(pkp_dev, c, cid);
        allocated_context_count--;
}


/*
 * Free memory 
 */
int 
cleanup_context(cavium_device *pkp_dev)
{
   Uint32 i;
   Uint8 *p;

   cavium_spin_lock_destroy(&pkp_dev->ctx_lock);
   if (pkp_dev->dram_present) 
   {
      if (pkp_dev->ctx_free_list) 
      {
         cavium_free_virt((Uint8 *)pkp_dev->ctx_free_list);
         pkp_dev->ctx_free_list = NULL;
      }
      
   }
   else 
   {
      if(pkp_dev->org_ctx_free_list) 
      {
         for(i = 0; i < pkp_dev->dram_chunk_count; i++) 
         {
            p = (Uint8 *)pkp_dev->org_ctx_free_list[i];
            if (p)
               cavium_free_dma(p);
         }
      }

      if(pkp_dev->ctx_free_list) 
      {
         cavium_free_virt((Uint8 *)pkp_dev->ctx_free_list);
         pkp_dev->ctx_free_list=NULL;
      }
      if(pkp_dev->org_ctx_free_list) 
      {
         cavium_free_virt((Uint8 *)pkp_dev->org_ctx_free_list);
         pkp_dev->org_ctx_free_list=NULL;
      }
   }

   pkp_dev->ctx_ipsec_free_index=0;
   pkp_dev->ctx_ssl_free_index=0;

   return 0;
}/*cleanup_context*/


/*
 * $Id: context_memory.c,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: context_memory.c,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:53  cding
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
 * Revision 1.8  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.7  2006/06/09 05:54:40  liqin
 * *** empty log message ***
 *
 * Revision 1.6  2006/06/08 08:14:18  liqin
 * *** empty log message ***
 *
 * Revision 1.5  2006/05/04 01:53:38  liqin
 * *** empty log message ***
 *
 * Revision 1.4  2006/05/03 12:21:16  liqin
 * *** empty log message ***
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
 * Revision 1.16  2005/10/24 06:51:59  kanantha
 * - Fixed RHEL4 warnings
 *
 * Revision 1.15  2005/10/13 09:21:59  ksnaren
 * fixed compile errors for windows xp
 *
 * Revision 1.14  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.13  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.12  2005/08/31 18:10:30  bimran
 * Fixed several warnings.
 * Fixed the corerct use of ALIGNMENT and related macros.
 *
 * Revision 1.11  2005/07/17 04:35:09  sgadam
 * 8 bytes alignment issue on linux-2.6.2 is fixed. README and Makefile in
 * apps/cavium_engine updated
 *
 * Revision 1.10  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.9  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.8  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.7  2004/06/03 21:22:56  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.6  2004/05/04 20:48:34  bimran
 * Fixed RESOURCE_CHECK.
 *
 * Revision 1.5  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.4  2004/04/30 00:00:33  bimran
 * Removed semaphoers from context memory in favour of just counts and a lock.
 *
 * Revision 1.3  2004/04/21 21:21:04  bimran
 * statis and free lists were using DMA memory for no reason. Changed the memory allocation to virtual.
 *
 * Revision 1.2  2004/04/20 02:23:17  bimran
 * Made code more generic. Divided context memory into two portions, one for Ipsec and One for SSL.
 * Fixed  bug where DDR was present and index was not pushed in free list.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

