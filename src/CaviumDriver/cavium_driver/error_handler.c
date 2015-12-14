////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: error_handler.c
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
#include "hw_lib.h"




int check_hard_reset_group(cavium_device * pkp_dev)
{
 if (check_PCI_master_abort_write(pkp_dev))
    return ERR_PCI_MASTER_ABORT_WRITE;

 if (check_PCI_target_abort_write(pkp_dev))
    return ERR_PCI_TARGET_ABORT_WRITE;

 if(check_PCI_master_retry_timeout_write(pkp_dev))
    return ERR_PCI_MASTER_RETRY_TIMEOUT_WRITE;

 if(check_outbound_fifo_cmd(pkp_dev))
    return ERR_OUTBOUND_FIFO_CMD;

 if(check_key_memory_parity(pkp_dev))
    return ERR_KEY_MEMORY_PARITY;

 return 0;
}


int check_soft_reset_group(cavium_device * pkp_dev)
{
 if(check_PCI_master_abort_req_read(pkp_dev))
    return ERR_PCI_MASTER_ABORT_REQ_READ;

 if(check_PCI_target_abort_req_read(pkp_dev))
    return ERR_PCI_TARGET_ABORT_REQ_READ;

 if(check_PCI_master_retry_timeout_req_read(pkp_dev))
    return ERR_PCI_MASTER_RETRY_TIMEOUT_REQ_READ;
 
 if(check_PCI_master_data_parity_req_read(pkp_dev))
    return ERR_PCI_MASTER_DATA_PARITY_REQ_READ;

 if(check_req_counter_overflow(pkp_dev))
    return ERR_REQ_COUNTER_OVERFLOW;

 return 0;

}


int check_exec_reset_group(cavium_device * pkp_dev)
{
 if(check_EXEC_reg_file_parity(pkp_dev))
   return ERR_EXEC_REG_FILE_PARITY;

 if(check_EXEC_ucode_parity(pkp_dev))
   return ERR_EXEC_UCODE_PARITY;

 return 0;
}



int check_seq_no_group(cavium_device * pkp_dev)
{
 if(check_PCI_master_abort_EXEC_read(pkp_dev))
   return ERR_PCI_MASTER_ABORT_EXEC_READ;

 if(check_PCI_target_abort_EXEC_read(pkp_dev))
   return ERR_PCI_TARGET_ABORT_EXEC_READ;

 if (check_PCI_master_retry_timeout_EXEC_read(pkp_dev))
   return ERR_PCI_MASTER_RETRY_TIMOUT_EXEC_READ;

 if(check_PCI_master_data_parity_EXEC_read(pkp_dev))
   return ERR_PCI_MASTER_DATA_PARITY_EXEC_READ;

 if(check_EXEC_general(pkp_dev))
   return ERR_EXEC_GENERAL;

 if(check_CMC_double_bit(pkp_dev))
   return ERR_CMC_DOUBLE_BIT;

 if(check_CMC_single_bit(pkp_dev))
   return ERR_CMC_SINGLE_BIT;

 return 0;
}

/*
 * Functions to check errors
 */

/* hard reset group */
int check_PCI_master_abort_write(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

   if(dwval & 0x20000)
   {
      dwval = 0;
      read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval); 

      if(dwval & 0x20000000)
      {
         cavium_print(("PCI Master Abort on Write Error.\n"));
         return 1;
      }
   }
   return 0;
}



int check_PCI_target_abort_write(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev, (pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

   if(dwval & 0x10000)
   {
      dwval = 0;
      read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval); 

      if((dwval & 0x50000100) == 0x50000100 )
      {
         cavium_print("PCI Target Abort on Write Error.\n");
         return 1;
      }
   }
   return 0;
}



int check_PCI_master_retry_timeout_write(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

   if(dwval & 0x40000)
   {
      dwval = 0;
      read_PCI_register(pkp_dev, PCI_CONFIG_58, &dwval); 

      if(dwval & 0x20000)
      {
         cavium_print("PCI Master Retry Timeout on Write Error.\n");
         return 1;
      }
   }

   return 0;
}



int check_outbound_fifo_cmd(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

   if(dwval & 0x80000)
   {
      cavium_print("Outbound FIFO command Error.\n");
      return 1;
   }

   return 0;
}


int check_key_memory_parity(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval); 

   if(dwval & 0x4)
   {
      cavium_print("Key memory parity error.\n");
      return 1;
   }

   return 0;
}



/* soft reset group */
int check_PCI_master_abort_req_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval); 

   if(dwval & 0x1E0)
   {
      dwval = 0;
      read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

      if(dwval & 0x2)
      {
         dwval = 0;
         read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval); 

         if(dwval & 0x20000000)
         {
            cavium_print("PCI Master Abort on Request Queue read.\n");
            return 1;
         }
      }
   }

 return 0;
}



int check_PCI_target_abort_req_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval); 

   if(dwval & 0x1E0)
   {
      dwval = 0;

      read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

      if(dwval & 0x1)
      {
         dwval = 0;
         read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval); 

         if((dwval & 0x50000100) == 0x50000100)
         {
            cavium_print("PCI Target Abort on Request Queue read.\n");
            return 1;
         }
      }
   }
   return 0;
}


int check_PCI_master_retry_timeout_req_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval); 

   if(dwval & 0x1E0)
   {
      dwval = 0;
      read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval); 

      if(dwval & 0x4)
      {
         dwval = 0;
         read_PCI_register(pkp_dev, PCI_CONFIG_58, &dwval); 

         if(dwval & 0x20000)
         {
            cavium_print("PCI Master retry timeout on Request Queue read.\n");
            return 1;
         }
      }
   }

   return 0;
}



int check_PCI_master_data_parity_req_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval); 

   if(dwval & 0x10000)
   {
      dwval = 0;
      read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval);

      if((dwval & 0x81000040) == 0x81000040)  
      {
         dwval=0;
         read_PCI_register(pkp_dev, PCI_CONFIG_4C, &dwval);
         
         if((dwval & 0x5000) == 0x5000)
         {
            dwval=0;
            read_PKP_register(pkp_dev,(pkp_dev->bar_0 + PCI_ERR_REG), &dwval);

            if(dwval & 0x8)
            {
               cavium_print("PCI Master Data parity error on Req queue read.\n");
               return 1;
            }
         } 
      }
   }
   return 0;
}



int check_req_counter_overflow(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval);

   if(dwval & 0x1E00)  /* if any of the bits are set */
   {
      cavium_print("Request counter overflow.\n");
      return 1;
   }

   return 0;
}



/* reset exec unit group*/
int check_EXEC_reg_file_parity(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval);

   if(dwval & 0x1)
   {
      cavium_print("EXEC register file parity error.\n");
      return 1;
   }

   return 0;
}




int check_EXEC_ucode_parity(cavium_device * pkp_dev)
{
 Uint32 dwval;

 dwval = 0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0 + ISR_REG), &dwval);
 
 if(dwval & 0x2)
   {
     cavium_print("EXEC ucode parity error.\n");
     return 1;
   }
 return 0;
}




/* seq no group */
int check_PCI_master_abort_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval);

   if(dwval & 0x20000000)
   {
      dwval = 0;
      read_PKP_register(pkp_dev,(pkp_dev->bar_0+PCI_ERR_REG), &dwval);

      if(dwval & 0x2)
      {
         cavium_print("PCI Master Abort on EXEC read.\n");
         return 1;
      }
   }
   return 0;
}




int check_PCI_target_abort_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval);

   if( (dwval & 0x50000100) == 0x50000100 )
   {
      dwval = 0;
      read_PKP_register(pkp_dev,(pkp_dev->bar_0+PCI_ERR_REG), &dwval);

      if(dwval & 0x1)
      {
         cavium_print("PCI Target abort on EXEC read.\n");
         return 1;
      }
   }
   return 0;
}





int check_PCI_master_retry_timeout_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PCI_register(pkp_dev, PCI_CONFIG_58, &dwval);

   if(dwval & 0x20000)
   {
      dwval = 0;
      read_PKP_register(pkp_dev,(pkp_dev->bar_0+PCI_ERR_REG), &dwval);

      if(dwval & 0x4)
      {
         cavium_print("PCI Master retry timeout exec EXEC read.\n");
         return 1;
      }
   }

   return 0;
}





int check_PCI_master_data_parity_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PCI_register(pkp_dev, PCI_CONFIG_04, &dwval);

   if((dwval & 0x81000040) == 0x81000040)
   {
      dwval = 0;
      read_PKP_register(pkp_dev,(pkp_dev->bar_0+PCI_ERR_REG), &dwval);

      if(dwval & 0x8)
      {
         dwval = 0;
         read_PCI_register(pkp_dev, PCI_CONFIG_4C, &dwval);

         if((dwval & 0x5000) == 0x5000)
         {
            cavium_print("PCI Master read data parity for exec.\n");
            return 1;
         }
      }
   }

   return 0;
}





int check_EXEC_general(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), &dwval);

   if(dwval & 0x8)
   {
      cavium_print("pkp: EXEC general error\n");
      return 1;
   }
   else 
      return 0;
}



int check_CMC_double_bit(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), &dwval);

   if(dwval & 0x8000)
   {
      cavium_print("pkp: CMC double bit error.\n");
      dump_pkp_registers(pkp_dev);
      return 1;
   }
   else 
      return 0;

}




int check_CMC_single_bit(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), &dwval);

   if(dwval & 0x4000)
   {
      cavium_print("pkp: CMC single bit error.\n");
      return 1;
   }
   else 
      return 0;
}




/*
 * Functions to clear error bits
 */


/*
 * hard reset group
 */
void clear_PCI_master_abort_write(cavium_device * pkp_dev)
{
 /* recovery not recommended */
}

void clear_PCI_target_abort_write(cavium_device * pkp_dev)
{
  /* recovery not recommended */
}

void clear_PCI_master_retry_timeout_write(cavium_device * pkp_dev)
{
  /* recovery not recommended */
}

void clear_outbound_fifo_cmd(cavium_device * pkp_dev)
{
  /* recovery not recommended */
}

void clear_key_memory_parity(cavium_device * pkp_dev)
{
  /* recovery not recommended */
}



/* 
 * Soft rseset group 
 */

void clear_PCI_master_abort_req_read(cavium_device * pkp_dev)
{
 /* bits would be cleared automatically during a soft reset */
}

void clear_PCI_target_abort_req_read(cavium_device * pkp_dev)
{
  /* bits would be cleared automatically during a soft reset */
}

void clear_PCI_master_retry_timeout_req_read(cavium_device * pkp_dev)
{
  /* bits would be cleared automatically during a soft reset */
}

void clear_PCI_master_data_parity_req_read(cavium_device * pkp_dev)
{
  /* bits would be cleared automatically during a soft reset */
}

void clear_req_counter_overflow(cavium_device * pkp_dev)
{
  /* bits would be cleared automatically during a soft reset */
}



/* reset exec unit group */
void clear_EXEC_reg_file_parity(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0x01;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), dwval);
}

void clear_EXEC_ucode_parity(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0x02;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), dwval);
}

/* seq number */
void clear_PCI_master_abort_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0x20000000;
   write_PCI_register(pkp_dev, PCI_CONFIG_04, dwval); 
   
   dwval = 0x2;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 +PCI_ERR_REG), dwval);
}



void clear_PCI_target_abort_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0x50000000;
   write_PCI_register(pkp_dev, PCI_CONFIG_04, dwval); 
   
   dwval = 0x1;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 +PCI_ERR_REG), dwval);
}


void clear_PCI_master_retry_timeout_EXEC_read(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0x20000;
   write_PCI_register(pkp_dev, PCI_CONFIG_58, dwval); 
   
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 +PCI_ERR_REG), 0x4);
}


void clear_PCI_master_data_parity_EXEC_read(cavium_device * pkp_dev)
{
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG),0x10000);

   write_PCI_register(pkp_dev, PCI_CONFIG_04, 0x81000040); 

   write_PCI_register(pkp_dev, PCI_CONFIG_4C, 0x5000); 

   write_PKP_register(pkp_dev,(pkp_dev->bar_0+PCI_ERR_REG), 0x8);
}



void clear_EXEC_general(cavium_device * pkp_dev)
{
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), 0x8);
}

void clear_CMC_double_bit(cavium_device * pkp_dev)
{
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), 0x8000);
}

void clear_CMC_single_bit(cavium_device * pkp_dev)
{
 write_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), 0x4000);
}




/*
 * Functions which actually take actions
 */

int handle_hard_reset(cavium_device * pkp_dev)
{
   pkp_dev->enable=0;
   cavium_print("PANIC PANIC PANIC PANIC PANIC PANIC PANIC PANIC PANIC PANIC\n");
   cavium_print("Severe Nitrox/System Failure.\n");
   cavium_print("PANIC PANIC PANIC PANIC PANIC PANIC PANIC PANIC PANIC PANIC\n");
   return 1;
}


int handle_soft_reset(cavium_device * pkp_dev)
{
 int ret;

 /* Disable the device */
 pkp_dev->enable=0;

 /* Enable the device after soft reset */
 ret = do_soft_reset(pkp_dev);

 if(ret == 0)
    pkp_dev->enable = 1;
 else
 {
    cavium_print("Cavium soft reset failed.\n");
    cavium_print("Cavium device has been disabled due to a system error.\n");
    cavium_print("Please restart the system!!!\n");
 }

 return ret;
}




int handle_exec_reset(cavium_device * pkp_dev)
{
 Uint32 fexec, uen;

 cavium_print("Disable faulty EXEC unit\n");
 
 /* first find out failing exec unit */
 fexec=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0 + FAILING_EXEC_REG), &fexec);
 
cavium_print("Failing EXEC register %08lx\n", fexec);

 /* now disable the failing exec unit */
 uen=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), &uen);
 cavium_print("Unit enable register %08lx\n", uen);

 uen = uen ^ fexec;

 cavium_print("Writing Unit Enable %08lx\n", uen);

 write_PKP_register(pkp_dev,(pkp_dev->bar_0+UNIT_ENABLE), uen);

 return 1;
}




int handle_seq_no_error(cavium_device * pkp_dev)
{
 /*   This error is handled thru timeout.
   The request causing the error will eventually
   timeout. The host program can retry the request.*/
 return 1;
}


void clear_error(cavium_device * pkp_dev, int error_code)
{

 switch(error_code)
 { 
 case ERR_PCI_MASTER_ABORT_WRITE:
   clear_PCI_master_abort_write(pkp_dev);
   break;

 case ERR_PCI_TARGET_ABORT_WRITE:
   clear_PCI_target_abort_write(pkp_dev);
   break;

 case ERR_PCI_MASTER_RETRY_TIMEOUT_WRITE:
   clear_PCI_master_retry_timeout_write(pkp_dev);
   break;

 case ERR_OUTBOUND_FIFO_CMD:
   clear_outbound_fifo_cmd(pkp_dev);
   break;

 case ERR_KEY_MEMORY_PARITY:
   clear_key_memory_parity(pkp_dev);
   break;

 /*soft reset group */
 case ERR_PCI_MASTER_ABORT_REQ_READ:
   clear_PCI_master_abort_req_read(pkp_dev);
   break;

 case ERR_PCI_TARGET_ABORT_REQ_READ:
   clear_PCI_target_abort_req_read(pkp_dev);
   break;

 case ERR_PCI_MASTER_RETRY_TIMEOUT_REQ_READ:
   clear_PCI_master_retry_timeout_req_read(pkp_dev);
   break;

 case ERR_PCI_MASTER_DATA_PARITY_REQ_READ:
   clear_PCI_master_data_parity_req_read(pkp_dev);
   break;

 case ERR_REQ_COUNTER_OVERFLOW:
   clear_req_counter_overflow(pkp_dev);
   break;

 /*EXEC reset group */
 case ERR_EXEC_REG_FILE_PARITY:
   clear_EXEC_reg_file_parity(pkp_dev);
   break;

 case ERR_EXEC_UCODE_PARITY:
   clear_EXEC_ucode_parity(pkp_dev);
   break;

 /*seq number based errors */
 case ERR_PCI_MASTER_ABORT_EXEC_READ:
   clear_PCI_master_abort_EXEC_read(pkp_dev);
   break;

 case ERR_PCI_TARGET_ABORT_EXEC_READ:
   clear_PCI_target_abort_EXEC_read(pkp_dev);
   break;

 case ERR_PCI_MASTER_RETRY_TIMOUT_EXEC_READ:
   clear_PCI_master_retry_timeout_EXEC_read(pkp_dev);
   break;

 case ERR_PCI_MASTER_DATA_PARITY_EXEC_READ:
   clear_PCI_master_data_parity_EXEC_read(pkp_dev);
   break;

 case ERR_EXEC_GENERAL:
    clear_EXEC_general(pkp_dev);
   break;

 case ERR_CMC_DOUBLE_BIT:
   clear_CMC_double_bit(pkp_dev);
   break;

 case ERR_CMC_SINGLE_BIT:
   clear_CMC_single_bit(pkp_dev);
   break;

 default:
   cavium_print("clear error: unknown error.\n");
   break;
 }
}





void dump_pkp_registers(cavium_device * pkp_dev)
{
 Uint32 dwval;
 /* dumps all registers */
 cavium_print("Dumping PKP registers:\n");

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+COMMAND_STATUS), &dwval);
 cavium_print("COMMAND_STATUS %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+UNIT_ENABLE), &dwval);
 cavium_print("UNIT_ENABLE %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+IMR_REG), &dwval);
 cavium_print("INTERRUPT MASK %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+ISR_REG), &dwval);
 cavium_print("INTERRUPT STATUS %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+FAILING_SEQ_REG), &dwval);
 cavium_print("FAILING_SEQ_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+FAILING_EXEC_REG), &dwval);
 cavium_print("FAILING_EXEC_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+ECH_STAT_COUNTER_HIGH_REG), &dwval);
 cavium_print("ECH_STAT_COUNTER_HIGH_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+ECH_STAT_COUNTER_LOW_REG), &dwval);
 cavium_print("ECH_STAT_COUNTER_LOW_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+EPC_STAT_COUNTER_HIGH_REG), &dwval);
 cavium_print("EPC_STAT_COUNTER_HIGH_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+EPC_STAT_COUNTER_LOW_REG), &dwval);
 cavium_print("EPC_STAT_COUNTER_LOW_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+PMLT_STAT_COUNTER_LOW_REG), &dwval);
 cavium_print("PMT/MLT_STAT_COUNTER_LOW_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+PMLT_STAT_COUNTER_HIGH_REG), &dwval);
 cavium_print("PMT/LMT_STAT_COUNTER_HIGH_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+CLK_STAT_COUNTER_HIGH_REG), &dwval);
 cavium_print("CLK_STAT_COUNTER_HIGH_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+CLK_STAT_COUNTER_LOW_REG), &dwval);
 cavium_print("CLK_STAT_COUNTER_LOW_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_0+PCI_ERR_REG), &dwval);
 cavium_print("PCI_ERR_REG %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_BASE_HIGH), &dwval);
 cavium_print("REQ0_BASE_HIGH %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_BASE_LOW), &dwval);
 cavium_print("REQ0_BASE_LOW %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_SIZE), &dwval);
 cavium_print("REQ0_SIZE %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ1_BASE_HIGH), &dwval);
 cavium_print("REQ1_BASE_HIGH %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ1_BASE_LOW), &dwval);
 cavium_print("REQ1_BASE_LOW %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ1_SIZE), &dwval);
 cavium_print("REQ1_SIZE %08lx\n", dwval);


 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_DOOR_BELL), &dwval);
 cavium_print("REQ0_DOOR_BELL %08lx\n", dwval);

 cavium_udelay(50);
 dwval=0;
 read_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ1_DOOR_BELL), &dwval);
 cavium_print("REQ1_DOOR_BELL %08lx\n", dwval);
 
}/* dump_pkp_registers*/


/*
 * Translate chip error codes to driver error codes.
 */
int check_completion_code(volatile Uint64 *p)
{
 int ret= ((Uint8)((*p) >> COMPLETION_CODE_SHIFT));

#ifdef INTERRUPT_ON_COMP
 /* Bit 6 indicates that an interrupt was generated for this request */
 ret = ret & (~0x40);
#endif

 if(ret)
    ret |= 0x40000000;

 return ret;
}/*check_completion_code*/

/*
 * $Id: error_handler.c,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: error_handler.c,v $
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
 * Revision 1.11  2005/10/13 09:23:04  ksnaren
 * fixed compile errors for windows xp
 *
 * Revision 1.10  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.9  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.8  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.7  2004/07/21 23:24:41  bimran
 * Fixed MC2 completion code issues on big endian systems.
 *
 * Revision 1.6  2004/06/28 20:37:42  tahuja
 * Fixed compiler warnings on NetBSD. changed mdelay in check_completion from 1ms to 2ms.
 *
 * Revision 1.5  2004/06/03 21:22:56  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.4  2004/05/10 20:45:26  bimran
 * Fixed condition code checking in INTERRUPT mode.
 *
 * Revision 1.3  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.2  2004/04/22 02:48:25  bimran
 * Fixed check_completion_code functions to just return the error code and try not to map that to any enymerated error code.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

