////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: error_handler.h
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


#ifndef _ERROR_HANDLER_H
#define _ERROR_HANDLER_H



/*define bitmasks for each possible interrupt from PKP */

/* the hard reset group */

#define BM_PCI_MASTER_ABORT_WRITE			0x20000

#define BM_PCI_TARGET_ABORT_WRITE			0x10000
#define BM_CR04_PCI_TARGET_ABORT_WRITE			0x100

#define BM_PCI_MASTER_RETRY_TIMEOUT_WRITE		0x40000

#define BM_PCI_ADD_ATTRIB_PHASE_PARITY			0
#define BM_CR04_ADD_ATTRIB_PHASE_PARITY			0x100

#define BM_PCI_MASTER_WRITE_PARITY			0
#define BM_PCI_TARGET_WRITE_DATA_PARITY			0
#define BM_MSI_TRANSACTION				0
#define BM_OUTBOUND_FIFO_CMD				0x80000
#define BM_KEY_MEMORY_PARITY				0x4



/* the soft reset group */
#define BM_PCI_MASTER_ABORT_REQ_READ			0x1E0

#define BM_PCI_TARGET_ABORT_REQ_READ			0x1E0
#define BM_CR04_PCI_TARGET_ABORT_REQ_READ		0x100

#define BM_PCI_MASTER_RETRY_TIMEOUT_REQ_READ		0x1E0
#define BM_PCI_MASTER_DATA_PARITY_REQ_READ		0x100000

#define BM_REQ_COUNTER_OVERFLOW				0x1E00
#define BM_CR04_REQ_COUNTER_OVERFLOW			0x40

/*EXEC reset group */
#define BM_EXEC_REG_FILE_PARITY				0x1
#define BM_EXEC_UCODE_PARITY				0x2


/*Error code return based errors */
#define BM_PCI_MASTER_ABORT_EXEC_READ			0x10

#define BM_PCI_TARGET_ABORT_EXEC_READ			0x10
#define BM_CR04_PCI_TARGET_ABORT_EXEC_READ		0x100

#define BM_PCI_MASTER_RETRY_TIMOUT_EXEC_READ		0x10
#define BM_PCI_MASTER_DATA_PARITY_EXEC_READ		0x10
#define BM_EXEC_GENERAL					0x8
#define BM_CMC_DOUBLE_BIT				0x8000
#define BM_CMC_SINGLE_BIT				0x4000

int check_completion_code(volatile Uint64 *p);

/* functions to check errors */

/* hard reset group */
int check_PCI_master_abort_write(cavium_device * pkp_dev);
int check_PCI_target_abort_write(cavium_device * pkp_dev);
int check_PCI_master_retry_timeout_write(cavium_device * pkp_dev);
int check_outbound_fifo_cmd(cavium_device * pkp_dev);
int check_key_memory_parity(cavium_device * pkp_dev);

void clear_PCI_master_abort_write(cavium_device * pkp_dev);
void clear_PCI_target_abort_write(cavium_device * pkp_dev);
void clear_PCI_master_retry_timeout_write(cavium_device * pkp_dev);
void clear_outbound_fifo_cmd(cavium_device * pkp_dev);
void clear_key_memory_parity(cavium_device * pkp_dev);


/* soft reset group */
int check_PCI_master_abort_req_read(cavium_device * pkp_dev);
int check_PCI_target_abort_req_read(cavium_device * pkp_dev);
int check_PCI_master_retry_timeout_req_read(cavium_device * pkp_dev);
int check_PCI_master_data_parity_req_read(cavium_device * pkp_dev);
int check_req_counter_overflow(cavium_device * pkp_dev);

void clear_PCI_master_abort_req_read(cavium_device * pkp_dev);
void clear_PCI_target_abort_req_read(cavium_device * pkp_dev);
void clear_PCI_master_retry_timeout_req_read(cavium_device * pkp_dev);
void clear_PCI_master_data_parity_req_read(cavium_device * pkp_dev);
void clear_req_counter_overflow(cavium_device * pkp_dev);


/* reset exec unit and reload ucode group */
int check_EXEC_reg_file_parity(cavium_device * pkp_dev);
int check_EXEC_ucode_parity(cavium_device * pkp_dev);

void clear_EXEC_reg_file_parity(cavium_device * pkp_dev);
void clear_EXEC_ucode_parity(cavium_device * pkp_dev);

/* seq no group */
int check_PCI_master_abort_EXEC_read(cavium_device * pkp_dev);
int check_PCI_target_abort_EXEC_read(cavium_device * pkp_dev);
int check_PCI_master_retry_timeout_EXEC_read(cavium_device * pkp_dev);
int check_PCI_master_data_parity_EXEC_read(cavium_device * pkp_dev);
int check_EXEC_general(cavium_device * pkp_dev);
int check_CMC_double_bit(cavium_device * pkp_dev);
int check_CMC_single_bit(cavium_device * pkp_dev);

void clear_PCI_master_abort_EXEC_read(cavium_device * pkp_dev);
void clear_PCI_target_abort_EXEC_read(cavium_device * pkp_dev);
void clear_PCI_master_retry_timeout_EXEC_read(cavium_device * pkp_dev);
void clear_PCI_master_data_parity_EXEC_read(cavium_device * pkp_dev);
void clear_EXEC_general(cavium_device * pkp_dev);
void clear_CMC_double_bit(cavium_device * pkp_dev);
void clear_CMC_single_bit(cavium_device * pkp_dev);


int check_hard_reset_group(cavium_device * pkp_dev);
int check_soft_reset_group(cavium_device * pkp_dev);
int check_exec_reset_group(cavium_device * pkp_dev);
int check_seq_no_group(cavium_device * pkp_dev);
int handle_hard_reset(cavium_device * pkp_dev);
int handle_soft_reset(cavium_device * pkp_dev);
int handle_exec_reset(cavium_device * pkp_dev);
int handle_seq_no_error(cavium_device * pkp_dev);

void dump_pkp_registers(cavium_device * pkp_dev);
void clear_error(cavium_device * pkp_dev, int error_code);

#endif


/*
 * $Id: error_handler.h,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: error_handler.h,v $
 * Revision 1.4  2015/01/06 08:57:47  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:36  cding
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
 * Revision 1.6  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.5  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.4  2004/06/28 20:37:42  tahuja
 * Fixed compiler warnings on NetBSD. changed mdelay in check_completion from 1ms to 2ms.
 *
 * Revision 1.3  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.2  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

