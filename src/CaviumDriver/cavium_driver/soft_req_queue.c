////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: soft_req_queue.c
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


#ifdef NPLUS

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
#include "soft_req_queue.h"


inline void ring_ctp_doorbell(cavium_device *pdev, Uint8 cid, Uint8 clrbit)
{
   core_t *core = &pdev->cores[cid];
   Request *req;
   Uint16 opcode;
   int ctp_idx = core->ctp_idx;
   if(core->doorbell)
   {
      if(!core->ready)
      {
          cavium_print("Core %d CTP doorbell rung when not ready\n", cid);
          return;
      }

#if 0
      /* Is there a 2048 bit operation pending in the CTP ? */
         /* Is the core's pair ready ? */
      if(core->pend2048 && !pdev->cores[cid+1].ready)
      {
         printk(KERN_CRIT "Skipping doorbell ring (%d)\n", cid);
         return;
      }
#endif

      if(clrbit)
      {
         /* Here CTP idx, points to the next oflast valid instruction */
         if(ctp_idx == 0)
            ctp_idx = (CTP_QUEUE_SIZE-1);
         else
            ctp_idx--;

         /* Clear bit 6 of this instructions opcode, since this is
          * the last instruction in the group */
         req = (Request *)(pdev->cores[cid].ctp_ptr
                    + (CTP_COMMAND_BLOCK_SIZE*ctp_idx));
         opcode = htobe16(betoh16(((Cmd *)&req->cmd)->opcode) & ~((Uint16)1<<6));
         ((Cmd *)&req->cmd)->opcode = opcode;
      }

      /* Keep the SRQ index here, for others to track req completion */
      core->lrsrq_idx = core->ctp_srq_idx;
      core->lrcc_ptr
         = pdev->microcode[core->ucode_idx].srq.ccptr[core->ctp_srq_idx];
      core->ctp_srq_idx = -1;
      core->ready = 0;

      /* Is there a 2048 bit operation pending in the CTP ? */
      if(core->pend2048)
      {
         cavium_dbgprint("Pending 2048 bit operation for core %d\n", cid);
         if(pdev->cores[cid+1].ucode_idx != pdev->cores[cid].ucode_idx)
         {
            cavium_print("Illegal core assignment found\n");
            return;
         }
         /* Is the core's pair ready ? */
         if(pdev->cores[cid+1].ready)
         {
            cavium_dbgprint("Core %d ready\n", cid+1);
            if (pdev->cores[cid+1].doorbell) {
               cavium_dbgprint("ring_ctp_doorbell: 2048 bit operation pending and doorbell also %d\n",pdev->cores[cid+1].ctp_srq_idx);
               ring_ctp_doorbell(pdev, cid+1, 1);
             } else {
             	/* We will mark the paired core as busy, 
              	 * and schedule this operation */
             	pdev->cores[cid+1].ready = 0;
            	pdev->cores[cid+1].lrsrq_idx = -1;
             	/* Disable-Enable this core */
             	cycle_exec_units_from_mask(pdev, 1<<cid);
       		core->doorbell = 0;
  	   }
         }
#ifdef PKP_DEBUG
         else
         {
            /* else : We will wait till, the paired core is ready, and cycle
             * this core then.
             */
            cavium_print("Paired core %d not ready\n", cid+1);
         }
#endif
      }
      else
      {
         /* Disable-Enable this core */
         cycle_exec_units_from_mask(pdev, 1<<cid);
         core->doorbell = 0;
      }

      cavium_dbgprint("Rang doorbell Unit-%d(doorbell %ld, srq_idx %d, clrbit %d ctp_idx %d)\n",
              cid, core->doorbell, core->lrsrq_idx, clrbit, core->ctp_idx);

   }
}

inline void mark_cores_ready(cavium_device *pdev, int ucode_idx)
{
   Uint8 cid = pdev->microcode[ucode_idx].core_id;
   softreq_t *srq = &(pdev->microcode[ucode_idx].srq);
   int srq_idx;
   int start;
#ifdef PKP_DEBUG
   int i = 0;
    if(pdev->microcode[ucode_idx].code_type != CODE_TYPE_SPECIAL)
    {
         cavium_print("mark_cores_ready called with code type %d\n",
                                 pdev->microcode[ucode_idx].code_type);
    }
#endif

   /* For each core for this microcode */
   while(cid != (Uint8)-1)
   {
      /* Check the last queued instruction in the CTP for this core */
      start = srq_idx = pdev->cores[cid].lrsrq_idx;
      if(srq_idx != -1 && srq->state[srq_idx] == SR_IN_CTP
         && 
	 ((Uint8)((*srq->ccptr[srq_idx])>>COMPLETION_CODE_SHIFT) != 0xff)
	)
      {
         /* Till we reach an instruction that is not in CTP,
          * or not completed, keep freeing them */
         while(srq->state[srq_idx] == SR_IN_CTP
               && 
	 ((Uint8)((*srq->ccptr[srq_idx])>>COMPLETION_CODE_SHIFT) != 0xff)
	)
         {
#ifdef PKP_DEBUG
            i++;
#endif
            if (srq->core_id[srq_idx] != cid)
                break;
            srq->state[srq_idx] = SR_FREE;
            srq->ccptr[srq_idx] = NULL;
            srq->qsize--;
            if(srq_idx == 0)
               srq_idx = MAX_SRQ_SIZE-1;
            else
               srq_idx--;
         }
#ifdef PKP_DEBUG
         srq_idx++;
         if(srq_idx==MAX_SRQ_SIZE)
            srq_idx=0;
         cavium_print("mcr: Freed %d->%d\n", srq_idx,start);
         cavium_print("Cleared %d entries size %ld core %d(%d) State %d\n",
           i, srq->qsize, cid, current->pid, srq->state[srq_idx]);
#endif
      }
      cid = pdev->cores[cid].next_id;
   }

   cid = pdev->microcode[ucode_idx].core_id;
   while(cid != (Uint8)-1)
   {
      srq_idx = pdev->cores[cid].lrsrq_idx;
      if(!pdev->cores[cid].ready) {
         if(srq_idx != -1 && srq->state[srq_idx] == SR_FREE)
         {
            /* Mark the core as ready */
            cavium_dbgprint("Marking Unit %d ready\n", cid);

            pdev->cores[cid].ready = 1;  
            pdev->cores[cid].lrsrq_idx = -1;
            pdev->cores[cid].lrcc_ptr = NULL;
            if(cid%2 == 0)
            {
              /* Even core */
              if(pdev->cores[cid].pend2048)
              {
                 cavium_dbgprint("2048 bit op completed for cores(%d,%d)\n", 
                                 cid, cid+1);

                 /* A 2048 bit operation was completed, so the paired core
                  * must also be free'd */
                 pdev->cores[cid+1].ready = 1;
                 pdev->cores[cid].pend2048 = 0;
              }
           }
           else
           {
              /* Odd core */
              /* Is the paired waiting to schedule a 2048-bit operation ? */
              if(pdev->cores[cid-1].pend2048)
              {
                 cavium_dbgprint("Paired core %d ready, scheduling 2048 bit op\n",
                                 cid);

                 if(pdev->cores[cid-1].lrsrq_idx != -1)
                 {
                    /* Doorbell was already attempted on (cid - 1)*/
                    if (pdev->cores[cid].doorbell) {
                      /* Some requests for which doorbell is pending */
 		      cavium_dbgprint("move_srq_entries: doorbell pending on %d and 2048bit operation pending on the paired core\n",cid);
                      ring_ctp_doorbell(pdev,cid, 1);
                    } else {
                      pdev->cores[cid].ready = 0;  
                      pdev->cores[cid].lrsrq_idx = -1;
                      cycle_exec_units_from_mask(pdev, 1<<(cid-1));
                    }
                 }
                 else /* Doorbell has not been attempted yet */
                    cavium_dbgprint("mark_cores_ready: Delaying on core %d\n", cid-1);
              }
           }
         }
      }
      cid = pdev->cores[cid].next_id;
   }
}

inline void wait_cores_idle(cavium_device *pdev, Uint8 cid)
{
   int ucode_idx = pdev->cores[cid].ucode_idx;
   struct MICROCODE *microcode = &(pdev->microcode[ucode_idx]);
   softreq_t *srq = &(microcode->srq);
   unsigned long endjiff = cavium_jiffies + 500;

   /* First lock the SRQ */
   cavium_spin_lock_softirqsave(&(srq->lock));

   do
   {
      if(pdev->cores[cid].doorbell == 0)
      {
         if(pdev->cores[cid].ready)
         {
            cavium_print("Done waiting for core %d\n", cid);
            break;
         }
         else
         {
            if(pdev->cores[cid].lrsrq_idx != -1
                      &&
	 ((Uint8)((*pdev->cores[cid].lrcc_ptr)>>COMPLETION_CODE_SHIFT) != 0xff)
	)
            {
               mark_cores_ready(pdev, pdev->cores[cid].ucode_idx);
            }
            cavium_udelay(100);
         }
      }
      else
      {
         if(pdev->cores[cid].ready)
         {
#ifdef PKP_DEBUG
            cavium_print("Ringing doorbell(%ld) core(%d)\n",
                         pdev->cores[cid].doorbell, cid);
#endif
            ring_ctp_doorbell(pdev, cid, 1);
         }
         else
         {
            cavium_dbgprint("Mark cores ready(%d) with doorbell\n", cid);
            mark_cores_ready(pdev, pdev->cores[cid].ucode_idx);
         }
      }
#ifdef N1_TIMER_ROLLOVER
   } while(cavium_time_before(cavium_jiffies,endjiff));
#else
   } while( cavium_jiffies < endjiff);
#endif

   /* We will queue another '0' instruction */
   cavium_memset(pdev->cores[cid].ctp_ptr, 0,
                   CTP_QUEUE_SIZE * CTP_COMMAND_BLOCK_SIZE);

   /* Disable-Enable this core */
   cycle_exec_units_from_mask(pdev, 1<<cid);

   /* We will wait till the core writes a non zero value to the
    * start of its CTP area.
    */
   while(*(volatile Uint64 *) pdev->cores[cid].ctp_ptr != 0)
   {
      cavium_dbgprint("Core %d CTP base 0x%p 0x%08lx%08lx\n",
                      cid, pdev->cores[cid].ctp_ptr,
            *(Uint32 *)(pdev->cores[cid].ctp_ptr),
            *(Uint32 *)(pdev->cores[cid].ctp_ptr+4));

      cavium_udelay(1);
   }

   /* Unlock the SRQ */
   cavium_spin_unlock_softirqrestore(&(srq->lock));
}

int
add_srq_entry(cavium_device *pdev, struct MICROCODE *microcode,
    Request *request, volatile Uint64 *ccptr, int prio)
{
   int ret = 0,disallow = 0;
   softreq_t *srq;

#ifdef PKP_DEBUG
    if(microcode->code_type != CODE_TYPE_SPECIAL)
    {
         cavium_print("add_srq_entry called with code type %d\n",
                                 microcode->code_type);
    }
#endif

   /* Lock the microcode-cores structure */
   cavium_spin_lock_softirqsave(&(pdev->mc_core_lock));

   srq = &(microcode->srq);

   /* First lock the SRQ */
   cavium_spin_lock_softirqsave(&(srq->lock));

#if 1
   /* No Priority request, Disallow if Qsize > Threshold max */
   /* TODO: If Number of Prio requests > Threshold max */
   if (srq->qsize >= MAX_SRQ_SIZE) {
	disallow = 1;
   } else if (!prio) {
      if (srq->qsize >= MAX_SRQ_NORMAL)
         disallow = 1;
   }
#endif
   /* Check that the tail pointer has a free entry */
   if(disallow || (srq->state[srq->tail] != SR_FREE))
   {
      if((srq->state[srq->tail] == SR_IN_CTP) &&
#if CAVIUM_ENDIAN == CAVIUM_LITTLE_ENDIAN
      (((Uint8)((*srq->ccptr[srq->tail])>>COMPLETION_CODE_SHIFT) != 0xff))
#else
         (((Uint8)(*srq->ccptr[srq->tail]) != 0xff))
#endif
	)
         {
            /* We mark cores ready, if any */
            int cid = srq->core_id[srq->tail];
            if(pdev->cores[cid].lrcc_ptr == srq->ccptr[srq->tail])
            {
               /* Mark the cores as ready */
               cavium_dbgprint("Marking core %d ready\n", cid);
               mark_cores_ready(pdev, pdev->cores[cid].ucode_idx);
            }
      } else {
	    /* Srq entry in Use or in CTP without completion */
            /* Most likely there are instructions sitting in the CTP
             * for a long time, without the doorbell being rung.
             * So ring the doorbell, and drop this request */
            core_t *core = &pdev->cores[srq->core_id[srq->tail]];
#ifdef PKP_DEBUG
            int tail = srq->tail;
            cavium_print("add_srq_entry: SRQ tail(%ld) state(%d) ccptr %p code %lx%lx\n",
               srq->tail, srq->state[srq->tail], srq->ccptr[srq->tail],
               (Uint32)(*srq->ccptr[srq->tail]>>32),
               (Uint32)(*srq->ccptr[srq->tail]));
            cavium_print("Core %d: ready %d, lrsrq_idx %d, doorbell %ld, ctp_srq_idx %d, ctp_idx %d\n",
               srq->core_id[tail], core->ready, core->lrsrq_idx,
               core->doorbell, core->ctp_srq_idx, core->ctp_idx);
#endif

            if(core->ready)
            {
               ring_ctp_doorbell(pdev, srq->core_id[srq->tail], 1);
            }
      }
      /* We dont seem to have enough SRQ */
      ret = -1;
      goto cleanup_ase;
   } 
   if (srq->state[srq->tail] != SR_FREE) {
	cavium_error("add_srq_entry: Allocating when not in free %d\n",srq->state[srq->tail]);
	goto cleanup_ase;
   }

   /* Copy the request over */
   cavium_memcpy((Uint8 *)&(srq->queue[srq->tail]), (Uint8 *)request,
                   COMMAND_BLOCK_SIZE);

   cavium_dbgprint("ase: Using %ld(%ld)\n",srq->tail, srq->qsize);

   srq->state[srq->tail] = SR_IN_USE;
   srq->ccptr[srq->tail] = ccptr;
   ret = srq->tail;

   cavium_dbgprint("add_srq_entry: adding %ld(Pid: %x)\n",
          srq->tail, current->pid);

   srq->qsize++;

   if(srq->qsize < 0 || srq->qsize > MAX_SRQ_SIZE)
   {
         cavium_print("add_srq_entry: Wrong Qsize(%ld)\n", srq->qsize);
   }

   if(++srq->tail >= MAX_SRQ_SIZE)
      srq->tail = 0;

cleanup_ase:
   /* Unlock the SRQ */
   cavium_spin_unlock_softirqrestore(&(srq->lock));

   /* Unlock the microcode-cores structure */
   cavium_spin_unlock_softirqrestore(&(pdev->mc_core_lock));

   return(ret);
}

int
del_srq_entry(cavium_device *pdev, int ucode_idx, int srq_idx,
   volatile Uint64 *ccptr)
{
   int ret = 0;
   Uint8 cid;
   softreq_t *srq;

#ifdef PKP_DEBUG
   if(pdev->microcode[ucode_idx].code_type != CODE_TYPE_SPECIAL)
   {
      cavium_print("del_srq_entry called with code type %d\n",
              pdev->microcode[ucode_idx].code_type);
   }
#endif

   /* Lock the microcode-cores structure */
   cavium_spin_lock_softirqsave(&(pdev->mc_core_lock));

   srq = &(pdev->microcode[ucode_idx].srq);

   /* First lock the SRQ */
   cavium_spin_lock_softirqsave(&(srq->lock));

   if(srq->ccptr[srq_idx] != ccptr)
   {
       goto cleanup_dse;
   }

   switch(srq->state[srq_idx])
   {
      case SR_IN_CTP:
      {
         /* The entry is already in the CTP. We will have to wait
          * till it is processed */
         cavium_dbgprint("Core %d not responding!!\n",srq->core_id[srq_idx]);
         cavium_dbgprint("SrqIdx: %d Rptr: %08lx%08lx\n",
                srq_idx, (Uint32) (srq->queue[srq_idx].rptr>>32),
                (Uint32) srq->queue[srq_idx].rptr);
         cavium_dbgprint("ccptr %p code %lx%lx\n",
             srq->ccptr[srq_idx], (Uint32)(*srq->ccptr[srq_idx]>>32),
             (Uint32)(*srq->ccptr[srq_idx]));

         /* For now, we will just ring the doorbell, and check back later
          * The delete must be retried by the caller. */
         cid = srq->core_id[srq_idx];
         if(pdev->cores[cid].ready) {
            ring_ctp_doorbell(pdev, cid, 1);
            ret = -1;
            break;
          } else {
              if (srq_idx != pdev->cores[cid].lrsrq_idx) {
                   cavium_dbgprint("Not Resetting core %d srq_idx %d lrsrq_idx %d\n",cid, srq_idx, pdev->cores[cid].lrsrq_idx);
 	     } else {
                  pdev->cores[cid].ready = 1;
                  pdev->cores[cid].lrsrq_idx = -1;
                  pdev->cores[cid].lrcc_ptr = NULL;
              }
          }
      }
      case SR_IN_USE:
      {
         /* Simplest case, where we are free to remove it out */
         cavium_dbgprint("dse: Freeing %d(%ld)\n",srq_idx, srq->qsize);
	 if (srq->state[srq_idx] == SR_IN_USE)
             cavium_error("Timing out the request(Unable to assign to any SPM core) %d\n",srq_idx);
         else
             cavium_error("Timing out the request %d in CTP \n",srq_idx);

         cid = srq->core_id[srq_idx];
         srq->state[srq_idx] = SR_FREE;
         srq->ccptr[srq_idx] = NULL;
         srq->qsize--;

         if(srq->qsize < 0 || srq->qsize > MAX_SRQ_SIZE)
         {
            cavium_print("del_srq_entry: Wrong Qsize(%ld)\n",
                    srq->qsize);
         }

         /* Clear the instruction */
         cavium_memset((Uint8 *)&(srq->queue[srq_idx]), 0, COMMAND_BLOCK_SIZE);

      }
      break;

      case SR_FREE:
      default:
      {
         /* Something is wrong. We dont expect this, so PANIC */
         cavium_print("PANIC: SRQ corrupted!!\n");
         ret = -2;
      }
      break;
   }

cleanup_dse:
   /* Unlock the SRQ */
   cavium_spin_unlock_softirqrestore(&(srq->lock));

   /* Unlock the microcode-cores structure */
   cavium_spin_unlock_softirqrestore(&(pdev->mc_core_lock));

   return(ret);
}

void move_srq_entries(cavium_device *pdev, int ucode_idx, int thresh)
{
   Uint8 cid = 0, curr_cid = 0;
   softreq_t *srq;

#ifdef PKP_DEBUG
    if(pdev->microcode[ucode_idx].code_type != CODE_TYPE_SPECIAL)
    {
         cavium_print("move_srq_entries called with code type %d\n",
                 pdev->microcode[ucode_idx].code_type);
    }
#endif

   /* Lock the microcode-cores structure */
   cavium_spin_lock_softirqsave(&(pdev->mc_core_lock));

   if(pdev->microcode[ucode_idx].core_id == (Uint8)-1)
   {
      /* Unlock the microcode-cores structure */
      cavium_spin_unlock_softirqrestore(&(pdev->mc_core_lock));
      cavium_print("Error: No cores assigned to microcode %d\n", ucode_idx);
      return;
   }

   srq = &(pdev->microcode[ucode_idx].srq);
   /* Lock the SRQ */
   cavium_spin_lock_softirqsave(&(srq->lock));
   if(srq->qsize == 0)
   {
      goto cleanup_move;
   }

   /* Look for any cores that are disabled */
   curr_cid = pdev->microcode[ucode_idx].core_id;
   while((srq->head != srq->tail) && (curr_cid != (Uint8)-1))
   {
      cid = curr_cid;
      /* Is the instruction valid ? */
      if(srq->state[srq->head] != SR_IN_USE)
      {
         cavium_dbgprint("move_srq_entry: Skipping %ld\n",srq->head);

         /* We skip this instruction, it may have been dequeued */
         if(++srq->head >= MAX_SRQ_SIZE)
            srq->head = 0;
         continue;
      }

      cavium_dbgprint("move_srq_entry: is core %d ready? %d\n",
                  cid, pdev->cores[cid].ready);

      /* Is the core ready? */
      if(!pdev->cores[cid].ready)
      {
         /* Has the currently queued request completed? */
         if(pdev->cores[cid].lrsrq_idx != -1
             && 
	 ((Uint8)((*pdev->cores[cid].lrcc_ptr)>>COMPLETION_CODE_SHIFT) != 0xff)
	)
         {
            /* Current request has completed */
#ifdef PKP_DEBUG
            if(srq->state[pdev->cores[cid].lrsrq_idx] == SR_FREE)
            {
               cavium_print("Already free %d size %ld head %ld tail %ld\n", 
                  pdev->cores[cid].lrsrq_idx,
                  srq->qsize, srq->head, srq->tail);
            }
#endif
            /* We will mark this core ready. We will also try seeing if any
             * cores are done processing. Mark them ready, and free the SRQ
             * entries. */
            mark_cores_ready(pdev, ucode_idx);
         }
         else
         {
            curr_cid = pdev->cores[cid].next_id;
         }
      }

      if (pdev->cores[cid&0xFE].pend2048) {
          curr_cid = pdev->cores[cid].next_id;
          continue;
      }

      if(pdev->cores[cid].ready)
      {
         Uint8 majorop = betoh16(((Cmd *)&(srq->queue[srq->head].cmd))->opcode)
                         & 0xff;
         /* Is this a 2048 bit operation ?? */
         if(majorop == MAJOR_OP_ME_PKCS_LARGE || majorop == MAJOR_OP_RSASERVER_LARGE)
         {
             cavium_dbgprint("Major op: %x\n", majorop);

             if(cid%2 != 0)
             {
                 cavium_dbgprint("2048 bit operation for %d?\n", cid);

                 /* We cannot schedule this instruction on a odd core */
                 curr_cid = pdev->cores[cid].next_id;
                 continue;
             }
             else
             {
                 /* We will remember that this core has a 2048 bit op pending */
                 pdev->cores[cid].pend2048 = 1;
             }
         }

         /* Queue another request to this core */
         cavium_dbgprint("move_srq_entry: move req to unit-%d CTP=0x%p ctp_idx %d\n",
            cid, pdev->cores[cid].ctp_ptr
            + (CTP_COMMAND_BLOCK_SIZE*pdev->cores[cid].ctp_idx),
            pdev->cores[cid].ctp_idx);

         cavium_memcpy(pdev->cores[cid].ctp_ptr
            +(CTP_COMMAND_BLOCK_SIZE*pdev->cores[cid].ctp_idx),
            &srq->queue[srq->head], CTP_COMMAND_BLOCK_SIZE);

         /* Mark the entry as moved to CTP, and remember the core */
         srq->state[srq->head] = SR_IN_CTP;
         srq->core_id[srq->head] = cid;

         /* Remember the SRQ index in the CTP */
         pdev->cores[cid].ctp_srq_idx = srq->head;

         /* Increment the doorbell counter */
         pdev->cores[cid].doorbell++;

         cavium_dbgprint("Doorbell %ld Threshold %d\n", 
              pdev->cores[cid].doorbell, thresh);

         /* Should we ring the doorbell ? */
         if(pdev->cores[cid].doorbell >= thresh)
         {
            ring_ctp_doorbell(pdev, cid, 0);
            curr_cid = pdev->cores[cid].next_id;
         }
         else
         {
            Uint16 opcode;
            Request *req = (Request *)(pdev->cores[cid].ctp_ptr
                    + (CTP_COMMAND_BLOCK_SIZE*pdev->cores[cid].ctp_idx));
            /* Set bit 6 of this instructions opcode */
            opcode = htobe16(betoh16(((Cmd *)&req->cmd)->opcode) | (1<<6));
            ((Cmd *)&req->cmd)->opcode = opcode;
         }

         /* Advance the CTP entry pointer */
         pdev->cores[cid].ctp_idx++;
         if(pdev->cores[cid].ctp_idx >= CTP_QUEUE_SIZE)
            pdev->cores[cid].ctp_idx = 0;

         /* Advance the SRQ head */
         if(++srq->head >= MAX_SRQ_SIZE)
            srq->head = 0;
      }
   }

   if(thresh == 0)
   {
      cid = pdev->microcode[ucode_idx].core_id;
      while(cid != (Uint8)-1)
      {
         if(pdev->cores[cid].ready)
            ring_ctp_doorbell(pdev, cid, 1);
         cid = pdev->cores[cid].next_id;
      }
   }
cleanup_move:

   /* Unlock the SRQ */
   cavium_spin_unlock_softirqrestore(&(srq->lock));

   /* Unlock the microcode-cores structure */
   cavium_spin_unlock_softirqrestore(&(pdev->mc_core_lock));
}

void
free_srq_entries(cavium_device *pdev, int ucode_idx, int srq_idx,
   volatile Uint64 *ccptr)
{
   softreq_t *srq;
   Uint8 cid;
   int move_entries = 0;

#ifdef PKP_DEBUG
   if(pdev->microcode[ucode_idx].code_type != CODE_TYPE_SPECIAL)
   {
       cavium_print("free_srq_entries called with code type %d\n",
              pdev->microcode[ucode_idx].code_type);
   }
#endif

   /* Lock the microcode-cores structure */
   cavium_spin_lock_softirqsave(&(pdev->mc_core_lock));

   srq = &(pdev->microcode[ucode_idx].srq);
   /* Lock the SRQ */
   cavium_spin_lock_softirqsave(&(srq->lock));

   cavium_dbgprint("f_srq_e: entry sidx %x uidx %x state %x rptr %08lx%08lx\n",
                   srq_idx, ucode_idx, srq->state[srq_idx],
                   (Uint32)(srq->queue[srq_idx].rptr>>32),
                   (Uint32)srq->queue[srq_idx].rptr);

   /*
    * Is this entry still in use and does it still have the rptr?
    * Since the rptr cannot be reused unless freed, it is unique 
    */
   if(srq->state[srq_idx] != SR_FREE &&
         srq->ccptr[srq_idx] == ccptr)
   {
      cid = srq->core_id[srq_idx];

      cavium_dbgprint("f_srq_e: core %d srq_idx %d\n", cid,
          pdev->cores[cid].lrsrq_idx);
      cavium_dbgprint("fse: Freeing %d(%ld) %d?\n",
          srq_idx, srq->qsize, pdev->cores[cid].lrsrq_idx);

      srq->state[srq_idx] = SR_FREE;
      srq->ccptr[srq_idx] = NULL;
      srq->qsize--;

      if(srq->qsize < 0 || srq->qsize > MAX_SRQ_SIZE)
      {
         cavium_print("free_srq_entry: Wrong Qsize(%ld)\n",srq->qsize);
      }

      if(pdev->cores[cid].lrcc_ptr != NULL
         && 
	 ((Uint8)((*pdev->cores[cid].lrcc_ptr)>>COMPLETION_CODE_SHIFT) != 0xff)
	)
      {
         /* We can mark multiple entries as free here
          * Ofcourse, this is based on the assumption that request
          * completion is sequential */
         mark_cores_ready(pdev, ucode_idx);
         move_entries = 1;
      }
   }

   /* Unlock the SRQ */
   cavium_spin_unlock_softirqrestore(&(srq->lock));

   /* Unlock the microcode-cores structure */
   cavium_spin_unlock_softirqrestore(&(pdev->mc_core_lock));

   /* Did we find a free core? */
   if(move_entries)
      move_srq_entries(pdev, ucode_idx, CTP_QUEUE_SIZE);
}

#endif /*NPLUS*/
