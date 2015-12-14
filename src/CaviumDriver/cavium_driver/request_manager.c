////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: request_manager.c
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
#include "request_manager.h"
#include "command_que.h"
#include "pending_free_list.h"
#include "pending_list.h"
#include "direct_free_list.h"
#include "sg_free_list.h"
#include "context_memory.h"
#include "completion_dma_free_list.h"
#include "init_cfg.h"
#include "buffer_pool.h"
#include "bl_nbl_list.h"
#include "hw_lib.h"
#ifdef NPLUS
#include "soft_req_queue.h"
#endif


#ifdef INTERRUPT_ON_COMP
extern int pending_count;
#endif

#ifdef NPLUS
int send_command(cavium_device *n1_dev, Request *request, int queue, int ucode_idx, Uint64 *ccptr)
#else
int
send_command(cavium_device *n1_dev, Request *request, int queue)
#endif
{
   int ret=0;
   Uint8 * command;
#ifdef NPLUS
   struct MICROCODE *microcode = &(n1_dev->microcode[ucode_idx]);   
#endif /*NPLUS*/

   if (cavium_debug_level > 1) 
   {
      cavium_dump("Request:", (Uint8 *)request,32);
   }
#ifdef NPLUS
   /* For SPM code, we have to queue the req to the CTP */
   if(microcode->code_type == CODE_TYPE_SPECIAL)
   {
      cavium_dbgprint("send_cmd: add srq entry(ucode idx=%d)\n",ucode_idx);

      /* Queue the request to the SRQ */
      if (queue == HIGH_PRIO_QUEUE) 
      {
         ret = add_srq_entry(n1_dev, microcode, request, ccptr,1);
      }
      else
         ret = add_srq_entry(n1_dev, microcode, request, ccptr,0);
      /* Attempt to move entries to the CTP */
      move_srq_entries(n1_dev, ucode_idx, CTP_QUEUE_SIZE);
      if(ret<0)
      {
         cavium_dbgprint("add_srq_entry failed in send_request\n");
         return(-1);
      }

   }
   else
   {
      cavium_dbgprint("send_cmd: It is not a SPM (id=%d)\n", ucode_idx);   
#endif /*NPLUS*/

   /* Send command to the chip */
      lock_command_queue(n1_dev, queue);
      command = (Uint8 *)(n1_dev->command_queue_front[queue]);
      cavium_memcpy(command, (Uint8 *)request, COMMAND_BLOCK_SIZE);
      inc_front_command_queue(n1_dev, queue);

   /* doorbell coalescing */
      n1_dev->door_bell_count[queue]++;
      if((n1_dev->door_bell_count[queue] >= n1_dev->door_bell_threshold[queue])
#ifdef INTERRUPT_ON_COMP
            ||(pending_count < 32)
#endif
      )
      {
         cavium_dbgprint("send command: hitting doorbell: %ld\n", n1_dev->door_bell_count[queue]);
         ring_door_bell(n1_dev, queue, n1_dev->door_bell_count[queue]);
         n1_dev->door_bell_count[queue]=0;
      }

      unlock_command_queue(n1_dev, queue);
#ifdef NPLUS
   }
#endif
   return ret;
}

int
do_cavium_request(cavium_device * n1_dev, n1_request_buffer *req, Uint32 *req_id)
{

   int ret = 0;
   struct PENDING_ENTRY *pending_entry = NULL;
   struct PKP_DIRECT_OPERATION_STRUCT *pkp_direct_operation = NULL;
   struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation = NULL;
   volatile Uint64 *completion_address;
   Cmd *strcmd;
   Request request;

/*#define SSH_REALTIME*/
#ifdef SSH_REALTIME
   if (req->res_order == CAVIUM_RESPONSE_REALTIME)
   {
#define MAX_REALTIME_WAIT 100000
      Uint64 *comp_addr;
      Uint8* cmp;
      int i;

      strcmd= (Cmd*)&request;
      strcmd->opcode = htobe16(req->opcode);
      strcmd->size = htobe16(req->size);
      strcmd->param = htobe16(req->param);
      strcmd->dlen = htobe16(req->dlen);

      /* Setup dptr */
      if (req->inptr[0])
         request.dptr = htobe64((Uint64)(Uint32)cavium_vtophys(req->inptr[0]));
      else
         request.dptr = 0;

      /* Setup rptr */
      request.rptr = htobe64((Uint64)(Uint32)cavium_vtophys(req->outptr[0]));
      comp_addr = (Uint64 *)((ptrlong)(req->outptr[0]) + (req->rlen));
      *comp_addr = COMPLETION_CODE_INIT;

      /* Setup cptr */
      if (req->ctx_ptr)
         request.cptr = htobe64((ptrlong)req->ctx_ptr);
      else
         request.cptr = 0;

      cavium_dbgprint("request.cptr=%p\n",request.cptr);
      lock_command_queue(n1_dev, req->req_queue);

      cavium_memcpy(
         (Uint8 *)(n1_dev->command_queue_front[req->req_queue]), 
         (Uint8 *)&request, 
         COMMAND_BLOCK_SIZE);
      inc_front_command_queue(n1_dev, req->req_queue);

      cavium_dbgprint("send command: hitting doorbell: %d\n", \
                                 n1_dev->door_bell_count[req->req_queue]+1);
      ring_door_bell(n1_dev, req->req_queue, \
                     n1_dev->door_bell_count[req->req_queue]+1);
      n1_dev->door_bell_count[req->req_queue]=0;

      unlock_command_queue(n1_dev, req->req_queue);
      
      cmp = (Uint8*)comp_addr;
      i = 0;

      while((*cmp == 0xff)&&(i++ < MAX_REALTIME_WAIT))
         cavium_udelay(100);

      return (Uint32)((*cmp) & 0x1f);
}
#endif /* SSH_REALTIME */

   pkp_direct_operation = NULL;
   pkp_sg_operation = NULL;

   pending_entry = get_pending_entry(n1_dev, req_id);
   if (pending_entry == NULL) 
   {
		//cavium_dbgprint("do_cavium_request: pending entry list full\n"); // comment by liqin
        //check_for_completion_callback(n1_dev);					// comment by liqin
		printk("do_cavium_request: pending entry list full\n"); 			// added by liqin
        ret = ERR_MEMORY_ALLOC_FAILURE;
        return ret;
   }

   req->time_in = cavium_jiffies;
   strcmd = (Cmd *)&request.cmd;

   switch(req->dma_mode) 
   {
      case CAVIUM_DIRECT:
      {
         cavium_dbgprint("CAVIUM_DIRECT\n");
         /* Get a direct operation struct from free pool */
         pkp_direct_operation = get_direct_entry(n1_dev);
         if (pkp_direct_operation == NULL) 
         {
            //cavium_dbgprint("do_cavium_request: direct entry entry list full\n"); 	// comment by liqin
            printk("do_cavium_request: direct entry entry list full\n"); 			// added by liqin
            ret = ERR_MEMORY_ALLOC_FAILURE;
            goto cleanup_direct;
         }

         /* Setup direct operation -- fill in {d,r,c}ptr */
         pkp_setup_direct_operation(n1_dev,req, pkp_direct_operation);

         /* write completion address of all 1's */
         completion_address = pkp_direct_operation->completion_address;

         /* 
          * Build the 8 byte command(opcode,size,param,dlen) 
          * and put it in the request structure
          */
#ifdef INTERRUPT_ON_COMP
         strcmd->opcode = htobe16((req->opcode | (0x1 << 15)));
#else
         strcmd->opcode = htobe16(req->opcode);
#endif
         strcmd->size = htobe16(req->size);
         strcmd->param = htobe16(req->param);
         strcmd->dlen = htobe16(pkp_direct_operation->dlen);

         /* Setup dptr */   
         if (pkp_direct_operation->dptr) 
         {
            request.dptr = htobe64(pkp_direct_operation->dptr_baddr);
         } 
         else
         {
            request.dptr = 0;
         }
         
         /* Setup rptr */
         request.rptr = htobe64(pkp_direct_operation->rptr_baddr);

         /* Setup cptr */ 
         if (pkp_direct_operation->ctx) 
         {
            if (!n1_dev->dram_present) 
            {
               /*   request.cptr = htobe64((Uint64)(cavium_vtophys((Uint32 *)(ptrlong)(pkp_direct_operation->ctx))));*/
               /* No longer necessary as the pkp_dev->ctx_free_list already has physical addresses in init_context */
               request.cptr = htobe64((ptrlong)(pkp_direct_operation->ctx));
            } 
            else 
            {
               request.cptr = htobe64(pkp_direct_operation->ctx);
            }
         } 
         else 
         {
            request.cptr = 0;
         }
         
         pending_entry->pkp_operation = (void *)pkp_direct_operation;
         if(cavium_debug_level > 2)
#ifdef MC2
            cavium_dump("dptr", pkp_direct_operation->dptr, pkp_direct_operation->dlen);
#else
            cavium_dump("DPTR", pkp_direct_operation->dptr, pkp_direct_operation->dlen*8);
#endif
         break;
      }
      case CAVIUM_SCATTER_GATHER:
      {
         /* 
          * Get a scatter/gather operation struct from free pool 
          */
         /* 
          * to scatter/gather module 
          */
         pkp_sg_operation = get_sg_entry(n1_dev);
         if (pkp_sg_operation == NULL) 
         {
            ret = ERR_MEMORY_ALLOC_FAILURE;
            // cavium_dbgprint("do_cavium_request: sg_entry list full\n"); // comment by liqin
            printk("do_cavium_request: sg_entry list full\n"); 			// added by liqin
            goto cleanup_sg;
         }

         /* Setup scatter/gather list */
         if (pkp_setup_sg_operation(n1_dev, req,
                     pkp_sg_operation)) 
         {
            ret = ERR_SCATTER_GATHER_SETUP_FAILURE;
            goto cleanup_sg;
         }

         cavium_dbgprint("do_req: completion address = %p\n", pkp_sg_operation->completion_dma);

         /* write completion address of all 1's */
         completion_address = (volatile Uint64 *)(pkp_sg_operation->completion_dma);

         /* 
          * Build the 8 byte command(opcode,size,param,dlen) 
          * and put it in the request structure
          */
         cavium_dbgprint("do_req: building command\n");
         strcmd->opcode = htobe16((req->opcode|(1 << 7)));
         strcmd->size = htobe16(req->size);
         strcmd->param = htobe16(req->param);
         strcmd->dlen = 
            htobe16((8 +
           (((pkp_sg_operation->gather_list_size + 3)/4
           +(pkp_sg_operation->scatter_list_size + 3)/4) * 40)));
#ifndef MC2
         strcmd->dlen = strcmd->dlen>>3;
#endif

         /* Setup dptr */
         cavium_dbgprint("do_req: setting up dptr\n");
         request.dptr = (Uint64)(Uint32)cavium_vtophys((volatile void*) (pkp_sg_operation->sg_dma));
         request.dptr = htobe64(request.dptr);

         /* Setup rptr */
         cavium_dbgprint("do_req: setting up rptr\n");
         request.rptr 
         = htobe64((Uint64)get_completion_dma_bus_addr(
            (volatile Uint64 *)(pkp_sg_operation->completion_dma)));
         
         /* Setup cptr */
         cavium_dbgprint("do_req: setting up cptr\n");
         if (pkp_sg_operation->ctx) 
         {
            if (!n1_dev->dram_present) 
            {
            /*      
               request.cptr 
               = htobe64((Uint64)(cavium_vtophys((Uint32 *)(ptrlong)(pkp_sg_operation->ctx))));
            
            */   
               request.cptr = htobe64((ptrlong)pkp_sg_operation->ctx);
            } 
            else 
            {
               request.cptr = htobe64(pkp_sg_operation->ctx);
            }
                   
         } 
         else 
         {
            request.cptr = 0;
         }

         pending_entry->pkp_operation = (void *)pkp_sg_operation;

         break;
      }
      default:
         cavium_error("Unknown dma mode\n");
         ret = ERR_INVALID_COMMAND;
         return ret;
   }

   /* Send the command to the chip */

   
   pending_entry->dma_mode = req->dma_mode;
   pending_entry->completion_address = (ptrlong)completion_address;
   pending_entry->tick = cavium_jiffies;
   pending_entry->status = ERR_REQ_PENDING;
   pending_entry->callback = req->callback;
   pending_entry->cb_arg = req->cb_arg;
   pending_entry->n1_buf = req;
   cavium_dbgprint("do_cavium_request: calling send_command()\n");
#ifdef NPLUS
   cavium_dbgprint("do_cavium_request: ucode_idx = %d\n", req->ucode_idx);
        pending_entry->ucode_idx = req->ucode_idx;
    pending_entry->srq_idx = 
      send_command(n1_dev, &request, req->req_queue, req->ucode_idx, (Uint64 *)completion_address);
   if (pending_entry->srq_idx < 0) 
   {
      ret = pending_entry->srq_idx;
      goto cleanup_direct;
   }
#else
   send_command(n1_dev, &request, req->req_queue); 	
#endif

   push_pending(pending_entry, req->res_order);

   // check_for_completion_callback(n1_dev); // comment by liqin
   ret = 0;

   return ret;

cleanup_direct:
   //check_for_completion_callback(n1_dev); // comment by liqin
   if (pkp_direct_operation)
      put_direct_entry(pkp_direct_operation);
   if (pending_entry)
      put_pending_entry(pending_entry);

   return ret;

cleanup_sg:
   //check_for_completion_callback(n1_dev); // comment by liqin
   if (pkp_sg_operation) 
   {
      if (pkp_sg_operation->completion_dma)
         put_completion_dma((volatile Uint64 *)(pkp_sg_operation->completion_dma));
      put_sg_entry(pkp_sg_operation);
   }
   if (pending_entry)
      put_pending_entry(pending_entry);

   return ret;
}

/*
 * n1_operation_buffer = n1_request_buffer + blocking/non-blocking 
 *                   operation. 
 * 
 * 
 * Fill in callback fn and its args. 
 * 
 * Call do_cavium_request()
 * 
 * If it is a blocking operation (n1_op->response_type),
 *    wait till the result appears or timeout
 * else
 *    return
 *
 * Questions::
 *    Why no scatter-gather?
 */

int
do_operation(cavium_device * n1_dev, n1_operation_buffer *n1_op)
{
   
   n1_user_info_buffer *user_info = NULL;
   n1_request_buffer *req = NULL;
   Uint32 dlen, rlen, req_id;
   Uint8 *in_buffer = NULL, *out_buffer = NULL;
   Uint64 *p;
   int total_size, mapped = 0;
   Uint32 i;
   int ret=0;
#ifdef NPLUS
   struct MICROCODE *microcode;
#endif


   user_info = (n1_user_info_buffer *)get_buffer_from_pool(n1_dev,
                  sizeof(n1_user_info_buffer));

   if (user_info == NULL) 
   {
      cavium_error(" OOM for user_info buffer\n");
      ret = 1;
      goto do_op_clean;
   }

   req = (n1_request_buffer *)get_buffer_from_pool(n1_dev,
                  sizeof(n1_request_buffer));

   if (req == NULL) 
   {
      cavium_error(" OOM for n1_request_buffer buffer\n");
      ret = 1;
      goto do_op_clean;
   }

   /* check for the modexp capability */
#ifdef NPLUS
   /* check for the number of cores assigned to this microcode index.
    * If not enough cores are present, then return error.
    */
   if((n1_op->opcode & 0x00ff) == MAJOR_OP_ME_PKCS_LARGE || 
                 (n1_op->opcode & 0x00ff) == MAJOR_OP_RSASERVER_LARGE)
   {
      /* It is a large operation. check core mask */ 
      unsigned long core_mask;

      core_mask = get_core_mask(n1_dev,n1_op->ucode_idx);
      if(get_core_pair(n1_dev, core_mask))
      {
         ret = ERR_OPERATION_NOT_SUPPORTED;
         goto do_op_clean;
      }
   } 
       
   
#else /* not NPLUS */

   /* Check for the Part number.
    * If part has less cores than needed, then return error.
    */
#if defined(CN501) || defined(CN1001)
         if((n1_op->opcode & 0x00ff) == MAJOR_OP_ME_PKCS_LARGE || 
       (n1_op->opcode & 0x00ff) == MAJOR_OP_RSASERVER_LARGE)
    {
      ret = ERR_OPERATION_NOT_SUPPORTED;
      goto do_op_clean;
    }
#endif /* part number */
   
#endif  /* NPLUS */


#ifndef CAVIUM_NO_MMAP
   if (n1_op->dma_mode == CAVIUM_DIRECT) 
   {
      if((n1_op->opcode & 0x00ff) == 
            MAJOR_OP_ENCRYPT_DECRYPT_RECORD) 
      {
           if(((ptrlong)n1_op->inptr[0] & 0x7) ||
                  ((ptrlong)n1_op->outptr[0] & 0x7) ||
                  (n1_op->incnt > 1) || 
                  (n1_op->outcnt > 1)) 
           {
              mapped = 0;
           } 
           else 
           {
              mapped = 1;
           }
      }
   } 
   else 
   {
      mapped = 0;
   }
#else
   mapped = 0;
#endif


#ifndef MC2
   dlen = n1_op->dlen * 8;
   rlen = n1_op->rlen * 8;
#else
   dlen = n1_op->dlen;
   rlen = ROUNDUP8(n1_op->rlen + 1);
#endif

   if (mapped) 
   {
      if(dlen)
      {
         in_buffer = cavium_get_kernel_address(n1_op->inptr[0]);
         if (in_buffer == NULL) 
         {
            ret = 1;
            goto do_op_clean;
         }
      }
      else
         in_buffer = NULL;
      
      out_buffer = cavium_get_kernel_address(n1_op->outptr[0]);
      if (out_buffer == NULL) 
      {
         ret = 1;
         goto do_op_clean;
      }

      goto build_user_info;
   }

   /* Not mapped */

   if (dlen) 
   {
      in_buffer = get_buffer_from_pool(n1_dev, dlen);
      if (in_buffer == NULL) 
      {
         cavium_error(" In buffer allocation failure\n");
         ret = 1;
         goto do_op_clean;
      }
         
      total_size = 0;
      for (i = 0; i < n1_op->incnt; i++) 
      {
         if(cavium_copy_in(&in_buffer[total_size], n1_op->inptr[i], 
               n1_op->insize[i]))
         {
            cavium_error("Failed to copy in user buffer=%ld, size=%ld\n",
                          i,n1_op->insize[i]);
            ret = 1;
            goto do_op_clean;
         }
         if (n1_op->inunit[i] == UNIT_64_BIT) 
         {
            p = (Uint64 *)&in_buffer[total_size];
            *p = htobe64(*p);
         }
         total_size += n1_op->inoffset[i];
      }
   }

   if (rlen) 
   {
      out_buffer = get_buffer_from_pool(n1_dev, rlen);
      if (out_buffer == NULL) 
      {
         cavium_print(" Out buffer allocation failure\n");
         ret = 1;
         goto do_op_clean;
      }
   }
   
build_user_info:
   /* Build user info buffer */
   user_info->n1_dev = n1_dev;
   user_info->req_type = n1_op->req_type;
   user_info->in_buffer = in_buffer;
   user_info->out_buffer = out_buffer;
   user_info->in_size = dlen;
   user_info->out_size = rlen;
   user_info->pid = cavium_get_pid();
   user_info->signo = CAVIUM_SIGNAL_NUM;
   user_info->mmaped = mapped;

   /* user mode pointers and request buffer*/
   user_info->outcnt = n1_op->outcnt;
   for (i = 0; i < user_info->outcnt; i++) 
   {
      user_info->outptr[i] = n1_op->outptr[i];
      user_info->outsize[i] = n1_op->outsize[i];
      user_info->outoffset[i] = n1_op->outoffset[i];
      user_info->outunit[i] = n1_op->outunit[i];
   }

   if (n1_op->req_type == CAVIUM_BLOCKING) 
   {
      cavium_get_channel(&user_info->channel);
   } 

   user_info->req = req;
   user_info->status = 0xff;
   push_user_info(user_info);

   /* Build request buffer */
   req->opcode = n1_op->opcode;
   req->size = n1_op->size;
   req->param = n1_op->param;
#ifdef MC2
   req->dlen = (Uint16) dlen;
   req->rlen = n1_op->rlen;
#else
   req->dlen = (Uint16) (dlen >> 3);
   req->rlen = (Uint16) (rlen - 8);
#endif
   req->reserved = 0;
   req->ctx_ptr = n1_op->ctx_ptr;
   req->incnt = n1_op->incnt;
   req->outcnt = n1_op->outcnt;
   req->inptr[0] = in_buffer;
   req->outptr[0] = out_buffer;
   req->dma_mode = n1_op->dma_mode;
    req->dma_mode = CAVIUM_DIRECT;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->timeout = n1_op->timeout;
   req->req_queue = n1_op->req_queue;
   req->callback = user_scatter;
   req->cb_arg = user_info;
#ifdef NPLUS
   req->ucode_idx = n1_op->ucode_idx;
#endif

   //ret = do_request(n1_dev, req, &req_id);		// comment by liqin 2006/06/09
   ret = do_cavium_request(n1_dev, req, &req_id);	// added by liqin 2006/06/09

   if(ret) 
   {
      cavium_error(" do_cavium_request() failed 0x%x\n", ret);
      goto do_op_clean;
   }

   /* Request id is sent to the application */
   n1_op->request_id = req_id; 
   req->request_id = req_id;

   if (n1_op->req_type == CAVIUM_BLOCKING) 
   {

      cavium_dbgprint("do_operation: blocking call: rptr=0x%p\n", user_info->out_buffer);
#ifdef NPLUS
      microcode = &(n1_dev[0].microcode[n1_op->ucode_idx]);
        /* Attempt to move entries to the CTP, once */
      if(microcode->code_type == CODE_TYPE_SPECIAL)
      {
         move_srq_entries(n1_dev, n1_op->ucode_idx, 0);
      }
#endif
      /* Blocking -- sleep peacefully:) -- when it is time, 
       * poll_thread() (user_scatter to be specific) will wake me 
       * up */
      /* Before sleeping, check for any requests that have been
       * completed, wake them up */
      check_for_completion_callback(n1_dev);
      
      /* Our work might have finished.. then a wake up would already 
       * 've been sent on the channel.. */
      while (user_info->status == 0xff) 
      {
#ifdef SLOW_CPU
         cavium_yield(&(user_info->channel),(10*CAVIUM_HZ)/1000);
#else 
         cavium_sleep_timeout(&(user_info->channel),(10*CAVIUM_HZ)/1000);
#endif 
         check_for_completion_callback(n1_dev);
            /* Wake up every 10ms seconds and check the result */
            /* This is necessary in cases of missed wakeups */
#ifdef NPLUS
            /* Attempt to move entries to the CTP, once */
                if(microcode->code_type == CODE_TYPE_SPECIAL)
                {
                   move_srq_entries(n1_dev, n1_op->ucode_idx, 0);
                }
#endif
      }

      ret = user_info->status;

      cavium_dbgprint("status = 0x%x\n", ret);

      del_user_info_from_list(user_info);
      if (mapped) 
      {
          if (req)
             put_buffer_in_pool(n1_dev, (Uint8 *)req);
         if (user_info) 
            put_buffer_in_pool(user_info->n1_dev, (Uint8 *)user_info);
         return ret;
      }

      if (user_info->out_size) 
      {
         int total_offset;
         if (cavium_debug_level > 2)
            cavium_dump("Response Pkt:", (Uint8 *)user_info->out_buffer, user_info->out_size);
         total_offset = 0;
         for (i = 0; i < user_info->outcnt; i++) 
         {
            if (user_info->outunit[i] == UNIT_64_BIT) 
            {
               p = (Uint64 *)&user_info->out_buffer[total_offset];
               *p = htobe64(*p);
            }
            if(cavium_copy_out(user_info->outptr[i], 
                  &user_info->out_buffer[total_offset], 
                  user_info->outsize[i]))
            {
               cavium_error("Failed to copy out %ld bytes to user buffer 0x%lx\n",
                     user_info->outsize[i], (ptrlong)user_info->outptr[i]);
            }
            total_offset += user_info->outoffset[i];
         }
      }

do_op_clean:      
      if (in_buffer && !mapped)
         put_buffer_in_pool(n1_dev, (Uint8 *)in_buffer);
      if (out_buffer && !mapped)
         put_buffer_in_pool(n1_dev, (Uint8 *)out_buffer);
       if (req)
          put_buffer_in_pool(n1_dev, (Uint8 *)req);
      if (user_info) 
         put_buffer_in_pool(n1_dev, (Uint8 *)user_info);
   } 
   else 
   {
      /* Non-blocking -- nothing else to do, just return */
      ret = ERR_REQ_PENDING;
   }

   return ret;
}


/*
 * Callback function to scatter result to user space pointers.
 */
void 
user_scatter(int status, void *arg)
{
   n1_user_info_buffer *user_info;

   user_info = (n1_user_info_buffer *)arg;
   user_info->status = status;

   if (user_info->req_type == CAVIUM_NON_BLOCKING) 
   {
       /* The user thread will do a poll()/ioctl() to 
       * find out the status */
   } else if (user_info->req_type == CAVIUM_SIGNAL) 
   {
      /* The user thread asked for a signal to be sent */
      cavium_send_signal(user_info->pid, user_info->signo);
   } 
   else 
   {
      /* Blocking call -- the caller is sleeping .. wake him up */
      cavium_dbgprint("user_scatter: waking up: rptr=0x%p\n", user_info->out_buffer);
      cavium_wakeup(&user_info->channel);
   }

   return;
}


/*
 * $Id: request_manager.c,v 1.4 2015/01/06 08:57:48 andy Exp $
 * $Log: request_manager.c,v $
 * Revision 1.4  2015/01/06 08:57:48  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:54  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:28  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:34  chen
 * AOS
 *
 * Revision 1.1.1.1  2007/09/27 06:28:06  jeff
 * Rebuild AOS trunk
 *
 * Revision 1.7  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.6  2006/06/09 05:54:40  liqin
 * *** empty log message ***
 *
 * Revision 1.5  2006/05/07 11:16:05  yangt
 * *** empty log message ***
 *
 * Revision 1.4  2006/05/05 01:31:29  liqin
 * *** empty log message ***
 *
 * Revision 1.3  2006/04/29 11:14:36  liqin
 * *** empty log message ***
 *
 * Revision 1.2  2006/04/06 02:25:00  lijing
 * add cavium_driver support by lijing
 *
 * Revision 1.1.2.1  2006/04/04 17:53:41  lijing
 * no message
 *
 * Revision 1.38  2005/10/13 09:26:12  ksnaren
 * fixed compile warnings
 *
 * Revision 1.37  2005/09/29 03:51:16  ksadasivuni
 * - Fixed some warnings
 *
 * Revision 1.36  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.35  2005/09/28 12:40:01  pyelgar
 *    - Fixed the delay in driver polling mode for RHEL3.
 *
 * Revision 1.34  2005/09/21 06:54:49  lpathy
 * Merging windows server 2003 release with CVS head
 *
 * Revision 1.33  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.32  2005/08/31 18:10:30  bimran
 * Fixed several warnings.
 * Fixed the corerct use of ALIGNMENT and related macros.
 *
 * Revision 1.31  2005/08/31 02:34:44  bimran
 * Fixed code to check for copy_in/out return values.
 *
 * Revision 1.30  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.29  2005/06/03 07:18:00  rkumar
 * Priority now associated with requests (WriteIPSecSA) -- and a reserved area is
 * earmarked for them in the SRQ list
 *
 * Revision 1.28  2005/05/24 09:14:50  rkumar
 * add_srq_entry: failed moved from cavium_print cavium_dbgprint
 *
 * Revision 1.27  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.26  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.25  2005/01/26 20:35:45  bimran
 * Added support to check for available core pairs for Modexp operation.
 *
 * Revision 1.24  2005/01/19 23:31:44  tsingh
 * *** empty log message ***
 *
 * Revision 1.23  2005/01/18 19:06:24  tsingh
 * Added check to look for microcode type before calling move_srq_entries()
 * (bimran)
 *
 * Revision 1.22  2005/01/14 23:48:43  tsingh
 * Fix for NPLUS in single core mode. (bimran)
 *
 * Revision 1.21  2005/01/06 18:43:32  mvarga
 * Added realtime support
 *
 * Revision 1.20  2004/07/30 18:02:42  tsingh
 * fix for interrupt mode from India office (Ram)
 *
 * Revision 1.1.1.1  2004/07/28 06:43:29  rkumar
 * Initial Checkin
 *
 * Revision 1.19  2004/07/09 01:09:00  bimran
 * fixed scatter gather support
 *
 * Revision 1.18  2004/06/03 21:22:56  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.17  2004/05/28 17:58:57  bimran
 * fixed a bug in mmap where in_buffer calcaluation was done even when not needed.
 *
 * Revision 1.16  2004/05/21 18:22:46  tsingh
 * Fixed unordered functionality
 *
 * Revision 1.15  2004/05/19 19:13:00  bimran
 * added CAVIUM_NO_MMAP to make it consistent with our older releases of SSL SDK.
 *
 * Revision 1.14  2004/05/11 20:50:32  tsingh
 * Changed some arguments passed through a function
 *
 * Revision 1.13  2004/05/11 03:10:55  bimran
 * some performance opt.
 *
 * Revision 1.12  2004/05/10 22:24:23  tsingh
 * Fix to decrease Latency on low loads
 *
 * Revision 1.11  2004/05/08 03:58:51  bimran
 * Fixed INTERRUPT_ON_COMP
 *
 * Revision 1.10  2004/05/05 06:48:10  bimran
 * Request ID is now copied to req buffer too.
 *
 * Revision 1.9  2004/05/02 19:45:31  bimran
 * Added Copyright notice.
 *
 * Revision 1.8  2004/05/01 07:14:37  bimran
 * Fixed non-blocking operation from user mode.
 *
 * Revision 1.7  2004/04/26 20:38:36  tsingh
 * moved debug print to avoid compile error
 *
 * Revision 1.6  2004/04/24 04:02:43  bimran
 * Fixed NPLUS related bugs.
 * Added some more debug prints.
 *
 * Revision 1.5  2004/04/21 21:33:58  bimran
 * added some more debug dumps.
 *
 * Revision 1.4  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.3  2004/04/17 00:00:40  bimran
 * Added more debug prints.
 * Fixed command dump.
 *
 * Revision 1.2  2004/04/16 03:21:14  bimran
 * Added doorbell coalescing support.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

