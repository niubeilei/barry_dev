////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium.c
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
#include "command_que.h"
#include "context_memory.h"
#include "hw_lib.h"
#include "error_handler.h"
#include "pending_free_list.h"
#include "pending_list.h"
#include "direct_free_list.h"
#include "sg_free_list.h"
#include "sg_dma_free_list.h"
#include "completion_dma_free_list.h"
#ifdef NPLUS
#include "soft_req_queue.h"
#endif

static void pkp_convert_sg_to_int_format(struct PKP_SG_OPERATION_STRUCT *);


/*
 */
#ifdef NPLUS
int
check_completion(cavium_device *n1_dev, volatile Uint64 *p, int max_wait_states, int ucode_idx, int srq_idx)
#else
int
check_completion(volatile Uint64 *p, int max_wait_states)
#endif /*NPLUS*/
{
   int i, ret;

   i=ret=0;
   
   cavium_dbgprint("comp-address: %08lx%08lx\n",(Uint32)((*p) >> 32),
           (Uint32)(*p));
#if CAVIUM_ENDIAN == CAVIUM_LITTLE_ENDIAN
   while (((Uint8)((*p) >> COMPLETION_CODE_SHIFT)) == 0xff) 
   {
#else
   while (((Uint8)(*p)) == 0xff) 
   {
#endif
#ifdef NPLUS
      /* If this check is for a SPM code, then we try to move requests
       * from SRQ to the CTP here
       */
      if(n1_dev->microcode[ucode_idx].code_type == CODE_TYPE_SPECIAL)
         move_srq_entries(n1_dev, ucode_idx, 0);
#endif /*NPLUS*/
         cavium_mdelay(2);
      i++;
      if(i > max_wait_states)
      {
         ret = ERR_REQ_TIMEOUT;
         break;
      }
   }

#ifdef NPLUS
   if(n1_dev->microcode[ucode_idx].code_type == CODE_TYPE_SPECIAL)
   {
      if(ret == 0)
      {
         /* We will have to free our entry in the SRQ */
         free_srq_entries(n1_dev, ucode_idx, srq_idx, p);
      }
      else
      {
         /* Our request timed out! :-(. We still need to remove our
          * entry from the SRQ, but make sure that no core is
          * looking at our instruction */
          del_srq_entry(n1_dev, ucode_idx, srq_idx, p);
      }
   }
#endif /*NPLUS*/
   if (!ret)
      ret = check_completion_code(p);

   cavium_dbgprint( "Completion code = %08lx%08lx\n", (Uint32)(*p>>32), (Uint32)(*p));
   return ret;

}


/* 
 *Direct operation setup 
 */
int pkp_setup_direct_operation(cavium_device *pdev,
      Csp1OperationBuffer *csp1_operation, 
      struct PKP_DIRECT_OPERATION_STRUCT *pkp_direct_operation)
{
   volatile Uint64 *completion_address;
   pkp_direct_operation->ctx = csp1_operation->ctx_ptr;

   pkp_direct_operation->dptr  = csp1_operation->inptr[0];
   pkp_direct_operation->dlen = csp1_operation->dlen;
   pkp_direct_operation->dptr_baddr = 
      (Uint64)cavium_map_kernel_buffer(pdev, 
                   pkp_direct_operation->dptr,
                   pkp_direct_operation->dlen,
                   CAVIUM_PCI_DMA_BIDIRECTIONAL);
 
   cavium_flush_cache(pdev, 
         pkp_direct_operation->dlen,
         pkp_direct_operation->dptr,
         pkp_direct_operation->dptr_baddr, 
         CAVIUM_PCI_DMA_BIDIRECTIONAL);

   pkp_direct_operation->rptr   = csp1_operation->outptr[0];
   pkp_direct_operation->rlen = csp1_operation->rlen;
   pkp_direct_operation->rptr_baddr = 
      (Uint64)cavium_map_kernel_buffer(pdev, 
                   pkp_direct_operation->rptr,
                   pkp_direct_operation->rlen,
                   CAVIUM_PCI_DMA_BIDIRECTIONAL);

   pkp_direct_operation->completion_address = (volatile Uint64 *)
   ((ptrlong)(pkp_direct_operation->rptr) + (pkp_direct_operation->rlen));

   completion_address = pkp_direct_operation->completion_address;
   *completion_address = COMPLETION_CODE_INIT;

   cavium_flush_cache(pdev, 
         COMPLETION_CODE_SIZE,
         (ptrlong)completion_address,
         pkp_direct_operation->rptr_baddr+pkp_direct_operation->rlen, 
         CAVIUM_PCI_DMA_BIDIRECTIONAL);
   return 0;

}/* pkp_setup_direct_operation */




/*
 * Scatter/Gather functions 
 */
int pkp_setup_sg_operation(cavium_device *pdev, 
            Csp1OperationBuffer * csp1_operation, 
            struct PKP_SG_OPERATION_STRUCT * pkp_sg_operation)
{
   Uint32 in_component, out_component, rem_ptr, comp,j;
   int i,test=0;
   volatile struct PKP_4_SHORTS *p;
   volatile struct CSP1_SG_LIST_COMPONENT *sg_comp;
   volatile Uint64 *completion_address;

   comp=0;
   memset(pkp_sg_operation, 0, sizeof(struct PKP_SG_OPERATION_STRUCT));

        pkp_sg_operation->ctx = csp1_operation->ctx_ptr;
   pkp_sg_operation->incnt = csp1_operation->incnt;
   pkp_sg_operation->outcnt = csp1_operation->outcnt;

   /* map all user input buffers */
   for(j=0;j<pkp_sg_operation->incnt; j++)
   {
      pkp_sg_operation->inbuffer[j].size = csp1_operation->insize[j];
      pkp_sg_operation->inbuffer[j].vaddr = (ptrlong)csp1_operation->inptr[j];
      pkp_sg_operation->inbuffer[j].baddr = 
         cavium_map_kernel_buffer(pdev,csp1_operation->inptr[j], \
                                       csp1_operation->insize[j], \
                                       CAVIUM_PCI_DMA_BIDIRECTIONAL);
      
      pkp_sg_operation->inunit[j] = csp1_operation->inunit[j];

      cavium_dbgprint("sg:%ld:size=%ld, ptr=0x%lx\n",j, pkp_sg_operation->inbuffer[j].size, \
                                                    pkp_sg_operation->inbuffer[j].vaddr);
      if(cavium_debug_level > 2)
      {
         cavium_dump("data", csp1_operation->inptr[j], csp1_operation->insize[j]);
      }
    }

   /* map all user output buffers */
   for(j=0;j<pkp_sg_operation->outcnt; j++)
   {
      pkp_sg_operation->outbuffer[j].size = csp1_operation->outsize[j];
      pkp_sg_operation->outbuffer[j].vaddr = (ptrlong)csp1_operation->outptr[j];
      pkp_sg_operation->outbuffer[j].baddr =
         cavium_map_kernel_buffer(pdev,csp1_operation->outptr[j], \
                                       csp1_operation->outsize[j], \
                                       CAVIUM_PCI_DMA_BIDIRECTIONAL);

      pkp_sg_operation->outunit[j] = csp1_operation->outunit[j];
      cavium_dbgprint("%ld:size=%ld, ptr=0x%lx\n",j, pkp_sg_operation->outbuffer[j].size, \
                                                     pkp_sg_operation->outbuffer[j].vaddr);
    }


   /* first do the input/gather side */
   if(csp1_operation->incnt != 0)
   {
      check_endian_swap(pkp_sg_operation, CAVIUM_SG_READ);
      pkp_sg_operation->gather_list_size = csp1_operation->incnt;
      in_component = (csp1_operation->incnt + 3)/4;
   }
   else
   {
      pkp_sg_operation->gather_list_size=1;
      in_component = 1;
   }
   
   /* now do output/scatter side*/
   if(csp1_operation->outcnt != 0)
   {
      /* No need to do check_endian_swap on output side right now. 
      That would be done after the request is completed*/

      pkp_sg_operation->scatter_list_size = csp1_operation->outcnt;
      out_component = ( csp1_operation->outcnt + 3)/4;
   }
   else
   {
      pkp_sg_operation->scatter_list_size=1;
      out_component = 1;
   }

   /* now allocate memory for DMA'able scatter/gather buffer */
   test=0;
   pkp_sg_operation->sg_dma_size = 8 + ( (in_component + out_component) * 40);
   pkp_sg_operation->sg_dma = (volatile Uint64 *)get_sg_dma(&test);
   if(test)
   {
      cavium_print("Unable to create pkp_sg_dma\n");
      /* unmap all buffers */
      pkp_unmap_user_buffers(pdev, pkp_sg_operation);
      return 1;
   }

   if(!pkp_sg_operation->sg_dma)
   {
      cavium_print("Unable to get sg_dma_virt address\n");
      /* unmap all buffers */
      pkp_unmap_user_buffers(pdev, pkp_sg_operation);
      return 1;
   }

   /* map sg_dma buffer*/
   pkp_sg_operation->sg_dma_baddr = 
      cavium_map_kernel_buffer(pdev, (Uint8 *)pkp_sg_operation->sg_dma,
            pkp_sg_operation->sg_dma_size,
            CAVIUM_PCI_DMA_TODEVICE);
   
   p = (volatile struct PKP_4_SHORTS *)pkp_sg_operation->sg_dma;
   
   p->short_val[0] = 0;
   p->short_val[1] = 0;
   
   p->short_val[2] = pkp_sg_operation->gather_list_size;
   p->short_val[3] = pkp_sg_operation->scatter_list_size;

   sg_comp = (volatile struct CSP1_SG_LIST_COMPONENT *)((volatile Uint8 *)(pkp_sg_operation->sg_dma) + 8);
  
  /* now we have the starting point to all gather and then scatter components */
   if(pkp_sg_operation->incnt)
   {
      rem_ptr = pkp_sg_operation->incnt%4;
      comp = 0;
      
      for(i=0; i<(int)(pkp_sg_operation->incnt/4); i++)
      {
         for(j=0; j<4; j++)
         {
            sg_comp[i].length[j] = (Uint16)pkp_sg_operation->inbuffer[comp].size;
            sg_comp[i].ptr[j] = (Uint64)pkp_sg_operation->inbuffer[comp].baddr;
            comp++;
         }
      }
      
      /* now copy the remaining pointers*/
      for(j=0; j<rem_ptr; j++)
      {
         sg_comp[i].length[j] = (Uint16)pkp_sg_operation->inbuffer[comp].size;
         sg_comp[i].ptr[j] = (Uint64)pkp_sg_operation->inbuffer[comp].baddr;
         comp++;
      }
   }
   else
   {
      /*since there is no incnt so I will set all pointers and all lengths to zero */
      for(j=0; j<4; j++)
      {
         sg_comp[0].length[j] = 0;
         sg_comp[0].ptr[j] = 0;
      }
   }


   /* now make sg_comp point to scatter components*/
   sg_comp = &sg_comp[in_component];

   /* repeat the same steps for scatter compnents */
   if(pkp_sg_operation->outcnt)
   {
      rem_ptr = pkp_sg_operation->outcnt%4;
      comp=0;
      
      for(i=0; i<(int)(pkp_sg_operation->outcnt/4); i++)
      {
         for(j=0; j<4; j++)
         {
            sg_comp[i].length[j] = (Uint16)pkp_sg_operation->outbuffer[comp].size;
            sg_comp[i].ptr[j] = (Uint64)pkp_sg_operation->outbuffer[comp].baddr;
            comp++;
         }
      }
      
      /* now copy the remaining pointers*/
      for(j=0; j<rem_ptr; j++)
      {
         sg_comp[i].length[j] = (Uint16)(Uint16)pkp_sg_operation->outbuffer[comp].size;
         sg_comp[i].ptr[j] = (Uint64)pkp_sg_operation->outbuffer[comp].baddr;
         comp++;
      }
   }
   else
   {
      /*since there is no incnt so I will set all pointers and all lengths to zero */
      for(j=0; j<4; j++)
      {
         sg_comp[0].length[j] = 0;
         sg_comp[0].ptr[j] = 0;
      }
   }

   /* Format scatter/gather list depending upon system endianness. */
   pkp_convert_sg_to_int_format(pkp_sg_operation);

   /* Now setup completion code */
   pkp_sg_operation->completion_dma = (volatile Uint64 *)get_completion_dma(&test);
   if(test)
   {
      cavium_print("Unable to allocate pkp_completion_dma\n");
      cavium_unmap_kernel_buffer(pdev, 
            pkp_sg_operation->sg_dma_baddr,
            pkp_sg_operation->sg_dma_size,
            CAVIUM_PCI_DMA_TODEVICE);
      put_sg_dma((volatile Uint8 *)pkp_sg_operation->sg_dma);
      pkp_sg_operation->sg_dma=NULL;
      /* unmap all buffers */
      pkp_unmap_user_buffers(pdev, pkp_sg_operation);
      return 1;
   }

   /* flush all user buffers. */
   pkp_flush_input_buffers(pdev,pkp_sg_operation);

   /* flush sg */
   cavium_flush_cache(pdev, pkp_sg_operation->sg_dma_size, 
      (Uint8 *)pkp_sg_operation->sg_dma,
      pkp_sg_operation->sg_dma_baddr,
      CAVIUM_PCI_DMA_BIDIRECTIONAL);

   completion_address = (volatile Uint64 *)pkp_sg_operation->completion_dma;
   *completion_address = COMPLETION_CODE_INIT;
   /* no need of flushing completion_dma because it was allocated i
    * from non-cached memory*/

   if(cavium_debug_level > 1)
      cavium_dump("scatter_gather", (volatile Uint8 *)pkp_sg_operation->sg_dma,    \
                                    pkp_sg_operation->sg_dma_size);

   return 0;

}/* pkp_setup_sg_operation */




void check_endian_swap(   struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation, int rw)
{
   int i;
   volatile Uint64 *p;
   Uint8 temp[8];

   if(rw == CAVIUM_SG_READ)
   {
      if(pkp_sg_operation->incnt)
      {
         for(i=0; i<pkp_sg_operation->incnt; i++)
         {
            if((pkp_sg_operation->inbuffer[i].size) && (pkp_sg_operation->inunit[i] == UNIT_64_BIT))
            {
               if(pkp_sg_operation->inbuffer[i].size != 8)
               {
                  cavium_print("pkp_check_endian_swap: got UNIT_64_BIT but more than 8 bytes\n");
                  continue;
               }
               else
               {
                  cavium_memcpy(temp, (Uint8 *)pkp_sg_operation->inbuffer[i].vaddr, 8);
                  p = (volatile Uint64 *)temp;
                  *p = htobe64(*p);
                  cavium_memcpy((Uint8*)pkp_sg_operation->inbuffer[i].vaddr,temp,8);
               }
            } /*if UINIT_64_BIT */
         }/* for incnt*/
      } /* if incnt */  
   } /* READ */
   else
   {
     if(pkp_sg_operation->outcnt)
    {
       for(i=0; i<pkp_sg_operation->outcnt; i++)
       {
          if((pkp_sg_operation->outbuffer[i].size) && (pkp_sg_operation->outunit[i] == UNIT_64_BIT))
          {
             if(pkp_sg_operation->outbuffer[i].size != 8)
             {
                cavium_print("pkp_check_endian_swap: got UNIT_64_BIT but more than 8 bytes\n");
                continue;
             }
             else
             {
                cavium_memcpy(temp, (Uint8 *)pkp_sg_operation->outbuffer[i].vaddr, 8);
                p = (volatile Uint64 *)temp;
                *p = htobe64(*p);
                cavium_memcpy((Uint8 *)pkp_sg_operation->outbuffer[i].vaddr,temp,8);
             }
          } /*if UNIT_64_BIT */
       }/* for outcnt*/
    } /* if outcnt */  
   } /* if WRITE */
}/*check_endian_swap*/


void pkp_convert_sg_to_int_format(struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation)
{
 volatile Uint64 *p;
 Uint16 i, gather_count, scatter_count;
 volatile struct PKP_4_SHORTS *short_list;
 volatile Uint64 *pkp_sg_dma;


 pkp_sg_dma = pkp_sg_operation->sg_dma;


 short_list = (volatile struct PKP_4_SHORTS *)pkp_sg_dma;

 gather_count = short_list->short_val[2];
 scatter_count = short_list->short_val[3];


 p = (volatile Uint64 *)pkp_sg_dma;

 /* first do the header */
#if __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
  *p = SWAP_SHORTS_IN_64(*p);
#endif

 /* now loop through each component and convert to integer format */

  p = (volatile Uint64 *)((pkp_sg_dma + 1));

 for(i=0; i<((gather_count +3)/4+ (scatter_count+3)/4); i++)
  {
#if __CAVIUM_BYTE_ORDER == __CAVIUM_LITTLE_ENDIAN
   *p = SWAP_SHORTS_IN_64(*p);
#endif

   p++;
   
   *p = htobe64(*p);
   p++;

   *p = htobe64(*p);
   p++;

   *p = htobe64(*p);
   p++;
  
   *p = htobe64(*p);
   p++;
  } 

}/* pkp_convert_sg_to_int_format */


/*
 * Unmap all inpout and output buffers provided by the application
 */
void pkp_unmap_user_buffers(cavium_device *pdev,struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation)
{
   int j;

   /* unmap all user input buffers */
   for(j=0;j<pkp_sg_operation->incnt; j++)
   {
      cavium_unmap_kernel_buffer(pdev,pkp_sg_operation->inbuffer[j].baddr, \
                                 pkp_sg_operation->inbuffer[j].size, \
                                 CAVIUM_PCI_DMA_BIDIRECTIONAL);
      pkp_sg_operation->inbuffer[j].baddr = 0;
    }

   /* unmap all user output buffers */
   for(j=0;j<pkp_sg_operation->outcnt; j++)
   {
      cavium_unmap_kernel_buffer(pdev,pkp_sg_operation->outbuffer[j].baddr, \
                                 pkp_sg_operation->outbuffer[j].size,       \
                                 CAVIUM_PCI_DMA_BIDIRECTIONAL);
      pkp_sg_operation->inbuffer[j].baddr = 0;
    }

}/*pkp_unmap_user_buffers*/


/*
 * Flushed the contents of all user buffers.
 */
void 
pkp_flush_input_buffers(cavium_device *pdev,struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation)
{
   int j;
   Uint32 size;
   ptrlong  vaddr, baddr;

   /* flush all user input buffers */
   for(j=0;j<pkp_sg_operation->incnt; j++)
   {
      size = pkp_sg_operation->inbuffer[j].size;
      vaddr = pkp_sg_operation->inbuffer[j].vaddr;
      baddr = pkp_sg_operation->inbuffer[j].baddr;
      cavium_flush_cache(pdev,size,vaddr,baddr,CAVIUM_PCI_DMA_BIDIRECTIONAL);
    }
} /* pkp_flush_input_buffers */

void 
pkp_invalidate_output_buffers(cavium_device *pdev,struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation)
{
   int j;
   Uint32 size;
   ptrlong vaddr, baddr;

   /* flush all user output buffers */
   for(j=0;j<pkp_sg_operation->outcnt; j++)
   {
      size = pkp_sg_operation->outbuffer[j].size;
      vaddr = pkp_sg_operation->outbuffer[j].vaddr;
      baddr = pkp_sg_operation->outbuffer[j].baddr;
      cavium_invalidate_cache(pdev,size,vaddr,baddr,CAVIUM_PCI_DMA_BIDIRECTIONAL);
    }

}/*pkp_invalidate_output_buffers*/

/*
 * $Id: cavium.c,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium.c,v $
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
 * Revision 1.19  2005/10/24 06:51:36  kanantha
 * - Fixed RHEL4 warnings
 *
 * Revision 1.18  2005/10/13 09:19:32  ksnaren
 * fixed compile warnings
 *
 * Revision 1.17  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.16  2005/09/06 07:11:23  ksadasivuni
 * - Merging FreeBSD 4.11 release with CVS Head
 *
 * Revision 1.15  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.14  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.13  2004/07/21 23:24:41  bimran
 * Fixed MC2 completion code issues on big endian systems.
 *
 * Revision 1.12  2004/07/09 01:08:59  bimran
 * fixed scatter gather support
 *
 * Revision 1.11  2004/06/28 20:37:42  tahuja
 * Fixed compiler warnings on NetBSD. changed mdelay in check_completion from 1ms to 2ms.
 *
 * Revision 1.10  2004/06/23 19:39:22  bimran
 * changed check_completion to accept volatile comp_addr
 *
 * Revision 1.9  2004/06/23 19:29:23  bimran
 * Fixed compiler warnings on NetBSD.
 * changed READ to CAVIUM_SG_READ.
 *
 * Revision 1.8  2004/06/03 21:21:58  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.7  2004/06/01 17:43:28  bimran
 * changed check_completion to not to sleep.
 *
 * Revision 1.6  2004/05/11 03:10:24  bimran
 * some performance opt.
 *
 * Revision 1.5  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.4  2004/04/21 19:18:57  bimran
 * NPLUS support.
 *
 * Revision 1.3  2004/04/17 00:59:32  bimran
 * fixed check completion to sleep instead of busy looping.
 *
 * Revision 1.2  2004/04/16 03:17:50  bimran
 * removed a print.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

