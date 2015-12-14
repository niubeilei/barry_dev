////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: key_memory.c
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
#include "request_manager.h"
#include "key_memory.h"
#include "cavium_endian.h"
#include "init_cfg.h"
#include "buffer_pool.h"
#include "error_handler.h"

/* Key Memory related variables */
extern int dev_count;
volatile Uint32 fsk_mem_chunk_count[MAX_DEV];
volatile Uint32 ex_key_mem_chunk_count[MAX_DEV];
volatile Uint32 host_key_mem_chunk_count[MAX_DEV];

/*
 * Key Memory Management Functions
 */
static int
init_fsk_memory(cavium_device *pkp_dev)
{
   Uint32 i;

   if(pkp_dev->fsk_free_list) 
   {
      cavium_error("Key memory free list already exists\n");
        return 1;
   }
   
   pkp_dev->fsk_chunk_count = (Uint32)(FSK_MAX/FSK_CHUNK_SIZE);
        /* Store the fsk chunk count in the global array fsk_mem_chunk_count */
        fsk_mem_chunk_count[dev_count-1] = pkp_dev->fsk_chunk_count;
   
   pkp_dev->fsk_free_list = (Uint16 *)cavium_malloc_virt(pkp_dev->fsk_chunk_count *
                      sizeof(Uint16));

   if(pkp_dev->fsk_free_list == NULL) 
   {
      cavium_error("Not enough memory in allocating fsk free_list\n");
      return 1;
   }
 
   for(i = 0; i < pkp_dev->fsk_chunk_count; i++) 
   {
     pkp_dev->fsk_free_list[i] = (Uint16) (FSK_BASE + (i*FSK_CHUNK_SIZE));
   } 
       
   pkp_dev->fsk_free_index = 0;

   return 0;
}


static int
init_ex_key_memory(cavium_device *pkp_dev)
{
   Uint32 i;

   if(pkp_dev->ex_keymem_free_list) 
   {
      cavium_print("Key memory free list already exists\n");
      return 1;
   }
   
   pkp_dev->ex_keymem_chunk_count = (Uint32)(EX_KEYMEM_MAX/EX_KEYMEM_CHUNK_SIZE);
   
  /* Store the ex key memory count in the global array ex_key_mem_chunk_count */
   ex_key_mem_chunk_count[dev_count -1] = pkp_dev->ex_keymem_chunk_count;
   pkp_dev->ex_keymem_free_list = (Uint32 *)cavium_malloc_virt(pkp_dev->ex_keymem_chunk_count*
                       sizeof(Uint32));
   
   if(pkp_dev->ex_keymem_free_list == NULL) 
   {
        cavium_print("Not enough memory in allocating ex_keymem free_list\n");
        return 1;
   }
 
   for (i = 0; i < pkp_dev->ex_keymem_chunk_count; i++) 
   {
      pkp_dev->ex_keymem_free_list[i] = i*EX_KEYMEM_CHUNK_SIZE;
   }    

   pkp_dev->ex_keymem_free_index = 0;

   return 0;
}


static int
init_key_host_memory(cavium_device *pkp_dev)
{
    Uint32 i;

   if(pkp_dev->host_keymem_free_list) 
   {
      cavium_error( "Key memory free list already exists\n");
      return 1;
   }

   pkp_dev->host_keymem_count = (Uint32)(HOST_KEYMEM_MAX/HOST_KEYMEM_CHUNK_SIZE);

/* Store host key memory chunk count in the global array host_key_mem_chunk_count */
        host_key_mem_chunk_count[dev_count -1] = pkp_dev->host_keymem_count;
   pkp_dev->host_keymem_free_list = (Uint32 *)cavium_malloc_virt(pkp_dev->host_keymem_count * sizeof(Uint32));
   if (pkp_dev->host_keymem_free_list == NULL) 
   {
      cavium_error("Not enough memory in allocating host_key_mem freelist\n");
      return -1;
   }

   pkp_dev->host_keymem_static_list = ((struct PKP_BUFFER_ADDRESS *)
               cavium_malloc_virt(pkp_dev->host_keymem_count * 
                       sizeof(struct PKP_BUFFER_ADDRESS))
                                 );

   if(pkp_dev->host_keymem_static_list == NULL) 
   {
      cavium_error("Not enough memory in allocating ctx_static_list\n");
      goto host_error;
      return 1;
   }

   cavium_memset((Uint8 *)pkp_dev->host_keymem_static_list, 0,
            pkp_dev->host_keymem_count*sizeof(struct PKP_BUFFER_ADDRESS));

   for (i = 0; i < pkp_dev->host_keymem_count; i++) 
   {
      pkp_dev->host_keymem_static_list[i].size = HOST_KEYMEM_CHUNK_SIZE;
      pkp_dev->host_keymem_static_list[i].vaddr =
                              (ptrlong)cavium_malloc_nc_dma(pkp_dev,
                              pkp_dev->host_keymem_static_list[i].size,
                              &pkp_dev->host_keymem_static_list[i].baddr);
      if (pkp_dev->host_keymem_static_list[i].vaddr) 
      {
         pkp_dev->host_keymem_free_list[i] = i;
      } 
      else 
      {
         goto host_error;
      }
   }
   return 0;
host_error:
   for (i = 0; i < pkp_dev->host_keymem_count; i++) 
   {
      if (pkp_dev->host_keymem_static_list == NULL) 
      {
         break;
      }
      if (pkp_dev->host_keymem_static_list[i].vaddr) 
      { 
         cavium_free_nc_dma(pkp_dev,
                            pkp_dev->host_keymem_static_list[i].size,
                            (Uint8 *)pkp_dev->host_keymem_static_list[i].vaddr,
                            pkp_dev->host_keymem_static_list[i].baddr);
      } 
      else 
      {
         break;
      }   
   }

   if (pkp_dev->host_keymem_static_list) 
   {
      cavium_free_virt(pkp_dev->host_keymem_static_list);
      pkp_dev->host_keymem_static_list=NULL;
   }
   if (pkp_dev->host_keymem_free_list) 
   {
      cavium_free_virt(pkp_dev->host_keymem_free_list);
      pkp_dev->host_keymem_free_list = NULL;
   }
   return 1;
}

int
init_key_memory(cavium_device * pkp_dev)
{
   int ret=0;

   cavium_spin_lock_init(&pkp_dev->keymem_lock);
   /* Initialize fsk memory ==> On-chip SRAM memory */
   ret = init_fsk_memory(pkp_dev);
   if (ret) 
   {
      return ret;
   }
 
   if(pkp_dev->dram_present) 
   {
      /* Initialize EX key memory on DRAM */
      ret = init_ex_key_memory(pkp_dev);
      if(ret) 
      {
         return ret;
      }
   }

   /* Initialize the host key memory */
   ret = init_key_host_memory(pkp_dev);
   if (ret) 
   {
      return ret;
   }

   /*cavium_spin_lock_init(&pkp_dev->keymem_lock);*/

   CAVIUM_INIT_LIST_HEAD(&pkp_dev->keymem_head);

   return ret;
}


static void
free_host_key_memory(cavium_device *pkp_dev)
{
   Uint32 i;

   for (i = 0; i < pkp_dev->host_keymem_count; i++) 
   {
      if (pkp_dev->host_keymem_static_list == NULL) 
      {
         break;
      }
      if (pkp_dev->host_keymem_static_list[i].vaddr) 
      {
         cavium_free_nc_dma(pkp_dev,
                            pkp_dev->host_keymem_static_list[i].size,
                            (Uint8 *)pkp_dev->host_keymem_static_list[i].vaddr,
                            pkp_dev->host_keymem_static_list[i].baddr);
      } 
      else 
      {
         break;
      }   
   }

   if (pkp_dev->host_keymem_static_list) 
   {
      cavium_free_virt(pkp_dev->host_keymem_static_list);
      pkp_dev->host_keymem_static_list = NULL;
   }
   if (pkp_dev->host_keymem_free_list) 
   {
      cavium_free_virt(pkp_dev->host_keymem_free_list);
      pkp_dev->host_keymem_free_list = NULL;
   }
   return;
}

void
cleanup_key_memory(cavium_device *pkp_dev)
{
   cavium_spin_lock_destroy(&pkp_dev->keymem_lock);
   if(pkp_dev->fsk_free_list) 
   cavium_free_virt(pkp_dev->fsk_free_list);
   pkp_dev->fsk_free_list = NULL;

   if (pkp_dev->dram_present) 
   {
      if (pkp_dev->ex_keymem_free_list)
      cavium_free_virt(pkp_dev->ex_keymem_free_list);
      pkp_dev->ex_keymem_free_list = NULL;
   }

   if(pkp_dev->host_keymem_free_list) 
   free_host_key_memory(pkp_dev);

   return;
}
   
static Uint64
alloc_fsk(cavium_device *pkp_dev)
{
   Uint64 key_handle = (Uint64)NULL;
#ifdef CAVIUM_PKEY_HOST_MEM
   return key_handle;
#endif
#ifdef CAVIUM_PKEY_LOCAL_DDR
   return key_handle;
#endif
   if (pkp_dev->fsk_free_index >= pkp_dev->fsk_chunk_count) 
   {
      return key_handle;
   }

#ifdef MC2
   key_handle = (Uint64)((pkp_dev->fsk_free_list[pkp_dev->fsk_free_index])
            | ((Uint64)1 << 48));
#else
   key_handle = (Uint64)(pkp_dev->fsk_free_list[pkp_dev->fsk_free_index]);
#endif
   pkp_dev->fsk_free_index++;
   return key_handle;
}

static Uint64
alloc_ex_key_mem(cavium_device *pkp_dev)
{
   Uint64 key_handle = (Uint64)NULL;

#ifdef CAVIUM_PKEY_HOST_MEM
   return key_handle;
#endif
#ifdef CAVIUM_PKEY_INTERNAL_SRAM
   return key_handle;
#endif
   if (pkp_dev->ex_keymem_free_index >= pkp_dev->ex_keymem_chunk_count) 
   {
      return key_handle;
   }

   key_handle = ((((Uint64)(pkp_dev->ex_keymem_free_list[pkp_dev->ex_keymem_free_index])) \
                                                                >> 7) | 0x8000);
   pkp_dev->ex_keymem_free_index++;
   return key_handle;
}

static Uint64
alloc_host_key_memory(cavium_device *pkp_dev)
{
   Uint64 key_handle = (Uint64)NULL;
   Uint32 free_key;

   if (pkp_dev->host_keymem_free_index >= pkp_dev->host_keymem_count) 
   {
      return key_handle;
   }
   free_key = pkp_dev->host_keymem_free_list[pkp_dev->host_keymem_free_index];
   pkp_dev->host_keymem_free_index ++;
#ifdef MC2
           key_handle = (pkp_dev->host_keymem_static_list[free_key].baddr);
#else
   key_handle = (pkp_dev->host_keymem_static_list[free_key].baddr) | ((Uint64)0x20000 << 32);
#endif
   return key_handle;
}

Uint64
alloc_key_memory(cavium_device *pkp_dev)
{
        struct KEYMEM_ALLOC_ENTRY *entry;
        Uint32 loc;
        Uint64 key_handle= 0;

        entry = (struct KEYMEM_ALLOC_ENTRY *)cavium_malloc(sizeof(struct KEYMEM_ALLOC_ENTRY), NULL);
        if(entry == NULL) 
        {
           cavium_error( "keymem alloc: Not enough memory in allocating keymem entry.\n");
           return 0;
        }

        cavium_spin_lock(&pkp_dev->keymem_lock);

   key_handle = alloc_fsk(pkp_dev);
   if (!key_handle) 
   {
      if (pkp_dev->dram_present) 
      {
         key_handle = (Uint64)alloc_ex_key_mem(pkp_dev);
      }

      if (!key_handle) 
      {
         key_handle = (Uint64)alloc_host_key_memory(pkp_dev);
         if (!key_handle) 
         {
            cavium_free(entry);
            cavium_spin_unlock(&pkp_dev->keymem_lock);
            cavium_error("No more key memory available\n");
            return (Uint64)NULL;
         }
         loc = OP_MEM_FREE_KEY_HOST_MEM;
      } 
      else 
      {
         loc = OP_MEM_FREE_KEY_DDR_MEM;
      }
   } 
   else 
   {
      loc = OP_MEM_FREE_KEY_SRAM_MEM;
   }
   
   /* insert into allocated keymem_alloc list */
   entry->proc_pid = cavium_get_pid();
   entry->loc = (Uint16) loc;
   entry->key_handle = key_handle;
   cavium_list_add_tail(&entry->list, &pkp_dev->keymem_head);

   cavium_spin_unlock(&pkp_dev->keymem_lock);
   cavium_dbgprint("alloc_key: key handle = %08lx%08lx, loc = %ld, pid = %d\n",
           (Uint32)(key_handle >> 32), (Uint32)key_handle, loc, entry->proc_pid);

   return key_handle;
}


static void
dealloc_fsk(cavium_device *pkp_dev, Uint64 key_handle)
{
   pkp_dev->fsk_free_index --;
   pkp_dev->fsk_free_list[pkp_dev->fsk_free_index] = (Uint16) key_handle;
   return;
}

static void
dealloc_ex_keymem(cavium_device *pkp_dev, Uint64 key_handle)
{
   pkp_dev->ex_keymem_free_index --;
   key_handle &= (Uint64)0xFFFFFFFFFFFF7FFFULL;
   pkp_dev->ex_keymem_free_list[pkp_dev->ex_keymem_free_index] = (Uint32) (key_handle << 7);
   return;
}

static void
dealloc_host_keymem(cavium_device *pkp_dev, Uint64 key_handle)
{
    Uint32 i;

#ifndef MC2
   /* turn off bit 49 */
   key_handle &= (((Uint64)0xfffdffff << 32) | (0xffffffff));
#endif

   for (i = 0; i <pkp_dev->host_keymem_count; i++) 
   {
      if (pkp_dev->host_keymem_static_list[i].baddr == key_handle) 
      {
         pkp_dev->host_keymem_free_index --;
         pkp_dev->host_keymem_free_list[pkp_dev->host_keymem_free_index] = i;
         return;
      }
   }
   cavium_error("CRIT ERROR ... KEY memory not found for deallocation\n");
   return;
}

void
dealloc_key_memory(cavium_device *pkp_dev, Uint64 key_handle)
{
   cavium_pid_t pid;
   struct cavium_list_head *tmp;
    struct KEYMEM_ALLOC_ENTRY *entry;

   pid = cavium_get_pid();
   cavium_spin_lock(&pkp_dev->keymem_lock);

   cavium_list_for_each(tmp, &pkp_dev->keymem_head) 
   {
      entry = cavium_list_entry(tmp, struct KEYMEM_ALLOC_ENTRY,
               list);
      
      if ((key_handle == entry->key_handle)) 
      {
         switch (entry->loc) 
         {
            case OP_MEM_FREE_KEY_SRAM_MEM:
               dealloc_fsk(pkp_dev, key_handle);
               break;
            case OP_MEM_FREE_KEY_DDR_MEM:
               dealloc_ex_keymem(pkp_dev, key_handle);
               break;
            case OP_MEM_FREE_KEY_HOST_MEM:
               dealloc_host_keymem(pkp_dev, key_handle);
               break;
         }
              cavium_list_del(&entry->list);
              cavium_free(entry);
         cavium_spin_unlock(&pkp_dev->keymem_lock);
         return;
      }
   }

   cavium_spin_unlock(&pkp_dev->keymem_lock);
   cavium_error("dealloc_key_memory: NOT FOUND 0x%lx\n", (ptrlong)key_handle);

   return;   
}

void
flush_key_memory(cavium_device *pkp_dev)
{
   cavium_pid_t pid;
   struct cavium_list_head *tmp;
   struct KEYMEM_ALLOC_ENTRY *entry;

   pid = cavium_get_pid();
   cavium_spin_lock(&pkp_dev->keymem_lock);

   cavium_list_for_each(tmp, &pkp_dev->keymem_head) 
   {
      entry = cavium_list_entry(tmp, struct KEYMEM_ALLOC_ENTRY,
                                                 list);
      
      if (pid == entry->proc_pid) 
      {
         switch (entry->loc) 
         {
            case OP_MEM_FREE_KEY_SRAM_MEM:
               dealloc_fsk(pkp_dev, entry->key_handle);
               break;
            case OP_MEM_FREE_KEY_DDR_MEM:
               dealloc_ex_keymem(pkp_dev, entry->key_handle);
               break;
            case OP_MEM_FREE_KEY_HOST_MEM:
               dealloc_host_keymem(pkp_dev, entry->key_handle);
               break;
         }
              cavium_list_del(&entry->list);
              cavium_free(entry);
         cavium_spin_unlock(&pkp_dev->keymem_lock);
         return;
      }
   }

   cavium_spin_unlock(&pkp_dev->keymem_lock);

   return;   
}

static int
store_host_keymem(cavium_device *pkp_dev, n1_write_key_buf key_buf)
{
   Uint32 key_handle,i;

#ifdef MC2
   key_handle = (Uint32)(key_buf.key_handle);
#else
   key_handle = (Uint32)(key_buf.key_handle & 0xfffdffffffffffffULL);
#endif
   cavium_dbgprint("KEY HANDLE 0x%lx\n",key_handle);

   for (i = 0; i <pkp_dev->host_keymem_count; i++) 
   {
      if (pkp_dev->host_keymem_static_list[i].baddr == key_handle) 
      {
         cavium_dbgprint("KEY HANDLE copied to 0x%lx \n", pkp_dev->host_keymem_static_list[i].vaddr);
         if (cavium_debug_level >=  2)
            cavium_dump("KEY copied:", key_buf.key, (Uint32)key_buf.length);

         cavium_memcpy((void *)(pkp_dev->host_keymem_static_list[i].vaddr), 
                                             key_buf.key, key_buf.length);
         return 0;
      }
   }
   cavium_error("store_host_keymem: key_handle not found\n");
   return -1;
}

#ifdef NPLUS
static int
store_ex_keymem(cavium_device *pkp_dev, n1_write_key_buf key_buf, int ucode_idx)
#else
static int
store_ex_keymem(cavium_device *pkp_dev, n1_write_key_buf key_buf)
#endif
{
   Cmd strcmd;
   Request request;
   Uint64 key_handle;
   Uint64 *completion_address;
   Uint8 *dptr = NULL, *rptr = NULL;
   int ret;
#ifdef NPLUS
   int srq_idx;
#endif
   
   cavium_dbgprint("Inside store_ex_key_mem: key_buf.key_length %d\n",key_buf.length);
   key_handle = (Uint64)(((key_buf.key_handle & (Uint64)0xffffffffffff7fffULL) << 7) |
               (Uint64)0x8000000000000000ULL);
   strcmd.opcode = htobe16(((0x2<<8) | MAJOR_OP_RANDOM_WRITE_CONTEXT));
   strcmd.size = htobe16(((key_buf.length >> 3) - 1));
   strcmd.param = htobe16(0);
   strcmd.dlen = htobe16(((key_buf.length) >> 3));
   cavium_memcpy((Uint8 *)&request, (Uint8 *)&strcmd, 8);

   dptr = (Uint8 *)get_buffer_from_pool(pkp_dev, (key_buf.length));
   if (dptr == NULL) 
   {
      cavium_error(" OOM for key buffer\n");
      ret = -1;
      goto store_exkeymem_clean;
   }
   cavium_memcpy(dptr, key_buf.key, key_buf.length);
   if (cavium_debug_level > 2)
      cavium_dump("KEY copied:", key_buf.key, key_buf.length);
   request.dptr = (Uint64)(Uint32)cavium_vtophys(dptr);
   request.dptr = htobe64(request.dptr);

   rptr = cavium_malloc_dma(8, NULL);
   if (rptr == NULL) 
   {
      cavium_error(" OOM for key buffer\n");
      ret = -1;
      goto store_exkeymem_clean;
   }
   completion_address = (Uint64 *)rptr;
   *completion_address = COMPLETION_CODE_INIT;
   request.rptr = (Uint64)(Uint32)cavium_vtophys(rptr);
   request.rptr = htobe64(request.rptr);

   request.cptr = htobe64(key_handle);

#ifdef NPLUS
   srq_idx = send_command(pkp_dev, &request, 1, ucode_idx, completion_address);
        ret = check_completion(pkp_dev, completion_address, 500, ucode_idx,srq_idx);
#else
   send_command(pkp_dev, &request, 1);
   ret = check_completion(completion_address, 100);
#endif
   if (ret) 
   {
      cavium_error( "Error: %x in storing FSK memory\n", ret);
      ret = -1;
   }

store_exkeymem_clean:
   if (rptr)
   { 
      cavium_free_dma(rptr);
      rptr = NULL;
   }
   if (dptr) 
   {
      put_buffer_in_pool(pkp_dev, dptr);
   }
   return ret;
}

#ifdef NPLUS
static int
store_fsk(cavium_device *pkp_dev, n1_write_key_buf key_buf, int ucode_idx)
#else
static int
store_fsk(cavium_device *pkp_dev, n1_write_key_buf key_buf)
#endif
{
   Cmd strcmd;
   Request request;
   Uint64 key_handle;
   Uint64 *completion_address, *p;
   Uint8 *dptr = NULL, *rptr = NULL;
   int ret;
#ifdef NPLUS
   int srq_idx;
#endif
   
   cavium_dbgprint("Inside store_fsk: key_buf.key_length %d\n",key_buf.length);
   strcmd.opcode = htobe16(((0x0<<8) | MAJOR_OP_RANDOM_WRITE_CONTEXT));
#ifdef MC2
   strcmd.size = htobe16(0);
   strcmd.param = htobe16(0);
   strcmd.dlen = htobe16(8 + key_buf.length);
#else
   strcmd.size = htobe16((key_buf.length >> 3));
   strcmd.param = htobe16(0x8);
   strcmd.dlen = htobe16(((8 + key_buf.length) >> 3));
#endif
   cavium_memcpy((Uint8 *)&request, (Uint8 *)&strcmd, 8);

   dptr = (Uint8 *)get_buffer_from_pool(pkp_dev, (8 + key_buf.length));
   if (dptr == NULL) 
   {
      cavium_error(" OOM for key buffer\n");
      ret = -1;
      goto store_fsk_clean;
   }
   key_handle = (Uint64)(key_buf.key_handle);
   cavium_memcpy(dptr, &key_handle, 8);
   p = (Uint64 *)dptr;
   *p = htobe64(*p);
   cavium_memcpy(dptr + 8, key_buf.key, key_buf.length);
   if (cavium_debug_level > 2)
      cavium_dump("KEY copied:", key_buf.key, key_buf.length);
   request.dptr = (Uint64)(Uint32)cavium_vtophys(dptr);
   request.dptr = htobe64(request.dptr);

   rptr = cavium_malloc_dma(8, NULL);
   if (rptr == NULL) 
   {
      cavium_error(" OOM for key buffer\n");
      ret = -1;
      goto store_fsk_clean;
   }
   completion_address = (Uint64 *)rptr;
   *completion_address = COMPLETION_CODE_INIT;
   request.rptr = (Uint64)(Uint32)cavium_vtophys(rptr);
   request.rptr = htobe64(request.rptr);
   request.cptr = htobe64(0);

#ifdef NPLUS
   srq_idx = send_command(pkp_dev, &request, 1, ucode_idx, completion_address);
        ret = check_completion(pkp_dev, completion_address, 500, ucode_idx,srq_idx);
#else
   send_command(pkp_dev, &request, 1);
   ret = check_completion(completion_address, 100);
#endif

   if (ret) 
   {
      cavium_error( "Error: %x in storing FSK memory\n", ret);
      ret = -1;
   }

store_fsk_clean:
   if (rptr)
   { 
      cavium_free_dma(rptr);
      rptr = NULL;
   }
   if (dptr) 
   {
      put_buffer_in_pool(pkp_dev, dptr);
   }
   return ret;
}

#ifdef NPLUS
int
store_key_mem(cavium_device *pkp_dev, n1_write_key_buf key_buf, int ucode_idx)
#else
int
store_key_mem(cavium_device *pkp_dev, n1_write_key_buf key_buf)
#endif
{
   struct cavium_list_head *tmp;
   struct KEYMEM_ALLOC_ENTRY *entry;
   Uint32 found = 0 ;
   int ret = -1;

   cavium_spin_lock(&pkp_dev->keymem_lock);

   cavium_list_for_each(tmp, &pkp_dev->keymem_head) 
   {
      entry = cavium_list_entry(tmp, struct KEYMEM_ALLOC_ENTRY,
                                list);

      cavium_dbgprint("entry key 0x%lx%lx\n",(Uint32)(entry->key_handle>>32), (Uint32)(entry->key_handle));
      cavium_dbgprint("entry key 0x%lx%lx\n",(Uint32)(key_buf.key_handle>>32), (Uint32)(key_buf.key_handle));
      if (entry->key_handle == key_buf.key_handle) 
      {
         found = 1;
         switch (entry->loc) 
         {
            case OP_MEM_FREE_KEY_SRAM_MEM:
#ifdef NPLUS
               ret = store_fsk(pkp_dev, key_buf, ucode_idx);
#else
               ret = store_fsk(pkp_dev, key_buf);
#endif
               
               break;
            case OP_MEM_FREE_KEY_DDR_MEM:
#ifdef NPLUS
               ret = store_ex_keymem(pkp_dev, key_buf, ucode_idx);
#else
               ret = store_ex_keymem(pkp_dev, key_buf);
#endif
               break;
            case OP_MEM_FREE_KEY_HOST_MEM:
               ret = store_host_keymem(pkp_dev, key_buf);
               break;
         }
      }
   }
   cavium_spin_unlock(&pkp_dev->keymem_lock);

   if (!found) 
   {
      cavium_error("store_key_mem: key_handle not found\n");
      return -1;
   }
   return ret;
}
#ifdef CAVIUM_RESOURCE_CHECK
int
insert_key_entry(struct cavium_list_head *key_head, Uint64 key_handle)
{
   struct KEY_ENTRY *entry;
   
   entry = cavium_malloc(sizeof(struct KEY_ENTRY), NULL);
   if (entry == NULL) 
   {
      cavium_error("Insert-key-entry: Not enough memory\n");
      return -1;
   }

   entry->key_handle = key_handle;
   cavium_list_add_tail(&entry->list, key_head);   
   
   return 0;
}
#endif

/*
 * $Id: key_memory.c,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: key_memory.c,v $
 * Revision 1.4  2015/01/06 08:57:47  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:54  cding
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
 * Revision 1.26  2005/10/24 06:53:51  kanantha
 * - Fixed RHEL4 warnings
 *
 * Revision 1.25  2005/10/13 09:25:13  ksnaren
 * changed cavium_malloc_dma to cavium_malloc_virt for the control structs
 *
 * Revision 1.24  2005/09/29 03:51:16  ksadasivuni
 * - Fixed some warnings
 *
 * Revision 1.23  2005/09/27 05:23:52  sgadam
 * Warning fixed
 *
 * Revision 1.22  2005/09/21 06:54:49  lpathy
 * Merging windows server 2003 release with CVS head
 *
 * Revision 1.21  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.20  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.19  2005/08/31 18:10:30  bimran
 * Fixed several warnings.
 * Fixed the corerct use of ALIGNMENT and related macros.
 *
 * Revision 1.18  2005/07/21 09:28:32  sgadam
 * pointer are asigned NULL after freeing them.
 *
 * Revision 1.17  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.16  2005/06/03 08:07:55  rkumar
 * Moved cavium_prints to cavium_dbgprint
 *
 * Revision 1.15  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.14  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.13  2004/07/29 19:55:26  tsingh
 * Bug fix from India office.
 *
 * Revision 1.1.1.1  2004/07/28 06:43:29  rkumar
 * Initial Checkin
 *
 * Revision 1.12  2004/06/23 20:45:26  bimran
 * compiler warnings on NetBSD.
 *
 * Revision 1.8  2004/05/04 20:48:34  bimran
 * Fixed RESOURCE_CHECK.
 *
 * Revision 1.7  2004/05/02 19:45:31  bimran
 * Added Copyright notice.
 *
 * Revision 1.6  2004/04/27 01:29:28  tsingh
 * Fixed another bug in extended key memory store. The function was doing RSHIFT of 7 bits whereas it had to do LSHIFT :-)
 *
 * Revision 1.5  2004/04/27 00:31:01  tsingh
 * Changed debug print to print
 *
 * Revision 1.4  2004/04/27 00:02:39  tsingh
 * Fixed extended key memory load bug. (bimran).
 *
 * Revision 1.3  2004/04/23 22:40:19  bimran
 * Fixed dealloc_host_key_mem() function. Bit 49 must be cleared for MC1.
 *
 * Revision 1.2  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

