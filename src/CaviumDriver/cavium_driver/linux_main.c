////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: linux_main.c
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

/*------------------------------------------------------------------------------
 * 
 *      Linux Driver main file -- this file contains the driver code.
 *
 *----------------------------------------------------------------------------*/


int kernel_call;
#include "cavium_sysdep.h"
#include "cavium_common.h"
#include "cavium_ioctl.h"
#include "cavium_endian.h"
#include <linux/poll.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
#include <linux/wrapper.h>
#else
#include <linux/page-flags.h>
#endif
#include <linux/kdev_t.h>
#include "cavium_list.h"
#include "cavium.h"
#include "init_cfg.h"
#include "linux_main.h"
#include "poll_thread.h"
#include "cavium_proc.h"
#include "request_manager.h"
#include "context_memory.h"
#include "microcode.h"
#include "bl_nbl_list.h"
#include "buffer_pool.h"
#include "hw_lib.h"
#include "key_memory.h"
#include "cavium_random.h"
#include "command_que.h"
#ifdef NPLUS
#include "soft_req_queue.h"
#endif
#include "completion_dma_free_list.h"
#include "direct_free_list.h"
#include "pending_free_list.h"
#include "pending_list.h"
//#include "cavium_kernel_common.h"

// begin added by liqin 
#include "aos_poll_timer.h"		
#include "aos_request.h" 	// 2006/06/09
// end added by liqin 


MODULE_AUTHOR("Cavium Networks <www.cavium.com>");
MODULE_DESCRIPTION("Nitrox-Lite driver");
MODULE_LICENSE("CAVIUM");
/*
 * Device driver entry points
 */
int    initmodule (void);
void   cleanupmodule (void);
int    n1_ioctl (struct inode *, struct file *, unsigned int, Uint32);
int    n1_open (struct inode *, struct file *);
int    n1_release (struct inode *, struct file *);
#ifndef CAVIUM_NO_MMAP
int	n1_mmap(struct file *, struct vm_area_struct *);
#endif
unsigned int n1_poll(struct file *, poll_table *);

struct N1_Dev *device_list = NULL;

extern cavium_device cavium_dev[];
extern int dev_count;
extern Uint8 cavium_version[3];
static int driver_removal = 0;
struct ni_extension *pdx;



#ifdef EXPORT_SYMTAB
EXPORT_SYMBOL(n1_ioctl);
EXPORT_SYMBOL(n1_open);
EXPORT_SYMBOL(n1_release);
#ifndef CAVIUM_NO_MMAP
EXPORT_SYMBOL(n1_mmap);
#endif
EXPORT_SYMBOL(n1_poll);
EXPORT_SYMBOL(init_blocking_non_blocking_lists);
EXPORT_SYMBOL(push_user_info);
EXPORT_SYMBOL(del_user_info_from_list);
EXPORT_SYMBOL(check_nb_command_pid);
EXPORT_SYMBOL(check_nb_command_id);
EXPORT_SYMBOL(cleanup_nb_command_id);
EXPORT_SYMBOL(cleanup_nb_command_pid);
EXPORT_SYMBOL(init_buffer_pool);
EXPORT_SYMBOL(free_buffer_pool);
EXPORT_SYMBOL(get_buffer_from_pool);
EXPORT_SYMBOL(put_buffer_in_pool);
EXPORT_SYMBOL(pkp_setup_direct_operation);
EXPORT_SYMBOL(pkp_setup_sg_operation);
EXPORT_SYMBOL(check_endian_swap);
EXPORT_SYMBOL(pkp_unmap_user_buffers);
EXPORT_SYMBOL(pkp_flush_input_buffers);
EXPORT_SYMBOL(pkp_invalidate_output_buffers);
EXPORT_SYMBOL(check_completion);
EXPORT_SYMBOL(init_rnd_buffer);
EXPORT_SYMBOL(cleanup_rnd_buffer);
EXPORT_SYMBOL(fill_rnd_buffer);
EXPORT_SYMBOL(get_rnd);
EXPORT_SYMBOL(reset_command_queue);
EXPORT_SYMBOL(inc_front_command_queue);
EXPORT_SYMBOL(cleanup_command_queue);
EXPORT_SYMBOL(init_command_queue);
EXPORT_SYMBOL(init_completion_dma_free_list);
EXPORT_SYMBOL(get_completion_dma);
EXPORT_SYMBOL(put_completion_dma);
EXPORT_SYMBOL(get_completion_dma_bus_addr);
EXPORT_SYMBOL(cleanup_completion_dma_free_list);
#ifdef CAVIUM_RESOURCE_CHECK
EXPORT_SYMBOL(insert_ctx_entry);
#endif
EXPORT_SYMBOL(init_context); 
EXPORT_SYMBOL(cleanup_context);
EXPORT_SYMBOL(alloc_context);
EXPORT_SYMBOL(dealloc_context);
EXPORT_SYMBOL(alloc_context_id);
EXPORT_SYMBOL(dealloc_context_id);
EXPORT_SYMBOL(init_direct_free_list);
EXPORT_SYMBOL(get_direct_entry);
EXPORT_SYMBOL(put_direct_entry);
EXPORT_SYMBOL(cleanup_direct_free_list);
EXPORT_SYMBOL(enable_request_unit);
EXPORT_SYMBOL(disable_request_unit);
EXPORT_SYMBOL(enable_exec_units);
EXPORT_SYMBOL(disable_all_exec_units);
EXPORT_SYMBOL(enable_data_swap);
EXPORT_SYMBOL(set_PCIX_split_transactions);
EXPORT_SYMBOL(set_PCI_cache_line);
EXPORT_SYMBOL(get_exec_units);
EXPORT_SYMBOL(set_soft_reset);
EXPORT_SYMBOL(do_soft_reset);
EXPORT_SYMBOL(count_set_bits);
EXPORT_SYMBOL(cavium_pow);
EXPORT_SYMBOL(get_exec_units_part);
EXPORT_SYMBOL(check_core_mask);
EXPORT_SYMBOL(enable_local_ddr);
EXPORT_SYMBOL(check_dram);
EXPORT_SYMBOL(enable_rnd_entropy);
EXPORT_SYMBOL(get_first_available_core);
EXPORT_SYMBOL(get_unit_id);
EXPORT_SYMBOL(enable_exec_units_from_mask);
EXPORT_SYMBOL(disable_exec_units_from_mask);
EXPORT_SYMBOL(setup_request_queues);
#if defined (CN1220) || defined (CN1230) || defined (CN1320) || defined (CN1330)
EXPORT_SYMBOL(init_twsi);
#endif
#ifdef NPLUS
EXPORT_SYMBOL(get_enabled_units);
EXPORT_SYMBOL(cycle_exec_units_from_mask);
#endif
#ifdef CAVIUM_RESOURCE_CHECK
EXPORT_SYMBOL(insert_key_entry);
#endif
EXPORT_SYMBOL(init_key_memory);
EXPORT_SYMBOL(cleanup_key_memory);
EXPORT_SYMBOL(store_key_mem);
EXPORT_SYMBOL(alloc_key_memory);
EXPORT_SYMBOL(dealloc_key_memory);
EXPORT_SYMBOL(flush_key_memory);
EXPORT_SYMBOL(init_pending_free_list);
EXPORT_SYMBOL(get_pending_entry);
EXPORT_SYMBOL(put_pending_entry);
EXPORT_SYMBOL(cleanup_pending_free_list);
EXPORT_SYMBOL(push_pending);
EXPORT_SYMBOL(push_pending_ordered);
EXPORT_SYMBOL(push_pending_unordered);
EXPORT_SYMBOL(poll_pending_ordered);
EXPORT_SYMBOL(poll_pending_unordered);
EXPORT_SYMBOL(finalize_request);
EXPORT_SYMBOL(get_queue_head_ordered);
EXPORT_SYMBOL(get_queue_head_unordered);
EXPORT_SYMBOL(check_for_completion_callback);
EXPORT_SYMBOL(send_command);
EXPORT_SYMBOL(do_operation);
EXPORT_SYMBOL(do_request);
EXPORT_SYMBOL(user_scatter);
#endif



/*
 * Global variables
 */

static struct file_operations n1_fops =
{
   open:      n1_open,
   release:   n1_release,
   read:      NULL,
   write:     NULL,      
   ioctl:     n1_ioctl,
#ifndef CAVIUM_NO_MMAP
   mmap:      n1_mmap,
#else
   mmap:      NULL,
#endif
   poll:      n1_poll,
};


/*
 * General
 */
int setup_interrupt(cavium_device *pdev);
void free_interrupt(cavium_device *pdev);
#ifdef INTERRUPT_RETURN
extern int
#else
extern void
#endif
cavium_interrupt_handler(void *);

int
cavium_init_one(struct pci_dev *dev)
{
   cavium_config cavium_cfg;
   Uint32 bar_0, bar_2;
#if LINUX_VERSION_CODE  < KERNEL_VERSION (2,6,0)
   int ret_val;
#endif

   /* Enable PCI Device */
   if(pci_enable_device(dev))
   {
      cavium_error("pci_enable_device failed\n");
      return -1;
   }

   /* Read BAR 0 and BAR 2 */
   bar_0 = pci_resource_start(dev, 0);
   bar_2 = pci_resource_start(dev, 2);
   
   cavium_print( "bar 0: %lx\n",bar_0);
   cavium_print( "bar 2: %lx\n",bar_2);

#if LINUX_VERSION_CODE  < KERNEL_VERSION (2,6,0)
   ret_val = check_region(bar_0, 0xff);
   if(ret_val < 0) {
      cavium_error("region checking for bar 0 io ports failed\n");
      return ret_val;
   }
#endif
   
#if LINUX_VERSION_CODE  > KERNEL_VERSION (2,6,0)
   if (!request_region(bar_0, 0xff,(const Uint8 *)"pkp_drv")) {
      cavium_error("region checking for bar 0 io ports failed\n");
      return -1;
   }
#else
   request_region(bar_0, 0xff,(const Uint8 *)"pkp_drv");
#endif
   
#if LINUX_VERSION_CODE  < KERNEL_VERSION (2,6,0)
   ret_val = check_region(bar_2,0xff);
   if(ret_val < 0) {
      cavium_error("region checking for bar 2 io ports failed\n");
      release_region(bar_0, 0xff);
      return ret_val;
   }
#endif
#if LINUX_VERSION_CODE  > KERNEL_VERSION (2,6,0)
   if (!request_region(bar_2, 0xff,(const Uint8 *)"pkp_drv")) {
      cavium_error("region checking for bar 2 io ports failed\n");
      return -1;
   }
#else
   request_region(bar_2, 0xff,(const Uint8 *)"pkp_drv");
#endif
   cavium_memset(&cavium_dev[dev_count], 0, sizeof(cavium_device));
   
   cavium_cfg.dev= dev;
   cavium_cfg.bus_number = dev->bus->number; 
   cavium_cfg.dev_number = PCI_SLOT(dev->devfn);
   cavium_cfg.func_number = PCI_FUNC(dev->devfn);
   cavium_cfg.bar_0 = bar_0;
   cavium_cfg.bar_2 = bar_2;
   
   /* nr. of 32 byte contiguous structures */
   cavium_cfg.command_queue_max = CAVIUM_COMMAND_QUEUE_SIZE; 
    	   
   /* context memory to be pre-allocated,
    * if DDR memory is not found.
    * Otherwise actual size is used. */ 
   cavium_cfg.context_max = CAVIUM_CONTEXT_MAX; 
   
   cavium_dev[dev_count].dev = dev;

   /* allocate command queue, initialize chip */
   if (cavium_init(&cavium_cfg)) {
      cavium_error("cavium_init failed.\n");
       release_region(bar_2, 0xff);
       release_region(bar_0, 0xff);
      return -ENOMEM;
   }
   
   return 0;
}

void cavium_cleanup_one(cavium_device *pkp_dev)
{
	if (pkp_dev->bar_1)
		release_region(pkp_dev->bar_1, 0xff);
	if (pkp_dev->bar_0)
		release_region(pkp_dev->bar_0, 0xff);
	cavium_cleanup(pkp_dev);
	return;
}

/*
 *  Standard module initialization function.
 *  This function scans the PCI bus looking for the right board 
 *   and allocates resources.
 */
int initmodule ()
{
   int ret_val=0, i;
   struct pci_dev *dev = NULL;
   cavium_general_config cavium_g_cfg;
    
   cavium_debug_level = CAVIUM_DEBUG_LEVEL;

   cavium_print("Cavium Driver --- %01d.%02d-%c\n",cavium_version[0],cavium_version[1],cavium_version[2]);
/*Alloc memory for extension structure, add by lijing*/
	pdx=cavium_malloc(sizeof(struct ni_extension), NULL);
	if(pdx==NULL){
		cavium_error("Cavium Driver malloc struct extension error!\n");
		return -ENOMEM;
	}
	pdx->resource=NULL;

   /* check if system supports pci bus */
#if LINUX_VERSION_CODE  < KERNEL_VERSION (2,6,0)
   if (!pci_present()) {
      cavium_error("This system does not support PCI");
      ret_val =  -EINVAL;
      goto init_error;
   }
#endif
  
   while (1) {
#if LINUX_VERSION_CODE  >= KERNEL_VERSION (2,6,11)
      dev = pci_get_subsys(VENDOR_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, dev);
#else
      dev = pci_find_subsys(VENDOR_ID, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, dev);
#endif
      if (!dev) {
          break;
      } else {
	 if(dev->device == N1_DEVICE) {
	 	cavium_print("N1 Device found at Bus %d Slot %d\n",
			     dev->bus->number, PCI_SLOT(dev->devfn));
	 }else if (dev->device == N1_LITE_DEVICE) {
		cavium_print("N1-Lite Device found at Bus %d Slot %d\n",
			     dev->bus->number, PCI_SLOT(dev->devfn));
	} else {
		cavium_print("CAVIUM Card found: But this driver is for"
			     "N1 and N1-Lite only.\n");
		continue;
	}
			 
       if (!cavium_init_one(dev)) {
                dev_count++;
                cavium_print("Finished Initializing this device\n");
                if (dev_count >= MAX_DEV) {
                    cavium_error("MAX %d N1 Devices supported\n", dev_count);
                    cavium_error("Ignoring other N1 devices\n");
                    break;
                }
          } else {
                cavium_error(" Cavium Init failed for device \n");
                ret_val = -ENOMEM;
                goto init_error;
          } 
      } 
   }

   if (dev_count == 0) {
       cavium_error("N1 not found \n");
       ret_val = -ENODEV;
       goto init_error;
   } else {
       cavium_print("Total Number of N1/N1-Lite Devices: %d\n", dev_count);
   }

   cavium_g_cfg.pending_max = CAVIUM_PENDING_MAX; 
                /* number of pending response structures to be pre-allocated. */
   cavium_g_cfg.direct_max = CAVIUM_DIRECT_MAX;	
                /* number of DIRECT operation structures to be pre-allocated. */
   cavium_g_cfg.sg_max = CAVIUM_SG_MAX;		
                /* number of SG operation structures to be pre-allocated. */
   cavium_g_cfg.sg_dma_list_max = CAVIUM_SG_DMA_MAX; 
                /* number of scatter/gather lists to be pre-allocated. */

   cavium_g_cfg.huge_buffer_max = HUGE_BUFFER_CHUNKS;
   cavium_g_cfg.large_buffer_max = LARGE_BUFFER_CHUNKS;
   cavium_g_cfg.medium_buffer_max = MEDIUM_BUFFER_CHUNKS;
   cavium_g_cfg.small_buffer_max = SMALL_BUFFER_CHUNKS;
   cavium_g_cfg.tiny_buffer_max = TINY_BUFFER_CHUNKS;
   cavium_g_cfg.ex_tiny_buffer_max = EX_TINY_BUFFER_CHUNKS;

   if(cavium_general_init(&cavium_g_cfg)) {
	   cavium_error("cavium_general_init failed.\n");
	   ret_val = -ENOMEM;
	   goto init_error;
   }

   /* create poll thread */
   /*if(init_poll_thread()) {
      cavium_print("init_poll_thread failed.\n");
      ret_val = -ENOMEM;
      goto init_error;
   }*/ // comment by liqin 2006/04/28

   aos_init_request();		// added by liqin 2006/06/09
   aos_init_poll_timer(); 	// added by liqin 2006/04/28

    /* now setup interrupt handler */
   for (i = 0; i < dev_count; i++) {
	if(setup_interrupt(&cavium_dev[i])) {
		int j;
		ret_val = -ENXIO;
		for (j = 0; j <i; j++) {
			free_interrupt(&cavium_dev[j]);
		}
		cavium_print("Error setting up interrupt.\n");
		goto init_error;
	}
   }

	/* initialize kernel mode stuff */
	init_kernel_mode();

   /* register driver */
   ret_val = register_chrdev(DEVICE_MAJOR,DEVICE_NAME,&n1_fops);
   if(ret_val <0)
   {
	for (i = 0; i <dev_count; i++) {
		free_interrupt(&cavium_dev[i]);
	}
      cavium_print("%s failed with %d\n", "Sorry, registering n1 device failed", ret_val);
      goto init_error;
   }

#ifdef NPLUS
   cavium_print("%s The major device number is %d\n", "Registeration is a success", DEVICE_MAJOR);
   cavium_print("To talk to the device driver, please use device appropriate device node.\n");
   cavium_print("Device's minor number corresponds to the microcode to be used.\n");
   cavium_print("Please read the README file for further instructions\n\n");
   
#else 
   cavium_print("%s The major device number is %d\n", "Registeration is a success", DEVICE_MAJOR);
   cavium_print("if you want to talk to the device driver,\n");
   cavium_print("I suggest you use:\n");
   cavium_print(" mknod %s c %d 0\n", DEVICE_NAME,DEVICE_MAJOR);
#endif

   if (cavium_init_proc()) {
	   cavium_print(" Support for proc filesystem failed\n");
	   cavium_print(" Still continuing ....\n");
   }
   cavium_print("Loaded Cavium Driver --- %01d.%02d-%c\n",cavium_version[0],cavium_version[1],cavium_version[2]);
	ret_val=Csp1Initialize(CAVIUM_DIRECT);
	

   return 0;

init_error:
	printk("init error! ret val is %d\n", ret_val);
    //free_poll_thread(); // comment by liqin 2006/04/28
	aos_kill_poll_timer(); // added by liqin 2006/04/28
    cavium_general_cleanup();
    for (i = 0; i < dev_count; i++)
	    cavium_cleanup_one(&cavium_dev[i]);
    return ret_val;
}/*initmodule*/



/*
 *  Standard module release function.
 */
void cleanupmodule (void)
{
  int ret, i;
#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
  if(!MOD_IN_USE)
  {    
#endif
	driver_removal = 1;
	cavium_print("Unregistering char device\n");
	ret = unregister_chrdev(DEVICE_MAJOR,DEVICE_NAME);

	if(ret < 0) {
		cavium_print("Error in unregistering pkp\n");
	} else {
		cavium_print("Device successfully unregistered\n");
	}

	cavium_print("Freeing kernel mode\n");
	free_kernel_mode();

	cavium_print("Freeing interrupt\n");
	for (i=0; i < dev_count; i++)
      		free_interrupt(&cavium_dev[i]);

	cavium_print("Freeing poll thread\n");
    //free_poll_thread(); // comment by liqin 2006/04/28
	aos_kill_poll_timer(); // added by liqin 2006/04/28
	
	cavium_print("dev_count %d \n", dev_count);

	for (i=0; i < dev_count; i++)
      		cavium_cleanup_one(&cavium_dev[i]);
	cavium_print("General cleanup \n");
	cavium_general_cleanup();

	cavium_print("Freeing proc resources \n");
	cavium_free_proc();


#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
   } else {
	cavium_error("Module in use\n"); 
	return;
   }
#endif
   cavium_error("UnLoaded Cavium Driver --- %01d.%02d-%c\n",cavium_version[0],cavium_version[1],cavium_version[2]);
}


/*
 *  Standard open() entry point.
 *  It simply increments the module usage count.
 */
int n1_kernel_open (void)
{
   int ret=driver_removal;

#ifdef CAVIUM_RESOURCE_CHECK
      struct CAV_RESOURCES *resource;
#endif

   if(ret)
   {
      cavium_print("open: returning error :%d\n", ENOMEM);
      return ENOMEM;
   }
/*
#ifdef NPLUS
   {
   u32 dev_id =0;
   int ucode_idx = MINOR(inode->i_rdev); 
   struct MICROCODE * microcode = &(cavium_dev[dev_id].microcode[ucode_idx]);
   cavium_dbgprint("Microcode code_type = %d\n",microcode->code_type); 
   cavium_dbgprint("Microcode idx = %d\n", ucode_idx); 

   switch (ucode_idx) {

	case SSL_MLM_IDX : 
	case IPSEC_MLM_IDX :
	 		   if (microcode->code_type != CODE_TYPE_MAINLINE)
			   { 
   cavium_error("Error openning n1 device :Wrong microcode specified by application\n");
			       return -ERESTARTSYS;
			   }
		           break;
	case SSL_SPM_IDX : 
	case IPSEC_SPM_IDX :
			   if (microcode->code_type != CODE_TYPE_SPECIAL)
		           {	
			cavium_error("Error openning n1 device :Wrong microcode specified by the application\n");
			       return -ERESTARTSYS;
			   }
 			   break;
	default :
		break;
   }
   }
#endif
*/
   ret=0;

      CAVIUM_MOD_INC_USE_COUNT;

      cavium_dbgprint ("n1: open\n");
      //cavium_dbgprint ("Device minor number %d.%d\n", 
         //inode->i_rdev >>8, inode->i_rdev & 0xff);
      
#ifdef CAVIUM_RESOURCE_CHECK
      resource = cavium_malloc(sizeof(struct CAV_RESOURCES), NULL);
      if (resource == NULL) {
	      cavium_error("Unable to allocate memory for Cavium resource\n");
	      return -ERESTARTSYS;
      }
      
      cavium_spin_lock_init(&resource->resource_check_lock);
      CAVIUM_INIT_LIST_HEAD(&resource->ctx_head);
      CAVIUM_INIT_LIST_HEAD(&resource->key_head);
      //file->private_data = resource;
      pdx->resource=resource;
      
#endif

   return (0);

}



/*
 *  Standard open() entry point.
 *  It simply increments the module usage count.
 */
int n1_open (struct inode *inode, struct file *file)
{
   int ret=driver_removal;
#ifdef CAVIUM_RESOURCE_CHECK
      struct CAV_RESOURCES *resource;
#endif

   if(ret)
   {
      cavium_print("open: returning error :%d\n", ENOMEM);
      return ENOMEM;
   }

#ifdef NPLUS
   {
   u32 dev_id =0;
   int ucode_idx = MINOR(inode->i_rdev); 
   struct MICROCODE * microcode = &(cavium_dev[dev_id].microcode[ucode_idx]);
   cavium_dbgprint("Microcode code_type = %d\n",microcode->code_type); 
   cavium_dbgprint("Microcode idx = %d\n", ucode_idx); 

   switch (ucode_idx) {

	case SSL_MLM_IDX : 
	case IPSEC_MLM_IDX :
	 		   if (microcode->code_type != CODE_TYPE_MAINLINE)
			   { 
   cavium_error("Error openning n1 device :Wrong microcode specified by application\n");
			       return -ERESTARTSYS;
			   }
		           break;
	case SSL_SPM_IDX : 
	case IPSEC_SPM_IDX :
			   if (microcode->code_type != CODE_TYPE_SPECIAL)
		           {	
			cavium_error("Error openning n1 device :Wrong microcode specified by the application\n");
			       return -ERESTARTSYS;
			   }
 			   break;
	default :
		break;
   }
   }
#endif

   ret=0;

      CAVIUM_MOD_INC_USE_COUNT;

      cavium_dbgprint ("n1: open\n");
      cavium_dbgprint ("Device minor number %d.%d\n", 
         inode->i_rdev >>8, inode->i_rdev & 0xff);
      
#ifdef CAVIUM_RESOURCE_CHECK
      resource = cavium_malloc(sizeof(struct CAV_RESOURCES), NULL);
      if (resource == NULL) {
	      cavium_error("Unable to allocate memory for Cavium resource\n");
	      return -ERESTARTSYS;
      }
      
      cavium_spin_lock_init(&resource->resource_check_lock);
      CAVIUM_INIT_LIST_HEAD(&resource->ctx_head);
      CAVIUM_INIT_LIST_HEAD(&resource->key_head);
      file->private_data = resource;
#endif

   return (0);

}



/*
 *  Standard release() entry point.
 *  This function is called by the close() system call.
 */
int n1_kernel_release (Uint32 device_id)
{

   int ret;

   ret=driver_removal;

   if(ret)
   {
      cavium_print("n1: close returning error %d\n", ENXIO);
      return ENXIO;
   }
   else
   {
#ifdef CAVIUM_RESOURCE_CHECK
      Uint32 dev_id;
      cavium_device *pdev;
      struct CAV_RESOURCES *resource = NULL;
      struct cavium_list_head *tmp, *tmp1;

#ifdef NPLUS
      dev_id=0;
#else

      //dev_id = MINOR(inode->i_rdev);
      dev_id=0;
      if (dev_id > (dev_count - 1)) {
		cavium_print("No N1 device associated with this minor device no. %ld\n", dev_id);
		return -ENODEV;
      }
#endif

      pdev = &cavium_dev[dev_id];
      cleanup_nb_command_pid(cavium_get_pid());
      //resource = file->private_data;
      resource=pdx->resource;
      if (resource == NULL) {
	    cavium_error("Resource not found while deallocating\n");
	    return -1;
      }
      cavium_spin_lock(&resource->resource_check_lock);

      cavium_list_for_each_safe(tmp, tmp1, &resource->ctx_head) {
	      struct CTX_ENTRY *entry = list_entry(tmp, struct CTX_ENTRY, list);
	      dealloc_context(pdev, entry->ctx_type, entry->ctx);
	      cavium_list_del(&entry->list);
	      cavium_free((Uint8 *)entry);
      }

      cavium_list_for_each_safe(tmp, tmp1, &resource->key_head) {
	      struct KEY_ENTRY *entry = list_entry(tmp, struct KEY_ENTRY, list);
	      dealloc_key_memory(pdev, entry->key_handle);
	      cavium_list_del(&entry->list);
	      cavium_free((Uint8 *)entry);
      }
      cavium_spin_unlock(&resource->resource_check_lock);
      cavium_free(resource);
	  pdx->resource=NULL;
#endif
      CAVIUM_MOD_DEC_USE_COUNT;
      cavium_dbgprint("n1: close\n");
      return(0);
   }
}



/*
 *  Standard release() entry point.
 *  This function is called by the close() system call.
 */
int n1_release (struct inode *inode, struct file *file)
{

   int ret;

   ret=driver_removal;

   if(ret)
   {
      cavium_print("n1: close returning error %d\n", ENXIO);
      return ENXIO;
   }
   else
   {
#ifdef CAVIUM_RESOURCE_CHECK
      Uint32 dev_id;
      cavium_device *pdev;
      struct CAV_RESOURCES *resource = NULL;
      struct cavium_list_head *tmp, *tmp1;

#ifdef NPLUS
      dev_id=0;
#else
      dev_id = MINOR(inode->i_rdev);
      if (dev_id > (dev_count - 1)) {
		cavium_print("No N1 device associated with this minor device no. %ld\n", dev_id);
		return -ENODEV;
      }
#endif

      pdev = &cavium_dev[dev_id];
      cleanup_nb_command_pid(cavium_get_pid());
      resource = file->private_data;
      if (resource == NULL) {
	    cavium_error("Resource not found while deallocating\n");
	    return -1;
      }
      cavium_spin_lock(&resource->resource_check_lock);

      cavium_list_for_each_safe(tmp, tmp1, &resource->ctx_head) {
	      struct CTX_ENTRY *entry = list_entry(tmp, struct CTX_ENTRY, list);
	      dealloc_context(pdev, entry->ctx_type, entry->ctx);
	      cavium_list_del(&entry->list);
	      cavium_free((Uint8 *)entry);
      }

      cavium_list_for_each_safe(tmp, tmp1, &resource->key_head) {
	      struct KEY_ENTRY *entry = list_entry(tmp, struct KEY_ENTRY, list);
	      dealloc_key_memory(pdev, entry->key_handle);
	      cavium_list_del(&entry->list);
	      cavium_free((Uint8 *)entry);
      }
      cavium_spin_unlock(&resource->resource_check_lock);
      cavium_free(resource);
#endif
      CAVIUM_MOD_DEC_USE_COUNT;
      cavium_dbgprint("n1: close\n");
      return(0);
   }
}

#ifdef NPLUS

int acquire_core(cavium_device *pdev, int ucode_idx, int core_id)
{
   Cmd strcmd;
   int ret = 0, insize = 8, outsize = 16;
   Uint8 *out_buffer=NULL;
   Uint8 *in_buffer=NULL;
   Request request;
   Uint64 *completion_address;
   Uint64 disabled_core;
   Uint32 disabled_mask = 0;

   cavium_dbgprint("Attempt to acquire core %d\n", core_id);


   in_buffer = (Uint8 *)get_buffer_from_pool(pdev, (insize + 8));
   if(in_buffer == NULL)
   {
     cavium_print("acquire_core: unable to allocate in_buffer.\n");
     ret = -1;
     goto ca_err;
   }

   out_buffer = (Uint8 *)get_buffer_from_pool(pdev, (outsize + 8));
   if(out_buffer == NULL)
   {
     cavium_print("acquire_core: unable to allocate out_buffer.\n");
     ret = -2;
     goto ca_err;
   }

   do
   {
      strcmd.opcode= (0x7f<<8) | MAJOR_OP_ACQUIRE_CORE;;
      strcmd.size  = 0;
      strcmd.param = 0;
      strcmd.dlen  = insize>>3;

      strcmd.opcode  = htobe16(strcmd.opcode);
      strcmd.size    = htobe16(strcmd.size);
      strcmd.param   = htobe16(strcmd.param);
      strcmd.dlen    = htobe16(strcmd.dlen);

      cavium_memcpy((unsigned char *)&request, (unsigned char *)&strcmd, 8);
      request.dptr = cavium_vtophys(in_buffer);
      request.rptr = cavium_vtophys(out_buffer);
      request.cptr = 0;
   
      request.dptr = htobe64(request.dptr);
      request.rptr = htobe64(request.rptr);
      request.cptr = htobe64(request.cptr);

      completion_address = (Uint64 *)(out_buffer + outsize);
      *completion_address = COMPLETION_CODE_INIT;

      if(send_command(pdev, &request, 0, ucode_idx, completion_address) < 0)
      {
        cavium_print("Error sending core acquire request.\n");
        goto ca_err;
      }

      ret = check_completion(pdev, completion_address, 100, ucode_idx, -1);
      if(ret)
      {
         cavium_print("Error: %x on acquire core request.\n", ret);
         goto ca_err;
      }

      disabled_core = betoh64(*(Uint64 *)(out_buffer+8));

      cavium_dbgprint("Acquired core %ld\n", (Uint32)(disabled_core));

      if(disabled_core == core_id)
      {
         break;
      }
      else
      {
         disabled_mask |= (1<<disabled_core);
         cavium_dbgprint("Acquired mask 0x%lx\n", disabled_mask);
      }
   } while(1);

ca_err:

   if(disabled_mask)
   {
      cycle_exec_units_from_mask(pdev, disabled_mask);
      cavium_dbgprint("Cycled cores 0x%lx\n", disabled_mask);
   }

   if(in_buffer)
      put_buffer_in_pool(pdev,(Uint8 *)in_buffer);
   if(out_buffer)
      put_buffer_in_pool(pdev,(Uint8 *)out_buffer);
   return(ret);

}

int 
nplus_init(cavium_device *pdev, int ucode_idx, Uint32 arg)
{
   int i,j, ret=0;
   Uint8 code_idx;
   Csp1InitBuffer *init_buffer;
   struct MICROCODE *microcode;

   init_buffer = (Csp1InitBuffer *)arg;
   
   cavium_dbgprint("got csp1_init code\n");
   cavium_dbgprint("size = %d\n", init_buffer->size);
            
  /* We only allow this IOCTL on "/dev/pkp_admin" */
  if(ucode_idx != ADMIN_IDX)
  {
     cavium_print("Inappropriate IOCTL for device");
     ret = ERR_INIT_FAILURE;
     goto cleanup_init;
  }

  /* Was this driver initialized earlier ? */
  if(pdev->initialized)
  {
     if(pdev->initialized == 1)
        cavium_error("Device already initialized\n");
     else
        cavium_error("Device incorrectly initialized\n");

     cavium_print("To reinitialize device, please unload & reload the driver\n");
     ret = ERR_INIT_FAILURE;
     goto cleanup_init;
  }

  /* get all the information from init buffer */
  for(i=0;i<init_buffer->size;i++)
  {
      code_idx = init_buffer->ucode_idx[i];
      microcode = &(pdev->microcode[code_idx]);

      /* Make sure it isnt previously initialized */
      if(microcode->code != NULL)
      {
         cavium_print("Code Index %d found more than once\n", code_idx);
         ret = ERR_INIT_FAILURE;
         goto cleanup_init;
      }

      /* code */
      microcode->code_type = init_buffer->version_info[i][0] & 0x7f;
      microcode->paired_cores
            = (init_buffer->version_info[i][0] & 0x80 ? 1:0);
      microcode->code_size = init_buffer->code_length[i];
      microcode->code = 
              (Uint8 *)get_buffer_from_pool(pdev,microcode->code_size);
               
      if (microcode->code == NULL)
      {
         cavium_print("Failed to allocate %ld bytes microcode buffer type %d\n", 
                                 microcode->code_size, microcode->code_type);
         ret = ERR_MEMORY_ALLOC_FAILURE;
         goto cleanup_init;
      }            

      if(cavium_copy_in(microcode->code, init_buffer->code[i],
                       microcode->code_size))
      {
        cavium_error("Failed to copy in microcode->code\n");
         ret = ERR_INIT_FAILURE;
         goto cleanup_init;
      }
               
      /* data */
      microcode->data_size = init_buffer->data_length[i];
      if(microcode->data_size)
      {
         int offset=40;

         microcode->data = 
            (Uint8 *)get_buffer_from_pool(pdev, microcode->data_size+offset);
                  
         if (microcode->data == NULL)
         {
            cavium_print("Failed to allocate %ld bytes admin cst buffer type %d\n", 
                                    microcode->data_size+offset,microcode->code_type);

            ret = ERR_MEMORY_ALLOC_FAILURE;
            goto cleanup_init;
         } 
                  
         cavium_memset(microcode->data,0,microcode->data_size+offset);
         if(cavium_copy_in(microcode->data+offset, init_buffer->data[i],
                              microcode->data_size))
         {
           cavium_error("Failed to copy in microcode->data\n")
           ret = ERR_INIT_FAILURE;
           goto cleanup_init;
         }
      }

      /* sram address */
      if(cavium_copy_in(microcode->sram_address,
		      init_buffer->sram_address[i], SRAM_ADDRESS_LEN))
      {
           cavium_error("Failed to copy in sram_address\n")
           ret = ERR_INIT_FAILURE;
           goto cleanup_init;
      }

      /* Initialize the SRQ */
      microcode->srq.head = microcode->srq.tail = 0;
      microcode->srq.qsize = 0;
      cavium_spin_lock_init(&microcode->srq.lock);
      for(j=0;j<MAX_SRQ_SIZE;j++)
      {
         microcode->srq.state[j] = SR_FREE;
      }

      cavium_dbgprint("Code type = %02x, code size = %lx, data size = %lx\n",
                 microcode->code_type, microcode->code_size,microcode->data_size);
  }

  /* check for any missing piece */
  if(pdev->microcode[BOOT_IDX].code == NULL)
  {
     cavium_print("Boot code not sent to driver.\n");
     cavium_print("Please check version information\n");
     ret = ERR_INIT_FAILURE;
     goto cleanup_init;
  }

  /* We have gathered all the required information from init_buffer
   * Now it is time for some action. Lets do it! 
   */
  cavium_dbgprint("nplus_init: calling do_init\n");
  ret = do_init(pdev);   

cleanup_init:
  if(ret != 0)
  {
     for(i=0;i<init_buffer->size;i++)
     {
         code_idx = init_buffer->ucode_idx[i];
         microcode = &(pdev->microcode[code_idx]);
         if(microcode->code)
         {
             put_buffer_in_pool(pdev, microcode->code);
             microcode->code = NULL;
         }
         if(microcode->data)
         {
             put_buffer_in_pool(pdev, microcode->data);
             microcode->data = NULL;
         }
     }
     pdev->initialized = -1;
  }
  else
      pdev->initialized = 1;

  return ret;
}/*nplus_init*/

#endif /*NPLUS*/

/*
 *  Standard ioctl() entry point.
 */
int n1_kernel_ioctl (Uint32 device_id, 
            unsigned int cmd,Uint32 arg)
{
	int ret=0;
	DebugRWReg *dw;
	cavium_device *pdev=NULL;
	Csp1InitBuffer *init_buffer;
	Uint32 dev_id=device_id;//??????????????
  
#ifdef NPLUS

	Uint32 dev_id=0;
	int ucode_idx = MINOR(inode->i_rdev);
	struct MICROCODE *microcode = &(cavium_dev[dev_id].microcode[ucode_idx]);

#else/* !NPLUS */

	kernel_call=1;

	//Uint32 dev_id = MINOR(inode->i_rdev);
	//Uint32 dev_id=0;//mabey dev_id=0.
	if (dev_id > (dev_count - 1)) {
		cavium_print("No N1 device associated with this minor device no. %ld\n", dev_id);
		return -ENODEV;
	}

#endif /*NPLUS*/

	pdev = &cavium_dev[dev_id];

 	cavium_dbgprint("Ioctl Cmd 0x%x called\n",cmd);
 
	switch (cmd) {
		/* write PKP register */
		case IOCTL_N1_DEBUG_WRITE_CODE:
			dw =  (DebugRWReg *)arg;
			cavium_dbgprint("pkp_drv: writing %lx at %lx\n", 
				     dw->data, dw->addr);
			write_PKP_register(pdev, 
					   dw->addr, dw->data);
			ret = 0;
			break;
	
		/* Read PKP register */
		case IOCTL_N1_DEBUG_READ_CODE:
			dw = (DebugRWReg *)arg;
			cavium_dbgprint( "Kernel: reading %lx \n", dw->addr);
			read_PKP_register(pdev,
					  dw->addr, &dw->data);
			ret = 0;
			break;

		/* Write PCI config space */
		case IOCTL_PCI_DEBUG_WRITE_CODE:
			dw =  (DebugRWReg *)arg;
			cavium_dbgprint("pkp_drv: writing %lx at PCI config %lx\n", 
				     dw->data, dw->addr);
#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
			((struct pci_dev *)(pdev->dev))->bus->ops->write_dword(
						(struct pci_dev *)(pdev->dev),
							    dw->addr,
							    dw->data);
#else
			((struct pci_dev *)(pdev->dev))->bus->ops->write(
						(struct pci_bus *)(((struct pci_dev *)(pdev->dev))->bus),
						((struct pci_dev *)(pdev->dev))->devfn,
							    dw->addr,
									4,
							    (dw->data));
#endif
			ret = 0;
			break;
	
		/* Read PCI config space */
		case IOCTL_PCI_DEBUG_READ_CODE:
			dw = (DebugRWReg *)arg;
			dw->data = 0;
			cavium_dbgprint("pkp_drv: reading PCI config %lx \n",
				     dw->addr);
#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
			((struct pci_dev *)(pdev->dev))->bus->ops->read_dword(
							(struct pci_dev *)(pdev->dev),
							dw->addr,
							(unsigned int *)&dw->data);
#else
			((struct pci_dev *)(pdev->dev))->bus->ops->read(
						(struct pci_bus *)(((struct pci_dev *)(pdev->dev))->bus),
							((struct pci_dev *)(pdev->dev))->devfn,
							dw->addr,
							4,
							(unsigned int *)&dw->data);
#endif
			ret = 0;
			break;
		
			/* run some basic test */
		case IOCTL_N1_API_TEST_CODE:
#if defined API_TEST
			ret = api_test(pdev);
#else
			ret = -1;
#endif /* API_TEST */
			break;

		case IOCTL_N1_DO_OPERATION:
		{
			n1_operation_buffer *buf;

			buf = (n1_operation_buffer *)arg;
			cavium_dbgprint("ioctl N1 do operation called with opcode 0x%x\n", 
					buf->opcode);
			buf->dma_mode = CAVIUM_DIRECT;
#ifdef NPLUS
			cavium_dbgprint("IOCTL_DO_OP:ucodeidx=%d\n", ucode_idx);
			buf->ucode_idx = ucode_idx;
#endif
			ret = do_operation(pdev, buf);
			if(ret == ERR_REQ_PENDING)
				ret = EAGAIN;
		}
			cavium_dbgprint("ioctl N1 do operation returning.\n");
			break;
		case IOCTL_N1_DO_SG_OPERATION:
		{
			n1_operation_buffer *buf;

			buf = (n1_operation_buffer *)arg;
			cavium_dbgprint("ioctl N1 do operation called with opcode 0x%x\n", 
					buf->opcode);
			buf->dma_mode = CAVIUM_SCATTER_GATHER;
/*
#ifdef NPLUS
			buf->ucode_idx = ucode_idx;
#endif
*/
			buf->opcode = buf->opcode & (~0x80);
			ret = do_operation(pdev, buf);
		}
			break;
		case IOCTL_N1_GET_REQUEST_STATUS:
		{
			cavium_dbgprint("Ioctl get request status called\n");
/*
#ifdef NPLUS
			if(microcode->code_type == CODE_TYPE_SPECIAL)
			{
			move_srq_entries(pdev, ucode_idx, 0);
			}
#endif */   /*NPLUS*/

			ret = check_nb_command_id((Uint32)arg);
			if(ret == ERR_REQ_PENDING)
				ret = EAGAIN;

			cavium_dbgprint("get_request_status: 0x%x\n", ret);

		}
			break;
		case IOCTL_N1_FLUSH_ALL_CODE:
		{
			cavium_dbgprint("Ioctl flush all code called\n");
			cleanup_nb_command_pid(current->pid);
		}
			break;
		case IOCTL_N1_FLUSH_CODE:
		{
			cavium_dbgprint("Ioctl N1 Flush code called\n");
			cleanup_nb_command_id((Uint32)arg);
		}
			break;
		case IOCTL_N1_ALLOC_CONTEXT:
		{
			n1_context_buf c;

			cavium_dbgprint("ioctl N1 alloc context called\n");
			c = (*(n1_context_buf *)arg);
			c.ctx_ptr = alloc_context(pdev,(c.type));
			ret = 0;
#ifdef CAVIUM_RESOURCE_CHECK
			{
			//struct CAV_RESOURCES *resource = file->private_data;
			struct CAV_RESOURCES *resource=pdx->resource;
			cavium_spin_lock(&resource->resource_check_lock);
			ret = insert_ctx_entry(&resource->ctx_head, 
						c.type,
						c.ctx_ptr);
			cavium_spin_unlock(&resource->resource_check_lock);
			}
#endif
			if(cavium_copy_out((caddr_t)arg, (unsigned char *)&c, sizeof(n1_context_buf)))
				cavium_error("Failed to copy out context\n");
		}
			cavium_dbgprint("ioctl N1 alloc context returning\n");
			break;

		case IOCTL_N1_FREE_CONTEXT:
		{
			n1_context_buf c;

			cavium_dbgprint("ioctl N1 free context called\n");
			c = (*(n1_context_buf *)arg);
			dealloc_context(pdev, c.type, c.ctx_ptr);
			ret = 0;
#ifdef CAVIUM_RESOURCE_CHECK
			{
			//struct CAV_RESOURCES *resource = file->private_data;
			struct CAV_RESOURCES *resource=pdx->resource;
      			struct cavium_list_head *tmp, *tmp1;
			cavium_spin_lock(&resource->resource_check_lock);
      			cavium_list_for_each_safe(tmp, tmp1, &resource->ctx_head) {
	      			struct CTX_ENTRY *entry = list_entry(tmp, struct CTX_ENTRY, list);
				if (entry->ctx == c.ctx_ptr) 
				{
	      				cavium_list_del(&entry->list);
					cavium_free((Uint8 *)entry);
				}
      			}
			cavium_spin_unlock(&resource->resource_check_lock);
			}
#endif
		}
			cavium_dbgprint("ioctl N1 free context returning\n");
			break;
		case IOCTL_N1_SOFT_RESET_CODE:
		{
			do_soft_reset(&cavium_dev[0]);
			ret = 0;
		}
			break;
		case IOCTL_N1_ALLOC_KEYMEM:
		{
			Uint64 key_handle;
			cavium_dbgprint("ioctl N1 alloc keymem called\n");
			key_handle = alloc_key_memory(pdev);
			if (key_handle == (Uint64)NULL) {
				cavium_error("Allocation of Key Memory failed\n");
				return -1;
			}
#ifdef CAVIUM_RESOURCE_CHECK
			{
			//struct CAV_RESOURCES *resource = file->private_data;
			struct CAV_RESOURCES *resource=pdx->resource;
			cavium_spin_lock_softirqsave(&resource->resource_check_lock);
			ret = insert_key_entry(&resource->key_head, 
						key_handle);
			cavium_spin_unlock_softirqrestore(&resource->resource_check_lock);
			}
#endif
			if(cavium_copy_out((caddr_t)arg, (unsigned char *)&key_handle, sizeof(Uint64)))
				cavium_error("Failed to copy out key_handle\n");
		}
			cavium_dbgprint("ioctl N1 alloc keymem returning.\n");
			break;
		case IOCTL_N1_FREE_KEYMEM:
		{
			n1_write_key_buf key_buf;
			cavium_dbgprint("ioctl N1 free keymem called\n");
			key_buf = (*(n1_write_key_buf *)arg);
			dealloc_key_memory(pdev, key_buf.key_handle);
#ifdef CAVIUM_RESOURCE_CHECK
			{
			//struct CAV_RESOURCES *resource = file->private_data;
			struct CAV_RESOURCES *resource=pdx->resource;
      			struct cavium_list_head *tmp, *tmp1;
			cavium_spin_lock_softirqsave(&resource->resource_check_lock);
      			cavium_list_for_each_safe(tmp, tmp1, &resource->key_head) {
	      			struct KEY_ENTRY *entry = list_entry(tmp, struct KEY_ENTRY, list);
				if (entry->key_handle == key_buf.key_handle) 
				{
	      				cavium_list_del(&entry->list);
					cavium_free((Uint8 *)entry);
				}
      			}
			cavium_spin_unlock_softirqrestore(&resource->resource_check_lock);
			}
#endif
		}
			cavium_dbgprint("ioctl N1 free keymem returning.\n");
			break;
		case IOCTL_N1_WRITE_KEYMEM:
		{
			n1_write_key_buf key_buf;
			Uint8 *key;
			key_buf = (*(n1_write_key_buf *)arg);
			key = cavium_malloc(key_buf.length, NULL);
			if (key == NULL) {
				cavium_error("Unable to allocate memory for key\n");
				return -1;
			}
			if(cavium_copy_in(key, key_buf.key, key_buf.length))
			{
				cavium_error("Unable to copy in key\n");
				return -1;
			}
			key_buf.key = key;
			if (store_key_mem(pdev, key_buf
/*
#ifdef NPLUS
						,ucode_idx
#endif
*/
						) < 0) 
			{
				cavium_error("n1_ioctl: store_key_mem failed\n");
				cavium_free(key);
				return -1;
			}
			cavium_free(key);
			ret = 0;
		}
			break;

		case IOCTL_N1_GET_RANDOM_CODE:
		{
			n1_operation_buffer *buf;

			if(!pdev->enable)
			{
				ret = ERR_DRIVER_NOT_READY;
				break;
			}
			buf = (n1_operation_buffer *)arg;
			ret = get_rnd(pdev, buf->outptr[0],
				buf->outsize[0]
/*
#ifdef NPLUS
				, ucode_idx
#endif
*/
				);
		}
			break;
		case IOCTL_N1_INIT_CODE:
		{
#if 0
#ifdef NPLUS
			cavium_dbgprint("calling nplus_init\n");
			ret = nplus_init(pdev, ucode_idx, arg);
#endif
#else


			int boot_info = 0;
			int mainline_info = 0;
			Uint8 code_type;
			int i;
			struct MICROCODE *microcode;

			init_buffer = (Csp1InitBuffer *)arg;

			microcode = pdev->microcode;
			
			boot_info = 0;
			mainline_info = 0;
			
			/* get all the information from init buffer */
			for(i=0;i<init_buffer->size;i++)
			{
				code_type = init_buffer->version_info[i][0];

				if(code_type == CODE_TYPE_BOOT)
				{
					if(boot_info)
					{
						cavium_print( "Code type boot found more than once\n");
						ret = ERR_INIT_FAILURE;
						break;
					}
					else
					{
						cavium_print( "got boot microcode\n");
						boot_info=1;
					}
				}
				else if (code_type == CODE_TYPE_MAINLINE)
				{
					if(mainline_info)
					{
						cavium_print( "Code type mainline found more than once\n");
						ret = ERR_INIT_FAILURE;
						break;
					}
					else
					{
						cavium_print( "got mainline microcode\n");
						mainline_info=1;
					}
				}
				else
				{
					cavium_print( "unknown microcode type\n");
					ret = ERR_INIT_FAILURE;
					break;
				}
				
				/* code */

				microcode[i].code_type = code_type;
				microcode[i].code_size = init_buffer->code_length[i];
				microcode[i].code = 
						(Uint8 *)get_buffer_from_pool(pdev, microcode[i].code_size);
				
				if (microcode[i].code == NULL)
				{
					cavium_print( "Failed to allocate %ld bytes microcode buffer type %d\n", 
										microcode[i].code_size, code_type);
					ret = ERR_MEMORY_ALLOC_FAILURE;
					break;
				}				

				if(cavium_copy_in(microcode[i].code, 
					       init_buffer->code[i],
					       microcode[i].code_size))
				{
					cavium_error("Failed to copy microcode->code for microcode %d\n", i);
					ret = ERR_INIT_FAILURE;
					break;
				}
				
				/* data */
				microcode[i].data_size = init_buffer->data_length[i];
				if(microcode[i].data_size)
				{
					int offset;
#ifdef MC2
					microcode[i].data = 
						(Uint8 *)get_buffer_from_pool(pdev, microcode[i].data_size + 40);
					offset = 40;
#else
#if 0
					microcode[i].data = 
						(Uint8 *)get_buffer_from_pool(pdev, microcode[i].data_size);
					offset = 0;
#endif
#endif
					
					if (microcode[i].data == NULL)
					{
						cavium_print( "Failed to allocate %ld bytes cst buffer type %d\n", 
											microcode[i].data_size,code_type);

						ret = ERR_MEMORY_ALLOC_FAILURE;
						break;
					} 
					cavium_memset(microcode[i].data, 0x0,
						      microcode[i].data_size + offset);
					 
					 
					if(cavium_copy_in(microcode[i].data + offset, 
						       init_buffer->data[i],
						       microcode[i].data_size))
					{
						cavium_error("Failed to copy in microcode->data for microcode %d\n", i);
						ret = ERR_INIT_FAILURE;
						break;
					}
				}

				/* sram address */
				if(cavium_copy_in(microcode[i].sram_address, 
					       init_buffer->sram_address[i],
					       SRAM_ADDRESS_LEN))
				{
					cavium_error("Failed to copy in microcode->sram_address for microcode %d\n", i);
					ret = ERR_INIT_FAILURE;
					break;
				}

			
				cavium_print("Code type = %02x, code size = %lx, data size = %lx\n",
					microcode[i].code_type,
					microcode[i].code_size,
					microcode[i].data_size);

			
			}/* for */

			/* check for any missing piece */
			if( !mainline_info || !boot_info ) {
			  cavium_print( "Not all of the information was sent to device driver.\n");
			  cavium_print( "Please check version information\n");
			  ret = ERR_INIT_FAILURE;
			  break;
			}

			/* Now we have gathered all the required information from init_buffer*/
			/* Now it is time for some action. */

			ret = do_init(pdev);
#endif /*NPLUS*/	
			break;
		}

#ifdef NPLUS
#if 0
      case IOCTL_CSP1_GET_CORE_ASSIGNMENT:
      {
         int i;
         Csp1CoreAssignment *core_assign = (Csp1CoreAssignment *)arg;

				 cavium_dbgprint("ioctl Get core assignment \n");
         cavium_spin_lock_softirqsave(&pdev->mc_core_lock);

         for(i=0;i<MICROCODE_MAX;i++)
         {
            core_assign->core_mask[i] = get_core_mask(pdev,i); 
            core_assign->mc_present[i] = (pdev->microcode[i].code==NULL)? 0:1;
         }
         cavium_spin_unlock_softirqrestore(&pdev->mc_core_lock);
      }
      break;

      case IOCTL_CSP1_SET_CORE_ASSIGNMENT:
      {
         int i;
         Uint8 id;
         Uint32 changed_mask_0_1 = 0, changed_mask_1_0 = 0;
         Uint32 core_mask, core_mask_1_0, core_mask_0_1;
         Csp1CoreAssignment *core_assign = (Csp1CoreAssignment *)arg;

				 cavium_dbgprint("ioctl set core assignment \n");

         if(pdev->initialized != 1)
         {
            ret = ERR_DRIVER_NOT_READY;
            break;
         }

         cavium_dbgprint("Assign Cores(%ld): {%lx %lx %lx %lx %lx %lx}\n",
            jiffies,
            core_assign->core_mask[0], core_assign->core_mask[1],
            core_assign->core_mask[2], core_assign->core_mask[3],
            core_assign->core_mask[4], core_assign->core_mask[5]);
   
         cavium_spin_lock_softirqsave(&pdev->mc_core_lock);

         /* This loop checks if the new assignments will be valid */
         for(i=0;i<MICROCODE_MAX && ret==0;i++)
         {
            /* Does this ucode require paired cores for 2048 bit ops ? */
            if(pdev->microcode[i].paired_cores)
            {
               core_mask = core_assign->core_mask[i];
               /* We will check if the new assignment will result in an
                * unpaired core
                */
               while(core_mask != 0)
               {
                  if((core_mask & 0x1) != ((core_mask>>1) & 0x1))
                  {
                     ret = ERR_ILLEGAL_ASSIGNMENT;
                     goto cleanup_set_cores;
                  }
                  core_mask = (core_mask >> 2);
               }
            }
            /*  Check the 0->1 transitions in the mask */
            core_mask = get_core_mask(pdev,i);
            core_mask_0_1 = (~core_mask & core_assign->core_mask[i]);
            if(core_mask_0_1)
            {
               if(changed_mask_0_1 & core_mask_0_1)
               {
                  ret = ERR_ILLEGAL_ASSIGNMENT;
                  goto cleanup_set_cores;
               }
               changed_mask_0_1 |= core_mask_0_1;
            }

            core_mask_1_0 = (core_mask & ~core_assign->core_mask[i]);
            if(core_mask_1_0)
            {
               /*  Check the 1->0 transitions in the mask */
               if(changed_mask_1_0 & core_mask_1_0)
               {
                  ret = ERR_ILLEGAL_ASSIGNMENT;
                  goto cleanup_set_cores;
               }
               changed_mask_1_0 |= core_mask_1_0;
               /* If we are reducing the cores to 0 for any microcode, there
                * should not be any open handles for that microcode */
               if((core_assign->core_mask[i] == 0)
                   && pdev->microcode[i].use_count)
               {
                  ret = ERR_ILLEGAL_ASSIGNMENT;
                  goto cleanup_set_cores;
               }
            }
         }
         /* Make sure the transitions match */
         if(changed_mask_1_0 != changed_mask_0_1)
         {
               ret = ERR_ILLEGAL_ASSIGNMENT;
               goto cleanup_set_cores;
         }

         /* We will first free cores */
         for(i=FREE_IDX+1; i<MICROCODE_MAX; i++)
         {
            Uint8 prev_id = (Uint8)-1;
            if(!(changed_mask_1_0 & get_core_mask(pdev, i)))
                continue;

            id = pdev->microcode[i].core_id;
            while(id != (Uint8)-1)
            {
               /* Is this core to be free'd ? */
               if(changed_mask_1_0 & (1<<id))
               {
                  /* First get the core to a "loop forever state" */
                  if(pdev->microcode[i].code_type == CODE_TYPE_MAINLINE)
                  {
                     if(acquire_core(pdev, i, id))
                     {
                        /* TODO: Need to consider error handling. */
                        cavium_print("Failed core %d acquisition!!\n", id);
                     }
                  }
                  else
                  {
                     /* First we will try to see if the core is ready */
                     wait_cores_idle(pdev, id);
                     /* Cleanup the core structure */
                     pdev->cores[id].ready = 0;
                     pdev->cores[id].pend2048 = 0;
                     pdev->cores[id].lrsrq_idx = -1;
                     pdev->cores[id].ctp_ptr = NULL;
                     pdev->cores[id].lrcc_ptr = NULL;
                  }

                  /* Delink from current list */
                  if(prev_id == (Uint8)-1)
                     pdev->microcode[i].core_id = pdev->cores[id].next_id;
                  else
                     pdev->cores[prev_id].next_id = pdev->cores[id].next_id;

                  /* Add to free list */
                  pdev->cores[id].next_id=pdev->microcode[FREE_IDX].core_id;
                  pdev->microcode[FREE_IDX].core_id = id; 
                  pdev->cores[id].ucode_idx = FREE_IDX;

                  if(prev_id == (Uint8) -1)
                      id = pdev->microcode[i].core_id;
                  else
                     id = pdev->cores[prev_id].next_id;
               }
               else
               {
                  prev_id = id; id = pdev->cores[prev_id].next_id;
               }
            }
         }

         /* TODO: We need to be sure they are done */
         /* Disable the cores */
         cavium_udelay(10);

         cavium_print("Disabling units: mask 0x%lx\n", changed_mask_1_0);

         disable_exec_units_from_mask(pdev, changed_mask_1_0);

         /* Now go ahead and add the cores to the new microcodes */
         for(i=FREE_IDX+1; i<MICROCODE_MAX; i++)
         {
            Uint8 prev_id = (Uint8)-1;
            Uint32 mask = 0;

            if(!(changed_mask_0_1 & core_assign->core_mask[i]))
               continue;

            cavium_print("Loading ucode %d\n", i);

            /* Load the microcode, except for FREE_IDX */
            if((i != FREE_IDX) && load_microcode(pdev, i))
            {
               cavium_print("Error loading microcode %d\n", i);
               ret = ERR_UCODE_LOAD_FAILURE;
               goto cleanup_set_cores;
            }

            id = pdev->microcode[FREE_IDX].core_id;
            while(id != (Uint8)-1)
            {
               /* Is this core to be allocated ? */
               if(changed_mask_0_1 & core_assign->core_mask[i] & (1<<id))
               {
                  /* Delink from free list */
                  if(prev_id == (Uint8)-1)
                      pdev->microcode[FREE_IDX].core_id
                              = pdev->cores[id].next_id;
                  else
                     pdev->cores[prev_id].next_id
                              = pdev->cores[id].next_id;

                  /* Add to microcode list */
                  pdev->cores[id].next_id=pdev->microcode[i].core_id;
                  pdev->microcode[i].core_id = id; 
                  pdev->cores[id].ucode_idx = i;
                  if(pdev->microcode[i].code_type == CODE_TYPE_SPECIAL)
                  {
                     pdev->cores[id].ctp_ptr = pdev->ctp_base
                        + (id*CTP_COMMAND_BLOCK_SIZE*CTP_QUEUE_SIZE);
                     /* Zero the CTP for the core */
                     cavium_memset(pdev->cores[id].ctp_ptr, 0 ,
                             CTP_COMMAND_BLOCK_SIZE*CTP_QUEUE_SIZE);

                     pdev->cores[id].ctp_idx = 0;
                     pdev->cores[id].ready = 1;
                     pdev->cores[id].lrsrq_idx = -1;
                     pdev->cores[id].lrcc_ptr = NULL;
                     pdev->cores[id].doorbell = 0;
                  }
                  mask |= (1<<id);

                  if(prev_id == (Uint8) -1)
                  {
                      id = pdev->microcode[FREE_IDX].core_id;
                  }
                  else
                  {
                     id = pdev->cores[prev_id].next_id;
                  }
               }
               else
               {
                  prev_id = id; id = pdev->cores[prev_id].next_id;
               }
            }

            cavium_dbgprint("Cycling cores: 0x%lx\n", mask);

            cavium_udelay(100);
            enable_exec_units_from_mask(pdev, mask);
         }

         cavium_udelay(100);


         cavium_dbgprint("Enabled cores: 0x%lx\n", get_enabled_units(pdev));

cleanup_set_cores:
         cavium_spin_unlock_softirqrestore(&pdev->mc_core_lock);
         if(ret != 0) break;

         if(pdev->enable == 0)
         {
            int idx;

            /* TODO: Assuming that MLM code is running (may not be true)
             * We will first search for the MLM code type. */
            for(idx=0;idx<MICROCODE_MAX;idx++)
            {
               if(pdev->microcode[idx].code_type == CODE_TYPE_MAINLINE)
                  break;
            }
            if(idx>=MICROCODE_MAX)
            {
               /* We did not find any mainline microcode, so we give up */
               ret = ERR_INIT_FAILURE;
               break;
            }

#ifdef SSL
            /* Now initialize encrypted master secret key and iv in the first 48
             * bytes of FSK */
            //if(init_ms_key(pdev, idx))
            if(init_ms_key(pdev, SSL_SPM_IDX))
            {
               cavium_print("Couldnot initialize encrypted master secret key and IV.\n");
               ret = ERR_INIT_FAILURE;
               break;
            }

            /* Fill random buffer */
            //if(fill_rnd_buffer(pdev, idx))
            if(fill_rnd_buffer(pdev, SSL_SPM_IDX))
            {
               cavium_print("Couldnot fill random buffer.\n");
               ret = ERR_INIT_FAILURE;
               break;
            }
#endif /*SSL*/
            pdev->rnd_index=0;
            pdev->enable=1;

            pdev->rnd_index=0;
            pdev->enable=1;

            /* disable master latency timer */ /* QUES: What for?? */
            write_PCI_register(pdev, 0x40, 0x00000001);
         }
         ret=0;
      }
      break;
#endif
#endif /*NPLUS*/ 

		/* Oops, sorry */
		default:
			cavium_print("cavium: Invalid request 0x%x\n", cmd);
			ret = -EINVAL;
		 break;

	} /* switch cmd*/

  return (ret);

}/*n1_ioctl*/

/*
 *  Standard ioctl() entry point.
 */
int n1_ioctl (struct inode *inode, struct file *file, 
            unsigned int cmd,Uint32 arg)
{
	int ret=0;
	DebugRWReg *dw;
	cavium_device *pdev=NULL;
	Csp1InitBuffer *init_buffer;
   	Uint32 dev_id = MINOR(inode->i_rdev);
   	
   	kernel_call=0;
#ifdef NPLUS

	Uint32 dev_id=0;
	int ucode_idx = MINOR(inode->i_rdev);
	struct MICROCODE *microcode = &(cavium_dev[dev_id].microcode[ucode_idx]);

#else /* !NPLUS */
	

	
	if (dev_id > (dev_count - 1)) {
		cavium_print("No N1 device associated with this minor device no. %ld\n", dev_id);
		return -ENODEV;
	}

#endif /*NPLUS*/

	pdev = &cavium_dev[dev_id];

 	cavium_dbgprint("Ioctl Cmd 0x%x called\n",cmd);
 
	switch (cmd) {
		/* write PKP register */
		case IOCTL_N1_DEBUG_WRITE_CODE:
			dw =  (DebugRWReg *)arg;
			cavium_dbgprint("pkp_drv: writing %lx at %lx\n", 
				     dw->data, dw->addr);
			write_PKP_register(pdev, 
					   dw->addr, dw->data);
			ret = 0;
			break;
	
		/* Read PKP register */
		case IOCTL_N1_DEBUG_READ_CODE:
			dw = (DebugRWReg *)arg;
			cavium_dbgprint( "Kernel: reading %lx \n", dw->addr);
			read_PKP_register(pdev,
					  dw->addr, &dw->data);
			ret = 0;
			break;

		/* Write PCI config space */
		case IOCTL_PCI_DEBUG_WRITE_CODE:
			dw =  (DebugRWReg *)arg;
			cavium_dbgprint("pkp_drv: writing %lx at PCI config %lx\n", 
				     dw->data, dw->addr);
#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
			((struct pci_dev *)(pdev->dev))->bus->ops->write_dword(
						(struct pci_dev *)(pdev->dev),
							    dw->addr,
							    dw->data);
#else
			((struct pci_dev *)(pdev->dev))->bus->ops->write(
						(struct pci_bus *)(((struct pci_dev *)(pdev->dev))->bus),
						((struct pci_dev *)(pdev->dev))->devfn,
							    dw->addr,
									4,
							    (dw->data));
#endif
			ret = 0;
			break;
	
		/* Read PCI config space */
		case IOCTL_PCI_DEBUG_READ_CODE:
			dw = (DebugRWReg *)arg;
			dw->data = 0;
			cavium_dbgprint("pkp_drv: reading PCI config %lx \n",
				     dw->addr);
#if LINUX_VERSION_CODE < KERNEL_VERSION (2,6,0)
			((struct pci_dev *)(pdev->dev))->bus->ops->read_dword(
							(struct pci_dev *)(pdev->dev),
							dw->addr,
							(unsigned int *)&dw->data);
#else
			((struct pci_dev *)(pdev->dev))->bus->ops->read(
						(struct pci_bus *)(((struct pci_dev *)(pdev->dev))->bus),
							((struct pci_dev *)(pdev->dev))->devfn,
							dw->addr,
							4,
							(unsigned int *)&dw->data);
#endif
			ret = 0;
			break;
		
			/* run some basic test */
		case IOCTL_N1_API_TEST_CODE:
#if defined API_TEST
			ret = api_test(pdev);
#else
			ret = -1;
#endif /* API_TEST */
			break;

		case IOCTL_N1_DO_OPERATION:
		{
			n1_operation_buffer *buf;

			buf = (n1_operation_buffer *)arg;
			cavium_dbgprint("ioctl N1 do operation called with opcode 0x%x\n", 
					buf->opcode);
			buf->dma_mode = CAVIUM_DIRECT;
#ifdef NPLUS
			cavium_dbgprint("IOCTL_DO_OP:ucodeidx=%d\n", ucode_idx);
			buf->ucode_idx = ucode_idx;
#endif
			ret = do_operation(pdev, buf);
			if(ret == ERR_REQ_PENDING)
				ret = EAGAIN;
		}
			cavium_dbgprint("ioctl N1 do operation returning.\n");
			break;
		case IOCTL_N1_DO_SG_OPERATION:
		{
			n1_operation_buffer *buf;

			buf = (n1_operation_buffer *)arg;
			cavium_dbgprint("ioctl N1 do operation called with opcode 0x%x\n", 
					buf->opcode);
			buf->dma_mode = CAVIUM_SCATTER_GATHER;
#ifdef NPLUS
			buf->ucode_idx = ucode_idx;
#endif
			buf->opcode = buf->opcode & (~0x80);
			ret = do_operation(pdev, buf);
		}
			break;
		case IOCTL_N1_GET_REQUEST_STATUS:
		{
			cavium_dbgprint("Ioctl get request status called\n");
#ifdef NPLUS
			if(microcode->code_type == CODE_TYPE_SPECIAL)
			{
			move_srq_entries(pdev, ucode_idx, 0);
			}
#endif /*NPLUS*/
			ret = check_nb_command_id((Uint32)arg);
			if(ret == ERR_REQ_PENDING)
				ret = EAGAIN;

			cavium_dbgprint("get_request_status: 0x%x\n", ret);

		}
			break;
		case IOCTL_N1_FLUSH_ALL_CODE:
		{
			cavium_dbgprint("Ioctl flush all code called\n");
			cleanup_nb_command_pid(current->pid);
		}
			break;
		case IOCTL_N1_FLUSH_CODE:
		{
			cavium_dbgprint("Ioctl N1 Flush code called\n");
			cleanup_nb_command_id((Uint32)arg);
		}
			break;
		case IOCTL_N1_ALLOC_CONTEXT:
		{
			n1_context_buf c;

			cavium_dbgprint("ioctl N1 alloc context called\n");
			c = (*(n1_context_buf *)arg);
			c.ctx_ptr = alloc_context(pdev,(c.type));
			ret = 0;
#ifdef CAVIUM_RESOURCE_CHECK
			{
			struct CAV_RESOURCES *resource = file->private_data;
			cavium_spin_lock(&resource->resource_check_lock);
			ret = insert_ctx_entry(&resource->ctx_head, 
						c.type,
						c.ctx_ptr);
			cavium_spin_unlock(&resource->resource_check_lock);
			}
#endif
			if(cavium_copy_out((caddr_t)arg, (unsigned char *)&c, sizeof(n1_context_buf)))
				cavium_error("Failed to copy out context\n");
		}
			cavium_dbgprint("ioctl N1 alloc context returning\n");
			break;

		case IOCTL_N1_FREE_CONTEXT:
		{
			n1_context_buf c;

			cavium_dbgprint("ioctl N1 free context called\n");
			c = (*(n1_context_buf *)arg);
			dealloc_context(pdev, c.type, c.ctx_ptr);
			ret = 0;
#ifdef CAVIUM_RESOURCE_CHECK
			{
			struct CAV_RESOURCES *resource = file->private_data;
      			struct cavium_list_head *tmp, *tmp1;
			cavium_spin_lock(&resource->resource_check_lock);
      			cavium_list_for_each_safe(tmp, tmp1, &resource->ctx_head) {
	      			struct CTX_ENTRY *entry = list_entry(tmp, struct CTX_ENTRY, list);
				if (entry->ctx == c.ctx_ptr) 
				{
	      				cavium_list_del(&entry->list);
					cavium_free((Uint8 *)entry);
				}
      			}
			cavium_spin_unlock(&resource->resource_check_lock);
			}
#endif
		}
			cavium_dbgprint("ioctl N1 free context returning\n");
			break;
		case IOCTL_N1_SOFT_RESET_CODE:
		{
			do_soft_reset(&cavium_dev[0]);
			ret = 0;
		}
			break;
		case IOCTL_N1_ALLOC_KEYMEM:
		{
			Uint64 key_handle;
			cavium_dbgprint("ioctl N1 alloc keymem called\n");
			key_handle = alloc_key_memory(pdev);
			if (key_handle == (Uint64)NULL) {
				cavium_error("Allocation of Key Memory failed\n");
				return -1;
			}
#ifdef CAVIUM_RESOURCE_CHECK
			{
			struct CAV_RESOURCES *resource = file->private_data;
			cavium_spin_lock_softirqsave(&resource->resource_check_lock);
			ret = insert_key_entry(&resource->key_head, 
						key_handle);
			cavium_spin_unlock_softirqrestore(&resource->resource_check_lock);
			}
#endif
			if(cavium_copy_out((caddr_t)arg, (unsigned char *)&key_handle, sizeof(Uint64)))
				cavium_error("Failed to copy out key_handle\n");
		}
			cavium_dbgprint("ioctl N1 alloc keymem returning.\n");
			break;
		case IOCTL_N1_FREE_KEYMEM:
		{
			n1_write_key_buf key_buf;
			cavium_dbgprint("ioctl N1 free keymem called\n");
			key_buf = (*(n1_write_key_buf *)arg);
			dealloc_key_memory(pdev, key_buf.key_handle);
#ifdef CAVIUM_RESOURCE_CHECK
			{
			struct CAV_RESOURCES *resource = file->private_data;
      			struct cavium_list_head *tmp, *tmp1;
			cavium_spin_lock_softirqsave(&resource->resource_check_lock);
      			cavium_list_for_each_safe(tmp, tmp1, &resource->key_head) {
	      			struct KEY_ENTRY *entry = list_entry(tmp, struct KEY_ENTRY, list);
				if (entry->key_handle == key_buf.key_handle) 
				{
	      				cavium_list_del(&entry->list);
					cavium_free((Uint8 *)entry);
				}
      			}
			cavium_spin_unlock_softirqrestore(&resource->resource_check_lock);
			}
#endif
		}
			cavium_dbgprint("ioctl N1 free keymem returning.\n");
			break;
		case IOCTL_N1_WRITE_KEYMEM:
		{
			n1_write_key_buf key_buf;
			Uint8 *key;
			key_buf = (*(n1_write_key_buf *)arg);
			key = cavium_malloc(key_buf.length, NULL);
			if (key == NULL) {
				cavium_error("Unable to allocate memory for key\n");
				return -1;
			}
			if(cavium_copy_in(key, key_buf.key, key_buf.length))
			{
				cavium_error("Unable to copy in key\n");
				return -1;
			}
			key_buf.key = key;
			if (store_key_mem(pdev, key_buf
#ifdef NPLUS
						,ucode_idx
#endif
						) < 0) 
			{
				cavium_error("n1_ioctl: store_key_mem failed\n");
				cavium_free(key);
				return -1;
			}
			cavium_free(key);
			ret = 0;
		}
			break;

		case IOCTL_N1_GET_RANDOM_CODE:
		{
			n1_operation_buffer *buf;

			if(!pdev->enable)
			{
				ret = ERR_DRIVER_NOT_READY;
				break;
			}
			buf = (n1_operation_buffer *)arg;
			ret = get_rnd(pdev, buf->outptr[0],
				buf->outsize[0]
#ifdef NPLUS
				, ucode_idx
#endif
				);
		}
			break;
		case IOCTL_N1_INIT_CODE:
		{
#ifdef NPLUS
			cavium_dbgprint("calling nplus_init\n");
			ret = nplus_init(pdev, ucode_idx, arg);
#else
			int boot_info = 0;
			int mainline_info = 0;
			Uint8 code_type;
			int i;
			struct MICROCODE *microcode;

			init_buffer = (Csp1InitBuffer *)arg;

			microcode = pdev->microcode;
			
			boot_info = 0;
			mainline_info = 0;
			
			/* get all the information from init buffer */
			for(i=0;i<init_buffer->size;i++)
			{
				code_type = init_buffer->version_info[i][0];

				if(code_type == CODE_TYPE_BOOT)
				{
					if(boot_info)
					{
						cavium_print( "Code type boot found more than once\n");
						ret = ERR_INIT_FAILURE;
						break;
					}
					else
					{
						cavium_print( "got boot microcode\n");
						boot_info=1;
					}
				}
				else if (code_type == CODE_TYPE_MAINLINE)
				{
					if(mainline_info)
					{
						cavium_print( "Code type mainline found more than once\n");
						ret = ERR_INIT_FAILURE;
						break;
					}
					else
					{
						cavium_print( "got mainline microcode\n");
						mainline_info=1;
					}
				}
				else
				{
					cavium_print( "unknown microcode type\n");
					ret = ERR_INIT_FAILURE;
					break;
				}
				
				/* code */

				microcode[i].code_type = code_type;
				microcode[i].code_size = init_buffer->code_length[i];
				microcode[i].code = 
						(Uint8 *)get_buffer_from_pool(pdev, microcode[i].code_size);
				
				if (microcode[i].code == NULL)
				{
					cavium_print( "Failed to allocate %ld bytes microcode buffer type %d\n", 
										microcode[i].code_size, code_type);
					ret = ERR_MEMORY_ALLOC_FAILURE;
					break;
				}				

				if(cavium_copy_in(microcode[i].code, 
					       init_buffer->code[i],
					       microcode[i].code_size))
				{
					cavium_error("Failed to copy microcode->code for microcode %d\n", i);
					ret = ERR_INIT_FAILURE;
					break;
				}
				
				/* data */
				microcode[i].data_size = init_buffer->data_length[i];
				if(microcode[i].data_size)
				{
					int offset;
#ifdef MC2
					microcode[i].data = 
						(Uint8 *)get_buffer_from_pool(pdev, microcode[i].data_size + 40);
					offset = 40;
#else
					microcode[i].data = 
						(Uint8 *)get_buffer_from_pool(pdev, microcode[i].data_size);
					offset = 0;
#endif
					
					if (microcode[i].data == NULL)
					{
						cavium_print( "Failed to allocate %ld bytes cst buffer type %d\n", 
											microcode[i].data_size,code_type);

						ret = ERR_MEMORY_ALLOC_FAILURE;
						break;
					} 
					cavium_memset(microcode[i].data, 0x0,
						      microcode[i].data_size + offset);
					 
					 
					if(cavium_copy_in(microcode[i].data + offset, 
						       init_buffer->data[i],
						       microcode[i].data_size))
					{
						cavium_error("Failed to copy in microcode->data for microcode %d\n", i);
						ret = ERR_INIT_FAILURE;
						break;
					}
				}

				/* sram address */
				if(cavium_copy_in(microcode[i].sram_address, 
					       init_buffer->sram_address[i],
					       SRAM_ADDRESS_LEN))
				{
					cavium_error("Failed to copy in microcode->sram_address for microcode %d\n", i);
					ret = ERR_INIT_FAILURE;
					break;
				}

			
				cavium_print("Code type = %02x, code size = %lx, data size = %lx\n",
					microcode[i].code_type,
					microcode[i].code_size,
					microcode[i].data_size);

			
			}/* for */

			/* check for any missing piece */
			if( !mainline_info || !boot_info ) {
			  cavium_print( "Not all of the information was sent to device driver.\n");
			  cavium_print( "Please check version information\n");
			  ret = ERR_INIT_FAILURE;
			  break;
			}

			/* Now we have gathered all the required information from init_buffer*/
			/* Now it is time for some action. */

			ret = do_init(pdev);
#endif /*NPLUS*/	
			break;
		}

#ifdef NPLUS
      case IOCTL_CSP1_GET_CORE_ASSIGNMENT:
      {
         int i;
         Csp1CoreAssignment *core_assign = (Csp1CoreAssignment *)arg;

				 cavium_dbgprint("ioctl Get core assignment \n");
         cavium_spin_lock_softirqsave(&pdev->mc_core_lock);

         for(i=0;i<MICROCODE_MAX;i++)
         {
            core_assign->core_mask[i] = get_core_mask(pdev,i); 
            core_assign->mc_present[i] = (pdev->microcode[i].code==NULL)? 0:1;
         }
         cavium_spin_unlock_softirqrestore(&pdev->mc_core_lock);
      }
      break;

      case IOCTL_CSP1_SET_CORE_ASSIGNMENT:
      {
         int i;
         Uint8 id;
         Uint32 changed_mask_0_1 = 0, changed_mask_1_0 = 0;
         Uint32 core_mask, core_mask_1_0, core_mask_0_1;
         Csp1CoreAssignment *core_assign = (Csp1CoreAssignment *)arg;

				 cavium_dbgprint("ioctl set core assignment \n");

         if(pdev->initialized != 1)
         {
            ret = ERR_DRIVER_NOT_READY;
            break;
         }

         cavium_dbgprint("Assign Cores(%ld): {%lx %lx %lx %lx %lx %lx}\n",
            jiffies,
            core_assign->core_mask[0], core_assign->core_mask[1],
            core_assign->core_mask[2], core_assign->core_mask[3],
            core_assign->core_mask[4], core_assign->core_mask[5]);
   
         cavium_spin_lock_softirqsave(&pdev->mc_core_lock);

         /* This loop checks if the new assignments will be valid */
         for(i=0;i<MICROCODE_MAX && ret==0;i++)
         {
            /* Does this ucode require paired cores for 2048 bit ops ? */
            if(pdev->microcode[i].paired_cores)
            {
               core_mask = core_assign->core_mask[i];
               /* We will check if the new assignment will result in an
                * unpaired core
                */
               while(core_mask != 0)
               {
                  if((core_mask & 0x1) != ((core_mask>>1) & 0x1))
                  {
                     ret = ERR_ILLEGAL_ASSIGNMENT;
                     goto cleanup_set_cores;
                  }
                  core_mask = (core_mask >> 2);
               }
            }
            /*  Check the 0->1 transitions in the mask */
            core_mask = get_core_mask(pdev,i);
            core_mask_0_1 = (~core_mask & core_assign->core_mask[i]);
            if(core_mask_0_1)
            {
               if(changed_mask_0_1 & core_mask_0_1)
               {
                  ret = ERR_ILLEGAL_ASSIGNMENT;
                  goto cleanup_set_cores;
               }
               changed_mask_0_1 |= core_mask_0_1;
            }

            core_mask_1_0 = (core_mask & ~core_assign->core_mask[i]);
            if(core_mask_1_0)
            {
               /*  Check the 1->0 transitions in the mask */
               if(changed_mask_1_0 & core_mask_1_0)
               {
                  ret = ERR_ILLEGAL_ASSIGNMENT;
                  goto cleanup_set_cores;
               }
               changed_mask_1_0 |= core_mask_1_0;
               /* If we are reducing the cores to 0 for any microcode, there
                * should not be any open handles for that microcode */
               if((core_assign->core_mask[i] == 0)
                   && pdev->microcode[i].use_count)
               {
                  ret = ERR_ILLEGAL_ASSIGNMENT;
                  goto cleanup_set_cores;
               }
            }
         }
         /* Make sure the transitions match */
         if(changed_mask_1_0 != changed_mask_0_1)
         {
               ret = ERR_ILLEGAL_ASSIGNMENT;
               goto cleanup_set_cores;
         }

         /* We will first free cores */
         for(i=FREE_IDX+1; i<MICROCODE_MAX; i++)
         {
            Uint8 prev_id = (Uint8)-1;
            if(!(changed_mask_1_0 & get_core_mask(pdev, i)))
                continue;

            id = pdev->microcode[i].core_id;
            while(id != (Uint8)-1)
            {
               /* Is this core to be free'd ? */
               if(changed_mask_1_0 & (1<<id))
               {
                  /* First get the core to a "loop forever state" */
                  if(pdev->microcode[i].code_type == CODE_TYPE_MAINLINE)
                  {
                     if(acquire_core(pdev, i, id))
                     {
                        /* TODO: Need to consider error handling. */
                        cavium_print("Failed core %d acquisition!!\n", id);
                     }
                  }
                  else
                  {
                     /* First we will try to see if the core is ready */
                     wait_cores_idle(pdev, id);
                     /* Cleanup the core structure */
                     pdev->cores[id].ready = 0;
                     pdev->cores[id].pend2048 = 0;
                     pdev->cores[id].lrsrq_idx = -1;
                     pdev->cores[id].ctp_ptr = NULL;
                     pdev->cores[id].lrcc_ptr = NULL;
                  }

                  /* Delink from current list */
                  if(prev_id == (Uint8)-1)
                     pdev->microcode[i].core_id = pdev->cores[id].next_id;
                  else
                     pdev->cores[prev_id].next_id = pdev->cores[id].next_id;

                  /* Add to free list */
                  pdev->cores[id].next_id=pdev->microcode[FREE_IDX].core_id;
                  pdev->microcode[FREE_IDX].core_id = id; 
                  pdev->cores[id].ucode_idx = FREE_IDX;

                  if(prev_id == (Uint8) -1)
                      id = pdev->microcode[i].core_id;
                  else
                     id = pdev->cores[prev_id].next_id;
               }
               else
               {
                  prev_id = id; id = pdev->cores[prev_id].next_id;
               }
            }
         }

         /* TODO: We need to be sure they are done */
         /* Disable the cores */
         cavium_udelay(10);

         cavium_print("Disabling units: mask 0x%lx\n", changed_mask_1_0);

         disable_exec_units_from_mask(pdev, changed_mask_1_0);

         /* Now go ahead and add the cores to the new microcodes */
         for(i=FREE_IDX+1; i<MICROCODE_MAX; i++)
         {
            Uint8 prev_id = (Uint8)-1;
            Uint32 mask = 0;

            if(!(changed_mask_0_1 & core_assign->core_mask[i]))
               continue;

            cavium_print("Loading ucode %d\n", i);

            /* Load the microcode, except for FREE_IDX */
            if((i != FREE_IDX) && load_microcode(pdev, i))
            {
               cavium_print("Error loading microcode %d\n", i);
               ret = ERR_UCODE_LOAD_FAILURE;
               goto cleanup_set_cores;
            }

            id = pdev->microcode[FREE_IDX].core_id;
            while(id != (Uint8)-1)
            {
               /* Is this core to be allocated ? */
               if(changed_mask_0_1 & core_assign->core_mask[i] & (1<<id))
               {
                  /* Delink from free list */
                  if(prev_id == (Uint8)-1)
                      pdev->microcode[FREE_IDX].core_id
                              = pdev->cores[id].next_id;
                  else
                     pdev->cores[prev_id].next_id
                              = pdev->cores[id].next_id;

                  /* Add to microcode list */
                  pdev->cores[id].next_id=pdev->microcode[i].core_id;
                  pdev->microcode[i].core_id = id; 
                  pdev->cores[id].ucode_idx = i;
                  if(pdev->microcode[i].code_type == CODE_TYPE_SPECIAL)
                  {
                     pdev->cores[id].ctp_ptr = pdev->ctp_base
                        + (id*CTP_COMMAND_BLOCK_SIZE*CTP_QUEUE_SIZE);
                     /* Zero the CTP for the core */
                     cavium_memset(pdev->cores[id].ctp_ptr, 0 ,
                             CTP_COMMAND_BLOCK_SIZE*CTP_QUEUE_SIZE);

                     pdev->cores[id].ctp_idx = 0;
                     pdev->cores[id].ready = 1;
                     pdev->cores[id].lrsrq_idx = -1;
                     pdev->cores[id].lrcc_ptr = NULL;
                     pdev->cores[id].doorbell = 0;
                  }
                  mask |= (1<<id);

                  if(prev_id == (Uint8) -1)
                  {
                      id = pdev->microcode[FREE_IDX].core_id;
                  }
                  else
                  {
                     id = pdev->cores[prev_id].next_id;
                  }
               }
               else
               {
                  prev_id = id; id = pdev->cores[prev_id].next_id;
               }
            }

            cavium_dbgprint("Cycling cores: 0x%lx\n", mask);

            cavium_udelay(100);
            enable_exec_units_from_mask(pdev, mask);
         }

         cavium_udelay(100);


         cavium_dbgprint("Enabled cores: 0x%lx\n", get_enabled_units(pdev));

cleanup_set_cores:
         cavium_spin_unlock_softirqrestore(&pdev->mc_core_lock);
         if(ret != 0) break;

         if(pdev->enable == 0)
         {
            int idx;

            /* TODO: Assuming that MLM code is running (may not be true)
             * We will first search for the MLM code type. */
            for(idx=0;idx<MICROCODE_MAX;idx++)
            {
               if(pdev->microcode[idx].code_type == CODE_TYPE_MAINLINE)
                  break;
            }
            if(idx>=MICROCODE_MAX)
            {
               /* We did not find any mainline microcode, so we give up */
               ret = ERR_INIT_FAILURE;
               break;
            }

#ifdef SSL
            /* Now initialize encrypted master secret key and iv in the first 48
             * bytes of FSK */
            //if(init_ms_key(pdev, idx))
            if(init_ms_key(pdev, SSL_SPM_IDX))
            {
               cavium_print("Couldnot initialize encrypted master secret key and IV.\n");
               ret = ERR_INIT_FAILURE;
               break;
            }

            /* Fill random buffer */
            //if(fill_rnd_buffer(pdev, idx))
            if(fill_rnd_buffer(pdev, SSL_SPM_IDX))
            {
               cavium_print("Couldnot fill random buffer.\n");
               ret = ERR_INIT_FAILURE;
               break;
            }
#endif /*SSL*/
            pdev->rnd_index=0;
            pdev->enable=1;

            pdev->rnd_index=0;
            pdev->enable=1;

            /* disable master latency timer */ /* QUES: What for?? */
            write_PCI_register(pdev, 0x40, 0x00000001);
         }
         ret=0;
      }
      break;

#endif /*NPLUS*/ 

		/* Oops, sorry */
		default:
			cavium_print("cavium: Invalid request 0x%x\n", cmd);
			ret = -EINVAL;
		 break;

	} /* switch cmd*/

  return (ret);

}/*n1_ioctl*/

/*
 * Poll for completion
 */
unsigned int
n1_poll(struct file *fp, poll_table *wait)
{
	Uint32 mask = 0, is_ready;
	cavium_pid_t pid = current->pid;

	is_ready = check_nb_command_pid(pid);

	if (is_ready) {
		mask |= POLLIN | POLLRDNORM;
	}

	return mask;
}
	

#ifndef CAVIUM_NO_MMAP
/* 
 *  VMA Operation called when an munmap of the entire VM segment is done
 */

void 
n1_vma_close(struct vm_area_struct *vma)
{
        Uint32 size;
        Uint32 virt_addr;
        Uint8 *kmalloc_ptr, *kmalloc_area;
#ifdef NPLUS
	Uint32 minor=0;
#else
	Uint32 minor = MINOR(vma->vm_file->f_dentry->d_inode->i_rdev);
#endif

        kmalloc_ptr = vma->vm_private_data;
        size = vma->vm_end - vma->vm_start;

        /* align it to page boundary */
        kmalloc_area = (Uint8 *)(((Uint32)kmalloc_ptr + PAGE_SIZE -1) & PAGE_MASK);

        /* Unreserve all pages */
        for(virt_addr = (Uint32)kmalloc_area; 
	    virt_addr < (Uint32)kmalloc_area + size; virt_addr +=PAGE_SIZE) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,0) 
                mem_map_unreserve(virt_to_page(virt_addr));
#else
				ClearPageReserved(virt_to_page(virt_addr));
#endif
        }

        put_buffer_in_pool(&cavium_dev[minor], kmalloc_ptr);

        cavium_dbgprint( "pkp_drv: UNmap returning successfully(pid=%d)\n",
                           current->pid);
        CAVIUM_MOD_DEC_USE_COUNT;
        return;

}

static struct vm_operations_struct n1_vma_ops = 
{
	NULL,
	n1_vma_close,
	NULL,
};


/*
 * mmap entry point
 */
int 
n1_mmap(struct file *file, struct vm_area_struct *vma)
{
        Uint32 size;
        Uint8 *kmalloc_ptr,*kmalloc_area;
        Uint32 virt_addr, offset;
#ifdef NPLUS
	Uint32 minor=0;
#else
	Uint32 minor = MINOR(file->f_dentry->d_inode->i_rdev);
#endif

        size = vma->vm_end - vma->vm_start;

        if(size % PAGE_SIZE) {
                cavium_error("n1_mmap: size (%ld) not multiple of PAGE_SIZE.\n", size);
                return -ENXIO;
        }

        offset = vma->vm_pgoff << PAGE_SHIFT;
        if(offset & ~PAGE_MASK) {
                cavium_error("n1_mmap: offset (%ld) not aligned.\n", offset);
                return -ENXIO;
        }

        kmalloc_ptr = (Uint8 *)get_buffer_from_pool(&cavium_dev[minor], size);
        if(kmalloc_ptr == NULL) {
                cavium_error("n1_mmap: not enough memory.\n");
                return -ENOMEM;
        }

        /* align it to page boundary */
        kmalloc_area = (Uint8 *)(((Uint32)kmalloc_ptr + PAGE_SIZE -1) & PAGE_MASK);

        /* reserve all pages */
        for (virt_addr = (Uint32)kmalloc_area; 
	     virt_addr < (Uint32)kmalloc_area + size; virt_addr +=PAGE_SIZE) {
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,0) 
                mem_map_reserve(virt_to_page(virt_addr));
#else
		SetPageReserved(virt_to_page(virt_addr));
#endif
		/*  get_page not required *
                get_page(virt_to_page(virt_addr)); */
        }
	
        /* Mark the vm-area Reserved*/
        vma->vm_flags |=VM_RESERVED;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11)
        if(io_remap_page_range(vma,vma->vm_start,
		            virt_to_phys((void *)(Uint32)kmalloc_area),
                            size, vma->vm_page_prot))

#elif LINUX_VERSION_CODE <= KERNEL_VERSION(2,4,18) 
        if(remap_page_range(vma->vm_start,
		            virt_to_phys((void *)(Uint32)kmalloc_area),
                            size, vma->vm_page_prot))
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,20) 
        if(remap_page_range(vma,vma->vm_start,
		            virt_to_phys((void *)(Uint32)kmalloc_area),
                            size, vma->vm_page_prot))
#endif
				{
 
                cavium_error("n1_mmap: remap page range failed.\n");
                return -ENXIO;
        }

        vma->vm_ops = &n1_vma_ops;
    		vma->vm_private_data = kmalloc_ptr;
				vma->vm_file = file;
	
        CAVIUM_MOD_INC_USE_COUNT;
        cavium_dbgprint( "n1_mmap: mmap returning successfully(pid=%d)\n",current->pid);
        return 0;
}

#endif


/*
 * Linux layer Intrerrupt Service Routine
 */
irqreturn_t linux_layer_isr(int irq, void *dev_id, struct pt_regs *regs)
{
#ifdef INTERRUPT_RETURN
	int ret;
	ret = cavium_interrupt_handler(dev_id);
	if (ret == 0) {
		return IRQ_HANDLED;
	} else {
		return IRQ_NONE;
	}
#else
 cavium_interrupt_handler(dev_id);
#endif
}

/* 
 * Hook the interrupt handler
 */
int setup_interrupt(cavium_device *pdev)
{
   int result;
   int interrupt_pin;

   interrupt_pin = ((struct pci_dev *)(pdev->dev))->irq;

   result = request_irq(interrupt_pin, linux_layer_isr,SA_SHIRQ,DEVICE_NAME,pdev);

   if(result)
   {
      cavium_print ("pkp_drv: can't get assigned irq : %x\n", interrupt_pin);
      return 1;
   }
   return 0;
}/* setup interrupt */


/* Let go the interrupt */
void free_interrupt(cavium_device *pdev)
{
   int interrupt_pin;
   
   interrupt_pin = ((struct pci_dev *)(pdev->dev))->irq;
   free_irq(interrupt_pin, pdev);
}

/* 
 * initialize kernel mode.
 * Calls user interface specific functions.
 */
int
init_kernel_mode ()
{
	struct N1_Dev *device_node = NULL, *prev = NULL;
	int i;

	prev = NULL;
	for (i = 0; i < dev_count; i++) {
	   device_node = cavium_malloc_dma((sizeof(struct N1_Dev)), NULL); 
	   if (i == 0) {
	       device_list = device_node;
	   }
	   device_node->id = i;
	   device_node->bus = cavium_dev[i].bus_number;
	   device_node->dev = cavium_dev[i].dev_number;
	   device_node->func = cavium_dev[i].func_number;
	   device_node->data = (void *)(&cavium_dev[i]);
	   device_node->next = prev;
	}
	
#if !defined(SSL) || defined(NPLUS)
	inter_module_register(N1ConfigDeviceName, THIS_MODULE, 
			      n1_config_device);
	inter_module_register(N1UnconfigDeviceName, THIS_MODULE, 
			      n1_unconfig_device); 
	inter_module_register(N1AllocContextName, THIS_MODULE, 
			      n1_alloc_context);
	inter_module_register(N1FreeContextName, THIS_MODULE, 
			      n1_free_context);
	inter_module_register(N1ProcessInboundPacketName, THIS_MODULE,
			      n1_process_inbound_packet);
	inter_module_register(N1ProcessOutboundPacketName, THIS_MODULE,
			      n1_process_outbound_packet);
	inter_module_register(N1WriteIpSecSaName, THIS_MODULE,
			      n1_write_ipsec_sa);
#endif

	return 0;
}/* init_kernel_mode */

/*
 * free kernel mode.
 * Calls user interface specific functions
 */
int
free_kernel_mode (void)
{
	struct N1_Dev *node = device_list;
/* 
 * */
	while (node != NULL) {
		struct N1_Dev *tmp;
		tmp = node->next;
		cavium_free_dma(node);
		node = tmp;
	}
#if !defined(SSL) || defined(NPLUS)
	inter_module_unregister(N1ConfigDeviceName);
	inter_module_unregister(N1UnconfigDeviceName);
	inter_module_unregister(N1AllocContextName);
	inter_module_unregister(N1FreeContextName);
	inter_module_unregister(N1ProcessOutboundPacketName);
	inter_module_unregister(N1ProcessInboundPacketName);
	inter_module_unregister(N1WriteIpSecSaName);
#endif
	return 0;
}/* free_kernel_mode */

module_init (initmodule);
module_exit (cleanupmodule);
/*
 * $Id: linux_main.c,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: linux_main.c,v $
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
 * Revision 1.8  2006/06/23 09:57:51  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.7  2006/06/09 05:54:40  liqin
 * *** empty log message ***
 *
 * Revision 1.6  2006/04/29 11:14:36  liqin
 * *** empty log message ***
 *
 * Revision 1.5  2006/04/28 02:43:10  liqin
 * *** empty log message ***
 *
 * Revision 1.4  2006/04/07 00:24:14  lijing
 * *** empty log message ***
 *
 * Revision 1.3  2006/04/06 13:32:19  lijing
 * *** empty log message ***
 *
 * Revision 1.2  2006/04/06 02:24:59  lijing
 * add cavium_driver support by lijing
 *
 * Revision 1.1.2.1  2006/04/04 17:53:41  lijing
 * no message
 *
 * Revision 1.34  2005/08/31 18:11:45  bimran
 * Added CAVIUM_NO_MMAP macro.
 *
 * Revision 1.33  2005/08/31 02:25:18  bimran
 * Fixed code to check for copy_in/out return values and for some other functions too.
 * Fixed for 2.6.11 kernel.
 *
 * Revision 1.32  2005/06/29 19:41:26  rkumar
 * 8-byte alignment problem fixed with N1_SANITY define.
 *
 * Revision 1.31  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.30  2005/06/10 09:12:07  rkumar
 * 7.3 compilation error fixed
 *
 * Revision 1.29  2005/06/03 08:20:27  rkumar
 * Preventing opening of devices when driver removal is being done
 *
 * Revision 1.28  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.27  2005/02/03 19:21:30  tsingh
 * fixed kernel version dependency
 *
 * Revision 1.26  2005/02/01 04:07:12  bimran
 * copyright fix
 *
 * Revision 1.25  2005/01/28 18:32:15  mvarga
 * Fixed kernel version dependenmcy (bimran)
 *
 * Revision 1.24  2005/01/26 20:34:04  bimran
 * Fixed dependency on RH distribution. Made it kernel version dependent.
 *
 * Revision 1.23  2004/08/03 20:44:10  tahuja
 * support for Mips Linux & HT.
 *
 * Revision 1.22  2004/07/07 17:59:31  tsingh
 * some compilation issues
 *
 * Revision 1.21  2004/06/23 02:20:20  mikev
 * Added api test.
 *
 * Revision 1.20  2004/06/09 01:57:27  bimran
 * Fixed a bug in NPLUS mode where microcode reference was taken from NULL pointer.
 *
 * Revision 1.19  2004/06/08 18:06:39  tsingh
 * fixed compile time issue
 *
 * Revision 1.18  2004/06/03 21:19:41  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.17  2004/06/02 19:02:53  tsingh
 * added one more debug print. (bimran)
 *
 * Revision 1.16  2004/06/02 02:08:23  tsingh
 * removed get_id() (bimran).
 *
 * Revision 1.15  2004/06/01 17:42:06  bimran
 * Made some locks softirq safe,
 *
 * Revision 1.14  2004/05/11 20:50:31  tsingh
 * Changed some arguments passed through a function
 *
 * Revision 1.13  2004/05/05 06:43:23  bimran
 * Fixed non blocking return code.
 *
 * Revision 1.12  2004/05/04 20:48:08  bimran
 * Fixed RESOURCE_CHECK.
 *
 * Revision 1.11  2004/05/02 19:43:58  bimran
 * Added Copyright notice.
 *
 * Revision 1.10  2004/05/01 07:13:37  bimran
 * Fixed return code get request status function.
 *
 * Revision 1.9  2004/05/01 00:48:10  tsingh
 * Fixed for NPLus (bimran).
 *
 * Revision 1.8  2004/04/24 04:01:26  bimran
 * Fixed NPLUS related bugs.
 * Added some more debug prints.
 *
 * Revision 1.7  2004/04/22 01:10:35  bimran
 * Added NPLUS registeration message.
 *
 * Revision 1.6  2004/04/21 19:15:18  bimran
 * Added Random pool support.
 * Added NPLUS specific initialization functions and core acquire functions.
 * Added NPLUS related ioctls.
 *
 * Revision 1.5  2004/04/20 17:40:27  bimran
 * changed all microcode references from cavium_device structure instead of global mirocode structure.
 *
 * Revision 1.4  2004/04/20 02:18:37  bimran
 * Removed an unreachabe code segment where ddr_present flag was checked before the driver checked for DDR memory presence.
 *
 * Revision 1.3  2004/04/19 18:36:32  bimran
 * Removed admin microcode requirement.
 *
 * Revision 1.2  2004/04/16 23:57:47  bimran
 * Added more debug prints.
 *
 * Revision 1.1  2004/04/15 22:40:47  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

