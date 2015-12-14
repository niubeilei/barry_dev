////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_proc.c
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
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <asm/errno.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include "cavium_list.h"
#include "cavium.h"
#include "linux_main.h"
#include "init_cfg.h"
#include "buffer_pool.h"

static struct proc_dir_entry    *cavium_dir, 
            *debug_level_file, 
            *timeout_file, 
            *data_file, 
            *version_file, 
            *stats_file,
            *contextStats_file,
            *keymemStats_file,
            *cmdquelocation_file,
            *reg_file;

int cavium_debug_level = 0;
extern Uint32 ipsec_chunk_count[MAX_DEV];
extern Uint32 ssl_chunk_count[MAX_DEV];
extern Uint32 fsk_mem_chunk_count[MAX_DEV];
extern Uint32 ex_key_mem_chunk_count[MAX_DEV];
extern Uint32 host_key_mem_chunk_count[MAX_DEV];
extern int dev_count;
extern cavium_device cavium_dev[];
extern Uint32 cavium_command_timeout;
extern Uint32 allocated_context_count;
extern Uint8 cavium_version[3];
extern Uint32    buffer_stats[], 
      alloc_buffer_stats[], 
      fragment_buf_stats[], 
      other_pools[];

static int
proc_write_timeout(struct file *file, const char *buffer,
             unsigned long count, void *data)
{
   char str[10], *strend;
   Uint32 timeout;

   CAVIUM_MOD_INC_USE_COUNT;

   if(cavium_copy_in(str, (unsigned char *)buffer, count)) 
   {
      cavium_error("cavium_copy_out failed\n");
      return -EFAULT;
   }
   str[count] = '\0';

   timeout = simple_strtoul(str, &strend, 0); 
   if (timeout > (2*60*60)) 
   { 
      cavium_error("Timeout too high. The maximum value is 7200\n");
      return -EFAULT;
   }
   cavium_command_timeout = timeout*HZ;

   CAVIUM_MOD_DEC_USE_COUNT;

   return count;
}
                
static int
proc_read_stats(char *page, char **start, off_t off, int count,
            int *eof, void *data)
{
   int len = 0;
   pool i;
   
   CAVIUM_MOD_INC_USE_COUNT;
   
   for ( i = 0; i < BUF_POOLS; i++) 
   {
#ifndef CAVIUM_HUGE_MEMORY
      if (i < 4)
         continue;
#endif
      switch (i) 
      {
         case ex_tiny:
            len += sprintf(page + len, "EX-TINY BUFFERS\n");
            len += sprintf(page + len, "***************\n");
            break;
         case tiny:
            len += sprintf(page + len, "TINY BUFFERS\n");
            len += sprintf(page + len, "*************\n");
            break;
         case small:
            len += sprintf(page + len, "SMALL BUFFERS\n");
            len += sprintf(page + len, "*************\n");
            break;
         case medium:
            len += sprintf(page + len, "MEDIUM BUFFERS\n");
            len += sprintf(page + len, "*************\n");
            break;
         case large:
            len += sprintf(page + len, "LARGE BUFFERS\n");
            len += sprintf(page + len, "*************\n");
            break;
         case huge_pool:
            len += sprintf(page + len, "HUGE BUFFERS\n");
            len += sprintf(page + len, "*************\n");
            break;
         case os:
            break;
      }
      len += sprintf(page + len, "Initial Buffer Count of pool: %ld\n", buffer_stats[i]);
      len += sprintf(page + len, "Allocated Buffers of pool: %ld\n", alloc_buffer_stats[i]);
      len += sprintf(page + len, "Fragmented Buffers of pool: %ld\n", fragment_buf_stats[i]);
      len += sprintf(page + len, "Buffers given for fragmentation from pool: %ld\n", other_pools[i]);
      len += sprintf(page + len, "#########################################\n");
   }

   CAVIUM_MOD_DEC_USE_COUNT;

   return len;
}

/********************************************* 
 *
 * Name : proc_read_context()
 *
 *  Function that will be called upon reading 
 *  the /proc/cavium/contextStats_file
 *
 *********************************************/
static int
proc_read_context(char *page, char **start, off_t off, int count,
                      int *eof, void *data)                
{
  int i,len = 0;

  /* Increment the module usage counter */
   
  CAVIUM_MOD_INC_USE_COUNT ;

  /* Get the value of the initial context from the cavium_device structure */

  len+= sprintf(page+len,"\n\n#### The Initial/Current count of the contexts allocated ####\n\n");

  for (i= 0 ; i < dev_count ; i++) 
  {
        len+= sprintf(page+len,"[Device %d] Initial IPSec Contexts --> %ld\n\n",i,ipsec_chunk_count[i]);
        len+= sprintf(page+len,"[Device %d] Initial SSL Contexts --> %ld\n\n",i,ssl_chunk_count[i]);
  }
 
  for (i = 0 ; i < dev_count ; i++) 
  {  
     len+= sprintf(page+len,"[Device %d] Current IPSec Contexts--> %d\n\n",i,cavium_dev[i].ctx_ipsec_count);
     len+= sprintf(page+len,"[Device %d] Current SSL Contexts--> %d\n\n",i,cavium_dev[i].ctx_ssl_count);
        len+= sprintf(page+len,"Total number of allocated contexts --> %ld\n\n",allocated_context_count);
  }

  CAVIUM_MOD_DEC_USE_COUNT;
 
  return len;
}

/**************************************** 
 * Name : proc_read_keymem
 *
 * This function is called when the file 
 * /proc/cavium/keymemStats file is read
 *
 ****************************************/

static int 
proc_read_keymem(char *page, char **start, off_t off, int count,
                     int *eof, void *data)
{
 int i,len=0;

 /* Increment the module usage counter */

 CAVIUM_MOD_INC_USE_COUNT;

 /* Get the value of the initial context from the 
    global array of initial key counters 
  */

 len+= sprintf(page+len,"\n\n#### The initial count of the key memories allocated ####\n\n");
 
 for (i=0; i< dev_count ; i++)
 {
      len+= sprintf(page+len,"[Device %d] Initial SRAM key memory chunk count --> %ld\n\n",i,fsk_mem_chunk_count[i]);
      len+= sprintf(page+len,"[Device %d] Initial DRAM key memory chunk count --> %ld\n\n",i,ex_key_mem_chunk_count[i]);
      len+= sprintf(page+len,"[Device %d] Initial Host key memory chunk count --> %ld\n\n\n\n",i,host_key_mem_chunk_count[i]);
 }

 len+= sprintf(page+len,"\n\n#### The Current count of the key memories allocated ####\n\n");

 for (i=0;i< dev_count ; i++)
 {
      len+= sprintf(page+len,"[Device %d] Current SRAM key memory chunk allocated --> %ld\n\n",i,cavium_dev[i].fsk_free_index);
      len+= sprintf(page+len,"[Device %d] Current SRAM key memory chunk remaining --> %ld\n\n\n",i,cavium_dev[i].fsk_chunk_count - cavium_dev[i].fsk_free_index);

      len+= sprintf(page+len,"[Device %d] Current DRAM key memory chunk allocated --> %ld\n\n",i,cavium_dev[i].ex_keymem_free_index );
      len+= sprintf(page+len,"[Device %d] Current DRAM key memory chunk remaining --> %ld\n\n\n",i,cavium_dev[i].ex_keymem_chunk_count - 
                                                                                                cavium_dev[i].ex_keymem_free_index );
      len+= sprintf(page+len,"[Device %d] Current Host key memory chunk allocated --> %ld\n\n",i,cavium_dev[i].host_keymem_free_index );
      len+= sprintf(page+len,"[Device %d] Current Host key memory chunk remaining --> %ld\n\n\n",i,cavium_dev[i].host_keymem_count -
                                                                                                cavium_dev[i].host_keymem_free_index );
 }

 CAVIUM_MOD_DEC_USE_COUNT;

 return len;
} 

/********************************************
 * Name : proc_read_cmd_queue
 *
 * This function is called when the file
 * /proc/cavium/cmdquelocation_file is read.
 *
 ********************************************/

static int 
proc_read_cmd_queue(char *page, char **start, off_t off, int count,
                          int *eof, void *data)
{
  int dev_no,que_no,len = 0;
  Uint32 queue_location;

  /* Increment the module usage counter */

  CAVIUM_MOD_INC_USE_COUNT;
 
  /* Get the value of the command queue pointer location 
     for all the command queues */

  len += sprintf(page+len,"\n\n #### The command queue pointer locations ####\n\n");

  for (dev_no=0; dev_no < dev_count; dev_no++)
 {
      for (que_no = 0; que_no < MAX_N1_QUEUES ; que_no++)
      {
            
           /* Get the difference in the address of the command_queue_front pointer and the command_queue_base pointer */
 
           queue_location = cavium_dev[dev_no].command_queue_front[que_no] - cavium_dev[dev_no].command_queue_base[que_no]; 
          queue_location = queue_location / COMMAND_BLOCK_SIZE;

           len+= sprintf(page+len,"[Device : %d Queue %d :] The command pointer is pointing to queue location : %ld\n\n",dev_no,que_no,queue_location);    
      }
  }

  CAVIUM_MOD_DEC_USE_COUNT;
 
  return len; 
}


static int
proc_read_timeout(char *page, char **start, off_t off, int count,
            int *eof, void *data)
{
   int len = 0;
   
   CAVIUM_MOD_INC_USE_COUNT;
   
   len = sprintf(page + len, "%ld\n", (cavium_command_timeout/HZ));

   CAVIUM_MOD_DEC_USE_COUNT;

   return len;
}

static int
proc_read_version(char *page, char **start, off_t off, int count,
            int *eof, void *data)
{
   int len = 0;
   
   CAVIUM_MOD_INC_USE_COUNT;
   
   len = sprintf(page + len, "%01d.%02d-%c\n", cavium_version[0],cavium_version[1],cavium_version[2]);

   CAVIUM_MOD_DEC_USE_COUNT;

   return len;
}

static int
proc_write_debug_level(struct file *file, const char *buffer,
             unsigned long count, void *data)
{
   char str[10], *strend;

   CAVIUM_MOD_INC_USE_COUNT;

   if(cavium_copy_in(str, (char *)buffer, count))
   {
      cavium_error("cavium_copy_out failed\n");
      return -EFAULT;
   }
   str[count] = '\0';

   cavium_debug_level = simple_strtoul(str, &strend, 0); 

   CAVIUM_MOD_DEC_USE_COUNT;

   return count;
}
                
static int
proc_read_debug_level(char *page, char **start, off_t off, int count,
            int *eof, void *data)
{
   int len = 0;
   
   CAVIUM_MOD_INC_USE_COUNT;
   
   len = sprintf(page + len, "%d\n", cavium_debug_level);

   CAVIUM_MOD_DEC_USE_COUNT;

   return len;
}



static int
proc_read_data(char *page, char **start, off_t off, int count, int *eof, 
          void *data)
{
   int len = 0, i,j;
   cavium_device * ptr;

   CAVIUM_MOD_INC_USE_COUNT;

   for (j = 0; j < dev_count; j++) {
      ptr = &cavium_dev[j];
      len += sprintf(page + len, "\tN1/N1-LITE DEVICE (%d)\n",j);
      len += sprintf(page + len, "PCI Vendor ID = 0x%x\n", VENDOR_ID);
      len += sprintf(page + len, "PCI Device ID = 0x%x\n", (((struct pci_dev *)(ptr->dev))->device));
      len += sprintf(page + len, "PCI BAR 0 = 0x%lx\n", ptr->bar_0);
      len += sprintf(page + len, "PCI BAR 2 = 0x%lx\n", ptr->bar_1);
      for ( i =0 ; i < 4; i++) 
      {
         len += sprintf(page + len, "Command Queue[%d] Virt Address %p Bus Address %08lx\n",
                i, ptr->command_queue_base[i], 
                          ptr->command_queue_bus_addr[i]);
         len += sprintf(page + len, "Command Queue[%d] Front %p End%p\n",
            i, ptr->command_queue_front[i], 
            ptr->command_queue_end[i]);   
      }
      len += sprintf(page + len, "Max command queue = %ld\n",
                ptr->command_queue_max);
      len += sprintf(page + len, "Debug Level = %d\n", cavium_debug_level);
      len += sprintf(page + len, "******************************************************\n");
   }

   CAVIUM_MOD_DEC_USE_COUNT;

   return len;
}

static int
proc_read_regs(char *page, char **start, off_t off, int count, int *eof, 
          void *data)
{
   int len = 0, i;
   Uint32 dwval = 0;
   cavium_device * ptr;

   CAVIUM_MOD_INC_USE_COUNT;

   for (i = 0; i < dev_count; i++)
   {
      ptr = &cavium_dev[i];
      /* BAR0 + 0 */
      read_PKP_register(ptr, (ptr->bar_0 + COMMAND_STATUS), &dwval);
      len += sprintf(page + len, "Command Status Register: 0x%lx\n", dwval);
      /* BAR0 + 10h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + UNIT_ENABLE), &dwval);
      len += sprintf(page + len, "Core Enable Register: 0x%lx\n", dwval);
      /* BAR0 + 18h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + UCODE_LOAD), &dwval);
      len += sprintf(page + len, "UCODE Load Register: 0x%lx\n", dwval);
      /* BAR0 + 20h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + IMR_REG), &dwval);
      len += sprintf(page + len, "Interrupt Enable Register: 0x%lx\n",
                dwval);
      /* BAR0 + 28h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + ISR_REG), &dwval);
      len += sprintf(page + len, "Interrupt Status Register: 0x%lx\n",
                dwval);
      /* BAR0 + 30h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + FAILING_SEQ_REG), &dwval);
      len += sprintf(page + len, "Core Error Address Register: 0x%lx\n",
                dwval);
      /* BAR0 + 38h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + FAILING_EXEC_REG), &dwval);
      len += sprintf(page + len, "Core Error Status Register: 0x%lx\n",
                dwval);
      /* BAR0 + 68h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + DEBUG_REG), &dwval);
      len += sprintf(page + len, "Internal Status Register: 0x%lx\n",
                dwval);
      /* BAR0 + D0h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_0 + PCI_ERR_REG), &dwval);
      len += sprintf(page + len, "PCI Error Register: 0x%lx\n",
                dwval);
      /* BAR2 + 00h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ0_BASE_HIGH), &dwval);
      len += sprintf(page + len, "IQM 0 Base Address High Register: 0x%lx\n",
                dwval);
      /* BAR2 + 08h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ0_BASE_LOW), &dwval);
      len += sprintf(page + len, "IQM 0 Base Address Low Register: 0x%lx\n",
                dwval);
      /* BAR2 + 10h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ0_SIZE), &dwval);
      len += sprintf(page + len, "IQM 0 Queue Size Register: 0x%lx\n",
                dwval);
      /* BAR2 + 18h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ0_DOOR_BELL), &dwval);
      len += sprintf(page + len, "IQM 0 Door Bell Register: 0x%lx\n",
                dwval);
      /* BAR2 + 20h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ1_BASE_HIGH), &dwval);
      len += sprintf(page + len, "IQM 1 Base Address High Register: 0x%lx\n",
                       dwval);
      /* BAR2 + 28h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ1_BASE_LOW), &dwval);
      len += sprintf(page + len, "IQM 1 Base Address Low Register: 0x%lx\n",
                  dwval);
      /* BAR2 + 30h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ1_SIZE), &dwval);
      len += sprintf(page + len, "IQM 1 Queue Size Register: 0x%lx\n",
                dwval);
      /* BAR2 + 38h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ1_DOOR_BELL), &dwval);
      len += sprintf(page + len, "IQM 1 Door Bell Register: 0x%lx\n",
                dwval);
      /* BAR2 + 40h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ2_BASE_HIGH), &dwval);
      len += sprintf(page + len, "IQM 2 Base Address High Register: 0x%lx\n",
                dwval);
      /* BAR2 + 48h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ2_BASE_LOW), &dwval);
      len += sprintf(page + len, "IQM 2 Base Address Low Register: 0x%lx\n",
                dwval);
      /* BAR2 + 50h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ2_SIZE), &dwval);
      len += sprintf(page + len, "IQM 2 Queue Size Register: 0x%lx\n",
                dwval);
      /* BAR2 + 58h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ2_DOOR_BELL), &dwval);
      len += sprintf(page + len, "IQM 2 Door Bell Register: 0x%lx\n",
                dwval);
      /* BAR2 + 60h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ3_BASE_HIGH), &dwval);
      len += sprintf(page + len, "IQM 3 Base Address High Register: 0x%lx\n",
                dwval);
      /* BAR2 + 68h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ3_BASE_LOW), &dwval);
      len += sprintf(page + len, "IQM 3 Base Address Low Register: 0x%lx\n",
                dwval);
      /* BAR2 + 70h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ3_SIZE), &dwval);
      len += sprintf(page + len, "IQM 3 Queue Size Register: 0x%lx\n",
                dwval);
      /* BAR2 + 78h */
      dwval = 0;
      read_PKP_register(ptr, (ptr->bar_1 + REQ3_DOOR_BELL), &dwval);
      len += sprintf(page + len, "IQM 3 Door Bell Register: 0x%lx\n",
                dwval);
   }

   CAVIUM_MOD_DEC_USE_COUNT;

   return len;
}


int 
cavium_init_proc(void)
{
        int rv = 0;

        /* create directory /proc/cavium */
        cavium_dir = proc_mkdir("cavium", NULL);
        if(cavium_dir == NULL) {
                rv = -ENOMEM;
                goto out;
        }

        cavium_dir->owner = THIS_MODULE;


        /* create debug_level */
        debug_level_file = create_proc_entry("debug_level", 0644, cavium_dir);
        if(debug_level_file == NULL)
        {
                rv = -ENOMEM;
                goto out;
        }

        debug_level_file->data = &cavium_debug_level;
        debug_level_file->read_proc = proc_read_debug_level;
        debug_level_file->write_proc = proc_write_debug_level;
        debug_level_file->owner = THIS_MODULE;


         /* create timeout file */
         timeout_file = create_proc_entry("command_timeout", 0644, cavium_dir);
         if(timeout_file == NULL)
         {
                 rv = -ENOMEM;
                 goto out;
         }
 
         timeout_file->data = &cavium_command_timeout;
         timeout_file->read_proc = proc_read_timeout;
         timeout_file->write_proc = proc_write_timeout;
         timeout_file->owner = THIS_MODULE;
 
    /* create version file*/
         version_file = create_proc_read_entry("version",
                                            0444, cavium_dir,
                                            proc_read_version,
                                            NULL);
         if (version_file == NULL)
         {
                 rv  = -ENOMEM;
       goto out;
         }
 
    version_file->owner = THIS_MODULE;

   /* create Stats file*/
        stats_file = create_proc_read_entry("stats",
                                           0444, cavium_dir,
                                           proc_read_stats,
                                           NULL);
        if(stats_file == NULL)
        {
                rv  = -ENOMEM;
      goto out;
        }

   stats_file->owner = THIS_MODULE;

        /* Creates Context Stats file */

        contextStats_file = create_proc_read_entry("contextStats",
                                                   0444, cavium_dir,
                                                   proc_read_context,
                                                   NULL);  
        
        if (contextStats_file == NULL)
        {
                 rv = -ENOMEM;
                 goto out;
        }

        contextStats_file->owner = THIS_MODULE;

        /* Creates Key Memory Stats file */
       
        keymemStats_file = create_proc_read_entry("keymemStats",
                               0444,cavium_dir,
                                                  proc_read_keymem,
                    NULL);
        if (keymemStats_file == NULL)
        {
           rv = -ENOMEM;
           goto out;
        } 
 
        keymemStats_file->owner = THIS_MODULE;

        /* Create Command queue pointer location file */

        cmdquelocation_file = create_proc_read_entry("cmdquelocation",
                                                     0444,cavium_dir,
                                                     proc_read_cmd_queue,
                                                     NULL);
                    

        cmdquelocation_file->owner = THIS_MODULE;
   /* create data_file */
        data_file = create_proc_read_entry("data",
                                           0444, cavium_dir,
                                           proc_read_data,
                                           NULL);
        if(data_file == NULL)
        {
           rv  = -ENOMEM;
           goto out;
        }

   data_file->owner = THIS_MODULE;

   /* create Register file*/
        reg_file  = create_proc_read_entry("regs",
                                           0444, cavium_dir,
                                           proc_read_regs,
                                           NULL);
        if(reg_file == NULL)
        {
                rv  = -ENOMEM;
                goto out;
        }

   reg_file->owner = THIS_MODULE;
        return 0;
out:
   if (reg_file)
   {
      remove_proc_entry("regs", cavium_dir);
   }
   if (data_file)
   {
      remove_proc_entry("data", cavium_dir);
   }
   if (version_file)
   {
      remove_proc_entry("version", cavium_dir);
   }
   if (stats_file)
   {
      remove_proc_entry("stats", cavium_dir);
   }
   if (contextStats_file)
   {
      remove_proc_entry("contextStats", cavium_dir);
   }
   if (keymemStats_file)
   {
      remove_proc_entry("keymemStats", cavium_dir);
   }
   if (cmdquelocation_file)
   {
      remove_proc_entry("cmdquelocation", cavium_dir);
   }
   if (timeout_file)
   {
        remove_proc_entry("command_timeout", cavium_dir);
   }
   if (debug_level_file)
   {
      remove_proc_entry("debug_level", cavium_dir);
   }
#if LINUX_VERSION_CODE > KERNEL_VERSION (2,6,0)
   if (cavium_dir)
   {
      remove_proc_entry("cavium", NULL);
   }
#endif
   
   return rv;
}


void
cavium_free_proc(void)
{
   if (reg_file)
   {
      cavium_print("Freeing regs\n");
      remove_proc_entry("regs", reg_file);
   }
   if (data_file)
   {
      cavium_print("Freeing data\n");
      remove_proc_entry("data", data_file);
   }
   if (version_file)
   {
      cavium_print("Freeing version\n");
      remove_proc_entry("version", version_file);
   }
   if (stats_file)
   {
      cavium_print("Freeing stats\n");
      remove_proc_entry("stats", stats_file);
   }
   if (contextStats_file)
   {
      cavium_print("Freeing contextstats\n");
      remove_proc_entry("contextStats", contextStats_file);
   }
   if (keymemStats_file)
   {
      cavium_print("Freeing keymemstats\n");
      remove_proc_entry("keymemStats", keymemStats_file);
   }
   if (cmdquelocation_file)
   {
      cavium_print("Freeing cmdqueuelocation\n");
      remove_proc_entry("cmdquelocation", cmdquelocation_file);
   }
   if (timeout_file)
   {
      cavium_print("Freeing commandtimeout\n");
      remove_proc_entry("command_timeout", timeout_file);
    }
   if (debug_level_file)
   {
      cavium_print("Freeing debuglevel\n");
      remove_proc_entry("debug_level", debug_level_file);
   }
#if LINUX_VERSION_CODE > KERNEL_VERSION (2,6,0)
   if (cavium_dir)
   {
      cavium_print("Freeing dir\n");
      remove_proc_entry("cavium", cavium_dir);
   }
#endif
   return;
}

/*
 * $Id: cavium_proc.c,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium_proc.c,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:52  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:29  cding
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
 * Revision 1.11  2005/10/24 06:42:42  kanantha
 * - remove_proc_entry API argument 2 is changed with parent directory to fix crash
 *
 * Revision 1.10  2005/10/13 09:16:40  ksnaren
 * fixed to get compiled for windows xp
 *
 * Revision 1.9  2005/08/31 02:17:24  bimran
 * Fixed bug where it was freeing proc entries with different names.
 *
 * Revision 1.8  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.7  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.6  2005/02/01 04:07:12  bimran
 * copyright fix
 *
 * Revision 1.5  2004/08/03 20:44:10  tahuja
 * support for Mips Linux & HT.
 *
 * Revision 1.4  2004/07/07 17:59:31  tsingh
 * some compilation issues
 *
 * Revision 1.3  2004/06/03 21:17:26  bimran
 * included cavium_list.h
 *
 * Revision 1.2  2004/05/02 19:43:58  bimran
 * Added Copyright notice.
 *
 * Revision 1.1  2004/04/15 22:40:47  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

