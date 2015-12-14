////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium.h
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


#ifndef _CAVIUM_H_
#define _CAVIUM_H_

#include "cavium_sysdep.h"
#include "cavium_endian.h"
#include "cavium_list.h"

/****************************************************************************/
/* PKP Register Offsets                                                     */
/****************************************************************************/
/* define register offsets here */


#define BAR_0            0x10
#define BAR_1            0x18
#define BAR_2            0x20
#define BAR_3            0x28

#define PCI_CONFIG_04         0x4
#define PCI_CONFIG_58         0X58
#define PCI_CONFIG_4C         0x4C
#define PCI_CACHE_LINE         0x0C
#define PCIX_SPLIT_TRANSACTION      0xE0
#ifndef PCI_INTERRUPT_LINE
#define PCI_INTERRUPT_LINE      0x3C
#endif

/*BAR 0*/
#define COMMAND_STATUS         0x00
#define UNIT_ENABLE         0x10
#define IMR_REG            0X20
#define ISR_REG            0x28 
#define FAILING_SEQ_REG         0x30
#define FAILING_EXEC_REG      0x38
#define ECH_STAT_COUNTER_HIGH_REG   0x88
#define ECH_STAT_COUNTER_LOW_REG   0x90
#define EPC_STAT_COUNTER_HIGH_REG   0x98
#define EPC_STAT_COUNTER_LOW_REG   0xA0
#define PMLT_STAT_COUNTER_LOW_REG   0xA8
#define PMLT_STAT_COUNTER_HIGH_REG   0xB0
#define CLK_STAT_COUNTER_HIGH_REG   0xB8
#define CLK_STAT_COUNTER_LOW_REG   0xC0
#define PCI_ERR_REG         0xD0 
#define DEBUG_REG         0x68
#define CMC_CTL_REG         0xD8
#define UCODE_LOAD         0x18
#define PSE_TO_HOST_DATA      0x58
#define HOST_TO_PSE_DATA      0x60

/*BAR 1*/
#define  REQ0_BASE_HIGH         0x00
#define  REQ0_BASE_LOW         0x08
#define  REQ0_SIZE         0x10

#define  REQ1_BASE_HIGH         0x20
#define  REQ1_BASE_LOW         0x28
#define  REQ1_SIZE         0x30

#define  REQ2_BASE_HIGH         0x40
#define  REQ2_BASE_LOW         0x48
#define  REQ2_SIZE         0x50

#define  REQ3_BASE_HIGH         0x60
#define  REQ3_BASE_LOW         0x68
#define  REQ3_SIZE         0x70

#define REQ0_DOOR_BELL         0x18
#define REQ1_DOOR_BELL         0x38
#define REQ2_DOOR_BELL         0x58
#define REQ3_DOOR_BELL         0x78

/*LDT specific registers */
#define LMT_CONTROL_REG               0xC0
#define LMT_INTERRUPT_CONTROL_REG      0xC8
#define LMT_INTERRUPT_DESTINATION_REG   0xD0
#define LMT_ERROR_REG               0xD8
#define LMT_EXPECTED_CRC_REG         0xE0
#define LMT_RCVD_CRC_REG            0xE8


/****************************************************************************/
/* Software specific macros                                       */
/****************************************************************************/


#define MAX_CORES_NITROX   24
   
#if defined(CN1230) || defined (CN1330)
#define CN1230_MAX_CORES      16
#define MAX_CORES         CN1230_MAX_CORES

#define CN1230_CORE_MASK_COUNT      3
#define CN1230_CORE_MASK_0      0x00FF00FFL
#define CN1230_CORE_MASK_1      0x0000FFFFL 
#define CN1230_CORE_MASK_2      0x000FFF0FL


#elif defined(CN1220) || defined(CN1320)
#define CN1220_MAX_CORES      8
#define MAX_CORES         CN1220_MAX_CORES

#define CN1220_CORE_MASK_COUNT      3
#define CN1220_CORE_MASK_0      0x000000FFL
#define CN1220_CORE_MASK_1      0x00000F0FL 
#define CN1220_CORE_MASK_2      0x000F000FL


#elif defined(CN1120)
#define CN1120_MAX_CORES      8
#define MAX_CORES         CN1120_MAX_CORES

#define CN1120_CORE_MASK_COUNT      3
#define CN1120_CORE_MASK_0      0x000000FFL
#define CN1120_CORE_MASK_1      0x00000F0FL
#define CN1120_CORE_MASK_2      0x000F000FL


#elif defined(CN1010)
#define CN1010_MAX_CORES      4
#define MAX_CORES         CN1010_MAX_CORES

#define CN1010_CORE_MASK_COUNT      1
#define CN1010_CORE_MASK_0      0x0000000FL


#elif defined(CN1005)
#define CN1005_MAX_CORES      2
#define MAX_CORES         CN1005_MAX_CORES

#define CN1005_CORE_MASK_COUNT      1
#define CN1005_CORE_MASK_0      0x00000003L


#elif defined(CN1001)
#define CN1001_MAX_CORES      1
#define MAX_CORES         CN1001_MAX_CORES

#define CN1001_CORE_MASK_COUNT      1
#define CN1001_CORE_MASK_0      0x00000001L
   

#elif defined(CN501)
#define CN501_MAX_CORES      1
#define MAX_CORES         CN501_MAX_CORES

#define CN501_CORE_MASK_COUNT      1
#define CN501_CORE_MASK_0      0x00000001L


#elif defined(CN505)
#define CN505_MAX_CORES         2
#define MAX_CORES         CN505_MAX_CORES

#define CN505_CORE_MASK_COUNT      1
#define CN505_CORE_MASK_0      0x00000003L

#else
#error "Part number (CN1230,CN1220, CN1120, CN1010, CN1005, CN1001, CN501 or CN505) not defined. Please check the Makefile."
#endif

   
#if defined(CN1010)||defined(CN1005)||defined(CN1001)||defined(CN501)
#define MAX_N1_QUEUES         2
#else
#define MAX_N1_QUEUES         4
#endif

#define COMMAND_BLOCK_SIZE      32
#define COMPLETION_CODE_INIT      (Uint64)0xFFFFFFFFFFFFFFFFULL
#define COMPLETION_CODE_SIZE      8

#ifdef MC2

#if CAVIUM_ENDIAN == CAVIUM_LITTLE_ENDIAN
#define COMPLETION_CODE_SHIFT      0
#else
#define COMPLETION_CODE_SHIFT      56
#endif

#define CTP_COMMAND_BLOCK_SIZE      32
#define CTP_QUEUE_SIZE         64   
#define SCRATCHPAD_SIZE         4096

#else /* MC1 */
#if CAVIUM_ENDIAN == CAVIUM_LITTLE_ENDIAN
#define COMPLETION_CODE_SHIFT      56
#else
#define COMPLETION_CODE_SHIFT      0
#endif

#endif

#define DRAM_BASE         (Uint32)0

#define CONTEXT_OFFSET         4194304

#define DRAM_CAS_LATENCY_INCR      1

#define CAVIUM_DEFAULT_TIMEOUT      (2*CAVIUM_HZ) /* 2 seconds*/
   /* This should be greater than the Microcode's timeout */
/* SRQ Timeout is (MAX_SRQ_TIMEOUT + 1)*CAVIUM_DEFAULT_TIMEOUT*/
#define MAX_SRQ_TIMEOUT         1

#ifdef SSL
#define DOOR_BELL_THRESHOLD      1
#else
#define DOOR_BELL_THRESHOLD      50
#endif


/* FSK memory */
#define FSK_BASE         48   
#define FSK_MAX            (8192 - FSK_BASE)
#define FSK_CHUNK_SIZE      640      

/* Extended Key memory stuff */
#define EX_KEYMEM_BASE   DRAM_BASE
#define EX_KEYMEM_MAX   CONTEXT_OFFSET
#define EX_KEYMEM_CHUNK_SIZE   1024   

/* Host Key memory */
//#define HOST_KEYMEM_MAX     (2048*1024)	
#define HOST_KEYMEM_MAX     (4096*1024)	
#define HOST_KEYMEM_CHUNK_SIZE 1024



#define SWAP_SHORTS_IN_64(val)               \
   ((val & (Uint64)0xff00000000000000ULL) >> 8)      \
         |                     \
   ((val & (Uint64)0x00ff000000000000ULL) << 8)      \
         |                     \
   ((val & (Uint64)0x0000ff0000000000ULL) >> 8)      \
         |                     \
   ((val & (Uint64)0x000000ff00000000ULL) << 8)      \
         |                     \
   ((val & (Uint64)0x00000000ff000000ULL) >> 8)      \
         |                     \
   ((val & (Uint64)0x0000000000ff0000ULL) << 8)      \
         |                     \
   ((val & (Uint64)0x000000000000ff00ULL) >> 8)      \
         |                     \
   ((val & (Uint64)0x00000000000000ffULL) << 8)      \

#define SPLIT_TRANSACTION_MASK            0x00700000

/* 
 * error codes used in handling error interrupts
 */
typedef enum
{
 /* hard reset group ( the tough guys )*/
 ERR_PCI_MASTER_ABORT_WRITE=2,
 ERR_PCI_TARGET_ABORT_WRITE,
 ERR_PCI_MASTER_RETRY_TIMEOUT_WRITE,
 ERR_OUTBOUND_FIFO_CMD,
 ERR_KEY_MEMORY_PARITY,

 /*soft reset group */
 ERR_PCI_MASTER_ABORT_REQ_READ,
 ERR_PCI_TARGET_ABORT_REQ_READ,
 ERR_PCI_MASTER_RETRY_TIMEOUT_REQ_READ,
 ERR_PCI_MASTER_DATA_PARITY_REQ_READ,
 ERR_REQ_COUNTER_OVERFLOW,

 /*EXEC reset group */
 ERR_EXEC_REG_FILE_PARITY,
 ERR_EXEC_UCODE_PARITY,

 /*seq number based errors */
 ERR_PCI_MASTER_ABORT_EXEC_READ,
 ERR_PCI_TARGET_ABORT_EXEC_READ,
 ERR_PCI_MASTER_RETRY_TIMOUT_EXEC_READ,
 ERR_PCI_MASTER_DATA_PARITY_EXEC_READ,
 ERR_EXEC_GENERAL,
 ERR_CMC_DOUBLE_BIT,
 ERR_CMC_SINGLE_BIT   
}PKP_ERROR;


/*
 * Error codes in DDR discovery
 */
typedef enum
{
   ERR_INIT_TWSI_FAILURE =100,
   ERR_DDR_NO_EEPROM_PRESENT,
   ERR_DDR_MEMORY_NOT_SRAM_DDR,
   ERR_DDR_UNSUPPORTED_NUM_COL_ADDR,
   ERR_DDR_UNSUPPORTED_NUM_ROW_ADDR,
   ERR_DDR_MORE_THAN_1_PHYS_BANK,
   ERR_DDR_UNSUPPORTED_MODULE_DATA_WIDTH,
   ERR_DDR_UNSUPPORTED_VOLT_INTERFACE_LEVEL,
   ERR_DDR_SDRAM_CYCLE_TIME_TOO_SHORT,
   ERR_DDR_UNSUPPORTED_MODULE_CONFIG,
   ERR_DDR_UNSUPPORTED_REFRESH_CLOCK,
   ERR_DDR_UNSUPPORTED_PRIMARY_SDRAM_WIDTH,
   ERR_DDR_REQUIRE_BURST_LENGTH_2,
   ERR_DDR_REQUIRE_4_DEV_BANKS,
   ERR_DDR_UNSUPPORTED_CAS_LATENCY,
   ERR_DDR_UNSUPPORTED_MODULE_BANK_DENSITY,
}DDR_ERROR;

typedef enum {huge_pool = 0, large, medium, small, tiny, ex_tiny, os} pool;

#ifdef N1_SANITY
#define ALIGNMENT   8
#define ALIGNMENT_MASK   (~(0x7L))
#endif

#define MAX_FRAGMENTS 32
typedef struct 
{
   struct cavium_list_head list;
   struct cavium_list_head alloc_list;
   Uint8 *big_buf;
   int frags_count;
   int index;
   pool p;
   Uint16 free_list[MAX_FRAGMENTS];
   Uint8 *address[MAX_FRAGMENTS];
   int free_list_index;
   int not_allocated;
} cavium_frag_buf_t;

typedef struct 
{
   cavium_spinlock_t buffer_lock;

   int chunks;
   int chunk_size;
   int real_size;  /* chunk size + tag size*/

#define MAX_BUFFER_CHUNKS      1500
   Uint8 *base;
   Uint8 *address[MAX_BUFFER_CHUNKS];
   Uint8 *address_trans[MAX_BUFFER_CHUNKS];
   Uint16 free_list[MAX_BUFFER_CHUNKS];
   int free_list_index;
   struct cavium_list_head frags_list;
} cavium_buffer_t;


#define MAX_DEV   4
typedef struct _pkp_device
{
void *dev; /* Platform specific device. For OSI it is opaque */
int    dram_present; /* flag. 1 = dram is local.0 = dram is implemented at host*/
Uint32    dram_max; /* total dram size.*/
ptrlong   dram_base; /* dram base address */
Uint32 dram_chunk_count;
Uint32   cmc_ctl_val; /* Context memory control register value*/

Uint32 bus_number;
Uint32 dev_number;
Uint32 func_number;
Uint32 bar_0;
Uint32 bar_1;
unsigned int interrupt_pin;
Uint32 uen;
Uint32 exec_units;
Uint32 boot_core_mask;
int   enable;
Uint32 imr;
cavium_wait_channel cav_poll;

/* command queue */
Uint32 command_queue_max;
Uint8 *command_queue_front[MAX_N1_QUEUES];
Uint8 *command_queue_end[MAX_N1_QUEUES];
Uint8 *command_queue_base[MAX_N1_QUEUES];
ptrlong command_queue_bus_addr[MAX_N1_QUEUES];
Uint8 *real_command_queue_base[MAX_N1_QUEUES];
ptrlong real_command_queue_bus_addr[MAX_N1_QUEUES];
Uint32 command_queue_size;
cavium_spinlock_t command_queue_lock[MAX_N1_QUEUES];
Uint32 door_addr[MAX_N1_QUEUES];
Uint32 door_bell_count[MAX_N1_QUEUES];
Uint32 door_bell_threshold[MAX_N1_QUEUES];

#ifdef MC2
Uint8 *ctp_base;
Uint8 *scratchpad_base;
Uint64 *error_val;
#endif

/* Context memory pool */
volatile Uint32 ctx_ipsec_free_index;
volatile Uint32 ctx_ssl_free_index;
ptrlong *ctx_free_list;
ptrlong *org_ctx_free_list;
cavium_spinlock_t ctx_lock;
int ctx_ipsec_count;
int ctx_ssl_count;

/* Key Memory */
cavium_spinlock_t keymem_lock;
struct cavium_list_head keymem_head;

Uint32 fsk_chunk_count;
Uint16 *fsk_free_list;
volatile Uint32 fsk_free_index;

Uint32 ex_keymem_chunk_count;
Uint32 *ex_keymem_free_list;
volatile Uint32 ex_keymem_free_index;

Uint32 host_keymem_count;
Uint32 *host_keymem_free_list;
struct PKP_BUFFER_ADDRESS *host_keymem_static_list;
volatile Uint32 host_keymem_free_index;

/* buffer pools */
#define BUF_POOLS    6
cavium_buffer_t buf[BUF_POOLS];
cavium_frag_buf_t fragments[MAX_BUFFER_CHUNKS];
Uint16 fragment_free_list[MAX_BUFFER_CHUNKS];
Uint16 fragment_free_list_index;
cavium_spinlock_t fragment_lock;

/* random number pool */
Uint8 *rnd_buffer;
Uint32 rnd_index;
cavium_spinlock_t rnd_lock;
cavium_spinlock_t id_lock;

/* poll thread wait channel */
cavium_wait_channel cav_poll_wait;

#ifdef INTERRUPT_ON_COMP
cavium_tasklet_t        interrupt_task;
#endif

struct MICROCODE microcode[MICROCODE_MAX];

#ifdef NPLUS
/* Cores list */
core_t cores[MAX_CORES_NITROX];
/*Lock for microcode & cores data structures */
cavium_spinlock_t mc_core_lock;
cavium_spinlock_t uenreg_lock;
int initialized;
#endif /*NPLUS*/

}cavium_device, *cavium_device_ptr;

#ifdef CAVIUM_RESOURCE_CHECK
struct CAV_RESOURCES 
{
   cavium_spinlock_t resource_check_lock;
   struct cavium_list_head ctx_head;
   struct cavium_list_head key_head;
};
#endif

/*
 * User Info Buffer
 */
typedef struct 
{
   cavium_device *n1_dev;
   struct cavium_list_head list;
   n1_request_buffer *req;
   n1_request_type req_type;
   int mmaped;
   Uint8 *in_buffer;
   Uint8 *out_buffer;
   Uint32 in_size;
   Uint32 out_size;
   cavium_pid_t pid;
   Uint32 signo;
   Uint32 outcnt;
   Uint8   *outptr[MAX_OUTCNT];
   Uint32  outsize[MAX_OUTCNT];
   Uint32  outoffset[MAX_OUTCNT];
   Uint32  outunit[MAX_OUTCNT];
   cavium_wait_channel channel;
   Uint32 status;
} n1_user_info_buffer;
/*
 * Buffer Address structure
 */
struct PKP_BUFFER_ADDRESS
{
   ptrlong vaddr; /* virtual address */
   ptrlong baddr; /* bus address */
   Uint32 size;
};


/*
 * Direct Operation structs
 */
struct PKP_DIRECT_OPERATION_STRUCT
{
 Uint64 ctx;

 Uint8 *dptr;
 Uint16 dlen;
 Uint64 dptr_baddr;

 Uint8 *rptr;
 Uint16 rlen;
 Uint64 rptr_baddr;

 volatile Uint64 *completion_address;

};


/*
 * Scatter/gather structs
 */

struct PKP_4_SHORTS
{
 Uint16 short_val[4];
};


struct CSP1_SG_LIST_COMPONENT
{
Uint16 length[4];
Uint64 ptr[4];
};


struct CSP1_SG_STRUCT
{
Uint16 unused[2];               /* unused locations */
Uint16 gather_list_size;
Uint16 scatter_list_size;                  
struct CSP1_SG_LIST_COMPONENT   *gather_component;
struct CSP1_SG_LIST_COMPONENT   *scatter_component;
};


struct CSP1_PATCH_WRITE
{
Uint8 prebytes[8];
Uint8 postbytes[8];
};


struct PKP_SG_OPERATION_STRUCT
{
 Uint64 ctx;

 Uint16 incnt;
 Uint16 outcnt;

 struct PKP_BUFFER_ADDRESS inbuffer[MAX_INCNT];
 Uint32   inunit[MAX_INCNT];

 struct PKP_BUFFER_ADDRESS outbuffer[MAX_OUTCNT];
 Uint32   outunit[MAX_OUTCNT];

 Uint16 gather_list_size;
 Uint16 scatter_list_size;
 ptrlong sg_dma_baddr;   
 volatile Uint64 *sg_dma;
 Uint32 sg_dma_size;
 volatile Uint64 *completion_dma; 
};


/*
 * Pending queues comprise of this struct.
 */

struct PENDING_ENTRY
{
 struct cavium_list_head list;
 Csp1DmaMode dma_mode;
 ptrlong completion_address;
 void *pkp_operation; 
 Uint32 tick;
 Uint32 status;
 void (*callback)(int,void *);   
 void *cb_arg;
 n1_request_buffer *n1_buf;
#ifdef NPLUS
 int ucode_idx;
 int srq_idx;
#endif
};


/*
 * General software functions
 */


/* Some useful macros */
#define ring_door_bell(pdev,q,cnt) write_PKP_register(pdev,((pdev)->door_addr[(q)]), (cnt))


/*
 * Direct
 */
int pkp_setup_direct_operation(cavium_device *pdev,
      Csp1OperationBuffer *csp1_operation, 
      struct PKP_DIRECT_OPERATION_STRUCT *pkp_direct_operation);

/*
 * Scatter/Gather
 */
int pkp_setup_sg_operation(cavium_device *pdev,
      Csp1OperationBuffer *csp1_operation, 
      struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation );

void check_endian_swap( struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation, int rw);

/*
 * Unmap all inpout and output buffers provided by the application
 */
void pkp_unmap_user_buffers(cavium_device *pdev,struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation);


/*
 * Flushed the contents of all user buffers.
 */
void 
pkp_flush_input_buffers(cavium_device *pdev,struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation);

void 
pkp_invalidate_output_buffers(cavium_device *pdev,struct PKP_SG_OPERATION_STRUCT *pkp_sg_operation);

#ifdef NPLUS
int
check_completion(cavium_device *n1_dev, volatile Uint64 *p, int max_wait_states, int ucode_idx, int srq_idx);
#else
int
check_completion(volatile Uint64 *p, int max_wait_states);
#endif /*NPLUS*/

#endif


/*
 * $Id: cavium.h,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium.h,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:35  cding
 * *** empty log message ***
 *
 * Revision 1.1.1.1  2008-09-22 05:49:30  cding
 * init
 *
 * Revision 1.1.1.1  2008-04-26 01:41:35  chen
 * AOS
 *
 * Revision 1.1.1.1  2007/09/27 06:28:06  jeff
 * Rebuild AOS trunk
 *
 * Revision 1.12  2006/06/23 09:57:50  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.11  2006/06/09 05:54:40  liqin
 * *** empty log message ***
 *
 * Revision 1.10  2006/06/08 08:14:18  liqin
 * *** empty log message ***
 *
 * Revision 1.9  2006/05/04 01:53:38  liqin
 * *** empty log message ***
 *
 * Revision 1.8  2006/05/03 13:23:21  liqin
 * *** empty log message ***
 *
 * Revision 1.7  2006/05/03 13:17:38  liqin
 * *** empty log message ***
 *
 * Revision 1.6  2006/05/03 12:21:16  liqin
 * *** empty log message ***
 *
 * Revision 1.5  2006/05/03 02:13:26  liqin
 * *** empty log message ***
 *
 * Revision 1.4  2006/04/30 05:42:37  liqin
 * *** empty log message ***
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
 * Revision 1.38  2005/10/24 06:44:44  kanantha
 * - Fixed RHEL4 Warnings
 *
 * Revision 1.37  2005/10/13 09:19:58  ksnaren
 * fixed compile errors for windows xp
 *
 * Revision 1.36  2005/09/28 15:50:26  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 Release with CVS Head
 * - Now context pointer given to user space applications is physical pointer.
 *   So there is no need to do cavium_vtophys() of context pointer.
 *
 * Revision 1.35  2005/09/06 14:38:57  ksadasivuni
 * - Some cleanup error fixing and spin_lock_destroy functionality added to osi.
 *   spin_lock_destroy was necessary because of FreeBSD 6.0.
 *
 * Revision 1.34  2005/09/06 07:08:22  ksadasivuni
 * - Merging FreeBSD 4.11 Release with CVS Head
 *
 * Revision 1.33  2005/08/31 18:10:30  bimran
 * Fixed several warnings.
 * Fixed the corerct use of ALIGNMENT and related macros.
 *
 * Revision 1.32  2005/07/17 04:35:09  sgadam
 * 8 bytes alignment issue on linux-2.6.2 is fixed. README and Makefile in
 * apps/cavium_engine updated
 *
 * Revision 1.31  2005/06/29 19:41:26  rkumar
 * 8-byte alignment problem fixed with N1_SANITY define.
 *
 * Revision 1.30  2005/06/13 06:35:42  rkumar
 * Changed copyright
 *
 * Revision 1.29  2005/06/03 07:10:19  rkumar
 * Timeout for SRQ_IN_USE entries increased by a factor MAX_SRQ_TIMEOUT(default 1)
 *
 * Revision 1.28  2005/05/20 14:34:05  rkumar
 * Merging CVS head from india
 *
 * Revision 1.27  2005/02/01 04:11:07  bimran
 * copyright fix
 *
 * Revision 1.26  2005/01/28 22:18:06  tsingh
 * Added support for HT part numbers.
 *
 * Revision 1.25  2005/01/19 23:16:21  tsingh
 * increased CTP_QUEUE_SIZE to 64
 *
 * Revision 1.24  2004/08/03 20:44:11  tahuja
 * support for Mips Linux & HT.
 *
 * Revision 1.23  2004/07/21 23:24:41  bimran
 * Fixed MC2 completion code issues on big endian systems.
 *
 * Revision 1.22  2004/07/09 01:09:00  bimran
 * fixed scatter gather support
 *
 * Revision 1.21  2004/06/23 19:40:11  bimran
 * changed check_completion to accept volatile comp_addr;
 * changed spinlock_t to OSI
 * added real addresses for command queues.
 *
 * Revision 1.20  2004/06/03 21:21:59  bimran
 * included cavium_list.h
 * fixed list* calls to use cavium_list
 *
 * Revision 1.19  2004/05/28 17:56:45  bimran
 * added id lock.
 *
 * Revision 1.18  2004/05/17 20:53:15  bimran
 * Fixed completion code shifts becuase now we will be also be supporting MC2 microcode on N1 parts.
 *
 * Revision 1.17  2004/05/11 03:10:24  bimran
 * some performance opt.
 *
 * Revision 1.16  2004/05/08 03:58:51  bimran
 * Fixed INTERRUPT_ON_COMP
 *
 * Revision 1.15  2004/05/04 20:48:34  bimran
 * Fixed RESOURCE_CHECK.
 *
 * Revision 1.14  2004/05/02 19:44:29  bimran
 * Added Copyright notice.
 *
 * Revision 1.13  2004/05/01 07:14:37  bimran
 * Fixed non-blocking operation from user mode.
 *
 * Revision 1.12  2004/04/30 21:22:11  bimran
 * Doorbell threshold is only 1 for SSL.
 *
 * Revision 1.11  2004/04/30 01:36:40  tsingh
 * Changed doorbell threshold to 25.(bimran)
 *
 * Revision 1.10  2004/04/30 00:00:09  bimran
 * Removed semaphoers from context memory in favour of just counts and a lock.
 *
 * Revision 1.9  2004/04/29 21:58:41  tsingh
 * Fixed doorbell threshold, completion code shofts values.(bimran)
 *
 * Revision 1.8  2004/04/26 23:26:25  bimran
 * Changed CTP queue size to 32.
 *
 * Revision 1.7  2004/04/26 19:04:30  bimran
 * Added 505 support.
 *
 * Revision 1.6  2004/04/21 19:18:58  bimran
 * NPLUS support.
 *
 * Revision 1.5  2004/04/20 17:41:30  bimran
 * Added microcode structure to  cavium_device structure instead of global mirocode structure.
 * Some early NPLUS related changes.
 *
 * Revision 1.4  2004/04/20 02:27:57  bimran
 * Removed DRAM_MAX macro.
 *
 * Revision 1.3  2004/04/19 18:37:54  bimran
 * Removed admin microcode support.
 *
 * Revision 1.2  2004/04/16 03:19:18  bimran
 * Added doorbell coalescing support.
 * Fixed MAX_N1_QUEUES so that it is dependent upon part number instead of Microcode type.
 * Fixed COMPLETION_CODE_INIT to be dependent upon Microcode type.
 *
 * Revision 1.1  2004/04/15 22:40:48  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

