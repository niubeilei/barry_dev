////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: hw_lib.c
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
#include "hw_lib.h"
#include "init_cfg.h"
#include "command_que.h"


#if defined(CN1230) || defined(CN1220) || defined(CN1330) || defined(CN1320)
static int
write_twsi(cavium_device *, Uint32);
static int
read_twsi(cavium_device *, Uint32, Uint32 , Uint32 *);
static int
query_ddr_sram(cavium_device *, Uint32 *, Uint32 *);
#endif

void 
enable_request_unit(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0; 
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), &dwval);
 
   cavium_udelay(10);
   dwval = dwval | 0x10000000;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), dwval);
}


void 
disable_request_unit(cavium_device *pkp_dev)
{
   Uint32 dwval;

   dwval = 0; 
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), &dwval);

   cavium_udelay(10); 
   dwval = dwval & 0x0fffffff;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), dwval);
}

void
disable_exec_units_from_mask(cavium_device *pkp_dev, Uint32 mask)
{
   Uint32 dwval = 0;

   read_PKP_register(pkp_dev, (pkp_dev->bar_0 + UNIT_ENABLE), &dwval);

   cavium_udelay(10);
   dwval = dwval & (~mask);

   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + UNIT_ENABLE), dwval);
}




void 
disable_all_exec_units(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0; 
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), &dwval);
 
   cavium_udelay(10);

   dwval = dwval & 0x10000000;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + UNIT_ENABLE), dwval);
 
}

void
enable_exec_units(cavium_device * pkp_dev)
{
   Uint32 dwval=0;

   read_PKP_register(pkp_dev,(pkp_dev->bar_0+UNIT_ENABLE), &dwval);
   dwval = (dwval & 0x10000000) | pkp_dev->uen;
   cavium_udelay(10);
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+UNIT_ENABLE), dwval);
}


void
enable_exec_units_from_mask(cavium_device * pkp_dev, Uint32 mask)
{
   Uint32 dwval=0;

   read_PKP_register(pkp_dev,(pkp_dev->bar_0+UNIT_ENABLE), &dwval);
   cavium_udelay(10);

   dwval |= mask;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0+UNIT_ENABLE), dwval);
}


void 
setup_request_queues(cavium_device * pkp_dev)
{
    volatile  Uint32 queue_base=0;
    volatile Uint32 length=0;
   int i;

    length = pkp_dev->command_queue_max;
 
 /* setup Request Queues */
    for(i = 0; i < MAX_N1_QUEUES; i++) 
    {
       queue_base = (Uint32)cavium_vtophys(
            (Uint32 *)pkp_dev->command_queue_base[i]);
       cavium_dbgprint ("queue_base = %lx\n", queue_base);
       cavium_dbgprint ("queue_length = %lx\n", length);
       cavium_udelay(1000);
       write_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_BASE_LOW +0x20*i),
               queue_base);
       cavium_udelay(1000);
       write_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_BASE_HIGH + 0x20*i), 0);
       cavium_udelay(1000);
       write_PKP_register(pkp_dev,(pkp_dev->bar_1+REQ0_SIZE +0x20*i), length);
       cavium_udelay(1000);
    }
}

void 
enable_data_swap(cavium_device * pkp_dev)
{
   Uint32 dwval=0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), &dwval);
 
   cavium_udelay(10); 

   dwval = dwval | 0x5;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), dwval);
}

void
enable_rnd_entropy(cavium_device *pkp_dev)
{
   Uint32 dwval=0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), &dwval);
 
   cavium_udelay(10); 

   dwval = dwval | 0x200;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), dwval);
}

void
set_PCIX_split_transactions(cavium_device * pkp_dev)
{
   Uint32 dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), &dwval);
   if (dwval & 0x1000) 
   {
      read_PCI_register(pkp_dev, PCIX_SPLIT_TRANSACTION, &dwval);
      dwval=dwval & ~SPLIT_TRANSACTION_MASK;
      write_PCI_register(pkp_dev, PCIX_SPLIT_TRANSACTION, dwval);
   }
   return;
}

void
set_PCI_cache_line(cavium_device * pkp_dev)
{
   write_PCI_register(pkp_dev, PCI_CACHE_LINE, 0x02);
}



Uint32 
get_exec_units(cavium_device * pkp_dev)
{
   Uint32 dwval;
   Uint32 ret=0;

   /* now determine how many exec units are present */
   dwval = 0x7cb;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + DEBUG_REG), dwval);
 
   cavium_udelay(10);

   dwval=0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + DEBUG_REG), &dwval);
 
   /* bits 21:12 of the register are the bit mask for EXECs 27-17 */
   /* right shift it by 11 bits */

   dwval = dwval >> 12;
   pkp_dev->uen = dwval << 17;

   ret = count_set_bits(dwval, 11);
 
   cavium_udelay(10);

   dwval = 0x7cc;
   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + DEBUG_REG), dwval);
 
   cavium_udelay(10);

   dwval=0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + DEBUG_REG), &dwval);
 
   /* bits 27:12 of the register are the bit mask for EXECs 16-0 */
   /* right shift it by 11 bits */
   dwval = dwval >> 12;
   pkp_dev->uen |= dwval;

   ret += count_set_bits(dwval, 17);


   return ret;
}/*int get_exec_units(void)*/


Uint32
get_exec_units_part(cavium_device * pkp_dev)
{
   Uint32 uen,i,core_count,unit;

   uen=0;
   core_count=0;
   unit=0;

   for(i=0;i<MAX_CORES_NITROX; i++)
   {
      unit = pkp_dev->uen & (cavium_pow(2,i));
      if(unit)
      {
         uen |= unit;
         core_count++;
         if(core_count == pkp_dev->exec_units)
            return uen;
      }
   }
   return 0;
}/*get_exec_units_part*/

/* bit mask should have only one bit set */
Uint32
get_unit_id(Uint32 bit_mask)
{
   Uint32 i;
 
   for(i=0; i<MAX_CORES_NITROX; i++)
   {
     if(((bit_mask >> i) & 0x00000001))
       break;
   }

   return i;

}

int 
check_core_mask(Uint32 uen_mask)
{

#if defined(CN1230) || defined(CN1330)
   
   if(uen_mask == CN1230_CORE_MASK_0)
      return 0;

   else if(uen_mask == CN1230_CORE_MASK_1)
      return 0;
   
   else if(uen_mask == CN1230_CORE_MASK_2)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN1230_CORE_MASK_0);
      cavium_print("(2) %08lx\n", CN1230_CORE_MASK_1);
      cavium_print("(3) %08lx\n", CN1230_CORE_MASK_2);
      return 1;
   }

#elif defined(CN1220) || defined(CN1320)
   
   if(uen_mask == CN1220_CORE_MASK_0)
      return 0;

   else if(uen_mask == CN1220_CORE_MASK_1)
      return 0;
   
   else if(uen_mask == CN1220_CORE_MASK_2)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN1220_CORE_MASK_0);
      cavium_print("(2) %08lx\n", CN1220_CORE_MASK_1);
      cavium_print("(3) %08lx\n", CN1220_CORE_MASK_2);
      return 1;
   }


#elif defined(CN1120)

   if(uen_mask == CN1120_CORE_MASK_0)
      return 0;

   else if(uen_mask == CN1120_CORE_MASK_1)
      return 0;
   
   else if(uen_mask == CN1120_CORE_MASK_2)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN1120_CORE_MASK_0);
      cavium_print("(2) %08lx\n", CN1120_CORE_MASK_1);
      cavium_print("(3) %08lx\n", CN1120_CORE_MASK_2);
      return 1;
   }


#elif defined(CN1010)
   
   if(uen_mask == CN1010_CORE_MASK_0)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN1010_CORE_MASK_0);
      return 1;
   }


#elif defined(CN1005)
   
   if(uen_mask == CN1005_CORE_MASK_0)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN1005_CORE_MASK_0);
      return 1;
   }


#elif defined(CN1001)

   if(uen_mask == CN1001_CORE_MASK_0)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN1001_CORE_MASK_0);
      return 1;
   }


#elif defined(CN501)

   if(uen_mask == CN501_CORE_MASK_0)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN501_CORE_MASK_0);
      return 1;
   }

#elif defined(CN505)

   if(uen_mask == CN505_CORE_MASK_0)
      return 0;

   else
   {
      cavium_print("Final core mask %08lx is not one of following:\n", uen_mask);
      cavium_print("(1) %08lx\n", CN505_CORE_MASK_0);
      return 1;
   }
   /* redundant #else. */
#else
#error "Part number (CN1230,CN1220, CN1120, CN1010, CN1005, CN1001, CN501 or CN505) not defined. Please check the Makefile."
#endif
}/* check_core_mask */



void 
set_soft_reset(cavium_device * pkp_dev)
{
   Uint32 dwval;

   dwval = 0;
   read_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), &dwval);

   dwval = dwval | 0x00000020;

   write_PKP_register(pkp_dev,(pkp_dev->bar_0 + COMMAND_STATUS), dwval);

}/*void set_soft_reset(void)*/


int 
count_set_bits(Uint32 value, int bit_count)
{
   int i, count;
   Uint32 dummy;
 
   count = 0;
   dummy = value;
 
   for(i=0; i<bit_count; i++)
   {
     if(((dummy >> i) & 0x00000001))
       count++;
   }

   return count;
}

/* returns the value of x raised by y*/
Uint32 
cavium_pow(Uint32 x, Uint32 y)
{
   Uint32 i;
   Uint32 ret=x;
   if(y == 0)
      return 1;
   for(i=0; i<y-1; i++)
      ret = ret*x;

   return ret;
}

Uint32  
get_first_available_core(Uint32 max, Uint32 mask)
{
   Uint32 i;

    for(i=0; i < max; i++) 
    {
       if(((mask >> i) & 0x00000001))
                break;
    } 
 
    return cavium_pow(2, i);
}



int do_soft_reset(cavium_device *pkp_dev)
{
   Uint32 i;

   set_soft_reset(pkp_dev);
   cavium_udelay(100);

   for (i = 0; i < MAX_N1_QUEUES; i++) 
   {
      cavium_spin_lock_destroy(&(pkp_dev->command_queue_lock[i]));
   }
   for (i = 0; i < MAX_N1_QUEUES; i++) 
   {
      cleanup_command_queue(pkp_dev, i);
   }
   for (i = 0; i < MAX_N1_QUEUES; i++) 
   {
      cavium_spin_lock_init(&(pkp_dev->command_queue_lock[i]));
   }
   for (i = 0; i < MAX_N1_QUEUES; i++) 
   {
      init_command_queue(pkp_dev, i);
   }

   if(pkp_init_board(pkp_dev))
      return 1;

   if(pkp_dev->dram_present)
                enable_local_ddr(pkp_dev);

   if(do_init(pkp_dev))
      return 1;
   return 0;
}

#if defined(CN1230) || defined(CN1220) || defined(CN1330) || defined(CN1320)

static int
write_twsi(cavium_device *pkp_dev, Uint32 value)
{
   Uint32 dwval = 0;
   
   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + HOST_TO_PSE_DATA), value);

   do 
   {
           read_PKP_register(pkp_dev, (pkp_dev->bar_0 + PSE_TO_HOST_DATA), &dwval);
   } while (!(dwval & 0x8000));

   if(dwval != 0xc3ff)
      return 1;
   else
      return 0;
}


int
init_twsi(cavium_device *pkp_dev)
{
    Uint32 dwval;
 
   /* initialize MI2C clock divisor */
   dwval = 0xb0000300;
   if(write_twsi(pkp_dev, dwval))
           return ERR_INIT_TWSI_FAILURE;

   /* initialize PSE clock divisor */
   dwval = 0xa0000041;
   if(write_twsi(pkp_dev, dwval))
           return ERR_INIT_TWSI_FAILURE;

   /* enable master mode */
   dwval = 0xa8000001;
   if(write_twsi(pkp_dev, dwval))
           return ERR_INIT_TWSI_FAILURE;

   /* enable twsi bus and interrupts */
   dwval = 0xb00002c4;
   if(write_twsi(pkp_dev, dwval))
      return ERR_INIT_TWSI_FAILURE;

   return 0;
}

static int
read_twsi(cavium_device *pkp_dev, Uint32 dev_addr, Uint32 int_addr, 
     Uint32 *value)
{
   int return_code;
   Uint32 dwval, start;

   dwval = 0x8c500000 | (dev_addr << 16) | (int_addr << 8);
   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + HOST_TO_PSE_DATA), dwval);

   dwval = 0;
   start = cavium_jiffies;

   do {
      read_PKP_register(pkp_dev, (pkp_dev->bar_0 + PSE_TO_HOST_DATA), &dwval);

#ifdef N1_TIMER_ROLLOVER
      if(cavium_time_after(cavium_jiffies,start+10))
#else
      if(cavium_jiffies > (start+10))
#endif
         return 1;
   } while (!(dwval & 0x8000));

   if (dwval & 0x4000) {
      *value = dwval & 0xff;
       return_code = 0;
   } else {
      *value = 0;
      return_code = dwval & 0xff;
    }

   return return_code;
}


static int
query_ddr_sram(cavium_device *pkp_dev, Uint32 *local_mem_size, Uint32 *cmc_ctl_val)
{
   int i,j;
   Uint32 dwval = 0;
   Uint32 dev_addr;
   Uint32 timing_config = 0;
   Uint32 silo_latency = 2;
   Uint32 cas_latency = 0;
   Uint32 refresh_interval = 0;
   Uint32 address_bnk_lsb = 0;
   Uint32 address_row_lsb = 0;
   Uint32 enable_ecc = 0;
   Uint16 module_width;
   unsigned char eeprom_data[64];

   int ddr_flag = 0;

   *local_mem_size = 0;
   *cmc_ctl_val = 0;

    /* 
     * detect eeprom 
     * EEPROMs always have address 1010xxx
     * read first byte of eeprom for first device address 
     * assume it is the valid length of data if it is >= 64, 
     * read first 62 bytes and checksum if checksum matches, 
     * we have found the DIMM EEPROM if checksum fails, 
     * move to next device address 
     */
   for (i = 0; i < 8; i++) 
   {
      dev_addr = 0x50 | i;
      if (!read_twsi(pkp_dev, dev_addr, 0, &dwval)) 
      {
         eeprom_data[0] = dwval & 0xff;
         if (eeprom_data[0] >= 64) 
         {
            for (j = 1; j < 64; j++) 
            {
               if (!read_twsi(pkp_dev, dev_addr, j,&dwval)) 
               {
                  eeprom_data[j] = dwval & 0xff;
               } 
               else 
               {
                  break;
               }
            }
            if (j == 64) 
            {
               unsigned char accum = 0;

               for (j = 0; j < 63; j++) 
               {
                  accum += eeprom_data[j];
               }
               if (accum == eeprom_data[63])
                  ddr_flag = 1;
            }
         }
      }
      if (ddr_flag)
         break;
   }

   if (!ddr_flag) 
   {
      return ERR_DDR_NO_EEPROM_PRESENT;
   }

   /* verify memory type is SDRAM DDR */
   if (eeprom_data[2] != 7) 
   {
      return ERR_DDR_MEMORY_NOT_SRAM_DDR;
   }

   /* number of column addresses */
   if ((eeprom_data[4] >= 9) && (eeprom_data[4] <= 13)) 
   {
      address_row_lsb = eeprom_data[4] - 9;
   } 
   else 
   {
      return ERR_DDR_UNSUPPORTED_NUM_COL_ADDR;
   }

   /* number of row addresses */
   i = (int)(eeprom_data[3] + eeprom_data[4] - 21);
   if ((i < 0) && (i > 5)) 
   {
      return ERR_DDR_UNSUPPORTED_NUM_ROW_ADDR;
   } 
   else 
   {
      address_bnk_lsb = i;
   }

   /* number of banks on DIMM */
   if (eeprom_data[5] != 1) 
   {
      return ERR_DDR_MORE_THAN_1_PHYS_BANK;
   }

   /* module data width */
   module_width = (((Uint16)eeprom_data[7] << 8) | 
         (Uint16)eeprom_data[6]);

   if ((module_width != 64) && (module_width != 72)) 
   {
      return ERR_DDR_UNSUPPORTED_MODULE_DATA_WIDTH;
   }

   /* voltage interface level */
   if (eeprom_data[8] != 4) 
   {
      return ERR_DDR_UNSUPPORTED_VOLT_INTERFACE_LEVEL;
   }

   /* module configuration: ECC or not */
   if (eeprom_data[11] == 2)
      enable_ecc = 1;
   else if (eeprom_data[11] == 0)
      enable_ecc = 0;
   else 
   {
      return ERR_DDR_UNSUPPORTED_MODULE_CONFIG;
   }

   /* refresh rate/type -- must have self refresh */
   if (eeprom_data[12] & 0x80)
      eeprom_data[12] &= 0x7f;

   switch (eeprom_data[12]) 
   {
      case 0: case 1: case 2: case 3: case 4: case 5:
           break;

      default:
           return ERR_DDR_UNSUPPORTED_REFRESH_CLOCK;
   }

   refresh_interval = 5;

    /* primary SDRAM width */
   if ((eeprom_data[13] != 4) && (eeprom_data[13] != 8) && 
       (eeprom_data[13] != 16)) 
   {
      return ERR_DDR_UNSUPPORTED_PRIMARY_SDRAM_WIDTH;
   }

   /* burst lengths supported */
   if (!(eeprom_data[16] & 0x2)) 
   {
      return ERR_DDR_REQUIRE_BURST_LENGTH_2;
   }

   /* number of banks on SDRAM device */
   if (eeprom_data[17] != 4) 
   {
      return ERR_DDR_REQUIRE_4_DEV_BANKS;
   }

   /* CAS latency */
   if (eeprom_data[18] &0x4) 
   {
      cas_latency = 0;
   } else if (eeprom_data[18] &0x8) 
   {
      cas_latency = 1;
   } 
   else 
   {
      return ERR_DDR_UNSUPPORTED_CAS_LATENCY;
   }

   timing_config = 2;
   
   /* module bank density */
   switch (eeprom_data[31]) 
   {
      case 0:
         *local_mem_size = 0x40000000;
         break;
      case 4:
         *local_mem_size = 0x1000000;
         break;
      case 8:
         *local_mem_size = 0x2000000;
         break;
      case 16:
         *local_mem_size = 0x4000000;
         break;
      case 32:
         *local_mem_size = 0x8000000;
         break;
      case 64:
         *local_mem_size = 0x10000000;
         break;
      case 128:
         *local_mem_size = 0x20000000;
         break;
      default:
         return ERR_DDR_UNSUPPORTED_MODULE_BANK_DENSITY;
   }
   cavium_print("Local Mem size %lx\n", *local_mem_size);

   /* number of banks */
   switch (eeprom_data[5]) 
   {
      case 0:
         return ERR_DDR_UNSUPPORTED_MODULE_BANK_DENSITY;
      default:
         cavium_print("Number of banks %x\n",eeprom_data[5]);
         *local_mem_size *= eeprom_data[5];
         break;
   }

   /* write config to CMC register */
   dwval=  (timing_config    << 16) | 
      (silo_latency     << 14) |
      (cas_latency      << 11) |
             (refresh_interval <<  8) |
             (address_bnk_lsb  <<  5) |
             (address_row_lsb  <<  2) | 
      (enable_ecc       <<  1);

   *cmc_ctl_val = dwval;

   return 0;
}
#endif

/*
 * Checks the presence of local ddr memory.
 */
void
check_dram(cavium_device *pkp_dev)
{
   int err;
   Uint32 local_mem_size=0, cmc_ctl_val=0;

#if defined(CN1230) || defined(CN1220) || defined(CN1330) || defined(CN1320)
   err = query_ddr_sram(pkp_dev, &local_mem_size, &cmc_ctl_val);
#else
   err=1;
#endif

   if (err) 
   {
      cavium_print( "NO DDR on the chip \n");
      cavium_print( "Resorting to Context memory on Host.\n");
      pkp_dev->dram_present   = 0;
   } 
   else 
   {
      cavium_print("DDR size : 0x%08lx\n", local_mem_size);
      cavium_dbgprint("CMC_CTL_VAL = %08lx\n", cmc_ctl_val);
      pkp_dev->dram_present   = 1;
      pkp_dev->dram_max   = local_mem_size - CONTEXT_OFFSET;
      pkp_dev->dram_base   = DRAM_BASE + CONTEXT_OFFSET; 
               /* leave lower 4MB for key storage.*/
      pkp_dev->cmc_ctl_val   = cmc_ctl_val; 
   }

   return;
}

void
enable_local_ddr(cavium_device *pkp_dev)
{
   Uint32 dwval, cas_incr;

   dwval = pkp_dev->cmc_ctl_val;
   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + CMC_CTL_REG), dwval);

   cavium_udelay(500);

   dwval |= 1;
   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + CMC_CTL_REG), dwval);

   cavium_mdelay(100);

   cas_incr = (dwval >> 11) & 7;
   cas_incr += DRAM_CAS_LATENCY_INCR;

   dwval = (dwval & 0xffff87ff)|((Uint32)cas_incr<<11);
   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + CMC_CTL_REG), dwval);

   cavium_udelay(100);

   /* set context memory present bit in command/status register */
   dwval=0;
   read_PKP_register(pkp_dev, (pkp_dev->bar_0 + COMMAND_STATUS), &dwval);

   cavium_udelay(50);
   dwval = dwval | 0x400;
   write_PKP_register(pkp_dev, (pkp_dev->bar_0 + COMMAND_STATUS), dwval);

}
#ifdef NPLUS
inline unsigned int get_core_mask(cavium_device *pdev, int ucode_idx)
{
   unsigned int core_mask = 0;
   Uint8 id = pdev->microcode[ucode_idx].core_id;
   while(id != (Uint8)-1)
   {
      core_mask |= 1 << id;
      id = pdev->cores[id].next_id;
   }
   return core_mask;
} 

Uint32 
get_enabled_units(cavium_device *pdev)
{
   Uint32 ret=0;

   /* now determine how many exec units are present */
   cavium_udelay(10);

   read_PKP_register(pdev, (pdev->bar_0 + UNIT_ENABLE), &ret);
   ret &= (0x0fffffff);

   cavium_dbgprint("UEN mask = %08ld\n", ret);

   return ret;
}/*int get_enabled_units()*/


void
cycle_exec_units_from_mask(cavium_device *pdev, Uint32 mask)
{
   Uint32 dwval = 0;
   
   cavium_spin_lock_softirqsave(&pdev->uenreg_lock);
   read_PKP_register(pdev, (pdev->bar_0 + UNIT_ENABLE), &dwval);

   dwval = dwval & (~mask);
   write_PKP_register(pdev, (pdev->bar_0 + UNIT_ENABLE), dwval);

   cavium_udelay(50);
 
   dwval = dwval | mask;   
   write_PKP_register(pdev, (pdev->bar_0 + UNIT_ENABLE), dwval);

   cavium_spin_unlock_softirqrestore(&pdev->uenreg_lock);
}

/*
 * returns 0 if it finds atleast one core pair
 * otherwise returns non-zero
 */

int get_core_pair(cavium_device *pdev, Uint32 mask)
{
   Uint32 tmp_core_mask = 0x03;
   int loop, i;
   
   loop = sizeof(mask)/2;
   for(i=0;i<loop;i++)
   {
      if(i!=0)
         tmp_core_mask = tmp_core_mask << 2;

      if( ((mask&tmp_core_mask)>>(i*2)) == 0x03)
         return 0;
   }
   return 1;
}

#endif /*NPLUS*/

/*
 * $Id: hw_lib.c,v 1.4 2015/01/06 08:57:47 andy Exp $
 * $Log: hw_lib.c,v $
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
 * Revision 1.21  2005/10/24 06:52:58  kanantha
 * - Fixed RHEL4 warnings
 *
 * Revision 1.20  2005/10/13 09:24:02  ksnaren
 * fixed compile warnings
 *
 * Revision 1.19  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.18  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.17  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.16  2005/08/31 18:10:30  bimran
 * Fixed several warnings.
 * Fixed the corerct use of ALIGNMENT and related macros.
 *
 * Revision 1.15  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.14  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.13  2005/02/04 00:12:27  tsingh
 * added 1330 and 1320
 *
 * Revision 1.12  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.11  2005/01/28 22:18:06  tsingh
 * Added support for HT part numbers.
 *
 * Revision 1.10  2005/01/26 20:34:56  bimran
 * Added NPLUS specific functions to check for available core pairs for Modexp operation.
 *
 * Revision 1.9  2004/10/06 19:31:54  tsingh
 * fixed some potential issues
 *
 * Revision 1.8  2004/06/23 20:08:38  bimran
 * compiler warnings on NetBSD.
 *
 * Revision 1.6  2004/05/10 21:32:26  bimran
 * query_ddr_sram is only called for supporting part numbers.
 *
 * Revision 1.5  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.4  2004/04/26 19:04:55  bimran
 * Added 505 support.
 *
 * Revision 1.3  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.2  2004/04/20 02:25:12  bimran
 * Fixed check_dram to use context_max passed in cavium_init() instead of defining its own DRAM_MAX macro.
 *
 * Revision 1.1  2004/04/15 22:40:49  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

