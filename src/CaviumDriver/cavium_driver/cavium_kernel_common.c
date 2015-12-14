////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: cavium_kernel_common.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
 #include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
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

#include "linux_main.h"
//#include "cavium_kernel_common.h"
#ifndef SSL
#define OP_WRITE_IPSEC_SA         0x14
#define OP_IPSEC_PACKET_INBOUND       0x10
#define OP_IPSEC_PACKET_OUTBOUND      0x11
#define OP_WRITE_INBOUND_IPSEC_SA      0x2014
#define OP_WRITE_OUTBOUND_IPSEC_SA      0x4014
#define OP_ERASE_CONTEXT         0x114
#define IPv4    0
#endif

int CSP1_driver_handle=-1;
Csp1DmaMode global_dma_mode=CAVIUM_DIRECT; /* default mode */
extern int kernel_call;
extern cavium_device cavium_dev[];


#ifdef NPLUS
Uint32 
Csp1Initialize(Csp1DmaMode dma_mode, int microcode_type)
#else
Uint32 
Csp1Initialize(Csp1DmaMode dma_mode)
#endif
{
   Uint32 cond_code = 0;

   if (CSP1_driver_handle < 0)
   {
#ifdef NPLUS
      if(microcode_type == ADMIN_IDX)
         CSP1_driver_handle = n1_kernel_open();
      else if(microcode_type == SSL_MLM_IDX)
         CSP1_driver_handle = n1_kernel_open();
      else if(microcode_type == SSL_SPM_IDX)
         CSP1_driver_handle = n1_kernel_open();
      else if(microcode_type == IPSEC_MLM_IDX)
         CSP1_driver_handle = n1_kernel_open();
      else if(microcode_type == IPSEC_SPM_IDX)
         CSP1_driver_handle = n1_kernel_open();
      else
         CSP1_driver_handle = -1;
         
#else
      CSP1_driver_handle = n1_kernel_open();
#endif

      if (CSP1_driver_handle < 0) 
         cond_code = -1;
      else {
         global_dma_mode = dma_mode;
         cond_code = 0;
      }
   }

   return cond_code;
}



Uint32 
Csp1Shutdown(void)
{
   Uint32 cond_code = 0;

   if(CSP1_driver_handle != -1)
      n1_kernel_release(CSP1_driver_handle);

   CSP1_driver_handle = -1;
   global_dma_mode=CAVIUM_DIRECT; /*default mode */

   return cond_code;
}



Uint32
Csp1CheckForCompletion(Uint32 request_id)
{
   Uint32 cond_code;
   Uint32 ret_val;


   cond_code = n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_POLL_CODE, request_id);
   ret_val = cond_code;

   return ret_val;
}


Uint32 
Csp1FlushAllRequests(void)
{
   Uint32 cond_code;
   
   cond_code = n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_FLUSH_ALL_CODE, 0);

   return cond_code;
}

Uint32 
Csp1FlushRequest(Uint32 request_id)
{
   Uint32 cond_code;

   cond_code = 
      n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_FLUSH_CODE, (Uint32)request_id);
   return cond_code;
}


Uint32 
Csp1AllocContext(ContextType cntx_type, 
       Uint64 *context_handle)
{
   Csp1OperationBuffer buffer;
   Uint32 cond_code;
   Uint32 ret_val;
   n1_context_buf cbuf;

   cbuf.type = cntx_type;

   buffer.opcode = 0;
   /* 
    * Set the context size to be allocated. 
    */
   buffer.size = cntx_type;
   buffer.param = 0;
   buffer.dlen = 0;
   buffer.rlen = 0;

   buffer.incnt = 0;      
   buffer.outcnt = 0;      

   cond_code = n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_ALLOC_CONTEXT, (ptrlong)&cbuf);

    if (cond_code) {
        ret_val=cond_code;
    } else { /* success*/   
        ret_val = 0;
        *context_handle = cbuf.ctx_ptr;
    }
   return ret_val;
}

Uint32 
Csp1FreeContext(ContextType cntx_type, 
      Uint64 context_handle)
{
   Csp1OperationBuffer buffer;
   Uint32 cond_code;
   Uint32 ret_val;
   n1_context_buf cbuf;


   if ((context_handle & 0x8000000000000000LL) != 0)
   {
      if ((context_handle & 0xf) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   }
   else
   {
      if ((context_handle & 0x7) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   }


   buffer.opcode = 0;
   /* 
    * Set the context type to be deallocated. 
    */
   buffer.size = cntx_type;
   buffer.param = 0;
   buffer.dlen = 0;
   buffer.rlen = 0;

   buffer.incnt = 0;      
   buffer.outcnt = 0;
   buffer.ctx_ptr = context_handle;      

   cbuf.type = cntx_type;
   cbuf.ctx_ptr = context_handle;
   cond_code = n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_FREE_CONTEXT, (ptrlong)&cbuf);

   if (cond_code)
      ret_val=cond_code;
   else /* success*/   
      ret_val = 0;

   return ret_val;
}

Uint32 
Csp1AllocKeyMem(KeyMaterialLocation key_material_loc, Uint64 *key_handle)
{
   Uint32 ret_val;
   n1_write_key_buf key_buf;

   ret_val = n1_kernel_ioctl(CSP1_driver_handle,IOCTL_N1_ALLOC_KEYMEM, (ptrlong)&key_buf);
   *key_handle = key_buf.key_handle;
   return ret_val;
}

Uint32 
Csp1FreeKeyMem(Uint64 key_handle)
{
   Uint32 ret_val;
   n1_write_key_buf key_buf;
#ifdef MC2
   /* turn off crt bit 49 */
   key_handle &= ((((Uint64)0xfffdffff) << 32) | (Uint64)0xffffffff);
#else
   /* turn off crt bit 48 */
   key_handle &= ((((Uint64)0xfffeffff) << 32) | (Uint64)0xffffffff);
#endif

   key_buf.key_handle = key_handle;
   ret_val = n1_kernel_ioctl(CSP1_driver_handle,IOCTL_N1_FREE_KEYMEM, (ptrlong)&key_buf);
   return ret_val;
}



Uint32 
Csp1StoreKey(Uint64 *key_handle, 
    Uint16 length, 
    Uint8 *key, 
    RsaModExType mod_ex_type)
{
    n1_write_key_buf key_buf;
    Uint32 ret_val;

#ifdef MC2
    /* turn off crt bit 49 */
    *key_handle &= ((((Uint64)0xfffdffff) << 32) | (Uint64)0xffffffff);
#else
    /* turn off crt bit 48 */
    *key_handle &= ((((Uint64)0xfffeffff) << 32) | (Uint64)0xffffffff);
#endif
    key_buf.key_handle = *key_handle;
    key_buf.length = length;
    key_buf.key = key;
    ret_val = n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_WRITE_KEYMEM, (ptrlong)&key_buf);

    if(!ret_val) {
   if (mod_ex_type == CRT_MOD_EX)
#ifdef MC2
      *key_handle |= (((Uint64)0x20000) << 32);
#else
      *key_handle |= (((Uint64)0x10000) << 32);
#endif
   }
   return ret_val;
}


Uint32 
Kernel_Csp1ReadEpci(n1_request_type request_type,
    Uint64 *key_handle, 
    Uint16 length,
    Uint8 *data,
    Uint32 *request_id,
    void (*call_back)(int, void *),
    void *connect)
{

	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* output;
	

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
	output=(Uint8*)cavium_malloc(length+8, NULL);
	if(output==NULL){
		printk("no memory\n");
		return -1;
	}

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		cavium_free(output);
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(cb_data);
		cavium_free(output);
		return -1;
	}
	memcpy(cb_data->input,key_handle,8);

	cb_data->in_len=8;
	cb_data->output=output;
	cb_data->out_len=(int)length;
	cb_data->outcnt=1;
	cb_data->outptr[0]=data;
	cb_data->outsize[0]=length;
	cb_data->outunit[0]=UNIT_8_BIT;
	
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0,sizeof(n1_request_buffer));	
   if ((length < 8) || (length > 880) || ((length & 0x7) != 0))
      return ERR_ILLEGAL_INPUT_LENGTH;	
	
   req->opcode = (0x8<<8) | (global_dma_mode<<7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
#ifdef MC2
   req->size = cb_data->out_len;
   req->param = 0;
   req->dlen = 8;
   req->rlen = cb_data->out_len;
#else
   req->size = cb_data->out_len>>3;
   req->param = 0x10;
   req->dlen = (8)>>3;
   req->rlen = (cb_data->out_len + 8)>>3;
#endif
   req->ctx_ptr = 0;

   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = 8;
   req->inoffset[0] = 8;
   req->inunit[0] = UNIT_64_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = cb_data->out_len;
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;	
	
   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		cavium_free(req);
		return -1;
	}	

	return 0;
}


Uint32 
Kernel_Csp1WriteEpci(n1_request_type request_type,
          Uint64 *key_handle, 
          Uint16 length,
          Uint8 *data,
          Uint32 *request_id,
		    void (*call_back)(int, void *),
		    void *connect)
{

	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* output;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	output=(Uint8*)cavium_malloc(8, NULL);
	if(output==NULL){
		printk("no memory\n");
		return -1;
	}
	kernel_call=1;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		cavium_free(output);
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;


	cb_data->input=(Uint8*)cavium_malloc(length+8,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(output);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,(Uint8*)key_handle,8);
	memcpy(cb_data->input+8,data,length);
	cb_data->in_len=8+length;
	cb_data->output=(Uint8*)(output);
	cb_data->outcnt=0;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	
   if ((length < 8) || (length > 880) || ((length & 0x7) != 0))
      return ERR_ILLEGAL_INPUT_LENGTH;	
	
   req->opcode = (0x0<<8) | (global_dma_mode<<7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
#ifdef MC2
   req->size = 0;
   req->param = 0;
   req->dlen = 8 + cb_data->in_len;
   req->rlen = 0;
#else
   req->size = cb_data->in_len>>3;
   req->param = 0x8;
   req->dlen = (8 + cb_data->in_len)>>3;
   req->rlen = (8)>>3;
#endif
   req->ctx_ptr = 0;
   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT;
   req->outptr[0]=cb_data->output;
   
   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = CAVIUM_SCATTER_GATHER;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("rc4_encrypt failed\n");
	}	

	return 0;

}


Uint32 
Kernel_Csp1ReadContext(n1_request_type request_type,
      Uint64 context_handle, 
      Uint16 length,
      Uint8 *data,
      Uint32 *request_id,
      void (*call_back)(int, void*),
      void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* output;

   if ((length < 8) || (length > 1024) || ((length & 0x7) != 0)) 
      return ERR_ILLEGAL_INPUT_LENGTH;

   if ((context_handle & 0x8000000000000000LL) != 0) {
   
      if ((context_handle & 0xf) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {
      if ((context_handle & 0x7) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   }
	kernel_call=1;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}

	output=(Uint8*)cavium_malloc(length+8, NULL);
	if(output==NULL){
		printk("no memory\n");
		return -1;
	}

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		cavium_free(output);
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;
	
	cb_data->output=output;
	cb_data->out_len=length;
	cb_data->outcnt=1;
	cb_data->outptr[0]=data;
	cb_data->outsize[0]=length;
	cb_data->outunit[0]=UNIT_8_BIT;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	




   req->opcode = (0x4<<8) | (global_dma_mode<<7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
#ifdef MC2
   req->size = cb_data->out_len;
#else
   req->size = cb_data->out_len>>3;
#endif
   req->param = 0;
   req->dlen = 0;
#ifdef MC2
   req->rlen = cb_data->out_len;
#else
   req->rlen = (cb_data->out_len + 8)>>3;
#endif
   req->ctx_ptr = context_handle;

   req->incnt = 0;      
   req->outcnt = 1;      

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = ROUNDUP8(cb_data->out_len);
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1ReadContext fail\n");
		return -1;
	}	

	return 0;

}

Uint32 
Csp1WriteContext(n1_request_type request_type,
       Uint64 context_handle, 
       Uint16 length,
       Uint8 *data,
       Uint32 *request_id)
{
   Csp1OperationBuffer buffer;
   Uint32 cond_code;
   Uint32 ret_val;

   if ((length < 8) || (length > 1024) || ((length & 0x7) != 0))
      return ERR_ILLEGAL_INPUT_LENGTH;
   
   if ((context_handle & 0x8000000000000000LL) != 0) {
      if ((context_handle & 0xf) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   } else {
      if ((context_handle & 0x7) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   }
   

   buffer.opcode = (0x2<<8) | (global_dma_mode<<7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
#ifdef MC2
   buffer.size = 0;
#else
   buffer.size = (length>>3) - 1;
#endif
   buffer.param = 0;
#ifdef MC2
   buffer.dlen = length;
   buffer.rlen = 0;
#else
   buffer.dlen = (length)>>3;
   buffer.rlen = (8)>>3;
#endif
   buffer.ctx_ptr = context_handle;

   buffer.incnt = 1;      
   buffer.outcnt = 0;      

   buffer.inptr[0] = data;
   buffer.insize[0] = length;
   buffer.inoffset[0] = length;
   buffer.inunit[0] = UNIT_8_BIT;

   buffer.req_queue = 0;
   buffer.req_type = request_type;
   buffer.res_order = CAVIUM_RESPONSE_UNORDERED;
   buffer.dma_mode = global_dma_mode;

   cond_code = 
      n1_kernel_ioctl(CSP1_driver_handle, IOCTL_N1_OPERATION_CODE, (ptrlong)&buffer);

   *request_id = buffer.request_id;
   ret_val=cond_code;

   return ret_val;
}

Uint32 
Kernel_Csp1WriteContext(n1_request_type request_type,
       Uint64 context_handle, 
       Uint16 length,
       Uint8 *data,
       Uint32 *request_id,
       void (*call_back)(int, void*),
       void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* output;
	kernel_call=1;
   if ((length < 8) || (length > 1024) || ((length & 0x7) != 0))
      return ERR_ILLEGAL_INPUT_LENGTH;
   
   if ((context_handle & 0x8000000000000000LL) != 0) {
      if ((context_handle & 0xf) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   } else {
      if ((context_handle & 0x7) != 0)
         return ERR_ILLEGAL_CONTEXT_HANDLE;
   }
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}

	output=(Uint8*)cavium_malloc(8, NULL);
	if(output==NULL){
		printk("no memory\n");
		return -1;
	}
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		cavium_free(output);
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(length,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(output);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,data,length);
	
	cb_data->in_len=length;
	cb_data->output=(Uint8*)(output);
	cb_data->outcnt=0;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	   

   req->opcode = (0x2<<8) | (global_dma_mode<<7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
   req->size = 0;
   req->param = 0;
   req->dlen = cb_data->in_len;
   req->rlen = 0;
   req->ctx_ptr = context_handle;

   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT;
   req->outptr[0]=cb_data->output;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1WriteContext failed\n");
		return -1;
	}	

	return 0;
}


Uint32 
Kernel_Csp1Random(n1_request_type request_type,
      Uint16 length, 
      Uint8 *random,
      Uint32 *request_id,
      void (*call_back)(int, void *),
      void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* output;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
	output=(Uint8*)cavium_malloc(length+8, NULL);
	if(output==NULL){
		printk("no memory!\n");
		return -1;
	}

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(output);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));

	cb_data->context=connect;
	cb_data->output=output;
	cb_data->out_len=length;
	cb_data->outcnt=1;
	cb_data->outptr[0]=random;
	cb_data->outsize[0]=length;
	cb_data->outunit[0]=UNIT_8_BIT;
	
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	
	   req->opcode = (0x1<<8) | (global_dma_mode<<7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
	   req->size = cb_data->out_len;
	   req->rlen = cb_data->out_len;
	   req->param = 0;
	   req->dlen = 0;

	   req->incnt = 0;      
	   req->outcnt = 1;      

	   req->outptr[0] = cb_data->output;
	   req->outsize[0] = cb_data->out_len;
	   req->outoffset[0] = ROUNDUP8(cb_data->out_len);
	   req->outunit[0] = UNIT_8_BIT;
	   req->ctx_ptr = 0;

	   req->req_queue = 0;
	   req->req_type = request_type;
	   req->res_order = CAVIUM_RESPONSE_UNORDERED;
	   req->dma_mode = global_dma_mode;
	
		req->callback=call_back;
		req->cb_arg=cb_data;
		
		cb_data->req=req;
		
		if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
		{
			cavium_free(cb_data->output);
			cavium_free(cb_data);
			cavium_free(req);
			printk("Kernel_Csp1Random failed\n");
		}	

		return 0;	
}



Uint32 
Kernel_Csp1Hmac(n1_request_type request_type,
    HashType hash_type, 
    Uint16 key_length, 
    Uint8 *key, 
    Uint16 message_length, 
    Uint8 *message, 
    Uint8 *hmac,
    Uint32 *request_id,
    void (*call_back)(int, void *),
    void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	int hash_size=0;
	Uint8* outbuf;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
   	if ((key_length < 8) || (key_length > 64) || ((key_length & 0x7) != 0))
      		return ERR_ILLEGAL_KEY_LENGTH;
	
	hash_size = (hash_type==MD5_TYPE)? 16:20;
	outbuf=(Uint8*)cavium_malloc(hash_size+8, NULL);
	if(outbuf==NULL){
		printk("no memory\n");
		return -1;
	}
	kernel_call=1;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(outbuf);
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(key_length+message_length,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(outbuf);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,key,key_length);
	memcpy(cb_data->input+key_length,message,message_length);
	
	cb_data->in_len=key_length+message_length;
	cb_data->output=outbuf;
	cb_data->out_len=hash_size;
	cb_data->outcnt=1;
	cb_data->outptr[0]=hmac;
	cb_data->outsize[0]=hash_size;
	cb_data->outunit[0]=UNIT_8_BIT;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}

#ifdef MC2
	   req->opcode = (0x3<<9) | (global_dma_mode<<7) | MAJOR_OP_HMAC;
#else
	   req->opcode = (0x3<<9) | (hash_type<<8) | (global_dma_mode<<7) | MAJOR_OP_HMAC;
#endif


#ifdef MC2
	   req->size = cb_data->in_len;
	   if (hash_type == SHA1_TYPE)
	      req->param = 0x02;
	   else if (hash_type == MD5_TYPE)
	      req->param = 0x01;
	   else
	      req->param = 0;
	   req->dlen = cb_data->in_len;
	   req->rlen = hash_size;
#else
	   req->size = cb_data->in_len;
	   req->param = (cb_data->in_len>>3) - 1;
	   req->dlen = (cb_data->in_len + ROUNDUP8(cb_data->in_len))>>3;
	   req->rlen = (24 + 8)>>3;
#endif

	   req->incnt = 1;      
	   req->outcnt = 1;      

	   req->inptr[0] = cb_data->input;
	   req->insize[0] = cb_data->in_len;
	   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
	   req->inunit[0] = UNIT_8_BIT;
	

	   req->outptr[0] = (Uint8 *)cb_data->output;
	   req->outsize[0] = cb_data->out_len;
	   req->outoffset[0] = 24;
	   req->outunit[0] = UNIT_8_BIT;

	   req->req_queue = 0;
	   req->req_type = request_type;
	   req->res_order = CAVIUM_RESPONSE_UNORDERED;
	   req->dma_mode = global_dma_mode;	

	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Hmac failed\n");
		return -1;
	}	

	return 0;

}



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
           void* connect)
           
{

#ifdef MC2
	   return    ERR_OPERATION_NOT_SUPPORTED;
#else
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* output;
	Uint32 hash_size;

#if defined(CSP1_API_DEBUG)
	   if ((context_handle & 0x8000000000000000LL) != 0) {

	      if ((context_handle & 0xf) != 0)

	         return ERR_ILLEGAL_CONTEXT_HANDLE;

	   } else {

	      if ((context_handle & 0x7) != 0)

	         return ERR_ILLEGAL_CONTEXT_HANDLE;

	   }

	   if ((key_length < 8) || (key_length > 64) || ((key_length & 0x7) != 0))

	      return ERR_ILLEGAL_KEY_LENGTH;
#endif
	kernel_call=1;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}

	output=(Uint8*)cavium_malloc(8, NULL);
	if(output==NULL){
		printk("no memory\n");
		return -1;
	}
	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(output);
		printk("no memory\n");
		return -1;
	}
	
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(key_length + message_length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		cavium_free(output);
		return -1;
	}
	memcpy(cb_data->input, key, key_length);
	memcpy(cb_data->input+key_length, message, message_length);
	cb_data->in_len=key_length + message_length;
	cb_data->output=output;
	cb_data->outcnt=0;


	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
	   req->opcode = (0x1<<9) | (hash_type<<8) | (global_dma_mode<<7) | MAJOR_OP_HMAC;
	//   hash_size = 20 - (hash_type<<2);
	   hash_size = (hash_type==MD5_TYPE)? 16:20;

	   req->size = cb_data->in_len;
	   req->param = (cb_data->in_len>>3) - 1;
	   req->dlen = (ROUNDUP8(cb_data->in_len))>>3;
	   req->rlen = (8)>>3;
	   req->ctx_ptr = context_handle;

	   req->incnt = 1;      
	   req->outcnt = 1;      

	   req->inptr[0] = cb_data->input;
	   req->insize[0] = key_length+message_length;
	   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
	   req->inunit[0] = UNIT_8_BIT;
	   req->outptr[0]=cb_data->output;
	   

	   req->req_queue = 0;
	   req->req_type = request_type;
	   req->res_order = CAVIUM_RESPONSE_UNORDERED;
	   req->dma_mode = CAVIUM_SCATTER_GATHER;

	
		req->callback=call_back;
		req->cb_arg=cb_data;
		
		cb_data->req=req;
		
		if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
		{
			cavium_free(cb_data->input);
			cavium_free(cb_data->output);
			cavium_free(cb_data);
			cavium_free(req);
			printk("Kernel_Csp1HmacStart failed\n");
		}	

		return 0;

#endif /*MC2*/
}

Uint32 
Kernel_Csp1HmacUpdate(n1_request_type request_type,
            Uint64 context_handle, 
            HashType hash_type, 
            Uint16 message_length, 
            Uint8 *message,
            Uint32 *request_id,
            void (*call_back)(int ,void*),
            void *connect)
{
#ifdef MC2
	   return ERR_OPERATION_NOT_SUPPORTED;
#else	
	Uint8* output;
	Uint32 hash_size;
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	
#if defined(CSP1_API_DEBUG)
   if ((context_handle & 0x8000000000000000LL) != 0) {
      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }
#endif
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	output=(Uint8*)cavium_malloc(8, NULL);
	if(output==NULL){
		cavium_free(cb_data);
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(ROUNDUP8(message_length), NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(output);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, message, message_length);
	cb_data->in_len=message_length;
	cb_data->output=output;
	cb_data->outcnt=0;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		return -1;
	}
	req->.opcode = (0x0<<9) | (hash_type<<8) | (global_dma_mode<<7) | MAJOR_OP_HMAC;
	//hash_size = 20 - (hash_type<<2);
	hash_size = (hash_type==MD5_TYPE)? 16:20;
	req->size = cb_data->in_len;
	req->param = 0;
	req->dlen = (ROUNDUP8(cb_data->in_len))>>3;
	req->rlen = (8)>>3;
	req->ctx_ptr = context_handle;
	req->incnt = 1;      
	req->outcnt = 1;      
	req->inptr[0] = cb_data->input
	req->insize[0] = cb_data->in_len;
	req->inoffset[0] = ROUNDUP8(cb_data->in_len);
	req->inunit[0] = UNIT_8_BIT;
	req->outptr[0]=output;
	req->req_queue = 0;
	req->req_type = request_type;
	req->res_order = CAVIUM_RESPONSE_UNORDERED;
	req->dma_mode = global_dma_mode;
	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1HmacUpdate failed\n");
	}	

	return 0;
#endif /*MC2*/
}


Uint32 
Kernel_Csp1HmacFinish(n1_request_type request_type,
      Uint64 context_handle, 
      HashType hash_type, 
      Uint16 message_length, 
      Uint8 *message, 
      Uint8 *final_hmac,
      Uint32 *request_id,
      void (*call_back)(int ,void*),
      void *connect)
{
#ifdef MC2
   return ERR_OPERATION_NOT_SUPPORTED;
#else
	Uint32 hash_size;
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* outbuf;
#if defined(CSP1_API_DEBUG)
   //if ((context_handle & 0x8000000000000000LL) != 0) {
   if((context_handle >> 64) != 0){//add by lijing

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }
#endif
  	 hash_size = (hash_type==MD5_TYPE)? 16:20;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	outbuf=(Uint8*)cavium_malloc(hash_size+8, NULL);
	if(outbuf==NULL){
		printk("no memory\n");
		return -1;
	}
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(outbuf);
		printk("no memory\n");
		return -1;
	}
	kernel_call=1;
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(message_length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(outbuf);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, message, message_length);
	cb_data->in_len=message_length;
	cb_data->output=outbuf;
	cb_data->out_len=hash_size;
	cb_data->outcnt=1;
	cb_data->outptr[0]=final_hmac;
	cb_data->outsize[0]=hash_size;
	cb_data->outunit[0]=UNIT_8_BIT;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->input)
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		return -1;
	}

   req->.opcode = (0x2<<9) | (hash_type<<8) | (global_dma_mode<<7) | MAJOR_OP_HMAC;
//   hash_size = 20 - (hash_type<<2);
   hash_size = (hash_type==MD5_TYPE)? 16:20;

   req->.size = cb_data->in_len;
   req->.param = 0;
   req->.dlen = (ROUNDUP8(cb_data->in_len))>>3;
   req->.rlen = (24 + 8)>>3;
   req->.ctx_ptr = context_handle;

   req->.incnt = 1;      
   req->.outcnt = 1;      

   req->.inptr[0] = cb_data->input;
   req->.insize[0] = cb_data->in_len;
   req->.inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->.inunit[0] = UNIT_8_BIT;

   req->.outptr[0] = (Uint8 *)cb_data->output;
   req->.outsize[0] = hash_size;
   req->.outoffset[0] = 24;
   req->.outunit[0] = UNIT_8_BIT;

   req->.req_queue = 0;
   req->.req_type = request_type;
   req->.res_order = CAVIUM_RESPONSE_UNORDERED;
   req->.dma_mode = global_dma_mode;

   	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1HmacFinish failed\n");
		return -1;
	}	

	return 0;
#endif /*MC2*/
}




Uint32 
Kernel_Csp1Me(n1_request_type request_type,
      Uint16 modlength,
      Uint16 explength,
      Uint16 datalength,
      Uint8 *modulus,
      Uint8 *exponent,
      Uint8 *data, 
      Uint8 *result,
      Uint32 *request_id,
      void (*call_back)(int, void*),
      void *connect)
{

	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* outbuf;
	//Uint32 length;

	
#if defined(CSP1_API_DEBUG)
	   if (result_location == CONTEXT_PTR) {

	      if ((context_handle & 0x8000000000000000LL) != 0) {
	         if ((context_handle & 0xf) != 0)

	            return ERR_ILLEGAL_CONTEXT_HANDLE;

	      } else {

	         if ((context_handle & 0x7) != 0)

	            return ERR_ILLEGAL_CONTEXT_HANDLE;

	      }
	   }

	   if ((modlength & 0x7) != 0)

	      return ERR_ILLEGAL_INPUT_LENGTH;
#endif	
	kernel_call=1;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	
	outbuf=(Uint8*)cavium_malloc(modlength+8, NULL);
	if(outbuf==NULL){
		printk("no memory\n");
		return -1;
	}
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(outbuf);
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(modlength*2+explength, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(outbuf);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,modulus,modlength);
	memcpy(cb_data->input+modlength,exponent,explength);
	memcpy(cb_data->input+explength+modlength,data,modlength);
	
	cb_data->in_len=modlength*2+explength;
	
	cb_data->output=outbuf;
	cb_data->out_len=modlength;
	cb_data->outcnt=1;
	cb_data->outptr[0]=result;
	cb_data->outsize[0]=modlength;
	cb_data->outunit[0]=UNIT_8_BIT;
	
	req=(n1_request_buffer*)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory\n");
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	
	memset(req, 0, sizeof(n1_request_buffer));

   if ((modlength >= 17) && (modlength <= 128)) {

      req->opcode = (MAJOR_OP_ME_PKCS) | (global_dma_mode<<7) ;
   } else if ((modlength > 128) && (modlength <= 256)) {

      req->opcode = (MAJOR_OP_ME_PKCS_LARGE) | (global_dma_mode<<7);

#if defined(CSP1_API_DEBUG)
   } else {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
   }
   req->ctx_ptr=0;
   req->dlen = modlength + explength + datalength;
   req->param = explength;
   req->size = modlength;
   req->rlen = modlength;

   req->incnt=1;

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT ;

        req->outcnt = 1;

        req->outptr[0] = cb_data->output;
        req->outsize[0] = modlength;
        req->outoffset[0] = ROUNDUP8(modlength);
        req->outunit[0] = UNIT_8_BIT;


    req->req_queue = 0;
    req->req_type = request_type;
    req->res_order = CAVIUM_RESPONSE_UNORDERED;
    req->dma_mode = global_dma_mode;


	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kerenel_Csp1Me failed\n");
		return -1;
	}	

	return 0;
}



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
            void* connect)

{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* outbuf;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
	outbuf=(Uint8*)cavium_malloc(modlength+8, NULL);
	if(outbuf==NULL){
		printk("no memory\n");
		return -1;
	}	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(outbuf);
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));

	

	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(modlength+explength+datalength,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(outbuf);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,modulus,modlength);
	memcpy(cb_data->input+modlength,exponent,explength);
	memcpy(cb_data->input+modlength+explength,data,datalength);
	
	cb_data->in_len=modlength+explength+datalength;
	cb_data->output=outbuf;
	cb_data->out_len=modlength;
	cb_data->outcnt=1;
	cb_data->outptr[0]=result;
	cb_data->outsize[0]=modlength;
	cb_data->outunit[0]=UNIT_8_BIT;
	
	
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
	
	 req->ctx_ptr=0;
        req->dlen = modlength + explength + datalength;
        req->size = modlength;
        req->param = (block_type) | (explength<<1);
        req->rlen = modlength;

   if ((modlength >= 17) && (modlength <= 128)) {

      req->opcode = (global_dma_mode<<7) | (0x3<<8) | MAJOR_OP_ME_PKCS;

   } else if ((modlength > 128) && (modlength <= 256)) {

      req->opcode = (global_dma_mode<<7) | (0x3<<8) | MAJOR_OP_ME_PKCS_LARGE;

#if defined(CSP1_API_DEBUG)
   } else {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
   }

   req->incnt = 1;

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] =ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT;


   req->outcnt = 1;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = modlength;
   req->outoffset[0] = ROUNDUP8(modlength);
   req->outunit[0] = UNIT_8_BIT;


   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Pkcs1v15Enc failed\n");
		return -1;
	}	

	return 0;

}




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
               Uint8 *result,
               Uint32 *request_id,
               void (*call_back)(int, void*),
               void *connect)

{

	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* outbuf;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}

	
#if defined(CSP1_API_DEBUG)
   if (result_location == CONTEXT_PTR) {

      if ((context_handle & 0x8000000000000000LL) != 0) {

         if ((context_handle & 0xf) != 0)

            return ERR_ILLEGAL_CONTEXT_HANDLE;

      } else {

         if ((context_handle & 0x7) != 0)

            return ERR_ILLEGAL_CONTEXT_HANDLE;

      }
   }


   if (block_type == BT2) 

      return ERR_ILLEGAL_BLOCK_TYPE;

   if ((modlength & 0x7) != 0)

      return ERR_ILLEGAL_INPUT_LENGTH;

   if (!(key_handle & (((Uint64)0x10000) << 32)))
   
      return ERR_ILLEGAL_MOD_EX_TYPE;
#endif


	outbuf=(Uint8*)cavium_malloc(modlength+8, NULL);
	if(outbuf==NULL){
		printk("no memory\n");
		return -1;
	}
	kernel_call=1;	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(outbuf);
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));

	cb_data->context=connect;

	cb_data->input=(Uint8*)cavium_malloc(modlength*2+modlength/2+datalength,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(outbuf);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,Q,modlength/2);
	memcpy(cb_data->input+modlength/2,Eq,modlength/2);
	memcpy(cb_data->input+modlength,P,modlength/2);
	memcpy(cb_data->input+modlength+modlength/2,Ep,modlength/2);
	memcpy(cb_data->input+modlength+modlength,iqmp,modlength/2);
	memcpy(cb_data->input+modlength*2+modlength/2,data,datalength);
	
	cb_data->in_len=modlength*2+modlength/2+datalength;
	cb_data->output=outbuf;
	cb_data->out_len=modlength;
	cb_data->outcnt=1;
	cb_data->outptr[0]=result;
	cb_data->outsize[0]=modlength;
	cb_data->outunit[0]=UNIT_8_BIT;
	
	

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	



   req->ctx_ptr=0;
   //req->dlen = 2.5 * modlength + datalength;
   req->dlen=modlength*2+datalength+modlength/2;
   req->size = modlength;
   req->param = (block_type);
   req->rlen = modlength;




   if ((modlength >= 34) && (modlength <= 128) && ((modlength & 0x1) == 0)) {



         req->opcode = (global_dma_mode<<7) | (0x4<<8) | MAJOR_OP_ME_PKCS;


   } else if ((modlength > 128) && (modlength <= 256) && ((modlength & 0x1) == 0)) {


      req->opcode = (global_dma_mode<<7) | (0x4<<8) | MAJOR_OP_ME_PKCS_LARGE;


#if defined(CSP1_API_DEBUG)
   } else {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
   }


   req->incnt = 1;

   req->inptr[0] = (Uint8 *)cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT;
  

   req->outcnt = 1;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = modlength;
   req->outoffset[0] = ROUNDUP8(modlength);
   req->outunit[0] = UNIT_8_BIT;



   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kerenel_Csp1Pkcs1v15CrtEnc failed\n");
		return -1;
	}	

	return 0;
}




Uint32 
Kernel_Csp1Pkcs1v15Dec(n1_request_type request_type,
            RsaBlockType block_type,
            Uint16 modlength, 
            Uint16 explength,
            Uint8 *modulus, 
            Uint8 *exponent, 
            Uint8 *data,
            Uint16 *out_length,
            Uint8 *result,
            Uint32 *request_id,
            void (*call_back)(int, void*),
            void *connect)

{

	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint8* outbuf;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
#if defined(CSP1_API_DEBUG)
   if (result_location == CONTEXT_PTR) {

      if ((context_handle & 0x8000000000000000LL) != 0) {

         if ((context_handle & 0xf) != 0)

            return ERR_ILLEGAL_CONTEXT_HANDLE;

      } else {

         if ((context_handle & 0x7) != 0)

            return ERR_ILLEGAL_CONTEXT_HANDLE;

      }
   }


   if ((modlength & 0x7) != 0)

      return ERR_ILLEGAL_INPUT_LENGTH;

   //if (key_handle & (((Uint64)0x10000) << 32))
   if(key_handle & 0x0001000000000000ULL)

      return ERR_ILLEGAL_MOD_EX_TYPE;
#endif

	outbuf=(Uint8*)cavium_malloc(2+modlength+8, NULL);
	if(outbuf==NULL){
		printk("no memory\n");
		return -1;
	}
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(outbuf);
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));

	cb_data->context=connect;
	cb_data->output=outbuf;
	
	cb_data->input=(Uint8*)cavium_malloc(modlength+explength+modlength, NULL);
	if(cb_data->input==NULL){
		cavium_free(outbuf);
		cavium_free(cb_data);
		printk("no memory\n");
		return -1;
	}
	memcpy(cb_data->input, modulus, modlength);
	memcpy(cb_data->input+modlength, exponent, explength);
	memcpy(cb_data->input+modlength+explength, data, modlength);
	cb_data->in_len=modlength*2+explength;
	cb_data->outcnt=2;
	cb_data->outptr[0]=(Uint8*)out_length;
	cb_data->outsize[0]=2;
	cb_data->outptr[1]=result;
	cb_data->outsize[1]=modlength;
	cb_data->out_len=2+modlength;
	cb_data->outunit[0]=UNIT_16_BIT;
	cb_data->outunit[1]=UNIT_8_BIT;
	
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	

   req->ctx_ptr=0;
   req->dlen = (2*modlength) + explength;
   req->size = modlength;
   req->param = (block_type) | (explength<<1);
   req->rlen = 2 + modlength; /* outlength + modlength bytes result */

   if ((modlength >= 17) && (modlength <= 128)) {
      req->opcode = (global_dma_mode<<7) | (0x1<<8) | MAJOR_OP_ME_PKCS;

   } else if ((modlength > 128) && (modlength <= 256)) {
      req->opcode = (global_dma_mode<<7) | (0x1<<8) | MAJOR_OP_ME_PKCS_LARGE;

#if defined(CSP1_API_DEBUG)
   } else {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
   }

  
   req->incnt = 1;

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = cb_data->in_len;
   req->inunit[0] = UNIT_8_BIT;
 

   req->outcnt = 1;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = ROUNDUP8(2+modlength);
   req->outunit[0] = UNIT_8_BIT;
	
   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;
    
	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Pkcs1v15Dec failed\n");
		return -1;
	}	

	return 0;
}









Uint32 
Kernel_Csp1InitializeRc4(n1_request_type request_type,
              Uint64 context_handle, 
              Uint16 key_length, 
              Uint8 *key,
              Uint32 *request_id,
              void (*call_back)(int, void*),
              void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;
	cb_data->input=(Uint8*)cavium_malloc(key_length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, key, key_length);
	kernel_call=1;
	cb_data->in_len=key_length;
	cb_data->output=(Uint8*)cavium_malloc(8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
		
	
   if ((context_handle & 0x8000000000000000LL) != 0) {

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }
	
   if ((key_length < 1) || (key_length >= 256))

      return ERR_ILLEGAL_KEY_LENGTH;

#ifdef MC2
   req->opcode = (0x9<<8) | (global_dma_mode << 7) | MAJOR_OP_RANDOM_WRITE_CONTEXT;
   req->size = 0;
   req->dlen = cb_data->in_len;
   req->rlen = 0;
#else      
   req->opcode = (0x0<<8) | (global_dma_mode<<7) | MAJOR_OP_ENCRYPT_DECRYPT;
   req->size = cb_data->in_len - 1;
   req->dlen = (ROUNDUP8(cb_data->in_len))>>3;
   req->rlen = (8)>>3;	
#endif	
   req->param = 0;
   req->ctx_ptr = context_handle;

   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT;
   req->outptr[0]=cb_data->output;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;	
	req->callback=call_back;
	req->cb_arg=cb_data;
	
	cb_data->req=req;
	
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1InitializeRc4 failed\n");
		return -1;
	}	

	return 0;
}

Uint32 
Kernel_Csp1EncryptRc4(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            Uint16 length, 
            Uint8 *input, 
            Uint8 *output,
            Uint32 *request_id,
              void (*call_back)(int, void*),
              void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	//Uint8* request;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	
	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;
	
	cb_data->input=(Uint8*)cavium_malloc(length,NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input,(unsigned char *)input,length);
	cb_data->output=(Uint8*)cavium_malloc(length+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}

	cb_data->in_len=length;
	cb_data->out_len=length;
	cb_data->outcnt=1;
	cb_data->outptr[0]=output;
	cb_data->outsize[0]=length;
	cb_data->outunit[0]=UNIT_8_BIT;

	
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
   if ((context_handle & 0x8000000000000000LL) != 0) {

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }	

#ifdef MC2
   req->opcode = (context_update<<13) | (global_dma_mode<<7) 
      | MAJOR_OP_ENCRYPT_DECRYPT;
   req->size = 0;
   req->param = 0;
   req->dlen = cb_data->in_len;
   req->rlen = cb_data->out_len;
#else
   req->opcode = (context_update<<13) | (0x2<<8) | (global_dma_mode<<7) 
      | MAJOR_OP_ENCRYPT_DECRYPT;
   req->size = cb_data->in_len;
   req->param = 0;
   req->dlen = (ROUNDUP8(cb_data->in_len))>>3;
   req->rlen = (ROUNDUP8(cb_data->out_len) + 8)>>3;
#endif
   req->ctx_ptr = context_handle;

   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = ROUNDUP8(cb_data->in_len);
   req->inunit[0] = UNIT_8_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = (global_dma_mode == global_dma_mode) ? ROUNDUP8(cb_data->out_len) : cb_data->out_len;
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;
   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
  
	if (do_request(&cavium_dev[0], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1EncryptRc4 failed\n");
		return -1;
	}	

   return 0;
}

Uint32 
Kernel_Csp1Initialize3DES(n1_request_type request_type,
               Uint64 context_handle, 
               Uint8 *iv, 
               Uint8 *key,
               Uint32 *request_id)
{
#ifdef MC2
   return ERR_OPERATION_NOT_SUPPORTED;
#else
   Uint8 temp[32];
   Uint32 ret_val;
   Uint32 dummy=0;

   if(request_type == CAVIUM_NON_BLOCKING)
      return ERR_OPERATION_NOT_SUPPORTED;
   memcpy(temp, iv, 8);
   memcpy(temp + 8, key, 24);

   ret_val = Csp1WriteContext(CAVIUM_BLOCKING, context_handle, 32, temp,&dummy);

   return ret_val;
#endif /*MC2*/

}


Uint32 
Csp1InitializeAES(n1_request_type request_type,
              Uint64 context_handle, 
              AesType aes_type, 
              Uint8 *iv, 
              Uint8 *key,
              Uint32 *request_id)
{
#ifdef MC2
   return ERR_OPERATION_NOT_SUPPORTED;
#else
   Uint8 temp[48];
   Uint32 ret_val;
   Uint32 dummy=0;

   if(request_type == CAVIUM_NON_BLOCKING)
      return ERR_OPERATION_NOT_SUPPORTED;

   memcpy(temp, iv, 16);

   memcpy(temp + 16, key, 16 + 8 * aes_type);

   ret_val = Csp1WriteContext(CAVIUM_BLOCKING,context_handle, (Uint16)(32 + 8 * aes_type), temp,&dummy);

   return ret_val;
#endif /*MC2*/
}



#ifdef MC2
Uint32 
Kernel_Csp1Hash(n1_request_type request_type,
    HashType hash_type, 
    Uint16 message_length, 
    Uint8 *message, 
    Uint8 *hash,
    Uint32 *request_id,
	void (*call_back)(int, void*),
	void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint32 hash_size;
	
	kernel_call=1;	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
   hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));

	cb_data->context=connect;
	cb_data->input=(Uint8*)cavium_malloc(message_length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, message, message_length);
	cb_data->in_len=message_length;
	cb_data->output=(Uint8*)cavium_malloc(hash_size+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->out_len=hash_size;
	cb_data->outcnt=1;
	cb_data->outptr[0]=hash;
	cb_data->outsize[0]=hash_size;
	cb_data->outunit[0]=UNIT_8_BIT;


	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	

   req->opcode = (0x3<<9) | (global_dma_mode<<7) | MAJOR_OP_HASH;


   req->size = 0;
   if (hash_type == SHA1_TYPE)
      req->param = 0x02;
   else if (hash_type == MD5_TYPE)
      req->param = 0x01;
   else
      req->param = 0;
   req->dlen = message_length;
   req->rlen = hash_size;
   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = message;
   req->insize[0] = message_length;
   req->inoffset[0] = ROUNDUP8(message_length);
   req->inunit[0] = UNIT_8_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = hash_size;
   req->outoffset[0] = 24;
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Hash failed\n");
		return -1;
	}	

   return 0;

}
#endif






Uint32 
Kernel_Csp1DecryptAes(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            AesType aes_type, 
            Uint16 length, 
            Uint8 *input, 
            Uint8 *output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void *),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint32 key_length;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
   if ((context_handle & 0x8000000000000000LL) != 0) {

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));

   if ((length & 0xf) != 0)

      return ERR_ILLEGAL_INPUT_LENGTH;
   if (aes_type == AES_128) {
      req->size = 0x0005;
      key_length = 16;
   } else if (aes_type == AES_192) {
      req->size = 0x0006;
      key_length = 24;
   } else if (aes_type == AES_256) {
      req->size = 0x0007;
      key_length = 32;
   } else {
      req->size = 0x0000;
      key_length = 0;
   }
	req->size=0x0007;
	key_length=32;
	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		cavium_free(req);
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));	
	
	cb_data->context=(void*)connect;
	cb_data->input=(Uint8*)cavium_malloc(16+key_length+length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		cavium_free(req);
		return -1;
	}
	memcpy(cb_data->input, iv, 16);
	memcpy(cb_data->input+16, key, key_length);
	memcpy(cb_data->input+16+key_length, input, length);
	cb_data->in_len=16+key_length+length;
	cb_data->output=(Uint8*)cavium_malloc(length+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->outcnt=1;
	cb_data->outptr[0]=output;
	cb_data->outsize[0]=length;
	cb_data->out_len=length;
	cb_data->outunit[0]=UNIT_8_BIT;



   req->opcode = (0x7<<8) | (global_dma_mode<<7) | MAJOR_OP_ENCRYPT_DECRYPT;

   req->param = 0;
   req->dlen = length + 16 + key_length;
   req->rlen = length;
   req->ctx_ptr = context_handle;
   req->incnt = 1;      
   req->outcnt = 1;      
   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = cb_data->in_len;
   req->inunit[0] = UNIT_8_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = cb_data->out_len;
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1DecryptAes failed\n");
		return -1;
	}	

   return 0;
}




Uint32 
Kernel_Csp1EncryptAes(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            AesType aes_type, 
            Uint16 length, 
            Uint8 *input, 
            Uint8 *output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void *),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	Uint32 key_length;
	//int i;
#ifdef MC2
	printk("define MC2\n");
#endif
	kernel_call=1;	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
   if ((context_handle & 0x8000000000000000LL) != 0) {

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }
	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));


	
   if (aes_type == AES_128) {
      req->size = 0x0005;
      key_length = 16;
   } else if (aes_type == AES_192) {
      req->size = 0x0006;
      key_length = 24;
   } else if (aes_type == AES_256) {
      req->size = 0x0007;
      key_length = 32;
   } else {
      req->size = 0x0000;
      key_length = 0;
   }
	req->size=0x0007;
	key_length=32;

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory, %s %d\n", __FILE__, __LINE__);
		cavium_free(req);
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));	
	
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(16+key_length+length, NULL);
	if(cb_data->input==NULL){
		printk("no memory, %s %d\n", __FILE__, __LINE__);
		cavium_free(cb_data);
		cavium_free(req);
		return -1;
	}
	
	memcpy(cb_data->input, iv, 16);
	memcpy(cb_data->input+16, key, key_length);
	memcpy(cb_data->input+16+key_length, input, length);
	cb_data->in_len=16+key_length+length;
	cb_data->output=(Uint8*)cavium_malloc(length+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->out_len=length;
	cb_data->outcnt=1;
	cb_data->outptr[0]=output;
	cb_data->outsize[0]=length;
	cb_data->outunit[0]=UNIT_8_BIT;

   //req->opcode = (0x6<<8) | (global_dma_mode<<7) | MAJOR_OP_ENCRYPT_DECRYPT;
	req->opcode=0x060e;

	printk("\n");
	
   req->param = 0;
   req->dlen = length + 16 + key_length;
   //req->rlen = ROUNDUP16(length);
   req->rlen = length;

   req->ctx_ptr = 0;


   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   //req->inoffset[0] = cb_data->in_len;
   //req->inunit[0] = UNIT_8_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   //req->outoffset[0] = cb_data->out_len;
   //req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1EncryptAes failed\n");
		return -1;
	}	

   return 0;

}




Uint32 
Kernel_Csp1Decrypt3Des(n1_request_type request_type,
            Uint64 context_handle, 
            ContextUpdate context_update, 
            Uint16 length, 
            Uint8 *input, 
            Uint8 *output,
            Uint8 *iv,
            Uint8 *key,
            Uint32 *request_id,
			void (*call_back)(int, void *),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;	
   if ((context_handle & 0x8000000000000000LL) != 0) {

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }

   if ((length & 0x7) != 0)

      return ERR_ILLEGAL_INPUT_LENGTH;

	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));	
	
	cb_data->context=(void*)connect;
	cb_data->input=(Uint8*)cavium_malloc(8+24+length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, iv, 8);
	memcpy(cb_data->input+8, key, 24);
	memcpy(cb_data->input+8+24, input, length);
	cb_data->in_len=8+24+length;
	cb_data->output=(Uint8*)cavium_malloc(length+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->out_len=length;
	cb_data->outcnt=1;
	cb_data->outptr[0]=output;
	cb_data->outsize[0]=length;
	cb_data->outunit[0]=UNIT_8_BIT;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));

	
   req->opcode =  (0x5<<8) | (global_dma_mode<<7) | MAJOR_OP_ENCRYPT_DECRYPT;
   req->size = 0;
   req->param = 0;
   req->dlen = length + 32;
   req->rlen = length;
   req->ctx_ptr = context_handle;


   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = cb_data->in_len;
   req->inunit[0] = UNIT_8_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = cb_data->out_len;
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Decrypt3Des failed\n");
		return -1;
	}	

   return 0;
			
			
}


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
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;
   if ((context_handle & 0x8000000000000000LL) != 0) {

      if ((context_handle & 0xf) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   } else {

      if ((context_handle & 0x7) != 0)

         return ERR_ILLEGAL_CONTEXT_HANDLE;

   }

   if ((length >= 0xffe0) != 0) 
         return ERR_ILLEGAL_INPUT_LENGTH;

	
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));	
	
	cb_data->context=(void*)connect;
	cb_data->input=(Uint8*)cavium_malloc(8+24+length, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, iv, 8);
	memcpy(cb_data->input+8, key, 24);
	memcpy(cb_data->input+8+24, input, length);
	cb_data->in_len=8+24+length;
	cb_data->output=(Uint8*)cavium_malloc(length+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->outcnt=1;
	cb_data->outptr[0]=output;
	cb_data->outsize[0]=length;
	cb_data->out_len=length;
	cb_data->outunit[0]=UNIT_8_BIT;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));	

	
	req->opcode = (context_update<<13) | (0x4<<8) | (global_dma_mode<<7) 
      | MAJOR_OP_ENCRYPT_DECRYPT;

   req->size = 0;
   req->param = 0;
   req->dlen = length + 32;
   req->rlen = ROUNDUP8(length);
   req->ctx_ptr = context_handle;

   req->incnt = 1;      
   req->outcnt = 1;      

   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = cb_data->in_len;
   req->inunit[0] = UNIT_8_BIT;

   req->outptr[0] = cb_data->output;
   req->outsize[0] = ROUNDUP8(cb_data->out_len);
   req->outoffset[0] = ROUNDUP8(cb_data->out_len);
   req->outunit[0] = UNIT_8_BIT;
   
   req->req_queue = 0;
   req->req_type = request_type;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;
   req->dma_mode = global_dma_mode;

   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Encrypt3Des failed\n");
		return -1;
	}	

   return 0;
}



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
               Uint8 *result,
               Uint32 *request_id,
			   void (*call_back)(int, void *),
			   void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;
	
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
#if defined(CSP1_API_DEBUG)
	  if ((context_handle & 0x8000000000000000LL) != 0) {

		 if ((context_handle & 0xf) != 0)

			return ERR_ILLEGAL_CONTEXT_HANDLE;

	  } else {

		 if ((context_handle & 0x7) != 0)

			return ERR_ILLEGAL_CONTEXT_HANDLE;

	  }



   if (block_type == BT1) 

      return ERR_ILLEGAL_BLOCK_TYPE;

   if ((modlength & 0x7) != 0)

      return ERR_ILLEGAL_INPUT_LENGTH;

   if (!(key_handle & (((Uint64)0x10000) << 32)))

      return ERR_ILLEGAL_MOD_EX_TYPE;
#endif
	kernel_call=1;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		printk("no memory\n");
		return -1;
	}
	memset(cb_data, 0, sizeof(struct call_back_data));	
	
	cb_data->context=(void*)connect;
	cb_data->input=(Uint8*)cavium_malloc(modlength*3+modlength/2, NULL);
	if(cb_data->input==NULL){
		printk("no memory\n");
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, Q, modlength/2);
	memcpy(cb_data->input+modlength/2, Eq, modlength/2);
	memcpy(cb_data->input+modlength, P, modlength/2);
	memcpy(cb_data->input+modlength+modlength/2, Ep, modlength/2);
	memcpy(cb_data->input+modlength*2, iqmp, modlength/2);
	memcpy(cb_data->input+modlength*2+modlength/2, data, modlength);
	cb_data->in_len=modlength*3+modlength/2;
	cb_data->output=(Uint8*)cavium_malloc(modlength+2+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->outcnt=2;
	cb_data->outptr[0]=(Uint8*)out_length;
	cb_data->outsize[0]=2;
	cb_data->outptr[1]=result;
	cb_data->outsize[1]=modlength;
	cb_data->out_len=modlength+2;
	cb_data->outunit[0]=UNIT_16_BIT;
	cb_data->outunit[1]=UNIT_8_BIT;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		cavium_free(cb_data->output);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));

   req->ctx_ptr=0;
   req->dlen = (modlength*2)+modlength/2 + modlength;
   req->size = modlength;
   req->param = (block_type);
   req->rlen = 2 + modlength;


   if ((modlength >= 34) && (modlength <= 128) && ((modlength & 0x1) == 0)) {

      req->opcode = (0x2<<8) | (global_dma_mode<<7) | MAJOR_OP_ME_PKCS;

   } else if ((modlength > 128) && (modlength <= 256) && ((modlength & 0x1) == 0)) {

      req->opcode = (0x2<<8) | (global_dma_mode<<7) | MAJOR_OP_ME_PKCS_LARGE;

#if defined(CSP1_API_DEBUG)
   } else {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
   }

   req->incnt = 1;
   req->inptr[0] = cb_data->input;
   req->insize[0] = cb_data->in_len;
   req->inoffset[0] = cb_data->in_len;
   req->inunit[0] = UNIT_8_BIT;

   req->outcnt = 1;
   req->outptr[0] = cb_data->output;
   req->outsize[0] = cb_data->out_len;
   req->outoffset[0] = cb_data->out_len;
   req->outunit[0] = UNIT_8_BIT;

   req->req_queue = 0;
    req->req_type = request_type;
    req->res_order = CAVIUM_RESPONSE_UNORDERED;
    req->dma_mode = global_dma_mode;
   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
   
	if (do_request(&cavium_dev[CSP1_driver_handle], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1Pkcs1v15CrtDec failed\n");
		return -1;
	}	

   return 0;



}






EXPORT_SYMBOL(Csp1Initialize);
EXPORT_SYMBOL(Csp1Shutdown);
EXPORT_SYMBOL(Csp1CheckForCompletion);
EXPORT_SYMBOL(Csp1FlushAllRequests);
EXPORT_SYMBOL(Csp1FlushRequest);
EXPORT_SYMBOL(Csp1AllocContext);
EXPORT_SYMBOL(Csp1FreeContext);
EXPORT_SYMBOL(Csp1AllocKeyMem);
EXPORT_SYMBOL(Csp1FreeKeyMem);
EXPORT_SYMBOL(Csp1StoreKey);
EXPORT_SYMBOL(Kernel_Csp1ReadEpci);
EXPORT_SYMBOL(Kernel_Csp1WriteEpci);
EXPORT_SYMBOL(Kernel_Csp1ReadContext);
EXPORT_SYMBOL(Kernel_Csp1WriteContext);
EXPORT_SYMBOL(Kernel_Csp1Random);
EXPORT_SYMBOL(Kernel_Csp1Hmac);
EXPORT_SYMBOL(Kernel_Csp1HmacStart);
EXPORT_SYMBOL(Kernel_Csp1HmacUpdate);
EXPORT_SYMBOL(Kernel_Csp1HmacFinish);
EXPORT_SYMBOL(Kernel_Csp1Me);
EXPORT_SYMBOL(Kernel_Csp1Pkcs1v15Enc);
EXPORT_SYMBOL(Kernel_Csp1Pkcs1v15CrtEnc);
EXPORT_SYMBOL(Kernel_Csp1Pkcs1v15Dec);
EXPORT_SYMBOL(Kernel_Csp1InitializeRc4);
EXPORT_SYMBOL(Kernel_Csp1EncryptRc4);
EXPORT_SYMBOL(Kernel_Csp1Initialize3DES);
EXPORT_SYMBOL(Csp1InitializeAES);
EXPORT_SYMBOL(Kernel_Csp1Hash);
EXPORT_SYMBOL(Kernel_Csp1DecryptAes);
EXPORT_SYMBOL(Kernel_Csp1EncryptAes);
EXPORT_SYMBOL(Kernel_Csp1Decrypt3Des);
EXPORT_SYMBOL(Kernel_Csp1Encrypt3Des);
EXPORT_SYMBOL(Kernel_Csp1Pkcs1v15CrtDec);
