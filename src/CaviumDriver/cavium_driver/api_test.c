////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: api_test.c
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
// 3. All manuals, brochures, user guides mentioning features or use of this software, 
//    and the About menu item in the software application must display the following
//    acknowledgement:
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

#include "cavium_sysdep.h"
#include "cavium_common.h"
#include "cavium_endian.h"
#include "cavium_list.h"
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

#include "api_test.h"

#define MAX_COMPARE_DATA 256


typedef struct _cb_data
{
	Uint8 cmp_data[MAX_COMPARE_DATA];
	int cmp_len;

	Uint8* in_data;
	Uint8* out_data;
} cb_data;


static int done;
static cavium_wait_channel wait;
static Uint32 ctx;

static void api_test_cb(int, void*);

static void random_test(cavium_device *pd);
static void des3_encrypt_test(cavium_device *pd);
static void aes_encrypt_test(cavium_device *pd);
static void rc4_encrypt_test(cavium_device *pd);
static void rc4_initialize_test(cavium_device *pd);
static void modexp_test(cavium_device *pd);

/*
 * This driver only supports asyncronous.
 */
int api_test(cavium_device *pd)
{
	init_waitqueue_head(&wait);
	ctx = alloc_context(pd,CONTEXT_IPSEC);

	random_test(pd);
	des3_encrypt_test(pd);
	aes_encrypt_test(pd);

	rc4_initialize_test(pd);
	rc4_encrypt_test(pd);

	modexp_test(pd);

	dealloc_context(pd, CONTEXT_IPSEC, ctx);
	
	return 0;
}

static void api_test_cb(int s, void* data)
{
	cb_data* cbd = (cb_data*)data;

	if (cbd->cmp_len)
	{
		if (!memcmp(cbd->out_data,cbd->cmp_data,cbd->cmp_len))
			printk("api_test_cb: compare passed\n");
		else
			printk("api_test_cb: compare failed\n");
	}
	else
		printk("api_test_cb: compare not done\n");

	if (cbd->in_data)
		cavium_free_dma(cbd->in_data);
	if (cbd->out_data)
		cavium_free_dma(cbd->out_data);
	cavium_free(cbd);
	
	done = 1;
	wake_up_interruptible(&wait);
}

static void random_test(cavium_device *pd)
{
	n1_request_buffer n1_buf;
	Uint32 req_id;
	cb_data* cbd;
	int i;

#ifdef NPLUS
	int ucode_idx;

//	ucode_idx = IPSEC_MLM_IDX;
	ucode_idx = SSL_SPM_IDX;
#endif
	printk("api_test: random_test started\n");

	cbd = (cb_data*)cavium_malloc(sizeof(cb_data),0);
	if (!cbd)
	{
		printk("api_test: random_test failed\n");
		return;
	}
	memset(cbd,0,sizeof(cb_data));

	cbd->out_data = cavium_malloc_dma(RANDOM_DATA_LEN + 8, 0);
	if (!cbd->out_data)
	{
		cavium_free(cbd);
		printk("api_test: random_test failed\n");
		return;
	}

	cavium_memset(&n1_buf, 0, sizeof(n1_buf));

	n1_buf.opcode = RANDOM_OP;
	n1_buf.size = RANDOM_DATA_LEN;
	n1_buf.param = 0;
	n1_buf.dlen = 0;
	n1_buf.rlen = RANDOM_DATA_LEN;
	n1_buf.reserved = 0;
	n1_buf.ctx_ptr = 0;
	n1_buf.incnt = 0;
	n1_buf.outcnt = 1;
	n1_buf.inptr[0] = NULL;
	n1_buf.outptr[0] = (Uint8 *)cbd->out_data;
	n1_buf.dma_mode = CAVIUM_DIRECT;
	n1_buf.res_order = 0;
	n1_buf.req_queue = 0;
	n1_buf.callback = api_test_cb;
	n1_buf.cb_arg = cbd;
#ifdef NPLUS
	n1_buf.ucode_idx = ucode_idx;
#endif

	done = 0;
	i = 0;

	if (do_request(pd, &n1_buf, &req_id)) 
	{
		cavium_free_dma(cbd->out_data);
		cavium_free(cbd);
		printk("api_test: random_test failed\n");

		return;
	}

	while((!done) && (i++ < 20))
		cavium_sleep_timeout(&wait,100);

	printk("api_test: random_test finished\n");
}

static void des3_encrypt_test(cavium_device *pd)
{
	n1_request_buffer n1_buf;
	Uint32 req_id;
	cb_data* cbd;
	int i;
	Uint8 *key = des3_key; 
	Uint8 *iv = des3_iv; 
	Uint8 *cmp_data = des3_cmp_data; 
	Uint8* data =  des3_data;

#ifdef NPLUS
	int ucode_idx;

//	ucode_idx = IPSEC_MLM_IDX;
	ucode_idx = SSL_SPM_IDX;
#endif
	printk("api_test: des3_encrypt_test started\n");

	cbd = (cb_data*)cavium_malloc(sizeof(cb_data),0);
	if (!cbd)
	{
		printk("api_test: des3_encrypt_test failed\n");
		return;
	}
	memset(cbd,0,sizeof(cb_data));

	cbd->in_data = cavium_malloc_dma(DES_DATA_LEN + 8 + 24, 0);
	if (!cbd->in_data)
	{
		cavium_free(cbd);
		printk("api_test: des3_encrypt_test failed\n");
		return;
	}
	memcpy(cbd->in_data,iv,8);
	memcpy(cbd->in_data+8,key,24);
	memcpy(cbd->in_data+32,data,DES_DATA_LEN);

	cbd->out_data = cavium_malloc_dma(DES_DATA_LEN + 8, 0);
	if (!cbd->out_data)
	{
		cavium_free(cbd);
		printk("api_test: des3_encrypt_test failed\n");
		return;
	}

	cbd->cmp_len = DES_DATA_LEN;
	memcpy(cbd->cmp_data,cmp_data,cbd->cmp_len);

	cavium_memset(&n1_buf, 0, sizeof(n1_buf));

	n1_buf.opcode = DES_OP;
	n1_buf.size = 0;
	n1_buf.param = 0;
	n1_buf.dlen = DES_DATA_LEN + 8 + 24;
	n1_buf.rlen = DES_DATA_LEN;
	n1_buf.reserved = 0;
	n1_buf.ctx_ptr = 0;
	n1_buf.incnt = 1;
	n1_buf.outcnt = 1;
	n1_buf.inptr[0] = cbd->in_data;
	n1_buf.outptr[0] = (Uint8 *)cbd->out_data;
	n1_buf.dma_mode = CAVIUM_DIRECT;
	n1_buf.res_order = 0;
	n1_buf.req_queue = 0;
	n1_buf.callback = api_test_cb;
	n1_buf.cb_arg = cbd;
#ifdef NPLUS
	n1_buf.ucode_idx = ucode_idx;
#endif

	done = 0;
	i = 0;

	if (do_request(pd, &n1_buf, &req_id)) 
	{
		cavium_free_dma(cbd->out_data);
		cavium_free(cbd);
		printk("api_test: des3_encrypt_test failed\n");

		return;
	}

	while((!done) && (i++ < 20))
		cavium_sleep_timeout(&wait,100);

	printk("api_test: des3_encrypt_test finished\n");
}

static void aes_encrypt_test(cavium_device *pd)
{
	n1_request_buffer n1_buf;
	Uint32 req_id;
	cb_data* cbd;
	int i;
	Uint8* key = aes_key; 
	Uint8* iv = aes_iv;
	Uint8* cmp_data =  aes_cmp_data;
	Uint8* data =  aes_data;

#ifdef NPLUS
	int ucode_idx;

//	ucode_idx = IPSEC_MLM_IDX;
	ucode_idx = SSL_SPM_IDX;
#endif
	printk("api_test: aes_encrypt_test started\n");

	cbd = (cb_data*)cavium_malloc(sizeof(cb_data),0);
	if (!cbd)
	{
		printk("api_test: aes_encrypt_test failed\n");
		return;
	}
	memset(cbd,0,sizeof(cb_data));

	cbd->in_data = cavium_malloc_dma(AES_DATA_LEN + 16 + 32, 0);
	if (!cbd->in_data)
	{
		cavium_free(cbd);
		printk("api_test:  aes_encrypt_test failed\n");
		return;
	}
	memcpy(cbd->in_data,iv,16);
	memcpy(cbd->in_data+16,key,32);
	memcpy(cbd->in_data+16+32,data,AES_DATA_LEN);

	cbd->out_data = cavium_malloc_dma(AES_DATA_LEN +8, 0);
	if (!cbd->out_data)
	{
		cavium_free(cbd);
		printk("api_test:  aes_encrypt_test failed\n");
		return;
	}

	cbd->cmp_len = AES_DATA_LEN;
	memcpy(cbd->cmp_data,cmp_data,cbd->cmp_len);

	cavium_memset(&n1_buf, 0, sizeof(n1_buf));

	n1_buf.opcode = AES_OP;
	n1_buf.size = 7;
	n1_buf.param = 0;
	n1_buf.dlen = AES_DATA_LEN + 16 +32;
	n1_buf.rlen = AES_DATA_LEN;
	n1_buf.reserved = 0;
	n1_buf.ctx_ptr = 0;
	n1_buf.incnt = 1;
	n1_buf.outcnt = 1;
	n1_buf.inptr[0] = cbd->in_data;
	n1_buf.outptr[0] = (Uint8 *)cbd->out_data;
	n1_buf.dma_mode = CAVIUM_DIRECT;
	n1_buf.res_order = 0;
	n1_buf.req_queue = 0;
	n1_buf.callback = api_test_cb;
	n1_buf.cb_arg = cbd;
#ifdef NPLUS
	n1_buf.ucode_idx = ucode_idx;
#endif

	done = 0;
	i = 0;

	if (do_request(pd, &n1_buf, &req_id)) 
	{
		cavium_free_dma(cbd->out_data);
		cavium_free(cbd);
		printk("api_test:  aes_encrypt_test failed\n");

		return;
	}

	while((!done) && (i++ < 20))
		cavium_sleep_timeout(&wait,100);

	printk("api_test:  aes_encrypt_test finished\n");
}


static void rc4_initialize_test(cavium_device *pd)
{
	n1_request_buffer n1_buf;
	Uint32 req_id;
	cb_data* cbd;
	int i;
	Uint8* key = rc4_key; 

#ifdef NPLUS
	int ucode_idx;

//	ucode_idx = IPSEC_MLM_IDX;
	ucode_idx = SSL_SPM_IDX;
#endif
	printk("api_test: rc4_initialize_test started\n");

	cbd = (cb_data*)cavium_malloc(sizeof(cb_data),0);
	if (!cbd)
	{
		printk("api_test: rc4_initialize_test failed\n");
		return;
	}
	memset(cbd,0,sizeof(cb_data));

	cbd->in_data = cavium_malloc_dma(RC4_KEY_LEN + 8, 0);
	if (!cbd->in_data)
	{
		cavium_free(cbd);
		printk("api_test:  rc4_initialize_test failed\n");
		return;
	}
	memcpy(cbd->in_data,key,RC4_KEY_LEN);

	cbd->out_data = cavium_malloc_dma(8, 0);
	if (!cbd->out_data)
	{
		cavium_free(cbd);
		printk("api_test:  rc4_initialize_test failed\n");
		return;
	}

	cbd->cmp_len = 0;

	cavium_memset(&n1_buf, 0, sizeof(n1_buf));

	n1_buf.opcode = RC4_INITIALIZE_OP;
	n1_buf.size = 0;
	n1_buf.param = 0;
	n1_buf.dlen = RC4_KEY_LEN;
	n1_buf.rlen = 0;
	n1_buf.reserved = 0;
	n1_buf.ctx_ptr = ctx;
	n1_buf.incnt = 1;
	n1_buf.outcnt = 1;
	n1_buf.inptr[0] = cbd->in_data;
	n1_buf.outptr[0] = cbd->out_data;
	n1_buf.dma_mode = CAVIUM_DIRECT;
	n1_buf.res_order = 0;
	n1_buf.req_queue = 0;
	n1_buf.callback = api_test_cb;
	n1_buf.cb_arg = cbd;
#ifdef NPLUS
	n1_buf.ucode_idx = ucode_idx;
#endif

	done = 0;
	i = 0;

	if (do_request(pd, &n1_buf, &req_id)) 
	{
		cavium_free_dma(cbd->out_data);
		cavium_free_dma(cbd->in_data);
		cavium_free(cbd);
		printk("api_test:  rc4_initialize_test failed\n");

		return;
	}

	while((!done) && (i++ < 20))
		cavium_sleep_timeout(&wait,100);

	printk("api_test:  rc4_initialize_test finished\n");
}

static void rc4_encrypt_test(cavium_device *pd)
{
	n1_request_buffer n1_buf;
	Uint32 req_id;
	cb_data* cbd;
	int i;
	Uint8* cmp_data =  rc4_cmp_data;
	Uint8* data =  rc4_data;

#ifdef NPLUS
	int ucode_idx;

//	ucode_idx = IPSEC_MLM_IDX;
	ucode_idx = SSL_SPM_IDX;
#endif
	printk("api_test: rc4_encrypt_test started\n");

	cbd = (cb_data*)cavium_malloc(sizeof(cb_data),0);
	if (!cbd)
	{
		printk("api_test: rc4_encrypt_test failed\n");
		return;
	}
	memset(cbd,0,sizeof(cb_data));

	cbd->in_data = cavium_malloc_dma(RC4_DATA_LEN, 0);
	if (!cbd->in_data)
	{
		cavium_free(cbd);
		printk("api_test:  rc4_encrypt_test failed\n");
		return;
	}
	memcpy(cbd->in_data,data,RC4_DATA_LEN);

	cbd->out_data = cavium_malloc_dma(RC4_DATA_LEN + 8, 0);
	if (!cbd->out_data)
	{
		cavium_free(cbd);
		printk("api_test:  rc4_encrypt_test failed\n");
		return;
	}

	cbd->cmp_len = RC4_DATA_LEN;
	memcpy(cbd->cmp_data,cmp_data,cbd->cmp_len);

	cavium_memset(&n1_buf, 0, sizeof(n1_buf));

	n1_buf.opcode = RC4_OP;
	n1_buf.size = 0;
	n1_buf.param = 0;
	n1_buf.dlen = RC4_DATA_LEN;
	n1_buf.rlen = RC4_DATA_LEN;
	n1_buf.reserved = 0;
	n1_buf.ctx_ptr = ctx;
	n1_buf.incnt = 1;
	n1_buf.outcnt = 1;
	n1_buf.inptr[0] = cbd->in_data;
	n1_buf.outptr[0] = (Uint8 *)cbd->out_data;
	n1_buf.dma_mode = CAVIUM_DIRECT;
	n1_buf.res_order = 0;
	n1_buf.req_queue = 0;
	n1_buf.callback = api_test_cb;
	n1_buf.cb_arg = cbd;
#ifdef NPLUS
	n1_buf.ucode_idx = ucode_idx;
#endif

	done = 0;
	i = 0;

	if (do_request(pd, &n1_buf, &req_id)) 
	{
		cavium_free_dma(cbd->out_data);
		cavium_free(cbd);
		printk("api_test:  rc4_encrypt_test failed\n");

		return;
	}

	while((!done) && (i++ < 20))
		cavium_sleep_timeout(&wait,100);

	printk("api_test:  rc4_encrypt_test finished\n");
}

static void modexp_test(cavium_device *pd)
{
	n1_request_buffer n1_buf;
	Uint32 req_id;
	cb_data* cbd;
	int i;
	Uint8* cmp_data =  modexp_cmp_data;
	Uint8* data =  modexp_data;
	Uint8* exp =  modexp_exp;
	Uint8* mod =  modexp_mod;

#ifdef NPLUS
	int ucode_idx;

//	ucode_idx = IPSEC_MLM_IDX;
	ucode_idx = SSL_SPM_IDX;
#endif
	printk("api_test: modexp_test started\n");

	cbd = (cb_data*)cavium_malloc(sizeof(cb_data),0);
	if (!cbd)
	{
		printk("api_test: modexp_test failed\n");
		return;
	}
	memset(cbd,0,sizeof(cb_data));

	cbd->in_data = cavium_malloc_dma(MODEXP_LEN*3, 0);
	if (!cbd->in_data)
	{
		cavium_free(cbd);
		printk("api_test:  modexp_test failed\n");
		return;
	}
	memcpy(cbd->in_data,mod,MODEXP_LEN);
	memcpy(cbd->in_data+MODEXP_LEN,exp,MODEXP_LEN);
	memcpy(cbd->in_data+MODEXP_LEN*2,data,MODEXP_LEN);

	cbd->out_data = cavium_malloc_dma(MODEXP_LEN + 8, 0);
	if (!cbd->out_data)
	{
		cavium_free(cbd);
		printk("api_test:  modexp_test failed\n");
		return;
	}

	cbd->cmp_len = MODEXP_LEN;
	memcpy(cbd->cmp_data,cmp_data,cbd->cmp_len);

	cavium_memset(&n1_buf, 0, sizeof(n1_buf));

	n1_buf.opcode = MODEXP_OP;
	n1_buf.size = MODEXP_LEN;
	n1_buf.param = MODEXP_LEN;
	n1_buf.dlen = MODEXP_LEN * 3;
	n1_buf.rlen = MODEXP_LEN;
	n1_buf.reserved = 0;
	n1_buf.ctx_ptr = 0;
	n1_buf.incnt = 1;
	n1_buf.outcnt = 1;
	n1_buf.inptr[0] = cbd->in_data;
	n1_buf.outptr[0] = (Uint8 *)cbd->out_data;
	n1_buf.dma_mode = CAVIUM_DIRECT;
	n1_buf.res_order = 0;
	n1_buf.req_queue = 0;
	n1_buf.callback = api_test_cb;
	n1_buf.cb_arg = cbd;
#ifdef NPLUS
	n1_buf.ucode_idx = ucode_idx;
#endif

	done = 0;
	i = 0;

	if (do_request(pd, &n1_buf, &req_id)) 
	{
		cavium_free_dma(cbd->out_data);
		cavium_free(cbd);
		printk("api_test:  modexp_test failed\n");

		return;
	}

	while((!done) && (i++ < 20))
		cavium_sleep_timeout(&wait,100);

	printk("api_test:  modexp_test finished\n");
}
