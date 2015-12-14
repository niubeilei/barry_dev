////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_common.h
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


/*! \defgroup SSL_OPS   SSL APIs */
/*! \defgroup MISC      Misc APIs */
/*! \defgroup GP_OPS    General Purpose APIs */
#ifndef _CAVIUM_COMMON_H_
#define _CAVIUM_COMMON_H_

#include "cavium_sysdep.h"
#include "aosUtil/List.h" 	// added by liqin 2006/06/09

#define N1_OPERATION_CODE   1
#define N1_ALLOC_CONTEXT   2
#define N1_FREE_CONTEXT      3
#define N1_REQUEST_STATUS   4
#define N1_ALLOC_KEYMEM         5
#define N1_FREE_KEYMEM          6
#define N1_WRITE_KEYMEM         7
#define N1_FLUSH_ALL_CODE       8
#define N1_FLUSH_CODE           9
#define N1_DEBUG_WRITE_CODE   10
#define N1_DEBUG_READ_CODE   11
#define PCI_DEBUG_WRITE_CODE   12
#define PCI_DEBUG_READ_CODE   13
#define N1_INIT_CODE      14
#define N1_SOFT_RESET_CODE   15
#define N1_API_TEST_CODE   16
#define N1_SG_OPERATION_CODE   17
#define N1_GET_RANDOM_CODE   20   
#ifdef NPLUS
#define N1_CORE_ASSIGNMENT   24
#endif /*NPLUS*/

#define MAJOR_OP_INIT 0
#define MAJOR_OP_RANDOM_WRITE_CONTEXT 1
#define MAJOR_OP_ME_PKCS_LARGE 2
#define MAJOR_OP_RSASERVER_LARGE 3
#define MAJOR_OP_ME_PKCS 4
#define MAJOR_OP_RSASERVER 5
#define MAJOR_OP_HASH 6
#define MAJOR_OP_HMAC 7
#define MAJOR_OP_HANDSHAKE 8
#define MAJOR_OP_OTHER 10
#define MAJOR_OP_FINISHED 11
#define MAJOR_OP_RESUME 12
#define MAJOR_OP_ENCRYPT_DECRYPT_RECORD 13
#define MAJOR_OP_ENCRYPT_DECRYPT 14
#ifdef NPLUS
#define MAJOR_OP_ACQUIRE_CORE 4
#endif

/* boot and admin codes */
#define OP_BOOT_INIT            0x0000
#define OP_BOOT_SETUP_UCODE         0x0016
#define OP_BOOT_LOAD_UCODE         0x0015
#if 0
#define OP_BOOT_IMPORT_PKCS_KEY      0x0115
#define OP_ADMIN_SETUP_UCODE      0x0016   /* ??? */
#define OP_ADMIN_GEN_RSA_KEY_PAIR   0x0016   /* ??? */
#define OP_ADMIN_SIGN            0x0017   /* ??? */
#define OP_ADMIN_LOAD_UCODE         0x0015
#define OP_ADMIN_LOGIN            0x0215
#define OP_ADMIN_LOGOUT            0x0415
#define OP_ADMIN_EXPORT_KEY_PKCS8   0x0815
#define OP_ADMIN_IMPORT_KEY_PKCS8   0x1015
#define OP_ADMIN_DELETE_KEY         0x0215
#endif /* if 0 */

/*microcode types*/
#define CODE_TYPE_MAINLINE   1
#define CODE_TYPE_BOOT      2
#define CODE_TYPE_SPECIAL   3

#define CODE_TYPE_ADMIN   CODE_TYPE_SPECIAL



#ifndef ROUNDUP4
#define ROUNDUP4(val) (((val) + 3)&0xfffffffc)
#endif

#ifndef ROUNDUP8
#define ROUNDUP8(val) (((val) + 7)&0xfffffff8)
#endif

#ifndef ROUNDUP16
#define ROUNDUP16(val) (((val) + 15)&0xfffffff0)
#endif

#ifdef CSP1_KERNEL

typedef struct _cmd
{
  Uint16 opcode;
  Uint16 size;
  Uint16 param;
  Uint16 dlen;
}
Cmd;

typedef struct _Request
{
  Uint64 cmd;         /* command portion of request */
  Uint64 dptr;         /* pointer to data buffer */
  Uint64 rptr;         /* pointer to result buffer */
  Uint64 cptr;         /* pointer to context */
}
Request;

#ifdef NPLUS

#define INVALID_CORE 255

/* Core tracking data */
typedef struct core {
   Uint8   next_id;
   Uint8   ucode_idx;
   Uint8   ready;
   Uint8   pend2048;
   int     lrsrq_idx; /* Index of last SRQ request */
   Uint32  doorbell;
   volatile Uint64  *lrcc_ptr;
   Uint8   *ctp_ptr;
   int     ctp_idx;
   int     ctp_srq_idx;
}core_t;

/* Max entries in SRQ */
#define MAX_SRQ_SIZE 1000
#define MAX_SRQ_NORMAL 900 /* 100 entries for Priority commands */
#define HIGH_PRIO_QUEUE 2

/* States of entries in SRQ */
#define SR_FREE      0
#define SR_IN_USE    1
#define SR_IN_CTP    2

/* Soft Request Queue */
typedef struct {
   cavium_spinlock_t lock;
   Uint32   head;
   Uint32   tail;
   Uint32   qsize;
   volatile Uint64   *ccptr[MAX_SRQ_SIZE];
   Uint8    state[MAX_SRQ_SIZE];
   Uint8    core_id[MAX_SRQ_SIZE];
   Request  queue[MAX_SRQ_SIZE];
} softreq_t;
#endif /* NPLUS */

#define SRAM_ADDRESS_LEN 8

struct MICROCODE
{
   Uint8 code_type;
   Uint8 *code;
   Uint32 code_size; /* in bytes*/
   Uint8 *data;     /* constants */
   Uint32 data_size; /* in bytes*/
   Uint8 sram_address[SRAM_ADDRESS_LEN];
#ifdef NPLUS
   Uint8 core_id;
   Uint8 paired_cores;
   /* Software Request Queue */
   softreq_t srq;
   /* Use count */
   int use_count;
#endif /*NPLUS*/
};

#endif /* CSP1_KERNEL */

#ifndef MC2
/*! \enum HashType Sha-1 or MD-5 */
typedef enum
{ SHA1_TYPE = 0, MD5_TYPE = 1 }
HashType;
/*! \enum AesType AES128CBC AES192CBC AES256CBC*/
typedef enum
{ AES_128 = 0, AES_192 = 1, AES_256 = 2 }
AesType;
#else
/*! \enum HashType Sha-1 or MD-5 */
typedef enum
{ SHA1_TYPE = 2, MD5_TYPE = 1 }
HashType;
/*! \enum AesType AES128CBC AES192CBC AES256CBC*/
typedef enum
{ AES_128 = 5, AES_192 = 6, AES_256 = 7}
AesType;
#endif
/*! \enum RsaBlockType Public BT1 Private BT2*/
typedef enum
{ BT1 = 0, BT2 = 1 }
RsaBlockType;
/*! \enum ContextUpdate YES/NO*/
typedef enum
{ CAVIUM_NO_UPDATE = 0, CAVIUM_UPDATE = 1 }
ContextUpdate;

/*! \enum ContextType Context Type to allocate or deallocate */
typedef enum
{ CONTEXT_SSL = 0, CONTEXT_IPSEC = 2 }
ContextType;
/*! \enum RsaModExType Normal or Chinese Remainder Theorem */
typedef enum
{ NORMAL_MOD_EX = 0, CRT_MOD_EX = 1 }
RsaModExType;

/*! \enum KeyMaterialInput How Key Material is stored */
typedef enum
{ READ_INTERNAL_SRAM = 0, KEY_HANDLE = 0, INPUT_DATA = 1, READ_LOCAL_DDR =
    2, READ_LOCAL_HOST = 3 }
KeyMaterialInput;

/*! \enum KeyMaterialLocation Where Key Material is stored */
typedef enum {INTERNAL_SRAM = 0, HOST_MEM = 1, LOCAL_DDR = 2} KeyMaterialLocation;

/* \enum  EncryptionAlgorithmIndentifier */
typedef enum
{ PBE_MD2_DES_CBC = 0x51, PBE_MD5_DES_CBC = 0x53 }
EncryptionAlgorithmIdentifier;

/*! \enum Csp1ResponseOrder */
typedef enum
{ CAVIUM_RESPONSE_ORDERED = 0, CAVIUM_RESPONSE_UNORDERED = 1, CAVIUM_RESPONSE_REALTIME=2 }
Csp1ResponseOrder, n1_response_order;


/*! \enum Csp1RequestType Blocking or NonBlocking */
typedef enum
{ CAVIUM_BLOCKING = 0, CAVIUM_NON_BLOCKING = 1, CAVIUM_SIGNAL = 2 }
Csp1RequestType, n1_request_type;

/*! \enum n1_request_type Blocking or NonBlocking #Csp1RequestType */

/*! \enum Csp1DmaMode Direct or Scatter Gather*/
typedef enum
{ CAVIUM_DIRECT = 0, CAVIUM_SCATTER_GATHER = 1 }
Csp1DmaMode, n1_dma_mode, DmaMode;

/*! \enum Csp1MicrocodeType Ipsec or SSL/GP Ops*/
typedef enum
{ CAVIUM_IPSEC_MICROCODE = 0, CAVIUM_SSL_MICROCODE = 1 }
Csp1MicrocodeType;

/*! \enum ResultLocation Cptr or Rptr */
typedef enum
{ CONTEXT_PTR = 0, RESULT_PTR = 1 }
ResultLocation;

/*! \enum Csp1InterruptMode */
typedef enum
{ CAVIUM_NO_INTERRUPT = 0, CAVIUM_GEN_INTERRUPT = 1 }
Csp1InterruptMode;

/*! \enum Cs1SgMode */
typedef enum
{ CAVIUM_SG_READ = 0, CAVIUM_SG_WRITE = 1 }
Csp1SgMode;

/*
 * IPSEC and IKE enumerated constants 
 */
typedef enum
{ TRANSPORT = 0, TUNNEL = 1 }
IpsecMode;
typedef enum
{ AH = 0, ESP = 1 }
IpsecProto;
typedef enum
{ IPV4 = 0, IPV6 = 1 }
Version;
#ifndef MC2
typedef enum
{ NO_CYPHER = 0, DES3CBC = 1, AES128CBC = 2, AES192CBC = 3, AES256CBC =
    4, DESCBC = 9 }
EncType;
#else
typedef enum
{ NO_CYPHER = 0, DESCBC = 1, DES3CBC = 2, AES128CBC = 3, AES192CBC =
    4, AES256CBC = 5 }
EncType;
typedef enum
{ INVALID_SA = 0, VALID_SA = 1 }
ValidSa;
#endif
typedef enum
{ NO_AUTH = 0, MD5HMAC96 = 1, SHA1HMAC96 = 2 }
AuthType;
typedef enum
{ NO_ERROR = 0, LENGTH_INCORRECT = 1, MODE_INCORRECT = 2, PROTOCOL_INCORRECT =
    3,
  AUTH_INCORRECT = 4, PADDING_INCORRECT = 5
}
IpsecError;
typedef enum
{ INBOUND = 0, OUTBOUND = 1 } 
Direction;

#define OP_DECRYPT         1
#ifndef MC2
#define IPSEC_DIRECTION_SHIFT 1
#define IPSEC_VERSION_SHIFT 2
#define IPSEC_MODE_SHIFT 3
#define IPSEC_PROT_SHIFT 4
#define IPSEC_AUTH_SHIFT 5
#define IPSEC_CIPHER_SHIFT 8
#define IPSEC_DF_SHIFT 12
#define IPSEC_UDP_SHIFT 13
#define IPSEC_COMPARE_SHIFT      14
#define IPSEC_INTERRUPT_SHIFT      15
#else
#define IPSEC_VALID_SHIFT 0
#define IPSEC_DIRECTION_SHIFT 1
#define IPSEC_VERSION_SHIFT 2
#define IPSEC_MODE_SHIFT 4
#define IPSEC_PROT_SHIFT 5
#define IPSEC_ENCAP_SHIFT 6
#define IPSEC_CIPHER_SHIFT 8
#define IPSEC_AUTH_SHIFT 11
#define IPSEC_DF_SHIFT 13
#define IPSEC_SELECTOR_SHIFT 13
#define IPSEC_FT_SHIFT 14
#define IPSEC_SELECTOR_TYPE_SHIFT 14
#define IPSEC_NEXT_SA_SHIFT 15
#endif

#define CIPHER_KEY_OFFSET   0
#define ESP_OFFSET      32
#define IV_OFFSET      40
#define HMAC_OFFSET      56

typedef struct
{
  Csp1RequestType request_type;
  Csp1ResponseOrder response_order;
  Csp1DmaMode dma_mode;
  Csp1InterruptMode interrupt_mode;
  Csp1MicrocodeType microcode_type;
}
Csp1RequestInfo;


#define MAX_INCNT   8
#define MAX_OUTCNT   8
#define MAX_BUFCNT   MAX_INCNT

struct aos_list_head; 	// added by liqin 2006/06/09

typedef struct
{
  Uint16 opcode;
  Uint16 size;
  Uint16 param;
  Uint16 dlen;         /* length in bytes of the input data */
  Uint16 rlen;         /* length in bytes of the output data */
  Uint16 reserved;      /* for future use */
  Uint64 ctx_ptr;
  Uint16 incnt;         /* for getting a pointer to the data */
  Uint16 outcnt;      /* for getting a pointer to the data */
  Uint8 *inptr[MAX_INCNT];
  Uint32 insize[MAX_INCNT];
  Uint32 inoffset[MAX_INCNT];
  Uint32 inunit[MAX_INCNT];
  Uint8 *outptr[MAX_OUTCNT];
  Uint32 outsize[MAX_OUTCNT];
  Uint32 outoffset[MAX_OUTCNT];
  Uint32 outunit[MAX_OUTCNT];
  void (*callback) (int, void *);
  void *cb_arg;
  Uint32 request_id;
  Uint32 time_in;      
  Uint32 timeout;   
  Uint32 req_queue;
  n1_dma_mode dma_mode;
  n1_request_type req_type;   /* Only for op buf */
  n1_response_order res_order;
  Uint64 *completion_address;


  // begin added by liqin 2006/06/09
  void* n1_dev;
  Uint32 req_id;
  struct aos_list_head aos_list;
  // end added by liqin 2006/06/09
  
#ifdef NPLUS
  int  ucode_idx;
#endif /*NPLUS*/
}
Csp1OperationBuffer, n1_request_buffer, n1_operation_buffer;


typedef struct
{
  Uint16 bufcnt;
  Uint32 *bufptr[MAX_BUFCNT];
  Uint32 bufsize[MAX_BUFCNT];
}
Csp1ScatterBuffer, n1_scatter_buffer;


typedef struct
{
  Uint32 condition_code;
  Uint64 ctx_ptr;
}
Csp1ResponseBuffer;


#ifdef NPLUS

enum ucode_id 
{
   BOOT_IDX = 0,
   ADMIN_IDX,
   SSL_MLM_IDX,
   SSL_SPM_IDX,
   IPSEC_MLM_IDX,
   IPSEC_SPM_IDX,
   MICROCODE_MAX   /* Must be the last entry */
};

typedef struct 
{
   Uint8  mc_present[MICROCODE_MAX];
   Uint32 core_mask[MICROCODE_MAX]; 
} Csp1CoreAssignment;



#define FREE_IDX BOOT_IDX
#define MAX_INIT MICROCODE_MAX


#else /* !NPLUS */
#define MICROCODE_MAX   2
#define MAX_INIT MICROCODE_MAX

#endif /*NPLUS*/ 



typedef struct 
{
   Uint8 size;         /* number of init buffers */
   Uint8 version_info[MAX_INIT][32];
   Uint32 code_length[MAX_INIT];
   Uint8* code[MAX_INIT];
   Uint32 data_length[MAX_INIT];
   Uint8 sram_address[MAX_INIT][8];
   Uint8* data[MAX_INIT];
   Uint8 signature[MAX_INIT][256];
#ifdef NPLUS
   Uint8 ucode_idx[MAX_INIT];
#endif /*NPLUS*/
} Csp1InitBuffer;


/*! \enum Csp1ErrorCodes FAILURE/PENDING codes*/
typedef enum
{
  /* Driver */
  ERR_DRIVER_NOT_READY = (0x40000000 | 256),   /* 0x40000100 */
  ERR_MEMORY_ALLOC_FAILURE,          /* 0x40000101 */
  ERR_DOOR_BEL_TIMEOUT,            /* 0x40000102 */
  ERR_REQ_TIMEOUT,            /* 0x40000103 */
  ERR_CONTEXT_ALLOC_FAILURE,         /* 0x40000104 */
  ERR_CONTEXT_DEALLOC_FAILURE,         /* 0x40000105 */
  ERR_KEY_MEM_ALLOC_FAILURE,         /* 0x40000106 */
  ERR_KEY_MEM_DEALLOC_FAILURE,         /* 0x40000107 */
  ERR_UCODE_LOAD_FAILURE,         /* 0x40000108 */
  ERR_INIT_FAILURE,            /* 0x40000109 */
  ERR_EXEC_WAIT_TIMEOUT,         /* 0x4000010a */
  ERR_OUTBOUND_FIFO_WAIT_TIMEOUT,      /* 0x4000010b */
  ERR_INVALID_COMMAND,            /* 0x4000010c */
  ERR_SCATTER_GATHER_SETUP_FAILURE,      /* 0x4000010d */
  ERR_OPERATION_NOT_SUPPORTED,         /* 0x4000010e */
  ERR_NO_MORE_DEVICE,            /* 0x4000010f */
  ERR_REQ_PENDING,            /* 0x40000110 */
  ERR_DIRECT_SETUP_FAILURE,         /* 0x40000111 */
  ERR_INVALID_REQ_ID,            /* 0x40000112 */
#ifdef NPLUS
  ERR_ILLEGAL_ASSIGNMENT,         /* 0x40000113 */
#endif
  ERR_UNKNOWN_ERROR,            /* 0x40000114 */

  /* API Layer */
  ERR_ILLEGAL_INPUT_LENGTH = (0x40000000 | 384),/* 0x40000180 */
  ERR_ILLEGAL_OUTPUT_LENGTH,         /* 0x40000181 */
  ERR_ILLEGAL_KEY_LENGTH,         /* 0x40000182 */
  ERR_ILLEGAL_KEY_HANDLE,         /* 0x40000183 */
  ERR_ILLEGAL_CONTEXT_HANDLE,         /* 0x40000184 */
  ERR_ILLEGAL_BLOCK_TYPE,         /* 0x40000185 */
  ERR_ILLEGAL_KEY_MATERIAL_INPUT,      /* 0x40000186 */
  ERR_BAD_PKCS_PAD_OR_LENGTH,         /* 0x40000187 */
  ERR_BAD_CIPHER_OR_MAC,         /* 0x40000188 */
  ERR_ILLEGAL_MOD_EX_TYPE         /* 0x40000189 */
}Csp1ErrorCodes;



struct call_back_data{
	void *context;
	unsigned char *input;
	unsigned char *output;
	void* req;
	Uint32 outcnt;
	Uint8* outptr[MAX_OUTCNT];
	Uint32 outsize[MAX_OUTCNT];
	Uint32 outunit[MAX_OUTCNT];
	int in_len;
	int out_len;
};
/* Microcode generated error codes */
/*!\page page1 General Info
 * \section errorCodes  Error Codes Info
 * \verbatim
 * SSL1.x error codes 
 * ------------------
 *
 *  ERR_BAD_RECORD               0x40000002 
 *  There was a MAC miscompare or otherwise a record was found bad on a 
 *  decrypt.
 *  
 *  ERR_BAD_SIZE_OR_DLEN_VAL            0x4000000b
 *  Either the size of the request was bad or the read stream input length
 *  (indicated either by the Dlen value or the scatter/gather list) did not
 *  match the length expected by the request.
 *  
 *  ERR_BAD_PKCS_PAD_OR_LENGTH            0x4000000c
 *  A PKCS#1v15 decrypt found a bad pad value or length
 *
 *  ERR_BAD_PKCS_TYPE               0x4000000e
 *  A PKCS#1v15 decrypt found a bad type.
 *  
 *  ERR_BAD_SCATTER_GATHER_WRITE_LENGTH         0x4000000d
 *  The write stream length indicated by the scatter list did not match the
 *  write stream length of the request.
 *
 *
 * IPsec 1.x error codes
 * ---------------------
 * 
 *  BAD_PACKET_LENGTH               0x40000080
 *  BAD_IPSEC_MODE               0x40000081
 *  BAD_IPSEC_PROTOCOL               0x40000082
 *  BAD_IPSEC_AUTHENTICATION            0x40000083
 *  BAD_IPSEC_PADDING               0x40000084
 *  BAD_IP_VERSION               0x40000085
 *  BAD_IPSEC_AUTH_TYPE               0x40000086
 *  BAD_IPSEC_ENCRYPT_TYPE            0x40000087
 *  BAD_IKE_DH_GROUP               0x40000088
 *  BAD_MODLENGTH               0x40000089
 *  BAD_PKCS_PAD_OR_LENGTH            0x4000008a
 *  BAD_PKCS_TYPE               0x4000008b
 *  BAD_IPSEC_SPI               0x4000008c
 *  BAD_CHECKSUM               0x4000008d
 *  BAD_IPSEC_CONTEXT               0x4000008e
 *
 *
 * MC2 (Microcode 2.x) SSL and IPsec combined error codes
 * ------------------------------------------------------
 *
 * BAD_OPCODE                  0x40000001
 * BAD_RECORD                  0x40000002
 * BAD_SCATTER_GATHER_LIST            0x40000003
 * BAD_SCATTER_GATHER_WRITE_LENGTH         0x4000000d
 * BAD_LENGTH                  0x4000000f 
 * BAD_BOOT_COMPLETION               0x40000011
 * BAD_PACKET_LENGTH               0x40000012
 * BAD_IPSEC_MODE               0x40000013
 * BAD_IPSEC_PROTOCOL               0x40000014
 * BAD_IPSEC_AUTHENTICATION            0x40000015
 * BAD_IPSEC_PADDING               0x40000016
 * BAD_IP_VERSION               0x40000017
 * BAD_AUTH_TYPE               0x40000018
 * BAD_PKCS_DATA               0x4000001b
 * BAD_IPSEC_SPI               0x4000001c
 * BAD_CHECKSUM                  0x4000001d
 * BAD_IPSEC_CONTEXT               0x4000001e
 * BAD_IPSEC_CONTEXT_DIRECTION            0x4000001f
 * BAD_IPSEC_CONTEXT_FLAG_MISMATCH         0x40000020
 * IPCOMP_PAYLOAD               0x40000021
 * BAD_FRAG_OFFSET               0x40000022
 * BAD_SELECTOR_MATCH               0x40000023
 * BAD_AES_TYPE                  0x40000024
 * BAD_FRAGMENT_SIZE               0x40000026
 * BAD_DSA_VERIFY               0x40000027
 * BAD_PUBLIC_KEY               0x40000028
 * BAD_IKE_DH_GROUP               0x4000002b
 *
 * \endverbatim
 */


enum
{
  UNIT_8_BIT,
  UNIT_16_BIT,
  UNIT_32_BIT,
  UNIT_64_BIT
};


typedef struct
{
  Uint16 opcode;
  Uint8 *inptr0;
  Uint32 insize0;
  Uint8 *inptr1;
  Uint32 insize1;
  Uint8 *inptr2;
  Uint32 insize2;
}
DownloadBuffer;


typedef struct
{
  unsigned long addr;
  unsigned long data;
}
DebugRWReg;


typedef struct
{
  unsigned long timeout_max;
}
Csp1Config;


typedef struct
{
  ContextType type;
  Uint64 ctx_ptr;
}
n1_context_buf;


/* Store Key Buffer */
typedef struct
{
  Uint64 key_handle;
  Uint16 length;
  Uint8 *key;
}
n1_write_key_buf;

#define cavium_dump(str_,buf_,len_) \
{ \
   Uint32 i=0; \
   cavium_print("%s\n",str_); \
        cavium_print("0x%04lX : ", i*8); \
   for (i=0;i<(Uint32)(len_);i++){    \
      if(i && ((i%8) == 0)) \
                        { \
         cavium_print( "%s", "\n"); \
                        cavium_print("0x%04lX : ", (i)); \
                        } \
      cavium_print("%02x ",(buf_)[i]);\
                     } \
   cavium_print("\n%s\n",str_); \
}

#define OP_MEM_ALLOC_KEY_SRAM_MEM       0
#define OP_MEM_ALLOC_KEY_HOST_MEM       1
#define OP_MEM_ALLOC_KEY_DDR_MEM        2
#define OP_MEM_FREE_KEY_HOST_MEM        3
#define OP_MEM_FREE_KEY_DDR_MEM         4
#define OP_MEM_FREE_KEY_SRAM_MEM        5
#define OP_MEM_STORE_KEY_HOST_MEM       6

//#ifndef CSP1_KERNEL

extern int CSP1_driver_handle;



/*+****************************************************************************/
/*!\page page1 General Info
 * \section keyMaterial  Key memory and format
 * \verbatim 
 *
 *  Key memory and format
 *
 * Asymmetric keys can come from three sources:
 *   - the input stream (i.e. the dptr)
 *   - the on-chip (FSK) key memory
 *    - the extended key memory in local DRAM
 *
 * The FSK memory is 8KB and is addressed, read, and
 * written in multiples of 64-bit words
 *
 * The extended key memory in local DRAM can be up to
 * 4MB.
 *
 * The chinese remainder theorem (CRT) is a theorem that
 * allows for faster private key modular exponentiations.
 * This can (conditionally) be used to improve performance.
 *
 * Without CRT, full modular exponentions are performed
 * (up to 2048-bit). The format of the key material is:
 *      modulus   (modlength 64-bit words)
 *       exponent  (modlength 64-bit words)
 *
 * (The operation is result = (input ^ exponent) mod modulus.)
 *
 * With CRT (on private keys), the format of the key material is:
 *      Q        (modlength/2 64-bit words)
 *      Eq       (modlength/2 64-bit words)
 *      P        (modlength/2 64-bit words)
 *      Ep       (modlength/2 64-bit words)
 *      iqmp     (modlength/2 64-bit words)
 *
 * The following are requirements of this key material with CRT:
 *   modulus = Q * P (Q, P are prime factors of modulus, P > Q)
 *   Q, P are 1/2 the length (in bits or words) of the modulus
 *   Eq = exponent mod (Q - 1)
 *   Ep = exponent mod (P - 1)
 *   iqmp = (q ^ -1) mod p = (q ^ (p-2)) mod p
 * Eq and Ep are the precomputed exponents. iqmp is also precomputed.
 *
 * With CRT, the calculation to  get result = (input ^ exponent) mod modulus
 * is:
 *   inputq = input mod Q
 *   inputp = input mod P
 *   Mq = (inputq ^ Eq) mod Q
 *   Mp = (inputp ^ Ep) mod P
 *      temp = Mp - Mq
 *   if(temp < 0)
 *      temp += p
 *   temp = (temp * iqmp) mod p
 *   result = temp * q + Mq  // modular multiplication not necessary since the result is < modulus
 *   
 *
 * #ifndef MC2
 *      The key material should be in integer format. That means that the least-significant 64-bit
 *      word should be the first word and the most-significant word is the last word. 
 *      (Within a word the bytes are still big-endian - the most-significant byte contains the 
 *      most-significant bits, as you might expect.)
 * #endif
 * \endverbatim
 */
/*-***************************************************************************/


/*+****************************************************************************/
/*!\ingroup MISC
 * Csp1Initialize
 *
 * Prepares the aplication.
 *
 * \input dma_mode  CAVIUM_DIRECT or CAVIUM_SCATTER_GATHER
 * \if NPLUS
 * \input microcode_type \arg ADMIN_IDX      NPLUS microcode and core administration.
 *                                     nplus_init uses this microcode type.
 *                                     
 *          \arg SSL_MLM_IDX    SSL 2.x microcode in fast path mode.
 *
 *          \arg SSL_SPM_IDX    SSL 2.x microcode in slow path mode.
 * 
 *          \arg IPSEC_MLM_IDX  IPsec2.x microcode in fast path mode.
 *
 *          \arg IPSEC_SPM_IDX  IPsec2.x microcode on slow path mode. 
 * \endif
 * 
 * \return completion code
 * \retval SUCCESS 0 
 * \retval FAILURE #Csp1ErrorCodes
 */
/*-***************************************************************************/
#ifdef NPLUS
Uint32 
Csp1Initialize(Csp1DmaMode dma_mode, int microcode_type);
#else
Uint32 
Csp1Initialize(Csp1DmaMode dma_mode);
#endif


/*+****************************************************************************/
/*!\ingroup MISC
 * Csp1Shutdown
 *
 * Cleanup the driver.
 *
 * \return Status
 * \retval SUCESS 0           
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32 Csp1Shutdown (void);

/*+****************************************************************************/
/*! \ingroup MISC 
 * Csp1CheckForCompletion
 *
 * Checks the status of the request.
 *
 * \input request_id
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes

 *
 */
/*-***************************************************************************/
Uint32 Csp1CheckForCompletion (Uint32 request_id);


/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1FlushAllRequests
 *
 * Removes all pending requests for the calling process. This call can make the 
 * current process go to sleep. The driver will wait for all pending requests 
 * to complete or timeout.
 *
 * \return  Status
 * \retval  SUCCESS 0
 * \retval  FAILURE/PENDING #Csp1ErrorCodes

 */
/*-***************************************************************************/
Uint32 Csp1FlushAllRequests (void);


/*+****************************************************************************/
/*! \ingroup MISC 
 * Csp1FlushRequest
 *
 * Removes the request for the calling process. This call can make the 
 * current process go to sleep. The driver will wait for the request 
 * to complete or timeout.
 *
 * \input request_id
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32 Csp1FlushRequest (Uint32 request_id);


/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1AllocContext
 *
 * Allocates a context segment (in the local DDR DRAM or the host memory 
 * depending on the system) and returns its handle that will be passed to the 
 * processor in the final 8 bytes of the request as Cptr.
 *
 * \input  cntx_type     CONTEXT_SSL or CONTEXT_IPSEC
 *
 * \output context_handle pointer to 8-byte address of the context for use by 
 *      the Cavium processor
 * \output cntx_type       type of context SSL IPSEC allocated??
 *
 * \return Status 
 * \retval SUCCESS 0 
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32 Csp1AllocContext (ContextType cntx_type, Uint64 * context_handle);


/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1FreeContext
 *
 * Free a context segment for use by another SSL connection.
 *
 * \input cntx_type       CONTEXT_SSL or CONTEXT_IPSEC
 * \input context_handle    8-byte address of the context for use by 
 *             the Cavium processor
 *
 * \return Status
 * \retval SUCCESS 0 
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32 Csp1FreeContext (ContextType cntx_type, Uint64 context_handle);



/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1AllocKeyMem
 *
 * Acquires the handle to a key memory segment and returns a handle.
 *
 * \input key_material_loc   INTERNAL_SRAM, HOST_MEM, or LOCAL_DDR
 *
 * \output key_handle      pointer to 8-byte handle to key memory segment
 *
 * \return Completion Code
 * \retval SUCCESS 0 
 * \retval COMPLETION_CODE #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/

Uint32
Csp1AllocKeyMem (KeyMaterialLocation key_material_loc, Uint64 * key_handle);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1FreeKeyMem
 *
 * Free a key memory segment.
 *
 * \input key_handle   8-byte handle to key memory segment
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/

Uint32 Csp1FreeKeyMem (Uint64 key_handle);

       
/*+****************************************************************************/
/*! \ingroup MISC
 * Csp1StoreKey
 *
 * Store a key to memory segment indicated by key handle.
 *
 * \input key_handle     8-byte handle to key memory segment
 * \input length    size of key in bytes
 * \input key       pointer to key
 * \input mod_ex_type    NORMAL_MOD_EX or CRT_MOD_EX
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/

Uint32
Csp1StoreKey (Uint64 * key_handle,
         Uint16 length, Uint8 * key, RsaModExType mod_ex_type);


/*****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1ReadEpci
 *
 * Routine to read the onchip SRAM memory
 *
 * \input request_type   CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input key_handle     64-bit key handle pointer.
 * \input length    size of data to read in bytes (8<length<=880, length%8=0).
 *
 * \output  data    Result data (size variable based on size)
 * \output request_id    Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/****************************************************************************/
Uint32
Csp1ReadEpci (n1_request_type request_type,
         Uint64 * key_handle,
         Uint16 length, Uint8 * data, Uint32 * request_id);


/*****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1WriteEpci
 * write data to onchip SRAM.
 *
 * \input request_type   CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input key_handle     64-bit key handle pointer.
 * \input length    size of data to write in bytes (8<length<=880, length%8=0).
 * \input data       input data 
 *
 * \output request_id    Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/****************************************************************************/
Uint32
Csp1WriteEpci (n1_request_type request_type,
          Uint64 * key_handle,
          Uint16 length, Uint8 * data, Uint32 * request_id);


/*****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1ReadContext
 *
 * Routine to read data from context.
 *
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit context handle pointer.
 * \input length     size of data to read in bytes (8<length<=1024, length%8=0).
 *
 * \output data        Result data (size variable based on size)
 * \output request_id     Unique ID for this request.
 *
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/****************************************************************************/
Uint32
Csp1ReadContext (n1_request_type request_type,
       Uint64 context_handle,
       Uint16 length, Uint8 * data, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1WriteContext
 *
 * Write data to context memory.
 *
 * \input request_type     CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit context handle pointer (context_handle%8=0)
 * \input length     size of the data in bytes (8<=length<=1024,length%8=0)
 * \input data        pointer to length bytes of data to be stored
 *
 * \output request_id     Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1WriteContext (n1_request_type request_type,
        Uint64 context_handle,
        Uint16 length, Uint8 * data, Uint32 * request_id);

/*+****************************************************************************
 *
 * Csp1WriteIpsecSa
 *
 * Write Ipsec SA data to context memory.
 *
 * Input
 *    request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *    proto = ESP or AH
 *    inner_version = Protocol version of inner IP header.
 *    outer_version = Protocol version of outer IP header.
 *    mode = SA mode (TUNNEL or TRANSPORT)
 *    dir = Direction (INBOUND or OUTBOUND)
 *    cypher = Encryption algorithm 
 *             (DESCBC, DES3CBC, AES128CBC, AES192CBC, AES256CBC)
 *    auth = Authentication algorithm
 *           (MD5HMAC96 or SHA1HMAC96)
 *    template = Template for Outer IP header
 *    spi = 32 bit SPI value
 *    copy_df = 0 (copy the df bit for packet fragments) or 1 (do not copy)
 *    udp_encap = 0 (no UDP encapsulation) or 1 (UDP encapsulation)
 *    context_handle = 64-bit context handle pointer (context_handle%8=0)
 *    next_context_handle = context handle pointer for next SA.
 *
 * output
 *    request_id = Unique ID for this request.
 *
 * Return Value
 *    0  = success 
 *    >0 = failure or pending
 *    see error_codes.txt
 *
 *-***************************************************************************/
#ifndef SSL
#ifndef MC2
Uint32
Csp1WriteIpsecSa(IpsecProto proto,
		Version version,
		IpsecMode mode,
		Direction dir,
		EncType cypher,
		Uint8 *e_key,
		AuthType auth,
		Uint8 *a_key,
		Uint8 template[40],
		Uint32 spi,
		Uint8 copy_df,
		Uint8 udp_encap,
		Uint64 context_handle,
		Uint64 next_context_handle,
		int res_order,
		int req_queue,
		Uint32 *request_id);
#else
Uint32
Csp1WriteIpsecSa(n1_request_type request_type,
		IpsecProto proto,
		Version inner_version,
		Version outer_version,
		IpsecMode mode,
		Direction dir,
		EncType cypher,
		Uint8 *e_key,
		AuthType auth,
		Uint8 *a_key,
		Uint8 template[40],
		Uint32 spi,
		Uint8 copy_df,
		Uint8 udp_encap,
		Uint64 context_handle,
		Uint64 next_context_handle,
		int res_order,
		int req_queue,
		Uint32 *request_id);
#endif
#endif
/*+****************************************************************************
 *
 * Csp1ProcessPacket
 *
 * Process outbound packet
 *
 * Input
 *    size = 
 *    param = 
 *    dlen = length of input (packet)
 *    inv = poniter to input data (packet to be processed)
 *    rlen = length of output buffer (processed packet)
 *    context_handle = 64-bit context handle pointer (context_handle%8=0)
 *    response_order = 
 *    req_queue = 
 *    
 * Output
 *   outv = pointer to output buffer
 *    request_id = Unique ID for this request.
 *
 * Return Value
 *    0  = success 
 *    >0 = failure or pending
 *    see error_codes.txt
 *
 *-***************************************************************************/
#ifndef SSL
Uint32
Csp1ProcessPacket(Uint16 size, 
			Uint16 param,
                        Direction dir,
			Uint16 dlen,
	                n1_scatter_buffer *inv,
			n1_scatter_buffer *outv, 
			int rlen,
	                Uint64 context_handle, 
	                int response_order, 
			int req_queue,
			Uint32 *request_id);
#endif
/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Random
 *
 * Get random data from random pool maintained by the driver.
 *
 * \input request_type CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING \n
 *          This api will only block if driver will have to refill
 *          its random number pool. THis argument is ignored by the 
 *          driver.
 * \input length       size of random data in bytes 
 *
 * \output random      pointer to length bytes of random data
 * \output request_id  Unique ID for this request. This argument is ignored by the 
 *                     driver.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1Random (n1_request_type request_type,
       Uint16 length, Uint8 * random, Uint32 * request_id);

/*+***************************************************************************
 *
 * Csp1Hash
 *
 * Compute the HASH of a complete message. Does not use context.
 *
 * Input
 * 	request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *	hash_type = MD5_TYPE or SHA1_TYPE 
 *	message_length = size of input in bytes (0<=message_length<=2^16-1)
 *	message = pointer to length bytes of input to be HMACed
 *
 * Output
 *	hash = pointer to the hash_size HASH result 
 *	request_id = Unique ID for this request.
 *
 * Return Value
 * 	0  = success 
 * 	>0 = failure or pending
 * 	see error_codes.txt
 *
 *-***************************************************************************/
#ifdef MC2
Uint32 
Csp1Hash(n1_request_type request_type,
	 HashType hash_type, 
	 Uint16 message_length, 
	 Uint8 *message, 
	 Uint8 *hash,
	 Uint32 *request_id);
#endif
/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Hmac
 *
 * Compute the HMAC of a complete message. Does not use context.
 *
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input hash_type     MD5_TYPE or SHA1_TYPE 
 * \input key_length     size of the key in bytes (key_length%8=0, 8<=key_length<=64)
 * \input key        pointer to key_length-byte key
 * \input message_length  size of input in bytes (0<=message_length<=2^16-1)
 * \input message     pointer to length bytes of input to be HMACed
 *
 * \output hmac        pointer to the hash_size HMAC result 
 * \output request_id     Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1Hmac (n1_request_type request_type,
     HashType hash_type,
     Uint16 key_length,
     Uint8 * key,
     Uint16 message_length,
     Uint8 * message, Uint8 * hmac, Uint32 * request_id);


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1HmacStart
 *
 *   Compute the first stage in a multi-step HMAC.
 *   
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input hash_type     MD5_TYPE or SHA1_TYPE 
 * \input key_length     size of the key in bytes (key_length%8=0, 8<=key_length<=64)
 * \input key        pointer to key_length-byte key
 * \input message_length  size of input in bytes (0<=message_length<=2^16-1)
 * \input message     pointer to length bytes of input to be HMACed
 *
 * \output request_id     Unique ID for this request.
 *
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1HmacStart (n1_request_type request_type,
          Uint64 context_handle,
          HashType hash_type,
          Uint16 key_length,
          Uint8 * key,
          Uint16 message_length, Uint8 * message, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1HmacUpdate
 *
 *   Compute an intermediate step in a multi-step HMAC.
 *
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input hash_type     MD5_TYPE or SHA1_TYPE 
 * \input message_length  size of input in bytes (0<=message_length<=2^16-1)
 * \input message     pointer to length bytes of input to be HMACed
 *
 * \output request_id     Unique ID for this request.
 *
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1HmacUpdate (n1_request_type request_type,
      Uint64 context_handle,
      HashType hash_type,
      Uint16 message_length, Uint8 * message, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1HmacFinish
 *
 *   Compute the final step in a multi-step HMAC.
 *
 * \input request_type CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle 64-bit pointer to context (context_handle%8=0)
 * \input hash_type MD5_TYPE or SHA1_TYPE 
 * \input message_length size of input in bytes (0<=message_length<=2^16-1)
 * \input message pointer to length bytes of input to be HMACed
 *
 * \output final_hmac pointer to the hash_size-word HMAC result 
 * \output request_id Unique ID for this request.
 *
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1HmacFinish (n1_request_type request_type,
      Uint64 context_handle,
      HashType hash_type,
      Uint16 message_length,
      Uint8 * message, Uint8 * final_hmac, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS 
 * Csp1Me
 *
 * Modular exponentiation.
 *
 * p = x^e mod m
 *
 * \input request_type   CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \if MC2
 * \input modlength   size of modulus in bytes (17<=modlength<=256)
 * \input explength   size of exponent in bytes 
 * \input datalength   size of data in bytes
 * \input modulus   pointer to modlength-byte modulus
 * \input exponent   pointer to explength-byte exponent
 * \input data      pointer to datalength-byte data
 *   
 * \else
 * \input result_location  CONTEXT_PTR or RESULT_PTR 
 * \input context_handle   64-bit pointer to context (context_handle%8=0)
 * \input modlength      size of modulus in bytes (modlength%8=0, 24<modlength<=256)
 * \input data pointer      to modlength-byte value to be exponentiated
 * \input modulus      pointer to modlength-byte modulus
 * \input exponent      pointer to modlength-byte exponent
 * \endif 
 *
 * \if MC2
 * \output result pointer to modlength-byte output
 * \else
 * \output result if (result_location == RESULT_PTR) pointer to modlength-byte 
 * \endif          output in byte format
 * \output  request_id Unique ID for this request.
 *
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 * \note exp_length <= mod_length \n
 *    data_length <= mod_length
 *
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Me (n1_request_type request_type,
   Uint16 modlength,
   Uint16 explength,
   Uint16 datalength,
   Uint8 * modulus,
   Uint8 * exponent, Uint8 * data, Uint8 * result, Uint32 * request_id);
#else
Uint32
Csp1Me (n1_request_type request_type,
   ResultLocation result_location,
   Uint64 context_handle,
   Uint16 modlength,
   Uint8 * data,
   Uint8 * modulus,
   Uint8 * exponent, Uint8 * result, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS 
 * Csp1Pkcs1v15Enc
 *
 * Creates PKCS#1v1.5 container.
 *
 * \input request_type   CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \if MC2
 * \input block_type   type of PKCS#1v1.5 padding (BT1 or BT2)
 * \input modlength   size of modulus in bytes (17 <= 256)
 * \input explength   size of exponent in bytes (explength <= modlength -11)
 * \input datalength   size of data in bytes (datalength <= modlength -11)
 * \input modulus   pointer to modlength-byte modulus
 * \input exponent   pointer to explength-byte exponent
 * \input data      pointer to datalength-byte data
 * \else
 * \input result_location    CONTEXT_PTR or RESULT_PTR 
 * \input context_handle     64-bit pointer to context (context_handle%8=0)
 * \input key_material_input KEY_HANDLE or INPUT_DATA
 * \input key_handle        64-bit handle for key memory 
 * \input block_type        type of PKCS#1v1.5 padding (BT1 or BT2)
 * \input modlength        size of modulus in bytes (modlength%8=0, 24<modlength<=256)
 * \input modulus        (key_material_input == INPUT_DATA) ? pointer to RSA modulus : don't care
 * \input exponent        (key_material_input == INPUT_DATA) ? pointer to RSA exponent : don't care
 * \input length        size of the input value 
 * \input data           pointer to length-byte value to be exponentiated
 * \endif
 *
 * 
 * \if MC2
 * \output result   pointer to modlength bytes of output
 * \else
 * \output result   (result_location == RESULT_PTR) ? (pointer to modlength bytes of output: don't care)
 * \endif 
 * \output request_id   Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Pkcs1v15Enc (n1_request_type request_type,
       RsaBlockType block_type,
       Uint16 modlength,
       Uint16 explength,
       Uint16 datalength,
       Uint8 * modulus,
       Uint8 * exponent,
       Uint8 * data, Uint8 * result, Uint32 * request_id);
#else
Uint32
Csp1Pkcs1v15Enc (n1_request_type request_type,
       ResultLocation result_location,
       Uint64 context_handle,
       KeyMaterialInput key_material_input,
       Uint64 key_handle,
       RsaBlockType block_type,
       Uint16 modlength,
       Uint8 * modulus,
       Uint8 * exponent,
       Uint16 length,
       Uint8 * data, Uint8 * result, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Pkcs1v15CrtEnc
 *
 * Creates PKCS#1v1.5 container using the Chinese Remainder Theorem.
 * The combination of block type BT2 and CRT may produce unpredictable results.
 *
 * \input request_type   CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \if MC2
 * \input block_type   type of PKCS#1v1.5 padding (BT1 only)
 * \input modlength   size of modulus in bytes (34 <= modlength <=256, modlength%2 !=0)
 * \input datalength   size of input data in bytes (datalength <= modlength -11).
 * \input Q      prime factor of RSA modulus
 * \input Eq      exponent mod(Q-1)
 * \input P      prime factor of RSA modulus
 * \input Ep      exponent mod(P-1)
 * \input iqmp      (Q^-1) mod P
 * \else
 * \input result_location     CONTEXT_PTR or RESULT_PTR 
 * \input context_handle      64-bit pointer to context (context_handle%8=0)
 * \input key_material_input  KEY_HANDLE or INPUT_DATA
 * \input key_handle         64-bit handle for key memory 
 * \input block_type         type of PKCS#1v1.5 padding (BT1 only)
 * \input modlength         size of modulus in bytes (modlength%8=0, 24<modlength<=256)
 * \input Q            (key_material_input == INPUT_DATA) ? prime factor of RSA modulus : don't care
 * \input Eq            (key_material_input == INPUT_DATA) ? exponent mod(Q-1) : don't care
 * \input P            (key_material_input == INPUT_DATA) ? prime factor of RSA modulus : don't care
 * \input Ep            (key_material_input == INPUT_DATA) ? exponent mod(P-1) : don't care
 * \input iqmp            (key_material_input == INPUT_DATA) ? (Q^-1) mod P : don't care
 * \input length         size of the input value 
 * \endif
 * \input data pointer to length-byte value to be exponentiated
 *
 * \if MC2
 * \output result     pointer to modlength bytes of output
 * \else
 * \output result   (result_location == RESULT_PTR) ? (pointer to modlength bytes of output : don't care
 * \endif
 * \output request_id   Unique ID for this request.
 * 
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 * \note modlength must be even
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Pkcs1v15CrtEnc (n1_request_type request_type,
          RsaBlockType block_type,
          Uint16 modlength,
          Uint16 datalength,
          Uint8 * Q,
          Uint8 * Eq,
          Uint8 * P,
          Uint8 * Ep,
          Uint8 * iqmp,
          Uint8 * data, Uint8 * result, Uint32 * request_id);
#else
Uint32
Csp1Pkcs1v15CrtEnc (n1_request_type request_type,
          ResultLocation result_location,
          Uint64 context_handle,
          KeyMaterialInput key_material_input,
          Uint64 key_handle,
          RsaBlockType block_type,
          Uint16 modlength,
          Uint8 * Q,
          Uint8 * Eq,
          Uint8 * P,
          Uint8 * Ep,
          Uint8 * iqmp,
          Uint16 length,
          Uint8 * data, Uint8 * result, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Pkcs1v15Dec
 *
 * Decrypts PKCS#1v1.5 container.
 *
 *
 * \input request_type   CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \if MC2
 * \input block_type   type of PKCS#1v1.5 padding (BT1 only)
 * \input modlength   size of modulus in bytes (17 <= modlength <=256)
 * \input explength   size of exponent in bytes (explength <= modlength - 11)
 * \input modulus   pointer to modlength-byte modulus
 * \input exponent   pointer to explength-byte exponent
 * \input data      pointer to modlength-11 bytes input
 * \else
 * \input result_location      CONTEXT_PTR or RESULT_PTR 
 * \input context_handle      64-bit pointer to context (context_handle%8=0)
 * \input key_material_input      KEY_HANDLE or INPUT_DATA
 * \input key_handle         64-bit handle for key memory 
 * \input block_type         type of PKCS#1v1.5 padding (BT1 or BT2)
 * \input modlength         size of modulus in bytes (modlength%8=0, 24<modlength<=256)
 * \input modulus         (key_material_input == INPUT_DATA) ? pointer to RSA modulus : don't care
 * \input exponent         (key_material_input == INPUT_DATA) ? pointer to RSA exponent : don't care
 * \input data            pointer to modlength-byte value to be exponentiated
 * \endif
 *
 * \if MC2
 * \output out_length   size of decrypted data in Network Byte order.
 * \output result   out_length byte size result
 * \else
 * \output result   (result_location == RESULT_PTR) ? (pointer to modlength bytes of output, 
 *             *out_length bytes used) : don't care
 * \output out_length   pointer to output length in bytes
 * \endif
 * \output request_id   Unique ID for this request.
 *
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Pkcs1v15Dec (n1_request_type request_type,
       RsaBlockType block_type,
       Uint16 modlength,
       Uint16 explength,
       Uint8 * modulus,
       Uint8 * exponent,
       Uint8 * data,
       Uint16 * out_length, Uint8 * result, Uint32 * request_id);
#else
Uint32
Csp1Pkcs1v15Dec (n1_request_type request_type,
       ResultLocation result_location,
       Uint64 context_handle,
       KeyMaterialInput key_material_input,
       Uint64 key_handle,
       RsaBlockType block_type,
       Uint16 modlength,
       Uint8 * modulus,
       Uint8 * exponent,
       Uint8 * data,
       Uint8 * result, Uint64 * out_length, Uint32 * request_id);
#endif

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Pkcs1v15CrtDec
 *
 * Decrypts PKCS#1v1.5 container using the Chinese Remainder Theorem.
 * The combination of block type 01 and CRT may produce unpredictable results.
 *
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \if MC2
 * \input block_type   type of PKCS#1v1.5 padding (BT2 only)
 * \input modlength   size of modulus in bytes (34 <= modlength <= 256)
 * \input Q      prime factor of RSA modulus
 * \input Eq      exponent mod(Q-1)
 * \input P      prime factor of RSA modulus
 * \input Ep      exponent mod(P-1)
 * \input iqmp      (Q^-1) mod P
 * \input data      pointer to modlength-byte value to be exponentiated
 * \else
 * \input result_location     CONTEXT_PTR or RESULT_PTR 
 * \input context_handle      64-bit pointer to context (context_handle%8=0)
 * \input key_material_input  KEY_HANDLE or INPUT_DATA
 * \input key_handle         64-bit handle for key memory 
 * \input block_type         type of PKCS#1v1.5 padding (BT2 only)
 * \input modlength         size of modulus in bytes (modlength%8=0, 24<modlength<=256)
 * \input Q            (key_material_input == INPUT_DATA) ? prime factor of RSA modulus : don't care
 * \input Eq            (key_material_input == INPUT_DATA) ? exponent mod(Q-1) : don't care
 * \input P            (key_material_input == INPUT_DATA) ? prime factor of RSA modulus : don't care
 * \input Ep            (key_material_input == INPUT_DATA) ? exponent mod(P-1) : don't care
 * \input iqmp            (key_material_input == INPUT_DATA) ? (Q^-1) mod P : don't care
 * \input data            pointer to modlength-byte value to be exponentiated
 * \endif
 *
 * \if MC2
 * \output out_length   pointer to output length in bytes (Network Byte order)
 * \output result   (pointer to modlength bytes of output,   *out_length bytes used)
 * \else
 * \output result   (result_location == RESULT_PTR) ? (pointer to modlength bytes of output, 
 *                     *out_length bytes used) : don't care
 * \output out_length   pointer to output length in bytes
 * \endif
 * \output request_id   Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 * \note modlength must be even
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Pkcs1v15CrtDec (n1_request_type request_type,
          RsaBlockType block_type,
          Uint16 modlength,
          Uint8 * Q,
          Uint8 * Eq,
          Uint8 * P,
          Uint8 * Ep,
          Uint8 * iqmp,
          Uint8 * data,
          Uint16 * out_length, Uint8 * result, Uint32 * request_id);
#else
Uint32
Csp1Pkcs1v15CrtDec (n1_request_type request_type,
          ResultLocation result_location,
          Uint64 context_handle,
          KeyMaterialInput key_material_input,
          Uint64 key_handle,
          RsaBlockType block_type,
          Uint16 modlength,
          Uint8 * Q,
          Uint8 * Eq,
          Uint8 * P,
          Uint8 * Ep,
          Uint8 * iqmp,
          Uint8 * data,
          Uint8 * result, Uint64 * out_length, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1InitializeRc4
 *
 * \input request_type     CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input key_length     size of key in bytes (1<=length<=256)
 * \input key        pointer to length-byte key 
 *
 * \output request_id   Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1InitializeRc4 (n1_request_type request_type,
         Uint64 context_handle,
         Uint16 key_length, Uint8 * key, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1EncryptRc4
 *
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input context_update  UPDATE or NO_UPDATE
 * \input length     size of input in bytes (0<=length<=2^16-1)
 * \input input        pointer to length-byte input
 *
 * \output output     pointer to length-byte output 
 * \output request_id     Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1EncryptRc4 (n1_request_type request_type,
      Uint64 context_handle,
      ContextUpdate context_update,
      Uint16 length,
      Uint8 * input, Uint8 * output, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Initialize3DES
 *
 * \input request_type      CAVIUM_BLOCKING
 * \input context_handle   64-bit pointer to context (context_handle%8=0)
 * \input iv         pointer to 8-byte initialization vector
 * \input key         pointer to 24-byte key 
 *
 * \output request_id           Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1Initialize3DES (n1_request_type request_type,
          Uint64 context_handle,
          Uint8 * iv, Uint8 * key, Uint32 * request_id);

/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Encrypt3Des
 *
 * \input request_type     CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input context_update  UPDATE or NO_UPDATE
 * \input length     size of input in bytes (0<=length<=2^16-8, length%8=0)
 * \input input        pointer to length-byte input
 * \if MC2
 * \input iv   pointer to 8-byte IV
 * \input key   pointer to 24-byte key
 * \endif
 *
 * 
 * \output output   pointer to ROUNDUP8(length)-byte output, 
 * \output request_id   Unique ID for this request.
 *      
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Encrypt3Des (n1_request_type request_type,
       Uint64 context_handle,
       ContextUpdate context_update,
       Uint16 length,
       Uint8 * input,
       Uint8 * output,
       Uint8 * iv, Uint8 * key, Uint32 * request_id);
#else
Uint32
Csp1Encrypt3Des (n1_request_type request_type,
       Uint64 context_handle,
       ContextUpdate context_update,
       Uint16 length,
       Uint8 * input, Uint8 * output, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1Decrypt3Des
 *
 * \input request_type    CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input context_update  UPDATE or NO_UPDATE
 * \input length     size of input in bytes (length%8=0, 0<=length<=2^16-1)
 * \input input        pointer to length-byte input
 * \if MC2
 * \input iv        pointer to 8-byte IV
 * \input key        pointer to 24-byte key
 * \endif
 *
 * \output output   pointer to length-byte output, 
 * \output request_id   Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1Decrypt3Des (n1_request_type request_type,
       Uint64 context_handle,
       ContextUpdate context_update,
       Uint16 length,
       Uint8 * input,
       Uint8 * output,
       Uint8 * iv, Uint8 * key, Uint32 * request_id);
#else
Uint32
Csp1Decrypt3Des (n1_request_type request_type,
       Uint64 context_handle,
       ContextUpdate context_update,
       Uint16 length,
       Uint8 * input, Uint8 * output, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1InitializeAES
 *
 * \input request_type     CAVIUM_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input aes_type     AES_128, AES_192, or AES_256
 * \input iv        pointer to 16-byte initialization vector
 * \input key        pointer to key, whose length depends on aes_type 
 *
 * \output request_id   Unique ID for this request. (ignored)
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
Uint32
Csp1InitializeAES (n1_request_type request_type,
         Uint64 context_handle,
         AesType aes_type,
         Uint8 * iv, Uint8 * key, Uint32 * request_id);


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1EncryptAes
 *
 * \input request_type      CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle   64-bit pointer to context (context_handle%8=0)
 * \input context_update   UPDATE or NO_UPDATE
 * \input aes_type      AES_128, AES_192, or AES_256
 * \input length      size of input in bytes (0<=length<=2^16-1)
 * \input input         pointer to length-byte input
 * \if MC2
 * \input iv      pointer to 16- byte IV
 * \input key      pointer to key depending upon aes type
 * \endif
 *
 * \output output   pointer to ROUNDUP16(length)-byte output
 * \output request_id   Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1EncryptAes (n1_request_type request_type,
      Uint64 context_handle,
      ContextUpdate context_update,
      AesType aes_type,
      Uint16 length,
      Uint8 * input,
      Uint8 * output, Uint8 * iv, Uint8 * key, Uint32 * request_id);
#else
Uint32
Csp1EncryptAes (n1_request_type request_type,
      Uint64 context_handle,
      ContextUpdate context_update,
      AesType aes_type,
      Uint16 length,
      Uint8 * input, Uint8 * output, Uint32 * request_id);
#endif


/*+****************************************************************************/
/*! \ingroup GP_OPS
 * Csp1DecryptAes
 *
 * \input request_type     CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 * \input context_handle  64-bit pointer to context (context_handle%8=0)
 * \input context_update  UPDATE or NO_UPDATE
 * \input aes_type     AES_128, AES_192, or AES_256
 * \input length     size of input in bytes (length%16=0, 0<=length<=2^16-1)
 * \input input        pointer to length-byte input
 * \if MC2
 * \input iv        pointer to 16- byte IV
 * \input key        pointer to key depending upon aes type
 * \endif
 *
 * \output output    pointer to length-byte output
 * \output request_id    Unique ID for this request.
 *
 * \return Status
 * \retval SUCCESS 0
 * \retval FAILURE/PENDING #Csp1ErrorCodes
 *
 */
/*-***************************************************************************/
#ifdef MC2
Uint32
Csp1DecryptAes (n1_request_type request_type,
      Uint64 context_handle,
      ContextUpdate context_update,
      AesType aes_type,
      Uint16 length,
      Uint8 * input,
      Uint8 * output, Uint8 * iv, Uint8 * key, Uint32 * request_id);
#else
Uint32
Csp1DecryptAes (n1_request_type request_type,
      Uint64 context_handle,
      ContextUpdate context_update,
      AesType aes_type,
      Uint16 length,
      Uint8 * input, Uint8 * output, Uint32 * request_id);
#endif
#ifdef MC2
/*+****************************************************************************
 *
 * Csp1Hash
 *
 * Compute the HASH of a complete message. Does not use context.
 *
 * Input
 *    request_type = CAVIUM_BLOCKING or CAVIUM_NON_BLOCKING
 *   hash_type = MD5_TYPE or SHA1_TYPE 
 *   message_length = size of input in bytes (0<=message_length<=2^16-1)
 *   message = pointer to length bytes of input to be HMACed
 *
 * Output
 *   hash = pointer to the hash_size HASH result 
 *   request_id = Unique ID for this request.
 *
 * Return Value
 *    0  = success 
 *    >0 = failure or pending
 *    see error_codes.txt
 *
 *-***************************************************************************/
Uint32 
Csp1Hash(n1_request_type request_type,
    HashType hash_type, 
    Uint16 message_length, 
    Uint8 *message, 
    Uint8 *hash,
    Uint32 *request_id);
    
//add by lijing, kernel call API
Uint32 
Kernel_Csp1ReadEpci(n1_request_type request_type,
    Uint64 *key_handle, 
    Uint16 length,
    Uint8* data,
    Uint32 *request_id,
    void (*call_back)(int, void *),
    void *connect);
Uint32 
Kernel_Csp1WriteEpci(n1_request_type request_type,
          Uint64 *key_handle, 
          Uint16 length,
          Uint8 *data,
          Uint32 *request_id,
		    void (*call_back)(int, void *),
		    void *connect);
Uint32 
Kernel_Csp1ReadContext(n1_request_type request_type,
      Uint64 context_handle, 
      Uint16 length,
      Uint8* data,
      Uint32 *request_id,
      void (*call_back)(int, void*),
      void *connect);
Uint32 
Kernel_Csp1WriteContext(n1_request_type request_type,
       Uint64 context_handle, 
       Uint16 length,
       Uint8 *data,
       Uint32 *request_id,
       void (*call_back)(int, void*),
       void *connect);
Uint32 
Kernel_Csp1Random(n1_request_type request_type,
      Uint16 length, 
      Uint8* random,
      Uint32 *request_id,
      void (*call_back)(int, void *),
      void *connect);
Uint32 
Kernel_Csp1Hmac(n1_request_type request_type,
    HashType hash_type, 
    Uint16 key_length, 
    Uint8 *key, 
    Uint16 message_length, 
    Uint8 *message, 
    Uint8* hmac,
    Uint32 *request_id,
    void (*call_back)(int, void *),
    void *connect);
Uint32 
Kernel_Csp1HmacStart(n1_request_type request_type,
           Uint64 context_handle, 
           HashType hash_type, 
           Uint16 key_length, 
           Uint8 *key, 
           Uint16 message_length, 
           Uint8 *message,
           Uint32 *request_id,
           void (*call_back)(int,void*),
           void* connect);
Uint32 
Kernel_Csp1HmacUpdate(n1_request_type request_type,
            Uint64 context_handle, 
            HashType hash_type, 
            Uint16 message_length, 
            Uint8 *message,
            Uint32 *request_id,
            void (*call_back)(int ,void*),
            void *connect);
Uint32 
Kernel_Csp1HmacFinish(n1_request_type request_type,
      Uint64 context_handle, 
      HashType hash_type, 
      Uint16 message_length, 
      Uint8 *message, 
      Uint8* final_hmac,
      Uint32 *request_id,
      void (*call_back)(int ,void*),
      void *connect);
Uint32 
Kernel_Csp1Me(n1_request_type request_type,
      Uint16 modlength,
      Uint16 explength,
      Uint16 datalength,
      Uint8 *modulus,
      Uint8 *exponent,
      Uint8 *data, 
      Uint8* result,
      Uint32 *request_id,
      void (*call_back)(int, void*),
      void *connect);
Uint32 
Kernel_Csp1Pkcs1v15Enc(n1_request_type request_type,
            RsaBlockType block_type,
            Uint16 modlength, 
            Uint16 explength,
            Uint16 datalength, 
            Uint8 *modulus, 
            Uint8 *exponent, 
            Uint8 *data,
            Uint8 *result,
            Uint32 *request_id,
            void (*call_back)(int, void *),
            void* connect);
Uint32 
Kernel_Csp1Pkcs1v15CrtEnc(n1_request_type request_type,
               RsaBlockType block_type,
               Uint16 modlength, 
               Uint16 datalength, 
               Uint8 *Q, 
               Uint8 *Eq, 
               Uint8 *P, 
               Uint8 *Ep, 
               Uint8 *iqmp, 
               Uint8 *data,
               Uint8* result,
               Uint32 *request_id,
               void (*call_back)(int, void*),
               void *connect);
Uint32 
Kernel_Csp1Pkcs1v15Dec(n1_request_type request_type,
            RsaBlockType block_type,
            Uint16 modlength, 
            Uint16 explength,
            Uint8 *modulus, 
            Uint8 *exponent, 
            Uint8 *data,
            Uint16 *out_length,
            Uint8* result,
            Uint32 *request_id,
            void (*call_back)(int, void*),
            void *connect);
Uint32 
Kernel_Csp1InitializeRc4(n1_request_type request_type,
              Uint64 context_handle, 
              Uint16 key_length, 
              Uint8 *key,
              Uint32 *request_id,
              void (*call_back)(int, void*),
              void* connect);
Uint32 
Kernel_Csp1EncryptRc4(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            Uint16 length, 
            Uint8 *input, 
            Uint8* output,
            Uint32 *request_id,
              void (*call_back)(int, void*),
              void* connect);
Uint32 
Kernel_Csp1Initialize3DES(n1_request_type request_type,
               Uint64 context_handle, 
               Uint8 *iv, 
               Uint8 *key,
               Uint32 *request_id);
Uint32 
Csp1InitializeAES(n1_request_type request_type,
              Uint64 context_handle, 
              AesType aes_type, 
              Uint8 *iv, 
              Uint8 *key,
              Uint32 *request_id);
Uint32 Kernel_Csp1Hash(n1_request_type request_type,
    HashType hash_type, 
    Uint16 message_length, 
    Uint8 *message, 
    Uint8* hash,
    Uint32 *request_id,
	void (*call_back)(int, void*),
	void *connect);
Uint32 
Kernel_Csp1DecryptAes(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            AesType aes_type, 
            Uint16 length, 
            Uint8 *input, 
            Uint8* output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void *),
			void *connect);
Uint32 
Kernel_Csp1EncryptAes(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            AesType aes_type, 
            Uint16 length, 
            Uint8 *input, 
            Uint8* output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void *),
			void *connect);
Uint32 
Kernel_Csp1Decrypt3Des(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            Uint16 length, 
            Uint8 *input, 
            Uint8* output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void *),
			void *connect);
Uint32 
Kernel_Csp1Encrypt3Des(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            Uint16 length, 
            Uint8 *input, 
            Uint8 *output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void*),
			void *connect);
Uint32 
Kernel_Csp1Pkcs1v15CrtDec(n1_request_type request_type,
               RsaBlockType block_type,
               Uint16 modlength, 
               Uint8 *Q, 
               Uint8 *Eq, 
               Uint8 *P, 
               Uint8 *Ep, 
               Uint8 *iqmp, 
               Uint8 *data,
               Uint16 *out_length,
               Uint8* result,
               Uint32 *request_id,
			   void (*call_back)(int, void *),
			   void *connect);
    
static inline int cavium_copy_in(unsigned char *dest, unsigned char *src, int size)
{
	int ret=0;
	//printk("kernel_call is %d\n", kernel_call);
	if(kernel_call){
		memcpy(dest, src, size);
	}else{
		ret=copy_from_user(dest, src, size);
	}
	return ret;
}

static inline int cavium_copy_out(unsigned char *dest, unsigned char *src, int size)
{
	int ret=0;
	//printk("kernel_call is %d\n", kernel_call);
	if(kernel_call){
		memcpy(dest, src, size);
	}else{
		ret=copy_to_user(dest, src, size);
	}
	return ret;
}	
    
//end add by ljing, kernel call API    
    
    
    
    
    
    
    

#endif
#define INC32(a)        {\
            Uint32 __tmp__    = (Uint32)a;\
            __tmp__    = (__tmp__ + 1)&0xffffffffL;\
            a       = __tmp__;\
         }

//#endif /*CSP1_KERNEL */

#endif /* _CAVIUM_COMMON_H_ */

/*
 * $Id: cavium_common.h,v 1.4 2015/01/06 08:57:46 andy Exp $
 * $Log: cavium_common.h,v $
 * Revision 1.4  2015/01/06 08:57:46  andy
 * *** empty log message ***
 *
 * Revision 1.3  2009-03-23 03:06:35  cding
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
 * Revision 1.6  2006/06/23 09:57:50  lixiaox
 * mod by lxx.Change the comment.
 *
 * Revision 1.5  2006/06/09 05:54:40  liqin
 * *** empty log message ***
 *
 * Revision 1.4  2006/04/21 06:47:41  liqin
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
 * Revision 1.28  2005/10/20 10:03:11  phegde
 * - Added 2 new function prototypes called Csp1WriteIpsecSa() and Csp1ProcessPacket() to support for IPSec functionality
 *
 * Revision 1.27  2005/10/13 08:56:40  ksnaren
 * removed compile warning
 *
 * Revision 1.26  2005/09/28 15:53:37  ksadasivuni
 * - Merging FreeBSD 6.0 AMD64 release with CVS Head
 *
 * Revision 1.25  2005/09/27 05:29:50  sgadam
 * Compilation error in FC4 fixed
 *
 * Revision 1.24  2005/09/21 06:37:44  lpathy
 * Merging windows server 2003 release with CVS head
 *
 * Revision 1.23  2005/09/08 12:56:26  sgadam
 * - Csp1Hash prototype Added
 *
 * Revision 1.22  2005/06/03 07:29:46  rkumar
 * Priority associated commands in SRQ
 *
 * Revision 1.21  2005/02/01 04:12:05  bimran
 * copyright fix
 *
 * Revision 1.20  2005/01/06 18:43:32  mvarga
 * Added realtime support
 *
 * Revision 1.19  2004/06/23 19:06:20  bimran
 * NetBSD port.
 * Fixed cavium_dump to become OSI
 *
 * Revision 1.18  2004/05/04 00:27:02  danny
 * 2.00b documentation ver 0.01, doxygen ccmment corrections
 *
 * Revision 1.17  2004/05/03 23:30:08  danny
 * Added Doxygen formating to header files
 *
 * Revision 1.16  2004/05/03 22:33:21  danny
 * Added Doxygen formating to header files
 *
 * Revision 1.15  2004/05/03 20:33:13  bimran
 * Removed all references to CAVIUM_IKE context_type.
 *
 * Revision 1.14  2004/05/03 19:53:57  bimran
 * Added all error codes.
 *
 * Revision 1.13  2004/05/02 19:45:59  bimran
 * Added Copyright notice.
 *
 * Revision 1.12  2004/05/01 07:15:35  bimran
 * Added non-blocking error codes.
 *
 * Revision 1.11  2004/05/01 05:58:06  bimran
 * Fixed a function descriptions on each function to match with the latest microcode and driver.
 *
 * Revision 1.10  2004/04/28 03:14:59  bimran
 * Fixed comments.
 *
 * Revision 1.9  2004/04/26 23:29:23  bimran
 * Removed unused data types.
 *
 * Revision 1.8  2004/04/26 22:32:52  tsingh
 * Fixed some typedefs for MC2 (bimran).
 *
 * Revision 1.7  2004/04/23 21:49:34  bimran
 * Csp1Initialze now accepts microcode type to support Plus mode.
 *
 * Revision 1.6  2004/04/22 02:49:34  bimran
 * Removed enumerated error codes from microcode.
 *
 * Revision 1.5  2004/04/22 01:12:11  bimran
 * Moved NPLUS related structures around to avoid compilation problems with user mode programs like nplus_init.
 *
 * Revision 1.4  2004/04/21 20:00:29  bimran
 * NPLUS support.
 *
 * Revision 1.3  2004/04/20 17:45:11  bimran
 * Defined microcode structure.
 * Some early NPLUS related changes.
 *
 * Revision 1.2  2004/04/17 01:37:49  bimran
 * Fixed includes.
 * Added function protos.
 *
 * Revision 1.1  2004/04/15 22:40:50  bimran
 * Checkin of the code from India with some cleanups.
 *
 */

