////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: pending_list.c
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
#include "error_handler.h"
#include "pending_free_list.h"
#include "pending_list.h"
#include "direct_free_list.h"
#include "context_memory.h"
#include "sg_free_list.h"
#include "sg_dma_free_list.h"
#include "completion_dma_free_list.h"
#include "pending_list.h"
#ifdef NPLUS
#include "soft_req_queue.h"
#endif


#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"

#define ENABLE_CAVIUM_UNORDERED

extern Uint32 cavium_command_timeout;
//extern Uint32 kernel_call;
/*
void finalize_request(cavium_device *pdev,
		struct PENDING_ENTRY *entry, 
		Csp1ResponseOrder response_order);
*/
/* Ordered processing pending list*/
cavium_spinlock_t ordered_list_lock = CAVIUM_SPIN_LOCK_UNLOCKED;
struct cavium_list_head ordered_list_head = {NULL, NULL}; 

/* Unordered processing pending list */
cavium_spinlock_t unordered_list_lock = CAVIUM_SPIN_LOCK_UNLOCKED;
struct cavium_list_head unordered_list_head = {NULL, NULL};

cavium_spinlock_t check_completion_lock = CAVIUM_SPIN_LOCK_UNLOCKED;
#ifdef INTERRUPT_ON_COMP
int pending_count = 0;
#endif
#ifdef NPLUS
int
check_srq_state(cavium_device *pdev, int ucode_idx, int srq_idx)
{
   softreq_t *srq;
   int state;

   /* Lock the microcode-cores structure */
   cavium_spin_lock_softirqsave(&(pdev->mc_core_lock));
   srq = &(pdev->microcode[ucode_idx].srq);
   /* Lock the SRQ */
   cavium_spin_lock_softirqsave(&(srq->lock));
   state = srq->state[srq_idx];
   /* Unlock the SRQ */
   cavium_spin_unlock_softirqrestore(&(srq->lock));
   /* Unlock the microcode-cores structure */
   cavium_spin_unlock_softirqrestore(&(pdev->mc_core_lock));
   return state;
}
#endif


/*
 * Pending list initialization.
 */
void
init_pending_lists(void)
{
	CAVIUM_INIT_LIST_HEAD(&ordered_list_head);
	CAVIUM_INIT_LIST_HEAD(&unordered_list_head);

	cavium_spin_lock_init(&ordered_list_lock);
	cavium_spin_lock_init(&unordered_list_lock);

	cavium_spin_lock_init(&check_completion_lock);
}/*init_pending_lists*/

void
cleanup_pending_lists(void)
{
	cavium_spin_lock_destroy(&ordered_list_lock);
	cavium_spin_lock_destroy(&unordered_list_lock);
	cavium_spin_lock_destroy(&check_completion_lock);
}

#define THRESHOLD_PENDING_COUNT 10
#define LOW_THRESHOLD_PENDING_COUNT 5
/*
 * Push to Ordered pending queue.
 */
void push_pending_ordered(struct PENDING_ENTRY *entry)
{
#ifdef INTERRUPT_ON_COMP
	pending_count++;
#endif
	cavium_spin_lock_softirqsave(&ordered_list_lock);
	cavium_list_add_tail(&entry->list, &ordered_list_head);
	cavium_spin_unlock_softirqrestore(&ordered_list_lock);
}/* push_pending_ordered*/


/*
 * Push to Record Processing pending queue.
 */
__inline void push_pending_unordered(struct PENDING_ENTRY *entry)
{
#ifdef INTERRUPT_ON_COMP
	pending_count++;
#endif
	cavium_spin_lock_softirqsave(&unordered_list_lock);
	cavium_list_add_tail(&entry->list, &unordered_list_head);
	cavium_spin_unlock_softirqrestore(&unordered_list_lock);
}/* push_pending_unordered*/



/*
 * Push pending structure to the corresponding queue 
 */
void push_pending(struct PENDING_ENTRY *entry, Csp1ResponseOrder response_order)
{
#ifdef ENABLE_CAVIUM_UNORDERED
	if(response_order == CAVIUM_RESPONSE_ORDERED)
		push_pending_ordered(entry);
	else
		push_pending_unordered(entry);
#else
	push_pending_ordered(entry);
#endif
}/* push_pending*/



void check_for_completion_callback(void *pdev)
{
 cavium_device *dev  = (cavium_device *)pdev;
 /*//unsigned int flags=0;*/

/* //cavium_spin_lock_irqsave(&check_completion_lock, flags);*/
/* cavium_spin_lock_softirqsave(&check_completion_lock); */

 /* clear the interrupt status */
/* write_PKP_register(dev,(dev->bar_0 + ISR_REG), 8);*/

#ifdef ENABLE_CAVIUM_UNORDERED
 /* poll ordered first */
 poll_pending_ordered(dev);

 /* now poll others */
 poll_pending_unordered(dev);

#else
 /* poll ordered only */
 poll_pending_ordered(dev);

#endif /*ENABLE_CAVIUM_UNORDERED*/

 /*//cavium_spin_unlock_irqrestore(&check_completion_lock, flags);*/
/*  cavium_spin_unlock_softirqrestore(&check_completion_lock); */
}/*check_for_completion_callback*/


/*
 * get entry at the head of the ordered queue
 */
__inline struct PENDING_ENTRY *  
get_queue_head_ordered(void)
{
   struct PENDING_ENTRY *pending_entry;
   struct cavium_list_head *first;

/*//   cavium_spin_lock_softirqsave(&ordered_list_lock);*/

   if(ordered_list_head.next != &ordered_list_head)
   {
      first = ordered_list_head.next;
      pending_entry = cavium_list_entry(first, struct PENDING_ENTRY, list);
   }
   else
      pending_entry =NULL;

/* //  cavium_spin_unlock_softirqrestore(&ordered_list_lock);*/
   return pending_entry;
}/*get_queue_head_ordered*/


/*
 * get entry at the head of the unordered queue
 */
__inline struct PENDING_ENTRY *  
get_queue_head_unordered(void)
{
   struct PENDING_ENTRY *pending_entry;
   struct cavium_list_head *first;

/*   //cavium_spin_lock_softirqsave(&unordered_list_lock);*/

   if(unordered_list_head.next != &unordered_list_head)
   {
      first = unordered_list_head.next;
      pending_entry = cavium_list_entry(first, struct PENDING_ENTRY, list);
   }
   else
      pending_entry =NULL;

/*   //cavium_spin_unlock_softirqrestore(&unordered_list_lock);*/
   return pending_entry;
}/*get_queue_head_unordered*/

/* Ordered list polling function */
void poll_pending_ordered(cavium_device *pdev)
{
	struct PENDING_ENTRY *entry;
	volatile Uint64 *p;

   cavium_spin_lock_softirqsave(&ordered_list_lock);

    while((entry = get_queue_head_ordered()) != NULL)
	{
		if (entry->status != ERR_REQ_PENDING) {
			/* Lying here for some time */
			break;
		}

		p = (volatile Uint64 *)entry->completion_address;
	
		/* no need of invalidating in case of Scatter/gather
		 * because completion_dma was allocated from non-cached memory
		 */	
		if(entry->dma_mode == CAVIUM_DIRECT)
		{
			Uint32 size;
			ptrlong vaddr, baddr;
			struct PKP_DIRECT_OPERATION_STRUCT *dir;
			
			dir = (struct PKP_DIRECT_OPERATION_STRUCT *)entry->pkp_operation;

			size = COMPLETION_CODE_SIZE;
			vaddr = (ptrlong)dir->completion_address;
			baddr = (ptrlong) (dir->rptr_baddr + dir->rlen);

			cavium_invalidate_cache(pdev, size, vaddr, baddr, CAVIUM_PCI_DMA_BIDIRECTIONAL);
		}

		if ((Uint8)(*p >> COMPLETION_CODE_SHIFT) == 0xff)
		{
			/* check for timeout */
#ifdef N1_TIMER_ROLLOVER
			if(cavium_time_before(entry->tick+CAVIUM_DEFAULT_TIMEOUT, cavium_jiffies))
#else
			if((entry->tick + CAVIUM_DEFAULT_TIMEOUT) < cavium_jiffies)
#endif
			{
#ifdef NPLUS 
			if(pdev->microcode[entry->ucode_idx].code_type == CODE_TYPE_SPECIAL)
			{
#ifdef N1_TIMER_ROLLOVER
				if(cavium_time_before(entry->tick+CAVIUM_DEFAULT_TIMEOUT*(MAX_SRQ_TIMEOUT + 1), cavium_jiffies))
#else
				if((entry->tick + CAVIUM_DEFAULT_TIMEOUT*(MAX_SRQ_TIMEOUT + 1)) < cavium_jiffies)
#endif
		               {
					if (del_srq_entry(pdev, entry->ucode_idx, entry->srq_idx, (Uint64 *)entry->completion_address) < 0) {
						entry->tick = cavium_jiffies;
						break;
					}
					entry->status = ERR_REQ_TIMEOUT;
					finalize_request(pdev, entry, CAVIUM_RESPONSE_ORDERED);
					continue;
			       } else 
			                break;
			} else {
				//cavium_error("poll_ordered: Oops! timed out.\n");
				aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "poll_ordered: Oops! timed out.");
				entry->status = ERR_REQ_TIMEOUT;
				finalize_request(pdev, entry, CAVIUM_RESPONSE_ORDERED);
			}
#else /*NPLUS*/
				//cavium_error("poll_ordered: Oops! timed out.\n");
				aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "NPLUS poll_ordered: Oops! timed out.");
				entry->status = ERR_REQ_TIMEOUT;
				finalize_request(pdev, entry, CAVIUM_RESPONSE_ORDERED);
#endif
			}
			else
				break;
		}
		else
		{
			cavium_dbgprint("poll_ordered: request completed\n");
#ifdef NPLUS
			if(pdev->microcode[entry->ucode_idx].code_type == CODE_TYPE_SPECIAL)
			{
				/* This is an SRQ request */
				free_srq_entries(pdev, entry->ucode_idx, entry->srq_idx, (Uint64 *)entry->completion_address);
			}
#endif /*NPLUS*/
			entry->status=0;
			finalize_request(pdev, entry, CAVIUM_RESPONSE_ORDERED);
		}
		cavium_dbgprint("poll_ordered: running\n");
	} /* while */
   cavium_spin_unlock_softirqrestore(&ordered_list_lock);

}/* poll_pending_ordered*/


/* Unordered list processing */
void poll_pending_unordered(cavium_device *pdev)
{
	struct PENDING_ENTRY *entry;
	volatile Uint64 *p;
	volatile int loop_count=0;

	cavium_spin_lock_softirqsave(&unordered_list_lock);
	
	while((entry = get_queue_head_unordered()) != NULL)
	{
		loop_count++;
		if(loop_count > MAX_CORES)
			break;

		if (entry->status != ERR_REQ_PENDING) {
			/* Lying here for some time */
			break;
		}

		p = (volatile Uint64 *)entry->completion_address;
		/* no need of invalidating in case of Scatter/gather
		 * because completion_dma was allocated from non-cached memory
		 */	
		if(entry->dma_mode == CAVIUM_DIRECT)
		{
			Uint32 size;
			ptrlong vaddr, baddr;
			struct PKP_DIRECT_OPERATION_STRUCT *dir;
			
			dir = (struct PKP_DIRECT_OPERATION_STRUCT *)entry->pkp_operation;

			size = COMPLETION_CODE_SIZE;
			vaddr = (ptrlong)dir->completion_address;
			baddr = (ptrlong)dir->rptr_baddr + dir->rlen;

			cavium_invalidate_cache(pdev, size,vaddr,baddr,CAVIUM_PCI_DMA_BIDIRECTIONAL);
		}
       
		if ((Uint8)(*p >> COMPLETION_CODE_SHIFT) != 0xff)
		{
#ifdef NPLUS
		   if(pdev->microcode[entry->ucode_idx].code_type == CODE_TYPE_SPECIAL)
		   {
			/* This is an SRQ request */
			free_srq_entries(pdev, entry->ucode_idx, entry->srq_idx, (Uint64 *)entry->completion_address);
		   }
#endif /*NPLUS*/
		   entry->status=0;
		   finalize_request(pdev,entry, CAVIUM_RESPONSE_UNORDERED);
		}
		else
		{
			/* check for timeout */
#ifdef N1_TIMER_ROLLOVER
			if(cavium_time_before(entry->tick+CAVIUM_DEFAULT_TIMEOUT,cavium_jiffies))
#else
			if((entry->tick + CAVIUM_DEFAULT_TIMEOUT) < cavium_jiffies)
#endif
			{
#ifdef NPLUS
			if(pdev->microcode[entry->ucode_idx].code_type == CODE_TYPE_SPECIAL)
			{
#ifdef N1_TIMER_ROLLOVER
				if(cavium_time_before(entry->tick+CAVIUM_DEFAULT_TIMEOUT*(MAX_SRQ_TIMEOUT+1), cavium_jiffies))
#else
				if((entry->tick + CAVIUM_DEFAULT_TIMEOUT*(MAX_SRQ_TIMEOUT+1)) < cavium_jiffies)
#endif
		               {
					if (del_srq_entry(pdev, entry->ucode_idx, entry->srq_idx, (Uint64 *)entry->completion_address) < 0) {
						entry->tick = cavium_jiffies;
						break;
					}
					entry->status = ERR_REQ_TIMEOUT;
					finalize_request(pdev, entry, CAVIUM_RESPONSE_UNORDERED);
					continue;
			       } else 
			                break;
			} else {
				//cavium_error("poll_ordered: Oops! timed out.\n");
				aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "NPLUS poll_unordered: Oops! timed out.");
				entry->status = ERR_REQ_TIMEOUT;
				finalize_request(pdev, entry, CAVIUM_RESPONSE_UNORDERED);
			}
#else /*NPLUS*/
				//cavium_error("poll_ordered: Oops! timed out.\n");
				aos_alarm( eAosMD_SSL, eAosAlarm_ProgErr, "poll_unordered: Oops! timed out.");
				entry->status = ERR_REQ_TIMEOUT;
				finalize_request(pdev, entry, CAVIUM_RESPONSE_UNORDERED);
#endif
			}
		}
            
	} /* while */
	
	cavium_spin_unlock_softirqrestore(&unordered_list_lock);

}/* poll_pending_unordered*/



void finalize_request(cavium_device *pdev,
		struct PENDING_ENTRY *entry, 
		Csp1ResponseOrder response_order)
{
	int cond_code;
	struct PKP_DIRECT_OPERATION_STRUCT *dir;
	struct PKP_SG_OPERATION_STRUCT *sg;
	struct call_back_data *cb_data;
	int i, position;
	
	cavium_list_del(&entry->list);
	
	if(response_order == CAVIUM_RESPONSE_ORDERED)
	  {
	    cavium_spin_unlock_softirqrestore(&ordered_list_lock);
	  }
	else
	  {
	    cavium_spin_unlock_softirqrestore(&unordered_list_lock);
	  }
	
	if(entry->dma_mode == CAVIUM_SCATTER_GATHER)
	{
		sg = (struct PKP_SG_OPERATION_STRUCT *)entry->pkp_operation;

		if(entry->status)
			cond_code = ERR_REQ_TIMEOUT;
		else
			cond_code = check_completion_code((Uint64 *)(entry->completion_address));

		/* flush all user buffers */
		pkp_invalidate_output_buffers(pdev, sg);

		/* unmap sg buffer */
		cavium_unmap_kernel_buffer(pdev, 
				sg->sg_dma_baddr,
				sg->sg_dma_size,
				CAVIUM_PCI_DMA_TODEVICE);

		/* unmap user buffers */
		pkp_unmap_user_buffers(pdev, sg);

		/* check endianness */
		check_endian_swap(sg, CAVIUM_SG_WRITE);

		cavium_dbgprint("finalize_request: calling callback function\n");		
		/* make callback function call*/
		entry->callback(cond_code,entry->cb_arg);
		
		put_sg_dma((volatile Uint8 *)sg->sg_dma);
		put_completion_dma((volatile Uint64 *)(sg->completion_dma));
		put_sg_entry(sg);	
	}
	else /* presumably DIRECT ;-) */
	{
		dir = (struct PKP_DIRECT_OPERATION_STRUCT *)entry->pkp_operation;

		/* invalidate output buffer */
		cavium_invalidate_cache(pdev, pkp_direct_operation->rlen,
		   pkp_direct_operation->rptr,
		   pkp_direct_operation->rptr_baddr, 
		   CAVIUM_PCI_DMA_BIDIRECTIONAL);     

		if(entry->status)
			cond_code = ERR_REQ_TIMEOUT;
		else
			cond_code = check_completion_code((Uint64 *)(entry->completion_address));
      
		/* make callback function call*/
		/* add by lijing */
		cavium_dbgprint("calling callback %p, cond_code=0x%x\n", entry->callback, cond_code);
		if(kernel_call==1){
			cb_data=(struct call_back_data*)entry->cb_arg;
			position=0;
			for(i=0;i<cb_data->outcnt;i++){
				memcpy(cb_data->outptr[i], cb_data->output+position, cb_data->outsize[i]);
				if(cb_data->outunit[i]==UNIT_16_BIT){
					Uint8 tmp;
					/*cb_data->outptr[i][0]=(cb_data->outptr[i][0]&0xff) ^ (cb_data->outptr[i][1]&0xff);
					cb_data->outptr[i][1]=(cb_data->outptr[i][0]&0xff) ^ (cb_data->outptr[i][1]&0xff);
					cb_data->outptr[i][0]=(cb_data->outptr[i][0]&0xff) ^ (cb_data->outptr[i][1]&0xff);
					*/
					tmp = cb_data->outptr[i][0];
					cb_data->outptr[i][0] = cb_data->outptr[i][1];
					cb_data->outptr[i][1] = tmp; 
				}
				if(cb_data->outunit[i]==UNIT_64_BIT){
					*(cb_data->outptr[i])=htobe64((Uint64)(*cb_data->outptr[i]));
				}
				if(cb_data->outunit[i]==UNIT_32_BIT){
					*(cb_data->outptr[i])=htobe32((Uint32)(*cb_data->outptr[i]));
				}
				position+=cb_data->outsize[i];
			}
			entry->callback(cond_code, cb_data->context);
			if(cb_data->input!=NULL && cb_data!=NULL){
				cavium_free(cb_data->input);
			}
			if(cb_data->output!=NULL && cb_data!=NULL){
				cavium_free(cb_data->output);
			}
			if(cb_data->req!=NULL && cb_data!=NULL){
				cavium_free(cb_data->req);
			}
			if(cb_data!=NULL){
				cavium_free(cb_data);
			}//add by lijing
		}else{
			entry->callback(cond_code, entry->cb_arg);
		}
		/* end add by lijing */

		/* unmap buffers */
		cavium_unmap_kernel_buffer(pdev,dir->dptr_baddr,
				dir->dlen,CAVIUM_PCI_DMA_BIDIRECTIONAL);

		cavium_unmap_kernel_buffer(pdev,dir->rptr_baddr,
				dir->rlen,CAVIUM_PCI_DMA_BIDIRECTIONAL);

		put_direct_entry(dir);
	}

	if(response_order == CAVIUM_RESPONSE_ORDERED)
	{
#ifdef INTERRUPT_ON_COMP
		pending_count --;
#endif
		/*//cavium_spin_lock_softirqsave(&ordered_list_lock);*/
/* 		cavium_list_del(&entry->list); */
		/*//cavium_spin_unlock_softirqrestore(&ordered_list_lock);*/
	}
	else
	{
#ifdef INTERRUPT_ON_COMP
		pending_count --;
#endif
		/*//cavium_spin_lock_softirqsave(&unordered_list_lock);*/
/* 		cavium_list_del(&entry->list); */
		/*//cavium_spin_unlock_softirqrestore(&unordered_list_lock);*/
	}

	put_pending_entry(entry);

	if(response_order == CAVIUM_RESPONSE_ORDERED)
	  {
	    cavium_spin_lock_softirqsave(&ordered_list_lock);
	  }
	else
	  {
	    cavium_spin_lock_softirqsave(&unordered_list_lock);
	  }
}/* finalize_request*/



/*
 * $Id: pending_list.c,v 1.4 2015/01/06 08:57:48 andy Exp $
 * $Log: pending_list.c,v $
 * Revision 1.4  2015/01/06 08:57:48  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:54  cding
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
 * Revision 1.11  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.10  2006/05/07 16:14:49  yangt
 * *** empty log message ***
 *
 * Revision 1.9  2006/05/07 11:16:05  yangt
 * *** empty log message ***
 *
 * Revision 1.8  2006/05/04 07:03:31  liqin
 * *** empty log message ***
 *
 * Revision 1.7  2006/05/04 02:18:01  liqin
 * *** empty log message ***
 *
 * Revision 1.6  2006/04/28 02:43:10  liqin
 * *** empty log message ***
 *
 * Revision 1.5  2006/04/07 00:44:12  lijing
 * no message
 *
 * Revision 1.4  2006/04/06 18:57:29  lijing
 * *** empty log message ***
 *
 * Revision 1.3  2006/04/06 12:06:14  lijing
 * *** empty log message ***
 *
 * Revision 1.2  2006/04/06 02:25:00  lijing
 * add cavium_driver support by lijing
 *
 * Revision 1.1.2.1  2006/04/04 17:53:41  lijing
 * no message
 *
 * Revision 1.34  2005/10/07 07:52:08  lpathy
 * windows server 2003 ipsec changes
 *
 * Revision 1.33  2005/09/29 03:51:16  ksadasivuni
 * - Fixed some warnings
 *
 * Revision 1.32  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.31  2005/09/21 06:54:49  lpathy
 * Merging windows server 2003 release with CVS head
 *
 * Revision 1.30  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.29  2005/09/06 08:52:10  ksadasivuni
 * Check in error corrected
 *
 * Revision 1.27  2005/08/31 18:10:30  bimran
 * Fixed several warnings.
 * Fixed the corerct use of ALIGNMENT and related macros.
 *
 * Revision 1.26  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.25  2005/06/03 07:20:03  rkumar
 * MAX_SRQ_TIMEOUT used for special microcode (for timing out commands), also
 * the timeout is applied after the command has moved to CTP
 *
 * Revision 1.24  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.23  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.22  2005/01/06 18:43:32  mvarga
 * Added realtime support
 *
 * Revision 1.21  2004/08/03 20:44:11  tahuja
 * support for Mips Linux & HT.
 *
 * Revision 1.20  2004/07/21 23:24:41  bimran
 * Fixed MC2 completion code issues on big endian systems.
 *
 * Revision 1.19  2004/07/09 01:09:00  bimran
 * fixed scatter gather support
 *
 * Revision 1.18  2004/06/28 21:13:43  tahuja
 * fixed a typo.
 *
 * Revision 1.17  2004/06/28 20:37:42  tahuja
 * Fixed compiler warnings on NetBSD. changed mdelay in check_completion from 1ms to 2ms.
 *
 * Revision 1.16  2004/06/23 20:52:20  bimran
 * compiler warnings on NetBSD.
 *
 * Revision 1.15  2004/06/09 00:23:21  bimran
 * Fixed poll_pending_unordered to break after polling MAX_CORES number of commands.
 *
 * Revision 1.14  2004/06/03 21:22:56  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.13  2004/06/01 17:47:53  bimran
 * fixed locks to work on SMP systems.
 *
 * Revision 1.12  2004/05/21 18:22:46  tsingh
 * Fixed unordered functionality
 *
 * Revision 1.11  2004/05/11 03:10:55  bimran
 * some performance opt.
 *
 * Revision 1.10  2004/05/08 03:58:51  bimran
 * Fixed INTERRUPT_ON_COMP
 *
 * Revision 1.9  2004/05/02 19:45:31  bimran
 * Added Copyright notice.
 *
 * Revision 1.8  2004/04/30 01:38:37  tsingh
 * Made some functions inline (bimran)
 *
 * Revision 1.7  2004/04/30 01:13:08  tsingh
 * Enable lock around poll_pending_* functions(bimran)
 *
 * Revision 1.6  2004/04/29 21:59:26  tsingh
 * Changed spinlocks to irqsave locks.(bimran)
 *
 * Revision 1.5  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.4  2004/04/19 17:25:25  bimran
 * Fixed a compiler warning.
 *
 * Revision 1.3  2004/04/17 02:53:22  bimran
 * Fixed check_for_completion_callback to not to write 8 to ISR register everytime it is called.
 *
 * Revision 1.2  2004/04/16 23:59:49  bimran
 * Added more debug prints.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

