////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: interrupt.c
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
#include "error_handler.h"
#include "pending_list.h"

#ifdef INTERRUPT_RETURN
int
#else
void 
#endif
cavium_interrupt_handler(void *arg);
int 
enable_all_interrupts(cavium_device * pkp_dev);
/*
 * interrupt handler
 */

#ifdef INTERRUPT_RETURN
int
#else
void 
#endif
cavium_interrupt_handler(void *arg)
{
   Uint32 dwval=0;
   int error_code =0;
   cavium_device *pdev;

   pdev = (cavium_device *)arg;

   /* first check to see if pkp has interrupted */
   read_PKP_register(pdev, (pdev->bar_0 + ISR_REG), &dwval);

   if(dwval == 0)
#ifdef INTERRUPT_RETURN
      return -1;
#else
      return;
#endif
   
   /* 
    * now since we know that it is pkp who has interrupted, 
    * mask all the interrupts 
    */
   write_PKP_register(pdev, (pdev->bar_0 + IMR_REG), 0);

   /* check if general error has occured*/
   if(dwval & 8) 
   {
#ifdef MC2
#define LARGE_ERROR_VAL   (2*1024)
      int dump_size = 8;
#ifdef MCODE_LARGE_DUMP
      dump_size = LARGE_ERROR_VAL;
#endif
      if(cavium_debug_level >= 3)
         cavium_dump("error_val", (Uint8 *)pdev->error_val, dump_size);
#endif
      
#ifdef INTERRUPT_ON_COMP 
      cavium_tasklet_schedule(&pdev->interrupt_task);
#endif
      write_PKP_register(pdev,(pdev->bar_0 + ISR_REG), dwval);
      write_PKP_register(pdev,(pdev->bar_0 + IMR_REG), pdev->imr);
#ifdef INTERRUPT_RETURN
      return 0;
#else
      return;
#endif
   }
   
   if (dwval & 0x10) 
   {
      cavium_print("cavium_interrupt: EXEC unit watchdog timeout.\n");
   } else if ((error_code = check_hard_reset_group(pdev))) 
   {
      /* hard reset group */ 
      cavium_print("HArd Reset Group\n");
      handle_hard_reset(pdev);  /* :-) */
      clear_error(pdev, error_code);
    } else if ((error_code = check_soft_reset_group(pdev))) 
    {
      /* soft reset group */
      cavium_print("Soft Reset Group\n");
      handle_soft_reset(pdev);
      clear_error(pdev,error_code);
   } else if ((error_code = check_exec_reset_group(pdev))) 
   {
      /* exec reset group */
      cavium_print("Exec reset group \n");
      handle_exec_reset(pdev);
      clear_error(pdev,error_code);
   } else if ((error_code = check_seq_no_group(pdev))) 
   {
      /* the others */
      cavium_print("Sequence Number group \n");
      handle_seq_no_error(pdev);
      clear_error(pdev,error_code);
   } else 
   {
      /* Oops! whats this? */
      cavium_error("pkp: undocumented interrupt occured. ISR= %08lx\n", 
              dwval);
   }

   /* clear the interrupt status */
   write_PKP_register(pdev,(pdev->bar_0 + ISR_REG), dwval);

   /* restore interrupts */
   write_PKP_register(pdev,(pdev->bar_0 + IMR_REG), pdev->imr);

#ifdef INTERRUPT_RETURN
   return 0;
#else
   return;
#endif
 
}/* cavium_interrupt_handler */

int 
enable_all_interrupts(cavium_device * pkp_dev)
{
   Uint32 imr_val, cr04_val, dwval;

   imr_val = 0;
   cr04_val = 0;

   imr_val = BM_PCI_MASTER_ABORT_WRITE |
             BM_PCI_TARGET_ABORT_WRITE |
             BM_PCI_MASTER_RETRY_TIMEOUT_WRITE |
             BM_PCI_ADD_ATTRIB_PHASE_PARITY |
             BM_PCI_MASTER_WRITE_PARITY |
             BM_PCI_TARGET_WRITE_DATA_PARITY |
             BM_MSI_TRANSACTION |
             BM_OUTBOUND_FIFO_CMD |
             BM_KEY_MEMORY_PARITY |
             BM_PCI_MASTER_ABORT_REQ_READ |
             BM_PCI_TARGET_ABORT_REQ_READ |
             BM_PCI_MASTER_RETRY_TIMEOUT_REQ_READ |
             BM_PCI_MASTER_DATA_PARITY_REQ_READ |
             BM_REQ_COUNTER_OVERFLOW |
             BM_EXEC_REG_FILE_PARITY |
             BM_EXEC_UCODE_PARITY |
             BM_PCI_MASTER_ABORT_EXEC_READ   |
             BM_PCI_TARGET_ABORT_EXEC_READ |
             BM_PCI_MASTER_RETRY_TIMOUT_EXEC_READ |
             BM_PCI_MASTER_DATA_PARITY_EXEC_READ |
             BM_EXEC_GENERAL |
             BM_CMC_DOUBLE_BIT |
             BM_CMC_SINGLE_BIT;


   cr04_val = BM_CR04_PCI_TARGET_ABORT_WRITE |
              BM_CR04_ADD_ATTRIB_PHASE_PARITY |
              BM_CR04_PCI_TARGET_ABORT_REQ_READ |
              BM_CR04_REQ_COUNTER_OVERFLOW |
              BM_CR04_PCI_TARGET_ABORT_EXEC_READ; 

   /* write Interrupt Mask Register */
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+IMR_REG), imr_val);

   /* remember imr */
   pkp_dev->imr = imr_val;

   dwval = 0;
   read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval);

   cr04_val = cr04_val | dwval;
   write_PCI_register(pkp_dev, PCI_CONFIG_04,cr04_val);

   cavium_dbgprint("Interrupt Mask Register = %08lx\n", imr_val);
   cavium_dbgprint("PCI Config 04 = %08lx\n", cr04_val);

   return 1;
} /* enable_all_interrupts*/


/*
 * $Id: interrupt.c,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: interrupt.c,v $
 * Revision 1.4  2015/01/06 08:57:47  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:53  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:30  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:36  chen
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
 * Revision 1.10  2005/10/13 09:24:39  ksnaren
 * fixed compile warnings
 *
 * Revision 1.9  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
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
 * Revision 1.5  2004/06/23 20:38:35  bimran
 * compiler warnings on NetBSD.
 *
 * Revision 1.3  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.2  2004/04/29 00:21:24  bimran
 * Added error_val dump in case of MC2
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

