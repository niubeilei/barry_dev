////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: init_cfg.c
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
#include "cavium_endian.h"
#include "cavium.h"
#include "init_cfg.h"
#include "microcode.h"
#include "interrupt.h"
#include "command_que.h"
#include "context_memory.h"
#include "hw_lib.h"
#include "error_handler.h"
#include "request_manager.h"
#include "pending_free_list.h"
#include "pending_free_list.h"
#include "buffer_pool.h"
#include "direct_free_list.h"
#include "cavium_random.h"
#include "sg_free_list.h"
#include "sg_dma_free_list.h"
#include "completion_dma_free_list.h"
#include "bl_nbl_list.h"
#include "key_memory.h"
#include "cavium_random.h"

/*
 * Device driver general function
 */
int send_init_request(cavium_device *pdev);
void pkp_convert_sg_to_int_format(struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation);
void patch_it(Uint32 start_addr, Uint32 end_addr, struct CSP1_PATCH_WRITE *patch_write);
static struct MICROCODE *
get_microcode(cavium_device *pkp_dev, Uint8 type);

/*
 * Response queues procesing 
 */
void init_pending_lists(void);
void cleanup_pending_lists(void);
/*
 * Global variables
 */
Uint32 cavium_command_timeout = 0;

Uint8 cavium_version[3] = {0,99,'k'};
int dev_count=0;
cavium_device cavium_dev[MAX_DEV];

#ifdef INTERRUPT_ON_COMP
extern void check_for_completion_callback(Uint32);
#endif

#ifdef NPLUS
static int
pkp_boot_setup_ucode(cavium_device *pkp_dev, int ucode_idx);
#else
static int
pkp_boot_setup_ucode(cavium_device *pkp_dev);
#endif

static int
pkp_boot_init(cavium_device *pkp_dev);

/*
 *  Initialzie the chip, do core discovery, load microcode and send init requests.
 */
int 
cavium_init(cavium_config *config)
{
  int i;
  int ret;
 
  if(dev_count >= MAX_DEV) 
  {
     cavium_error("MAX_DEV reached.\n");
     goto init_error;
  }
  cavium_command_timeout = CAVIUM_DEFAULT_TIMEOUT;
  cavium_dbgprint("Inside cavium_init\n");
  /* cavium_memset(&cavium_dev[0],0x00,sizeof(cavium_dev));*/
  cavium_memset(&cavium_dev[dev_count], 0, sizeof(cavium_device)); 

  cavium_dev[dev_count].uen=0;
  cavium_dev[dev_count].enable=0;

  /* copy bar addresses and opaque device */
  cavium_dev[dev_count].dev = config->dev;
  cavium_dev[dev_count].bar_0 = config->bar_0;
  cavium_dev[dev_count].bar_1 = config->bar_2;
  cavium_dev[dev_count].bus_number = config->bus_number;
  cavium_dev[dev_count].dev_number = config->dev_number;
  cavium_dev[dev_count].func_number = config->func_number;
  cavium_dev[dev_count].command_queue_max = config->command_queue_max;

  set_soft_reset(&cavium_dev[dev_count]);

#ifdef NPLUS
  for(i=0;i<MICROCODE_MAX;i++)
  {
     cavium_dev[dev_count].microcode[i].core_id = (Uint8)-1;
     cavium_dev[dev_count].microcode[i].use_count = 0;
     cavium_dev[dev_count].microcode[i].code = NULL;
     cavium_dev[dev_count].microcode[i].data = NULL;
  }
  cavium_spin_lock_init(&cavium_dev[dev_count].mc_core_lock);
  /* Init the UEN Register Spin lock */
  cavium_spin_lock_init(&cavium_dev[dev_count].uenreg_lock);

#endif

#ifdef MC2
  cavium_dbgprint("cavium_init: allocating CTP BASE\n");
  cavium_dev[dev_count].ctp_base 
     = (Uint8 *)cavium_malloc_dma(CTP_COMMAND_BLOCK_SIZE *CTP_QUEUE_SIZE*MAX_CORES_NITROX, 
                NULL);
  if (cavium_dev[dev_count].ctp_base == NULL) 
  {
     cavium_error("cavium_init: Unable to allocate memory for CTP\n");
     goto init_error;
  }
  cavium_dbgprint("cavium_init: CTP BASE 0x%p\n",cavium_dev[dev_count].ctp_base);

  cavium_dev[dev_count].scratchpad_base = (Uint8 *)cavium_malloc_dma(
                           SCRATCHPAD_SIZE*
                           MAX_CORES_NITROX,
                         NULL);
  if (cavium_dev[dev_count].scratchpad_base == NULL) 
  {
     cavium_error("cavium_init: Unable to allocate memory for Scratch Pad\n");
     goto init_error;
  }
  cavium_dbgprint("cavium_init: Scratchpad BASE 0x%p\n",cavium_dev[dev_count].ctp_base);

  cavium_memset(cavium_dev[dev_count].scratchpad_base, 0, SCRATCHPAD_SIZE*MAX_CORES_NITROX);
#ifndef MCODE_LARGE_DUMP
   cavium_dev[dev_count].error_val = (Uint64*)cavium_malloc(sizeof(Uint64), NULL);
#else
#define LARGE_ERROR_VAL (2*1024)
   cavium_dev[dev_count].error_val = (Uint64*)cavium_malloc(LARGE_ERROR_VAL, NULL);
#endif
   if (cavium_dev[dev_count].error_val == NULL) 
   {
      cavium_error("cavium_init: Unable to allocate memory for error_val\n");
      goto init_error;
   }
#endif

  /* initialize command_queue */
  cavium_dbgprint("cavium_init: allocating command queues.\n");

  for(i=0;i<MAX_N1_QUEUES;i++)
  {
      cavium_spin_lock_init(&(cavium_dev[dev_count].command_queue_lock[i]));   
  }
  for (i = 0; i < MAX_N1_QUEUES; i++) 
  {
     if(init_command_queue(&cavium_dev[dev_count], i)) 
     { 
        cavium_error("init_command_queue failed\n");
        goto init_error;
     } 
     else 
     {
        cavium_dev[dev_count].door_addr[i] = (cavium_dev[dev_count].bar_1 +
                    REQ0_DOOR_BELL + 0x20*i); 
     }
  }

  /* initialize cavium_chip */
  cavium_dbgprint("cavium_init: clearing error regs,setting up command qs\n");
  if(pkp_init_board(&cavium_dev[dev_count])) 
  {
     cavium_error("pkp_init_board failed\n");
     goto init_error;
  }

  /* initialize dram_max just in case if DDR is not found */
  /* If DDR memory is found, then these values will be overwritten*/
  cavium_dev[dev_count].dram_max = config->context_max;
  cavium_dev[dev_count].dram_base = 0;

  /* check the presence of dram */
  check_dram(&cavium_dev[dev_count]);
 
  if(cavium_dev[dev_count].dram_present)
     enable_local_ddr(&cavium_dev[dev_count]);

   /* allocate a pool of context memory chunks */
    cavium_dbgprint("cavium_general_init: init context.\n");
    if(init_context(&cavium_dev[dev_count])) 
    {
      ret = ERR_MEMORY_ALLOC_FAILURE;
      cavium_error("init_context failed\n");
      goto init_error;
   }

  /* do core discovery and init requests */
    cavium_dbgprint("cavium_init: doing core discovery, load microcode\n");

  /* enable interrupts */
  cavium_dbgprint("cavium_init: enabling error interrupts.\n");
  enable_all_interrupts(&cavium_dev[dev_count]);

#ifdef INTERRUPT_ON_COMP
    /* Initialize tasklet */
    cavium_tasklet_init(&(cavium_dev[dev_count].interrupt_task),
                                check_for_completion_callback,
                                (ptrlong)(&cavium_dev[dev_count]));
#endif

  cavium_dbgprint("cavium_init: returning with success.\n");
  return 0;

init_error:
  
    for (i = 0; i <= dev_count; i++) 
    {
        cleanup_context(&cavium_dev[i]);
#ifdef MC2
       if (cavium_dev[i].ctp_base) 
       {
         cavium_free_dma(cavium_dev[i].ctp_base);
       }
       if (cavium_dev[i].scratchpad_base) 
       {
          cavium_free_dma(cavium_dev[i].scratchpad_base);
       }
       if (cavium_dev[i].error_val) 
       {
          cavium_free(cavium_dev[dev_count].error_val);
   }
#endif
    }
 
    for(i=0;i<MAX_N1_QUEUES;i++)
    {
   cavium_spin_lock_destroy(&(cavium_dev[dev_count].command_queue_lock[i]));
    } 
    for ( i = 0; i < MAX_N1_QUEUES; i++) 
    {
      cleanup_command_queue(&cavium_dev[dev_count], i);
    }

  cavium_dbgprint("cavium_init: returning with failure.\n");
  return 1;
}/* cavium_init*/


typedef enum
{
   NONE_SUCCESS,
   INIT_BUFFER_POOL_SUCCESS,
   INIT_RND_BUFFER_SUCCESS,
#ifdef SSL
   INIT_KEY_MEMORY_SUCCESS,
#endif
   INIT_COMPLETION_DMA_FREE_LIST_SUCCESS,
   INIT_PENDING_LISTS_SUCCESS,
   INIT_PENDING_FREE_LIST_SUCCESS,
   INIT_DIRECT_FREE_LIST_SUCCESS,   
   INIT_SG_FREE_LIST_SUCCESS,
   INIT_SG_DMA_FREE_LIST_SUCCESS,
   INIT_BLOCKING_NON_BLOCKING_LISTS_SUCCESS,
}cavium_general_init_status;
int cavium_general_init(cavium_general_config *gconfig)
{
   int ret=0, i,j;
   cavium_general_init_status status = NONE_SUCCESS;   
   
   for (i = 0; i < dev_count; i++) 
   {
      status = NONE_SUCCESS;
      cavium_dbgprint("cavium_general_init:init buffer pools.\n");
      if (init_buffer_pool(&cavium_dev[i], gconfig)) 
      {
         ret = ERR_MEMORY_ALLOC_FAILURE;
         goto init_general_error;
      }
      status = INIT_BUFFER_POOL_SUCCESS;
      /* initialize random pool */
      cavium_dbgprint("cavium_general_init:init rnd pool.\n");
      if(init_rnd_buffer(&cavium_dev[i]))
      {
         ret = ERR_MEMORY_ALLOC_FAILURE;
         goto init_general_error;
      }
      status = INIT_RND_BUFFER_SUCCESS;
      /* Initialize key memory chunks */
#ifdef SSL
      cavium_dbgprint("cavium_general_init:init key memory.\n");
      if (init_key_memory(&cavium_dev[i])) 
      {
         ret = ERR_MEMORY_ALLOC_FAILURE;
         goto init_general_error;
      }
      status = INIT_KEY_MEMORY_SUCCESS;
#endif
      cavium_dbgprint("cavium_general_init:init completion dma free list.\n");
      if(init_completion_dma_free_list(&cavium_dev[i],gconfig->sg_dma_list_max))
      {
         ret = ERR_MEMORY_ALLOC_FAILURE;
         goto init_general_error;
      }
      status = INIT_COMPLETION_DMA_FREE_LIST_SUCCESS; 
   } /* dev loop */

   /* initialize pending lists/ response queues*/
   cavium_dbgprint("cavium_general_init: initializing pending queues.\n");
   init_pending_lists();
   status = INIT_PENDING_LISTS_SUCCESS;   

   /* allocate a pool of pending structs for response queue */
   cavium_dbgprint("cavium_general_init: init pending free list.\n");
   if(init_pending_free_list(gconfig->pending_max)) 
   {
      ret = ERR_MEMORY_ALLOC_FAILURE;
      goto init_general_error;
   }
   status = INIT_PENDING_FREE_LIST_SUCCESS;

   /* allocate a pool of operation structs*/
   cavium_dbgprint("cavium_general_init: init direct free list\n");
   if(init_direct_free_list(gconfig->direct_max))
   {
      ret = ERR_MEMORY_ALLOC_FAILURE;
      goto init_general_error;
   }
   status = INIT_DIRECT_FREE_LIST_SUCCESS;

   /* allocate a pool of operation structs*/
   cavium_dbgprint("cavium_general_init: init SG free list\n");
   if(init_sg_free_list(gconfig->sg_max))
   {
      ret = ERR_MEMORY_ALLOC_FAILURE;
      goto init_general_error;
   }
   status = INIT_SG_FREE_LIST_SUCCESS;

   /* allocate a pool of scatter/gather dma lists */
   cavium_dbgprint("cavium_general_init: init sg dma free list.\n");
   if(init_sg_dma_free_list(gconfig->sg_dma_list_max))
   {
      ret = ERR_MEMORY_ALLOC_FAILURE;
      goto init_general_error;
   }
   status = INIT_SG_DMA_FREE_LIST_SUCCESS;

   cavium_dbgprint("cavium_general_init: init blocking and non-blocking list.\n");
   init_blocking_non_blocking_lists();
   status = INIT_BLOCKING_NON_BLOCKING_LISTS_SUCCESS;
   cavium_dbgprint("cavium_general_init: returning with success.\n");
   return ret;

init_general_error:
   switch(status)
   {
      case INIT_BLOCKING_NON_BLOCKING_LISTS_SUCCESS: 
      {
         return ret;
      }
      case INIT_SG_DMA_FREE_LIST_SUCCESS:
      {
         cleanup_sg_dma_free_list();
      }
      case INIT_SG_FREE_LIST_SUCCESS:
      {
         cleanup_sg_free_list();
      }
      case INIT_DIRECT_FREE_LIST_SUCCESS:
      {
         cleanup_direct_free_list();   
      }
      case INIT_PENDING_FREE_LIST_SUCCESS:
      {
         cleanup_pending_free_list();
      }
      case INIT_PENDING_LISTS_SUCCESS:
      {
         cleanup_pending_lists();
      }
      /* Now for loop cleanups */
      case INIT_COMPLETION_DMA_FREE_LIST_SUCCESS:
      #ifdef SSL
      case INIT_KEY_MEMORY_SUCCESS:
      #endif
      case INIT_RND_BUFFER_SUCCESS:
      case INIT_BUFFER_POOL_SUCCESS: 
      case NONE_SUCCESS:
      {
         if( (i==0) && (status==NONE_SUCCESS) ) return ret;
         if(i!=dev_count)
         switch(status)
         {
            case INIT_COMPLETION_DMA_FREE_LIST_SUCCESS:
            {
               cleanup_completion_dma_free_list(&cavium_dev[i]);
            }
            #ifdef SSL
            case INIT_KEY_MEMORY_SUCCESS:
            {
               cleanup_key_memory(&cavium_dev[i]);
            }
            #endif
            case INIT_RND_BUFFER_SUCCESS:
            {
               cleanup_rnd_buffer(&cavium_dev[i]);
            }
            case INIT_BUFFER_POOL_SUCCESS:
            {
               free_buffer_pool(&cavium_dev[i]);
            }
            default:break;
   
         }
         for(j=i-1;j>=0;j--)
         {
            cleanup_completion_dma_free_list(&cavium_dev[j]);
            #ifdef SSL
            cleanup_key_memory(&cavium_dev[j]);
            #endif
            cleanup_rnd_buffer(&cavium_dev[j]);
            free_buffer_pool(&cavium_dev[j]);
         }
      }
      default:return ret;
      
   }   

   
   cavium_dbgprint("cavium_general_init: returning with failure.\n");
   return ret;

} /* general entry */


/*
 *  Standard module release function.
 */
void cavium_cleanup(void *dev)
{
    cavium_device *pdev = (cavium_device *)dev;
    int i;

     cavium_dbgprint("cavium_cleanup: entering\n");
    set_soft_reset(pdev);

#ifdef INTERRUPT_ON_COMP
    cavium_tasklet_kill(&pdev->interrupt_task);
#endif
    
    for(i=0;i<MAX_N1_QUEUES;i++)
    {
   cavium_spin_lock_destroy(&(pdev->command_queue_lock[i]));
    }    
    /* cleanup command queue */
    for (i = 0; i < MAX_N1_QUEUES; i++) 
    {
       cleanup_command_queue(pdev, i);
    }
    cavium_dbgprint("cavium_cleanup: after cleanup_command_queue\n");
    cleanup_context(pdev);
    cavium_dbgprint("cavium_cleanup: after cleanup_context\n");
#ifdef MC2
    if (pdev->ctp_base) 
    {
      cavium_free_dma(pdev->ctp_base);
    }
    cavium_dbgprint("cavium_cleanup: after freeing ctp_base\n");
    if (pdev->scratchpad_base) 
    {
      cavium_free_dma(pdev->scratchpad_base);
    }
    cavium_dbgprint("cavium_cleanup: after freeing scratchpad_base\n");
    if (pdev->error_val) 
    {
      cavium_free(pdev->error_val);
    }
    cavium_dbgprint("cavium_cleanup: after freeing error_val\n");
#endif
    cleanup_blocking_non_blocking_lists();
    cavium_dbgprint("cavium_cleanup: after cleanup_blocking_non_blocking_lists()\n");
    return;
}


/*
 * General cleanup function
 */
int cavium_general_cleanup(void)
{
    int i;

   cavium_dbgprint("cavium_general_cleanup: entering\n");
   
   cleanup_pending_lists();
   cavium_dbgprint("cavium_general_cleanup: after cleanup_pending_lists\n");
   cleanup_pending_free_list();
   cavium_dbgprint("cavium_general_cleanup: after cleanup_pending_free_list\n");
   cleanup_direct_free_list();
   cavium_dbgprint("cavium_general_cleanup: after cleanup_direct_free_list\n");
   cleanup_sg_free_list();
   cavium_dbgprint("cavium_general_cleanup: after cleanup_sg_free_list\n");
   cleanup_sg_dma_free_list();
   cavium_dbgprint("cavium_general_cleanup: after cleanup_sg_dma_free_list\n");
   for (i = 0; i < dev_count; i++)
   {
       cleanup_completion_dma_free_list(&cavium_dev[i]);
       cavium_dbgprint("cavium_general_cleanup: after cleanup_completion_dma_free_list i %u\n", i);
       cleanup_rnd_buffer(&cavium_dev[i]);
       cavium_dbgprint("cavium_general_cleanup: after cleanup_rnd_buffer i %u\n", i);
#ifdef SSL
       cleanup_key_memory(&cavium_dev[i]);
#endif
       free_buffer_pool(&cavium_dev[i]);
       cavium_dbgprint("cavium_general_cleanup: after free_buffer_pool i %u\n", i);
   }
   cavium_dbgprint("cavium_general_cleanup: returning\n");
   return 0;
} /* cleanup general */


/*
 * get the microcode
 */
static struct MICROCODE *
get_microcode(cavium_device *pkp_dev, Uint8 type)
{
   struct MICROCODE *microcode;

#ifdef NPLUS
   microcode = &pkp_dev->microcode[type];
   return microcode;
#else
   int i;
   microcode = pkp_dev->microcode;

   for(i=0;i<MICROCODE_MAX;i++)
   {
      if(microcode[i].code_type == type)
         return &microcode[i];
   }

   return NULL;
#endif

}/* get_microcode*/

/*
 * pkp_boot_setup_ucode()
 */
#ifdef NPLUS
static int
pkp_boot_setup_ucode(cavium_device *pkp_dev, int ucode_idx)
#else
static int
pkp_boot_setup_ucode(cavium_device *pkp_dev)
#endif
{
   struct MICROCODE *microcode;
   Request request;
   Cmd* strcmd = (Cmd*)&request;
   Uint64 *completion_address;
   Uint8 *outbuffer = NULL;
   int ret;
#ifdef NPLUS
   int srq_idx;
   microcode = get_microcode(pkp_dev, ucode_idx);
#else
   microcode = get_microcode(pkp_dev, CODE_TYPE_MAINLINE);
#endif

   if(microcode == NULL) 
   {
           cavium_print( 
             "Unable to get microcode struct in boot setup ucode\n");
           ret=1;
           goto boot_setup_err;
   }

   if(cavium_debug_level > 2)
   {
      cavium_dump("sram address", microcode->sram_address,8);
   }

   strcmd->opcode = htobe16(OP_BOOT_SETUP_UCODE);
   strcmd->size  = htobe16(0);
   strcmd->param = htobe16((Uint16)(betoh64(*(Uint64 *)
                           (microcode->sram_address))));
#ifndef MC2
   strcmd->dlen  = htobe16((Uint16)(microcode->data_size >> 3));
#else
   strcmd->dlen  = htobe16((Uint16)8);
#endif

#ifdef N1_SANITY
   outbuffer = (Uint8 *)cavium_malloc_dma(8 + ALIGNMENT, NULL);
#else
   outbuffer = (Uint8 *)cavium_malloc_dma(8, NULL);
#endif
   if (outbuffer == NULL) 
   {
     cavium_error( "unable to allocate outbuffer in sending boot setup ucode\n");
     ret = 1;
     goto boot_setup_err;
   }

#ifndef MC2
   request.dptr = htobe64(cavium_vtophys(microcode->data));
#else
   request.dptr = htobe64(cavium_vtophys(((Uint32)microcode->data + 40)));
#endif
#ifdef N1_SANITY
   request.rptr = htobe64(cavium_vtophys((((ptrlong)outbuffer + ALIGNMENT) & ALIGNMENT_MASK)));
#else
   request.rptr = htobe64(cavium_vtophys(outbuffer));
#endif
   request.cptr = htobe64(0);

#ifdef N1_SANITY
   completion_address = (Uint64 *)(((ptrlong)outbuffer + ALIGNMENT) & ALIGNMENT_MASK);
#else
   completion_address = (Uint64 *)outbuffer;
#endif
   *completion_address = COMPLETION_CODE_INIT;
   
   if(cavium_debug_level > 2)
#ifndef MC2
      cavium_dump("dptr", microcode->data, microcode->data_size);
#else
      cavium_dump("dptr", microcode->data+40, microcode->data_size);
#endif


#ifdef NPLUS
   srq_idx = send_command(pkp_dev, &request, 0, BOOT_IDX, completion_address);
        ret = check_completion(pkp_dev, completion_address, 500, BOOT_IDX,srq_idx);
#else
   send_command(pkp_dev, &request, 0);
   /*cavium_udelay(100);*/
   ret = check_completion(completion_address, 500);
#endif

   if (ret) 
   {
      cavium_error( "Error: %x in sending setup ucode request\n", ret);
      ret = 1;
      goto boot_setup_err;
   }

   ret = 0;

boot_setup_err:
   if (outbuffer)
           /*kfree(out_buffer);*/
      cavium_free_dma((Uint8 *)outbuffer);

   return ret;
}

/*
 * Intialize Encrypted master secret key and IV in the first 48 bytes of FSK.
 */
#ifdef NPLUS
int
init_ms_key(cavium_device *pkp_dev, int ucode_idx)
#else
int
init_ms_key(cavium_device *pkp_dev)
#endif
{
   int ret = 0;
   Uint8 *out_buffer = NULL;
   Uint8 *in_buffer = NULL;
   Uint64 *completion_address;
   Request request;
   Cmd* strcmd = (Cmd*)&request;
   Uint16 km_size = 48;    /* key material size */
#ifdef NPLUS
   int srq_idx;
#endif

   strcmd->opcode = htobe16((0x1<<8) | MAJOR_OP_RANDOM_WRITE_CONTEXT);
#ifdef MC2
   strcmd->size = htobe16(km_size);
#else
   strcmd->size = htobe16(km_size >> 3);
#endif
   strcmd->param = htobe16(0);
   strcmd->dlen = htobe16(0);

#ifdef N1_SANITY
   out_buffer = (Uint8 *)cavium_malloc_dma((km_size + 8 + ALIGNMENT), NULL);
#else
   out_buffer = (Uint8 *)cavium_malloc_dma((km_size + 8), NULL);
#endif
   if(out_buffer == NULL) 
   {
     cavium_dbgprint( "unable to allocate out_buffer in init_ms_key.\n");
     ret = 1;
     goto ms_init_err;
    }

   request.dptr = htobe64(0);
#ifdef N1_SANITY
   request.rptr = htobe64(cavium_vtophys((((ptrlong)out_buffer + ALIGNMENT) & ALIGNMENT_MASK)));
#else
   request.rptr = htobe64(cavium_vtophys(out_buffer));
#endif
   request.cptr = htobe64(0);

#ifdef N1_SANITY
   completion_address = (Uint64 *)(((ptrlong)out_buffer + km_size + ALIGNMENT) & ALIGNMENT_MASK);
#else
   completion_address = (Uint64 *)(out_buffer + km_size);
#endif
   *completion_address = COMPLETION_CODE_INIT;

#ifdef NPLUS
   srq_idx = send_command(pkp_dev, &request, 0, ucode_idx, completion_address);
        ret = check_completion(pkp_dev, completion_address, 500, ucode_idx,srq_idx);
#else
   send_command(pkp_dev, &request, 0);
   ret = check_completion(completion_address, 500);
#endif
   if (ret) 
   {
      cavium_dbgprint( "Error: %x out while sending random request in init_ms_key.\n", ret);
      ret=1;
      goto ms_init_err;
   }

   /* now we have random number in out_buffer.
      Copy 48 bytes to the begining of the FSK memory.*/

   cavium_dbgprint( "Sending WriteEpci request in init_ms_key.\n");

   /* Here starts WriteEpci call */
   strcmd->opcode= htobe16((0x0<<8) | MAJOR_OP_RANDOM_WRITE_CONTEXT);
#ifdef MC2
   strcmd->size  = htobe16(0);
   strcmd->param = htobe16(0x0);
   strcmd->dlen  = htobe16(km_size + 8);
#else
   strcmd->size = htobe16(km_size >> 3);
   strcmd->param = htobe16(0x8);
   strcmd->dlen  = htobe16((km_size + 8)>>3);
#endif

#ifdef N1_SANITY
   in_buffer = (Uint8 *)cavium_malloc_dma((km_size + 8 + ALIGNMENT), NULL);
#else
   in_buffer = (Uint8 *)cavium_malloc_dma((km_size + 8), NULL);
#endif
   if (in_buffer == NULL) 
   {
     cavium_print( "unable to allocate in_buffer in init_ms_key.\n");
     ret = 1;
     goto ms_init_err;
   }

#ifdef N1_SANITY
   cavium_memset(in_buffer, 0, 48 + ALIGNMENT);
   cavium_memcpy((Uint8 *)(((ptrlong)in_buffer + 8 + ALIGNMENT) & ALIGNMENT_MASK), out_buffer, km_size);       /* now first 8 bytes have zeros, our key handle.*/
#else
   cavium_memset(in_buffer, 0, 48);
   cavium_memcpy(in_buffer + 8, out_buffer, km_size);       /* now first 8 bytes have zeros, our key handle.*/
#endif

#ifdef N1_SANITY
   request.dptr = htobe64(cavium_vtophys((((ptrlong)in_buffer + ALIGNMENT) & ALIGNMENT_MASK)));
   request.rptr = htobe64(cavium_vtophys((((ptrlong)out_buffer + ALIGNMENT) & ALIGNMENT_MASK)));
#else
   request.dptr = htobe64(cavium_vtophys(in_buffer));
   request.rptr = htobe64(cavium_vtophys(out_buffer));
#endif
   request.cptr = htobe64(0);

#ifdef N1_SANITY
   completion_address = (Uint64 *)(((ptrlong)out_buffer + ALIGNMENT) & ALIGNMENT_MASK);
#else
   completion_address = (Uint64 *)(out_buffer);
#endif
   *completion_address = COMPLETION_CODE_INIT;
#ifdef NPLUS
   srq_idx = send_command(pkp_dev, &request, 0, ucode_idx, completion_address);
        ret = check_completion(pkp_dev, completion_address, 500, ucode_idx,srq_idx);
#else
   send_command(pkp_dev, &request, 0);
   ret = check_completion(completion_address, 500);
#endif
   if(ret) 
   {
      cavium_print( "Error: %x while sending WriteEpci request in init_ms_key.\n", ret);
      ret=1;
      goto ms_init_err;
   }

   cavium_dbgprint( "Encrypted Master Secret successfully initialized.\n");

   ret =0;

ms_init_err:
   if(out_buffer)
           cavium_free_dma((Uint8 *)out_buffer);
   if(in_buffer)
           cavium_free_dma((Uint8 *)in_buffer);
   return ret;
}/* init_ms_key */




/*
 * boot init
 */
static int
pkp_boot_init(cavium_device *pkp_dev) 
{
   struct MICROCODE *microcode;
   Request request;
   Cmd* strcmd = (Cmd*)&request;
   volatile Uint64 *completion_address;
   Uint8 *outbuffer = NULL;
   int ret;
#ifdef NPLUS
   int srq_idx;
   microcode = get_microcode(pkp_dev, BOOT_IDX);
#else
   microcode = get_microcode(pkp_dev, CODE_TYPE_BOOT);
#endif
   if (microcode == NULL) 
   {
      cavium_print(  "Unable to get microcode struct in boot init\n");
      ret=1;
      goto boot_init_err;
   }

#ifdef N1_SANITY
   outbuffer = cavium_malloc_dma(8 +ALIGNMENT, NULL);
#else
   outbuffer = cavium_malloc_dma(8, NULL);
#endif
   if (outbuffer == NULL) 
   {
      cavium_print( "unable to allocate out_buffer in sending boot init request\n");
      ret=1;
      goto boot_init_err;
   }

   strcmd->opcode = htobe16(OP_BOOT_INIT);
#ifndef MC2
   strcmd->size  = htobe16(0);
   strcmd->param = htobe16(0);
   strcmd->dlen  = htobe16(microcode->data_size >> 3);
#else
   strcmd->size  = htobe16(CTP_QUEUE_SIZE);
   strcmd->param = htobe16(SCRATCHPAD_SIZE);
   strcmd->dlen  = htobe16(microcode->data_size + 40);
#endif


#ifdef MC2
   /* Format the first 40 bytes, with WD_TIMEOUT, SPI_KEY, CTX window base,
    * Error address, scratchpad base & CTP base
    */
   *((Uint32 *)microcode->data) = htobe32(0xffffffff);
   *((Uint32 *)(microcode->data+4)) = htobe32(0);
   *((Uint64 *)(microcode->data+8)) = htobe64(0);
#ifndef MCODE_LARGE_DUMP
   cavium_memset(pkp_dev->error_val, 0xff, sizeof(Uint64));
#else
   cavium_memset(pkp_dev->error_val, 0xff, 2*1024);
#endif

   *((Uint64 *)(microcode->data+16)) = htobe64((Uint64)cavium_vtophys(pkp_dev->error_val));
   *((Uint64 *)(microcode->data+24)) = htobe64((Uint64)cavium_vtophys(pkp_dev->scratchpad_base));
   *((Uint64 *)(microcode->data+32)) = htobe64((Uint64)cavium_vtophys(pkp_dev->ctp_base));
#endif
   cavium_dbgprint("Microcode data size %ld dlen %x\n", \
                                         microcode->data_size, strcmd->dlen);
   request.dptr = htobe64(cavium_vtophys(microcode->data));
#ifdef N1_SANITY
   request.rptr = htobe64(cavium_vtophys((((ptrlong)outbuffer + ALIGNMENT)
                                            & ALIGNMENT_MASK)));
#else
   request.rptr = htobe64(cavium_vtophys(outbuffer));
#endif
   request.cptr = htobe64(0);

#ifdef N1_SANITY
   completion_address = (Uint64 *)(((ptrlong)outbuffer + ALIGNMENT) & ALIGNMENT_MASK);
#else
   completion_address = (Uint64 *)outbuffer;
#endif
   *completion_address = COMPLETION_CODE_INIT;

   if(cavium_debug_level > 2)
#ifndef MC2
      cavium_dump("dptr", microcode->data, microcode->data_size);
#else
      cavium_dump("dptr", microcode->data, microcode->data_size+40);
#endif

#ifdef NPLUS
   srq_idx = send_command(pkp_dev, &request, 0, BOOT_IDX, (Uint64 *)completion_address);
   ret = check_completion(pkp_dev, completion_address, 500, BOOT_IDX,srq_idx);
#else
   send_command(pkp_dev, &request, 0);
   /*cavium_udelay(100);*/
   ret = check_completion(completion_address, 500);
#endif
   if(ret) 
   {
      cavium_error( 
         "Error : %x in sending boot init request.\n", ret);
      ret=1;
      goto boot_init_err;
   }

   ret = 0;

boot_init_err:
   if (outbuffer)
      cavium_free_dma(outbuffer);

   return ret;
}
             

/* 
 * load microcode, do core discovery, send init requests.
 */
int
do_init(cavium_device *pdev)
{
   int i,ret=0;
   Uint32 dwval=0,uen=0, imr =0,fexec =0, isr=0;
#ifdef NPLUS
   int id;
#endif
   
   /* Step 0: disable all units */
   cavium_udelay(10);
   disable_request_unit(pdev);
   cavium_udelay(10);
   disable_all_exec_units(pdev);
   cavium_udelay(10);

   /* Load boot microcode */


   if(load_microcode(pdev, 
#ifdef NPLUS
      BOOT_IDX
#else
      CODE_TYPE_BOOT
#endif
    ))
   {
      cavium_error("Error loading boot microcode\n");
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }
    
   /* Step 1: enable request and boot cores */
   cavium_dbgprint("Enable request and boot cores\n");
   enable_request_unit(pdev);
   cavium_udelay(10);      
   enable_data_swap(pdev);
   cavium_udelay(10);
   enable_rnd_entropy(pdev);    
   cavium_udelay(10);

   /* Step 2: Core discovery mechanism */
   cavium_dbgprint("Core Discovery Mechanism\n");

   /* Disable Interrupts */
   isr=0;
   imr=0;
   read_PKP_register(pdev,(pdev->bar_0 + IMR_REG),&imr);
   cavium_udelay(10);
   write_PKP_register(pdev,(pdev->bar_0 + IMR_REG),0);
   cavium_udelay(10);
   read_PKP_register(pdev,(pdev->bar_0 + ISR_REG),&isr);
   cavium_udelay(10);
   write_PKP_register(pdev,(pdev->bar_0 + ISR_REG),isr);

   /* read bist register */
   dwval=0;
   read_PKP_register(pdev,(pdev->bar_0 + FAILING_EXEC_REG), &dwval);
   cavium_udelay(10);

   cavium_dbgprint("read %08lx from FAILING_EXE_REG\n", dwval);
   /* enable the cores which have passed BIST */
   dwval = ((~dwval) & 0x10ffffff) | 0x10000000; /*keep request unit alive */

   cavium_dbgprint("Enabling Units : %08lx \n",dwval);
   /* write unit enable to enable only good cores */
   cavium_udelay(10);
   write_PKP_register(pdev,(pdev->bar_0 + UNIT_ENABLE), dwval);
   uen = dwval;
   cavium_udelay(10);
   
   /* check for ucode parity error*/
   dwval = 0;
   read_PKP_register(pdev,(pdev->bar_0 + ISR_REG), &dwval);
   cavium_dbgprint("do_int : isr reg %lx\n", dwval);
   if(dwval & 0x1) 
   {
      fexec=0;
      read_PKP_register(pdev, (pdev->bar_0 + FAILING_EXEC_REG), &fexec);
      uen = uen ^ fexec;
      cavium_error("Ucode parity error. Failing exec: %08lx\n",fexec);
   }

   /* check register file parity error */
   if(dwval & 0x2) 
   {
           /* read failing exec register */
           fexec=0;
           read_PKP_register(pdev, (pdev->bar_0 + FAILING_EXEC_REG), &fexec);
           uen = uen ^ fexec;
           cavium_error( "register file parity error. Failing exec: %08lx\n", fexec);
   }

   cavium_udelay(10);

   /* Now enable all good cores */
   write_PKP_register(pdev, (pdev->bar_0 + UNIT_ENABLE), uen);

   /* now enable interrupts */
   write_PKP_register(pdev, (pdev->bar_0 + ISR_REG), dwval);
   write_PKP_register(pdev, (pdev->bar_0 + IMR_REG), imr);

   /* now read all good cores */
   /* it will populate all good cores. pdev->uen */
   dwval = get_exec_units(pdev);

   if(dwval < MAX_CORES) 
   {
      cavium_error("%ld good cores found whereas this part reuires %d cores\n",dwval,MAX_CORES);
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }
   pdev->exec_units = MAX_CORES;

   uen = get_exec_units_part(pdev);
   if(!uen) 
   {
      cavium_error("Couldnot find %ld cores for current part\n",pdev->exec_units);
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }

   /* check if we have got proper core mask */
   if(check_core_mask(uen)) 
   {
      cavium_error("Final core mask does not comply with the expected value(s).\n");
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }

   cavium_print("UEN = 0x%lx\n", uen);

   pdev->uen = uen;
   pdev->boot_core_mask = get_first_available_core(MAX_CORES_NITROX, 
                     pdev->uen);

   cavium_print("Final Core Mask = 0x%08lx\n", uen);

   cavium_print("Loaded Boot microcode\n");

   /*disable all cores, leave request unit at the present state */
   disable_all_exec_units(pdev);

   cavium_dbgprint( "enabling boot core.\n");
   enable_exec_units_from_mask(pdev, pdev->boot_core_mask);

   /* send boot init command */
   cavium_dbgprint( "sending boot init request\n");

   if (pkp_boot_init(pdev)) 
   {
      cavium_error( "Error sending boot_init command\n");
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }

#ifdef NPLUS
   /* We do this for each microcode that we intend to run. */
   for(i=1;i<MICROCODE_MAX;i++)
   {
      if(pdev->microcode[i].code == NULL)
         continue;
      if(pkp_boot_setup_ucode(pdev, i))
      {
         cavium_error("Error sending boot_setup_ucode command\n");
         ret = ERR_INIT_FAILURE;
         goto do_init_err;
      }
   }
#else /*!NPLUS*/
   /* send boot setup ucode command */
   cavium_dbgprint( "sending boot setup ucode command\n");

   if (pkp_boot_setup_ucode(pdev)) 
   {
      cavium_error( "Error sending boot_setup_ucode command\n");
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }
#endif /*NPLUS*/

   /* disable boot unit. Not required anymore */
   cavium_dbgprint( "Disabling boot core\n");
   disable_exec_units_from_mask(pdev, pdev->boot_core_mask);

#ifdef NPLUS
   /* We dont enable the units now, but delay it till the IOCTL_CSP1_INIT_CODE
    * is made by the admin utility. The MS Key initialization, and random
    * number buffer fill, is also delayed till then.
    */

   /* We add the cores to the list of free core */
   for(id=0;id<MAX_CORES_NITROX;id++)
   {
      if(pdev->uen & (1<<id))
      {
         pdev->cores[id].next_id = pdev->microcode[FREE_IDX].core_id;
         pdev->microcode[FREE_IDX].core_id = id;
         pdev->cores[id].ucode_idx = FREE_IDX;
         pdev->cores[id].ctp_ptr = NULL;
         pdev->cores[id].ctp_idx = 0;
      }
   }
   ret = 0;
   /* return with success */
   goto do_init_err;

#else /*!NPLUS*/

   /* load mainline microcode */
   if(load_microcode(pdev, CODE_TYPE_MAINLINE)) 
   {
      cavium_print("Error loading mainline microcode\n");
      ret = ERR_INIT_FAILURE;
      goto do_init_err;
   }

   /* enable only part number specific cores*/
   enable_exec_units(pdev);

   cavium_udelay(10);

   cavium_print( "Loaded Main microcode\n");
   /* 
    * Now initialize encrypted master secret key and IV in the 
    * first 48 bytes of FSK 
    */
#ifdef SSL
   if(init_ms_key(pdev))
   {
           cavium_print( "Couldnot initialize encrypted master secret key and IV.\n");
           ret = ERR_INIT_FAILURE;
           goto do_init_err;
   }
   
   /* Fill random buffer */
   if(fill_rnd_buffer(pdev)) 
   {
           cavium_print( "Couldnot fill random buffer.\n");
           ret = ERR_INIT_FAILURE;
           goto do_init_err;
   }
#endif
   
   pdev->rnd_index=0;
   pdev->enable=1;

   /* disable master latency timer */ 
   write_PCI_register(pdev, 0x40, 0x00000001);

        /* set doobell thresholds for each queue */
   for(i=0;i<MAX_N1_QUEUES;i++)
   {
      lock_command_queue(pdev, i);
      pdev->door_bell_threshold[i] = DOOR_BELL_THRESHOLD;
      unlock_command_queue(pdev, i);
   }   
   ret=0;
   cavium_print("Loading of Microcodes successful\n");
#ifdef PROCFS_SUPPORT
   cavium_print("Check /proc/cavium directory for more information\n");
#endif
#endif /*NPLUS*/

do_init_err:
           return ret;

}/* do_init*/

/*------------------------------------------------------------------------------
 * 
 *  Device initialization.
 *  This function initializes the board. 
 *
 *----------------------------------------------------------------------------*/

int pkp_init_board(cavium_device *pdev)
{

   Uint32 dwval;

   setup_request_queues(pdev);

#if defined(CN1230) || defined(CN1220) || defined(CN1330) || defined(CN1320) 
   {
      int ret;
      ret = init_twsi(pdev);

      if (ret) 
      {
         cavium_print( "InitTwsi failure. err: %d\n", ret);
         return 1;
      }
   }
#endif

   cavium_udelay(10);
   /* clear ISR register */
   dwval = 0x00007fff;
   write_PKP_register(pdev,(pdev->bar_0+ISR_REG), dwval);

   cavium_udelay(10);
#ifdef CAVIUM_LDT
   dwval = 0x180f00;
   write_PKP_register(pdev,(pdev->bar_1+LMT_ERROR_REG), dwval);
   cavium_udelay(10);

   /* clear CRC error */
   dwval = 0;
   read_PCI_register(pdev, 0x44, &dwval);
   dwval |= 0xf00; /* set 11:8 */
   write_PCI_register(pdev, 0x44, dwval);
   cavium_udelay(10);
   
   /* set LMT command/status config register */
   write_PCI_register(pdev, 0x04, 0x78000105);

   /* set LMT control */
   dwval = 0x5c00000;
   write_PKP_register(pdev, (pdev->bar_1 + LMT_CONTROL_REG), dwval);

   /* LMT interrupt control register is set in setup_interrupt() */
#else
   /* clear pci error reg */
   dwval = 0x0007ffff;
   write_PKP_register(pdev,(pdev->bar_0+PCI_ERR_REG), dwval);

   cavium_udelay(10);
   write_PCI_register(pdev, 0x04, 0x02b00045);

   cavium_udelay(10);
   set_PCI_cache_line(pdev);
    set_PCIX_split_transactions(pdev);

   cavium_udelay(10);
   
   /* Disable Master Latency Timer */
   write_PCI_register(pdev, 0x40, 1);

#endif /* CAVIUM_LDT */
   
   return 0;

}/*pkp_init_board*/



/*
 * load microcode
 */
int 
load_microcode(cavium_device *pdev, int type)
{
   Uint32 i,size,instruction;
   struct MICROCODE * microcode;
   Uint8 *code;
   
   size=0;

   if(type > MICROCODE_MAX || type < 0)
      return 1;

   microcode = get_microcode(pdev, (Uint8)type);
   if(microcode == NULL)
   {
      cavium_error("Unable to get microcode (%d)\n", type);
      return 1;
   }

   code = microcode->code;
   size = microcode->code_size;

   cavium_dbgprint("microcode type=%d, code size=%ld\n", type, size);
   if(!code)
      return 1;

#ifdef NPLUS

   /* For NPLUS mode microcode load verification, core on which
    * microcode is being loaded has to be known. In non-NPLus mode
    * code below, it is always assumed to be core 0. In NPLus that 
    * might not be true. So here I will resort to good old microcode
    * load procedure.
    */

   for (i=0; i<size/4; i++)
   {
      instruction = ntohl((Uint32)*((Uint32*)code+i));

      write_PKP_register(pdev,(pdev->bar_0+UCODE_LOAD), instruction);
      cavium_udelay(50);
   }

#else /* non-NPLUS*/

#if defined(CN1230) || defined(CN1220) || defined (CN1120) || defined(CN1320) || defined(CN1330)
   /* Some N1 boards have debug mask such that microcode load verification
    * will never work. But that is a board level issue not related to chip
    * at all.
    */ 
   for (i=0; i<size/4; i++)
   {
      instruction = ntohl((Uint32)*((Uint32*)code+i));

      write_PKP_register(pdev,(pdev->bar_0+UCODE_LOAD), instruction);
      cavium_udelay(50);
   }
#else
   /* write debug command */
   write_PKP_register(pdev,(pdev->bar_0 + DEBUG_REG), 0xf);

   for (i=0; i<size/4; i++)
   {
      Uint32 debug_val;

      instruction = betoh32((Uint32)*((Uint32*)code+i));

      /* write it once */
      write_PKP_register(pdev,(pdev->bar_0 + UCODE_LOAD),instruction);
      cavium_udelay(50);

      /* write it once more */
      write_PKP_register(pdev,(pdev->bar_0 + UCODE_LOAD),instruction);
      cavium_udelay(50);
      
      /* read debug register */
      debug_val=0;
      read_PKP_register(pdev,(pdev->bar_0 + DEBUG_REG), &debug_val);

      debug_val >>= 12;

      debug_val &= 0xffff;
      instruction &= 0xffff;

      if(debug_val != instruction)
      {
         cavium_error("Ucode load failure: %ld: (actual=)0x%lx, (debug=)0x%lx\n", i, instruction, debug_val);
         return 1;
      }
   }
#endif
#endif /* NPLUS */

   return 0;
}



/*
 * Send initialization requests
 */
int send_init_request(cavium_device *pdev)
{
 Uint32 i;
 Uint8 *command;
 Request request;
 Cmd *strcmd = (Cmd*)&request;
 struct MICROCODE *microcode;
 Uint8 *cst;


 microcode = get_microcode(pdev, CODE_TYPE_MAINLINE);
 if(microcode == NULL)
    return 1;
 if(microcode->data_size)
 {
#ifdef N1_SANITY
 cst = (Uint8 *)cavium_malloc_dma(microcode->data_size + ALIGNMENT, NULL);
#else
 cst = (Uint8 *)cavium_malloc_dma(microcode->data_size, NULL);
#endif
 if(cst == NULL)
    return 1;

#ifdef N1_SANITY
 cavium_memcpy((Uint8 *)(((ptrlong)cst + ALIGNMENT) & ALIGNMENT_MASK), microcode->data, microcode->data_size);
#else
 cavium_memcpy(cst, microcode->data, microcode->data_size);
#endif


 strcmd->opcode= 0;
 strcmd->size  = 0;
 strcmd->param = 0;
 strcmd->dlen  = (Uint16)microcode->data_size/8;

 strcmd->opcode   = htobe16(strcmd->opcode);
 strcmd->size    = htobe16(strcmd->size);
 strcmd->param    = htobe16(strcmd->param);
 strcmd->dlen    = htobe16(strcmd->dlen);

#ifdef N1_SANITY
 request.dptr = (Uint64)(Uint32)cavium_vtophys((((ptrlong)cst + ALIGNMENT) & ALIGNMENT_MASK));
#else
 request.dptr = (Uint64)(Uint32)cavium_vtophys(cst);
#endif
 request.rptr = 0;
 request.cptr = 0;
   
 request.dptr = htobe64(request.dptr);
 
 lock_command_queue(pdev, 0);
 cavium_dbgprint("send_init: sending %ld init requests\n", pdev->exec_units); 
 for (i=0;i <pdev->exec_units; i++)
 { 
    command = (Uint8 *)pdev->command_queue_front[0];      
    cavium_memcpy(command, (Uint8 *)&request,32);
    ring_door_bell(pdev, 0, 1);
   cavium_udelay(50);
    inc_front_command_queue(pdev, 0);
 }

 unlock_command_queue(pdev, 0);

 cavium_free_dma(cst);
 }
 else
 {
    cavium_print("send_init_requests: No init to send.\n");
 }
 return 0; 
}

/*
 * $Id: init_cfg.c,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: init_cfg.c,v $
 * Revision 1.4  2015/01/06 08:57:47  andy
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
 * Revision 1.4  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
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
 * Revision 1.36  2005/10/13 09:24:19  ksnaren
 * fixed compile warnings
 *
 * Revision 1.35  2005/10/05 07:38:41  ksadasivuni
 * - cleanup_pending_lists() was not being called in cavium_general_cleanup().
 *   It is being called now. some spin locks are destroyed here which is
 *   required for FreeBSD 6.0
 *
 * Revision 1.34  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.33  2005/09/21 06:54:49  lpathy
 * Merging windows server 2003 release with CVS head
 *
 * Revision 1.32  2005/09/14 13:23:39  ksadasivuni
 * - A small fix for handling multiple card initialization.
 *
 * Revision 1.31  2005/09/09 08:55:02  sgadam
 * - Warning Reomoved
 *
 * Revision 1.30  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.29  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.28  2005/06/29 19:41:26  rkumar
 * 8-byte alignment problem fixed with N1_SANITY define.
 *
 * Revision 1.27  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.26  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.25  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.24  2005/01/11 00:22:49  mvarga
 * Fixed problems with compiler optimizations.
 *
 * Revision 1.23  2004/08/03 20:44:11  tahuja
 * support for Mips Linux & HT.
 *
 * Revision 1.22  2004/08/02 18:57:31  tsingh
 * fixed bug for loading driver on CN1120
 *
 * Revision 1.21  2004/06/28 20:37:42  tahuja
 * Fixed compiler warnings on NetBSD. changed mdelay in check_completion from 1ms to 2ms.
 *
 * Revision 1.20  2004/06/23 20:34:59  bimran
 * compiler warnings on NetBSD.
 * So much other stuff :-)
 *
 * Revision 1.16  2004/05/05 17:23:54  bimran
 * Moved set_soft_reset() from MC2 to all cases.
 *
 * Revision 1.15  2004/05/05 06:46:31  bimran
 * Fixed general initialization and cleanup routines to dupport multiple devices.
 *
 * Revision 1.14  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.13  2004/04/26 23:28:24  bimran
 * Used MAX_CORE_NITROX instead to hard coded values for ctp_base and scaratchpad allocations.
 *
 * Revision 1.12  2004/04/24 04:02:13  bimran
 * Fixed NPLUS related bugs.
 * Added some more debug prints.
 *
 * Revision 1.11  2004/04/23 21:48:22  bimran
 * Fixed SMP compile issue.
 *
 * Revision 1.10  2004/04/22 17:17:08  bimran
 * Modified microcode load verfication functionality to be used only on Nitrox-Lite parts due to some boards debug pins mapping issues.
 *
 * Revision 1.9  2004/04/21 21:33:13  bimran
 * added soem more debug dumps.
 *
 * Revision 1.8  2004/04/21 21:20:08  bimran
 * Added Cavium default debug level
 * Added some prints.
 * Temprarily disabled microcode load verification.
 *
 * Revision 1.7  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.6  2004/04/20 17:42:36  bimran
 * changed get_microcode() to reference microcode from cavium_device structure instead of global mirocode structure.
 *
 * Revision 1.5  2004/04/20 02:25:57  bimran
 * Fixed cavium_init() to use context_max passed in cavium_config structure.
 *
 * Revision 1.4  2004/04/19 18:38:45  bimran
 * Removed admin microcode support.
 *
 * Revision 1.3  2004/04/19 17:26:33  bimran
 * Fixed boot_setup_ucode() which should have always had 8 as dlen for 2.0 microcode.
 *
 * Revision 1.2  2004/04/16 03:20:38  bimran
 * Added doorbell coalescing support.
 * Microcode load verification support.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

