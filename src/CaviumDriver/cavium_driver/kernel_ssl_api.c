////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: kernel_ssl_api.c
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
#include "cavium_ssl.h"

extern Csp1DmaMode global_dma_mode;
extern int CSP1_driver_handle;
//static void pkp_leftfill (unsigned char input[], int length,
//			  unsigned char output[], int finallength);
//static void swap_word_openssl (unsigned char *d, unsigned char *s, int len);
extern int kernel_call;
extern cavium_device cavium_dev[];
extern Uint32 Kernel_Csp1WriteEpci(n1_request_type request_type,
          Uint64 *key_handle, 
          Uint16 length,
          Uint8 *data,
          Uint32 *request_id,
		    void (*call_back)(int, void *),
		    void *connect);


DmaMode Csp1GetDmaMode (void)
{
  return global_dma_mode;
}


int Csp1GetDriverState (void)
{
  if (CSP1_driver_handle != -1)
    return 0;
  else
    return -1;
}

Uint32 Kernel_Csp1SetEncryptedMasterSecretKey (Uint8 * key, Uint32 *request_id, void (*call_back)(int, void *), void *connect)
{
  Uint32 ret_val;
  Uint32 dummy = 0;
  //ret_val = Kernel_Csp1WriteEpci (CAVIUM_NON_BLOCKING, NULL, 48, key, &dummy, call_back, connect);
  ret_val=Kernel_Csp1WriteEpci(CAVIUM_NON_BLOCKING,
          NULL, 
          48,
          key,
          &dummy,
		    call_back,
		    connect);

  *request_id=dummy;
  return ret_val;
}



Uint32
Kernel_Csp1RsaServerFullRc4 (n1_request_type request_type,
		      Uint64 context_handle,
		      Uint64 * key_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      Rc4Type rc4_type,
		      MasterSecretReturn master_secret_ret,
		      Uint16 modlength,
		      Uint8 * encrypt_premaster_secret,
		      Uint8 * client_random,
		      Uint8 * server_random,
		      Uint16 handshake_length,
		      Uint8 * handshake,
		      Uint8 * client_finished_message,
		      Uint8 * server_finished_message,
		      Uint8 * encrypt_master_secret, 
			  Uint32 *request_id,
			  void (*call_back)(int, void*),
			  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 finished_size;
	Uint16 hash_size;
	Uint64 key_handle_temp;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
	

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {
      if ((context_handle & 0xf) != 0)
		  return ERR_ILLEGAL_CONTEXT_HANDLE;
    }
  else
    {
      if ((context_handle & 0x7) != 0)
		  return ERR_ILLEGAL_CONTEXT_HANDLE;
    }

  if ((modlength >= 64) && (modlength <= 128))
    {
      req->opcode =
	(master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER;

    }
  else if ((modlength > 128) && (modlength <= 256))
    {
      req->opcode =
	(master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER_LARGE;

#if defined(CSP1_API_DEBUG)
    }
  else
    {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
    }

  finished_size = 16 + 24 * ssl_version;

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;
	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8 + modlength + 32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("fable to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	//*key_handle=htobe64((Uint64)(*key_handle));//*****************************************************************
	//memcpy(cb_data->input, key_handle, 8);
	key_handle_temp = htobe64((Uint64)(*key_handle));
	memcpy(cb_data->input, &key_handle_temp, 8);
	memcpy(cb_data->input+8, encrypt_premaster_secret, modlength);
	memcpy(cb_data->input+modlength+8, client_random, 32);
	memcpy(cb_data->input+modlength+40, server_random, 32);
	memcpy(cb_data->input+modlength+72, handshake, handshake_length);
	cb_data->in_len=8+modlength+64+handshake_length;
	
  if (master_secret_ret == RETURN_ENCRYPTED)
    {
	  cb_data->outcnt=3;
	  cb_data->output=(Uint8*)cavium_malloc(48+2*(finished_size+hash_size)+8, NULL);
	  if(cb_data->output==NULL){
		  printk("no memory");
		  cavium_free(req);
		  cavium_free(cb_data->input);
		  cavium_free(cb_data);
		  return -1;
	  }
	  cb_data->outptr[0]=encrypt_master_secret;
	  cb_data->outsize[0]=48;
	  cb_data->outunit[0]=UNIT_8_BIT;
	  cb_data->outptr[1]=client_finished_message;
	  cb_data->outsize[1]=finished_size+hash_size;
	  cb_data->outunit[1]=UNIT_8_BIT;
	  cb_data->outptr[2]=server_finished_message;
	  cb_data->outsize[2]=finished_size+hash_size;
	  cb_data->outunit[2]=UNIT_8_BIT;
	  cb_data->out_len=48+2*(finished_size+hash_size);
    }
  else
    {
	  cb_data->outcnt=2;
	  cb_data->output=(Uint8*)cavium_malloc(2*(finished_size+hash_size)+8, NULL);
	  if(cb_data->output==NULL){
		  printk("no memory");
		  cavium_free(req);
		  cavium_free(cb_data->input);
		  cavium_free(cb_data);
		  return -1;
	  }	  
	  cb_data->outptr[0]=client_finished_message;
	  cb_data->outsize[0]=finished_size+hash_size;
	  cb_data->outunit[0]=UNIT_8_BIT;
	  cb_data->outptr[1]=server_finished_message;
	  cb_data->outsize[1]=finished_size+hash_size;
	  cb_data->outunit[1]=UNIT_8_BIT;
	  cb_data->out_len=2*(finished_size+hash_size);
    }
  req->size = modlength;
  req->param =
    (hash_type) | (ssl_version << 2) | (rc4_type << 3) | (1 << 7);
  req->dlen = 8 + modlength + 32 + 32 + handshake_length;
  req->rlen = cb_data->out_len;
  req->ctx_ptr = context_handle;

  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

	req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1RsaServerFullRc4 failed\n");
		return -1;
	}	

   return 0;
  
}



Uint32
Kernel_Csp1RsaServerVerifyRc4 (n1_request_type request_type,
			Uint64 context_handle,
			Uint64 * key_handle,
			HashType hash_type,
			SslVersion ssl_version,
			Rc4Type rc4_type,
			MasterSecretReturn master_secret_ret,
			Uint16 modlength,
			Uint8 * encrypt_premaster_secret,
			Uint8 * client_random,
			Uint8 * server_random,
			Uint16 handshake_length,
			Uint8 * handshake,
			Uint8 * verify_data,
			Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 hash_size;
	Uint64 key_handle_temp;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {
      if ((context_handle & 0xf) != 0)
		  return ERR_ILLEGAL_CONTEXT_HANDLE;
    }
  else
    {
      if ((context_handle & 0x7) != 0)
		  return ERR_ILLEGAL_CONTEXT_HANDLE;
    }


  if ((modlength >= 64) && (modlength <= 128))
    {


      req->opcode =
	(master_secret_ret << 14) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER;


    }
  else if ((modlength > 128) && (modlength <= 256))
    {


      req->opcode =
	(master_secret_ret << 14) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER_LARGE;

#if defined(CSP1_API_DEBUG)
    }
  else
    {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
    }

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;
	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8 + modlength + 32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}
	//*key_handle=htobe64((Uint64)(*key_handle));
	//memcpy(cb_data->input, key_handle, 8);
	key_handle_temp = htobe64((Uint64)(*key_handle));
	memcpy(cb_data->input, &key_handle_temp, 8);
	memcpy(cb_data->input+8, encrypt_premaster_secret, modlength);
	memcpy(cb_data->input+modlength+8, client_random, 32);
	memcpy(cb_data->input+modlength+40, server_random, 32);
	memcpy(cb_data->input+modlength+72, handshake, handshake_length);
	cb_data->in_len=modlength+72+handshake_length;

  if (master_secret_ret == RETURN_ENCRYPTED)
    {
		cb_data->outcnt=2;
		cb_data->output=(Uint8*)cavium_malloc(48+36+8, NULL);
		if(cb_data->output==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data->input);
			cavium_free(cb_data);
			return -1;
		}
		cb_data->outptr[0]=encrypt_master_secret;
		cb_data->outsize[0]=48;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->outptr[1]=verify_data;
		cb_data->outsize[1]=36;
		cb_data->outunit[1]=UNIT_8_BIT;
		cb_data->out_len=48+36;
    }
  else
    {
		cb_data->outcnt=1;
		cb_data->output=(Uint8*)cavium_malloc(36+8, NULL);
		if(cb_data->output==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data->input);
			cavium_free(cb_data);
			return -1;
		}
		cb_data->outptr[0]=verify_data;
		cb_data->outsize[0]=36;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->out_len=36;
    }	

  req->size = modlength;
  req->param =
    (hash_type) | (ssl_version << 2) | (rc4_type << 3) | (1 << 7);
  req->dlen = 8 + modlength + 32 + 32 + handshake_length;
  req->rlen=cb_data->out_len;

  req->ctx_ptr = context_handle;

  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

	
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1RsaServerVerifyRc4 failed\n");
		return -1;
	}	

   return 0;
}




Uint32
Kernel_Csp1RsaServerFull3Des (n1_request_type request_type,
		       Uint64 context_handle,
		       Uint64 * key_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       DesType des_type,
		       MasterSecretReturn master_secret_ret,
		       ClientFinishMessageOutput clnt_fin_msg_out,
		       ServerFinishMessageOutput srvr_fin_msg_out,
		       Uint16 client_pad_length,
		       Uint16 server_pad_length,
		       Uint16 modlength,
		       Uint8 * encrypt_premaster_secret,
		       Uint8 * client_random,
		       Uint8 * server_random,
		       Uint16 handshake_length,
		       Uint8 * handshake,
		       Uint8 * client_finished_message,
		       Uint8 * server_finished_message,
		       Uint8 * encrypt_master_secret, Uint32 * request_id,
			   void (*call_back)(int, void*),
			   void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 finished_size;
	Uint16 hash_size;
	Uint16 param = 0;
	Uint32 out_length=0;
	Uint64 key_handle_temp;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {
      if ((context_handle & 0xf) != 0)
		  return ERR_ILLEGAL_CONTEXT_HANDLE;
    }
  else
    {
      if ((context_handle & 0x7) != 0)
		  return ERR_ILLEGAL_CONTEXT_HANDLE;
    }

  if ((modlength & 0x7) != 0)
    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((modlength >= 64) && (modlength <= 128))
    {
      req->opcode =
	(master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER;

    }
  else if ((modlength > 128) && (modlength <= 256))
    {

      req->opcode =
	(master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER_LARGE;

#if defined(CSP1_API_DEBUG)
    }
  else
    {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
    }

  finished_size = 16 + 24 * ssl_version;
  param = (hash_type) | (ssl_version << 2) | (des_type << 3) | (1 << 7);

  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    param |= (1 << 8);

  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    param |= (1 << 9);

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8 + modlength + 32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	//*key_handle=htobe64((Uint64)(*key_handle));
	//memcpy(cb_data->input, key_handle, 8);
	key_handle_temp = htobe64((Uint64)(*key_handle));
	memcpy(cb_data->input, &key_handle_temp, 8);
	memcpy(cb_data->input+8, encrypt_premaster_secret, modlength);
	memcpy(cb_data->input+8+modlength, client_random, 32);
	memcpy(cb_data->input+modlength+40, server_random, 32);
	memcpy(cb_data->input+72+modlength, handshake, handshake_length);
	cb_data->in_len=72+modlength+handshake_length;

  if (master_secret_ret == RETURN_ENCRYPTED)
    {
	  out_length=48;
		cb_data->outcnt=3;

		cb_data->outptr[0]=encrypt_premaster_secret;
		cb_data->outsize[0]=48;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->outptr[1]=client_finished_message;
		
      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
	{
		  out_length+=ROUNDUP8 (finished_size + hash_size + 1);
			cb_data->outsize[1]=ROUNDUP8 (finished_size + hash_size + 1);
			cb_data->outunit[1]=UNIT_8_BIT;
	}
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
	{
		  out_length+=finished_size;
		cb_data->outsize[1]=finished_size;
		cb_data->outunit[1]=UNIT_8_BIT;
	}
		cb_data->outptr[2]=server_finished_message;
	
      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
	{
		  out_length+=ROUNDUP8 (finished_size + hash_size + 1);
		cb_data->outsize[2]=ROUNDUP8 (finished_size + hash_size + 1);
		cb_data->outunit[2]=UNIT_8_BIT;
	}
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
	{
		  out_length+=finished_size;
		cb_data->outsize[2]=finished_size;
		cb_data->outunit[2]=UNIT_8_BIT;
	}
    }
  else if (master_secret_ret == NOT_RETURNED)
    {
	  cb_data->outcnt=2;
		cb_data->outptr[0]=client_finished_message;
      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
	{
		  out_length+=ROUNDUP8 (finished_size + hash_size + 1);
		cb_data->outsize[0]=ROUNDUP8 (finished_size + hash_size + 1);
		cb_data->outunit[0]=UNIT_8_BIT;
	}
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
	{
		  out_length+=finished_size;
		cb_data->outsize[0]=finished_size;
		cb_data->outunit[0]=UNIT_8_BIT;
	}
		cb_data->outptr[1]=server_finished_message;
      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
	{
		  out_length+=ROUNDUP8 (finished_size + hash_size + 1);
			cb_data->outsize[1]=ROUNDUP8 (finished_size + hash_size + 1);
			cb_data->outunit[1]=UNIT_8_BIT;
	}
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
	{
		  out_length+=finished_size;
		cb_data->outsize[1]=finished_size;
		cb_data->outunit[1]=UNIT_8_BIT;
	}
    }	
	cb_data->output=(Uint8*)cavium_malloc(out_length+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->out_len=out_length;

	req->size = modlength;
  req->dlen = 8 + modlength + 32 + 32 + handshake_length;
  req->rlen = cb_data->out_len;
  req->ctx_ptr = context_handle;
  req->param = param;

  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
	 req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1RsaServerFull3Des failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1RsaServerVerify3Des (n1_request_type request_type,
			 Uint64 context_handle,
			 Uint64 * key_handle,
			 HashType hash_type,
			 SslVersion ssl_version,
			 DesType des_type,
			 MasterSecretReturn master_secret_ret,
			 Uint16 modlength,
			 Uint8 * encrypt_premaster_secret,
			 Uint8 * client_random,
			 Uint8 * server_random,
			 Uint16 handshake_length,
			 Uint8 * handshake,
			 Uint8 * verify_data,
			 Uint8 * encrypt_master_secret, Uint32 * request_id,
			 void (*call_back)(int, void*),
			 void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 hash_size;
	Uint64 key_handle_temp;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {
      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((modlength >= 64) && (modlength <= 128))
    {
      req->opcode =
	(master_secret_ret << 14) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER;
    }
  else if ((modlength > 128) && (modlength <= 256))
    {
      req->opcode =
	(master_secret_ret << 14) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER_LARGE;

#if defined(CSP1_API_DEBUG)
    }
  else
    {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
    }

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8 + modlength + 32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	//*key_handle=htobe64((Uint64)(*key_handle));
	//memcpy(cb_data->input, key_handle, 8);
	key_handle_temp = htobe64((Uint64)(*key_handle));
	memcpy(cb_data->input, &key_handle_temp, 8);
	memcpy(cb_data->input+8, encrypt_premaster_secret, modlength);
	memcpy(cb_data->input+8+modlength, client_random, 32);
	memcpy(cb_data->input+40+modlength, server_random, 32);
	memcpy(cb_data->input+72+modlength, handshake, handshake_length);
	cb_data->in_len=72+modlength+handshake_length;

  if (master_secret_ret == RETURN_ENCRYPTED)
    {
	  cb_data->outcnt=2;
	  cb_data->out_len=48+36;
		cb_data->outptr[0]=encrypt_master_secret;
		cb_data->outsize[0]=48;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->outptr[1]=verify_data;
		cb_data->outsize[1]=36;
		cb_data->outunit[1]=UNIT_8_BIT;
    }
  else
    {
	  cb_data->out_len=36;
	  cb_data->outcnt=1;
		cb_data->outptr[0]=verify_data;
		cb_data->outsize[0]=36;
		cb_data->outunit[0]=UNIT_8_BIT;
    }	
  cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
  if(cb_data->output==NULL){
	  printk("no memory\n");
	  cavium_free(req);
	  cavium_free(cb_data->input);
	  cavium_free(cb_data);
	  return -1;
  }

  req->size = modlength;
  req->param =
    (hash_type) | (ssl_version << 2) | (des_type << 3) | (1 << 7);
  req->dlen = 8 + modlength + 32 + 32 + handshake_length;
  req->rlen=cb_data->out_len;

  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1RsaServerVerify3Des failed\n");
		return -1;
	}	

   return 0;
}


Uint32
Kernel_Csp1RsaServerFullAes (n1_request_type request_type,
		      Uint64 context_handle,
		      Uint64 * key_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      AesType aes_type,
		      MasterSecretReturn master_secret_ret,
		      ClientFinishMessageOutput clnt_fin_msg_out,
		      ServerFinishMessageOutput srvr_fin_msg_out,
		      Uint16 client_pad_length,
		      Uint16 server_pad_length,
		      Uint16 modlength,
		      Uint8 * encrypt_premaster_secret,
		      Uint8 * client_random,
		      Uint8 * server_random,
		      Uint16 handshake_length,
		      Uint8 * handshake,
		      Uint8 * client_finished_message,
		      Uint8 * server_finished_message,
		      Uint8 * encrypt_master_secret, Uint32 * request_id,
			  void (*call_back)(int, void*),
			  void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 finished_size;
	Uint16 hash_size;
	Uint16 param = 0;
	Uint64 key_handle_temp;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((modlength & 0x7) != 0)

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((modlength >= 64) && (modlength <= 128))
    {
      req->opcode =
	(master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER;
    }
  else if ((modlength > 128) && (modlength <= 256))
    {
      req->opcode =
	(master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER_LARGE;
#if defined(CSP1_API_DEBUG)
    }
  else
    {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
    }

  finished_size = 16 + 24 * ssl_version;

  param = (hash_type) | (ssl_version << 2) | (aes_type << 3) | (1 << 7);

  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    param |= (1 << 8);

  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    param |= (1 << 9);

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8 + modlength + 32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	//*key_handle=htobe64((Uint64)(*key_handle));
	//memcpy(cb_data->input, key_handle, 8);
	key_handle_temp = htobe64((Uint64)(*key_handle));
	memcpy(cb_data->input, &key_handle_temp, 8);
	memcpy(cb_data->input+8, encrypt_premaster_secret, modlength);
	memcpy(cb_data->input+8+modlength, client_random, 32);
	memcpy(cb_data->input+40+modlength, server_random, 32);
	memcpy(cb_data->input+72+modlength, handshake, handshake_length);
	cb_data->in_len=72+modlength+handshake_length;
  if (master_secret_ret == RETURN_ENCRYPTED)
    {
	  cb_data->out_len=48;
		cb_data->outcnt=3;

		cb_data->outptr[0]=encrypt_master_secret;
		cb_data->outsize[0]=48;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->outptr[1]=client_finished_message;
     if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
	{
		 cb_data->outsize[1]=ROUNDUP16 (finished_size + hash_size + 1);
		 cb_data->out_len+=ROUNDUP16 (finished_size + hash_size + 1);
		 cb_data->outunit[1]=UNIT_8_BIT;
	}
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
	{
		  cb_data->out_len+=finished_size;
		  cb_data->outsize[1]=finished_size;
		  cb_data->outunit[1]=UNIT_8_BIT;
	}
		cb_data->outptr[2]=server_finished_message;
      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
	{
		  cb_data->out_len+=ROUNDUP16 (finished_size + hash_size + 1);
		  cb_data->outsize[2]=ROUNDUP16 (finished_size + hash_size + 1);
		  cb_data->outunit[2]=UNIT_8_BIT;
	}
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
	{
		  cb_data->out_len+=finished_size;
		  cb_data->outsize[2]=finished_size;
		  cb_data->outunit[2]=UNIT_8_BIT;
	}
	}
  else if (master_secret_ret == NOT_RETURNED)
    {
	  cb_data->outcnt=2;
	  cb_data->outptr[0]=client_finished_message;
      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
	{
		  cb_data->out_len+=ROUNDUP16 (finished_size + hash_size + 1);
			cb_data->outsize[0]=ROUNDUP16 (finished_size + hash_size + 1);
			cb_data->outunit[1]=UNIT_8_BIT;
	}
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
	{
		  cb_data->out_len+=finished_size;
		  cb_data->outsize[0]=finished_size;
		  cb_data->outunit[0]=UNIT_8_BIT;
	}
		cb_data->outptr[1]=server_finished_message;
      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
	{
		  cb_data->out_len+=ROUNDUP16 (finished_size + hash_size + 1);
			cb_data->outsize[1]=ROUNDUP16 (finished_size + hash_size + 1);
			cb_data->outunit[1]=UNIT_8_BIT;
	}
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
	{
		  cb_data->out_len+=finished_size;
		  cb_data->outsize[1]=finished_size;
		  cb_data->outunit[1]=UNIT_8_BIT;
	}
    }
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}

  req->size = modlength;
  req->dlen = 8 + modlength + 32 + 32 + handshake_length;
  req->rlen = cb_data->out_len;
  req->param = param;
  req->ctx_ptr = context_handle;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1RsaServerVerify3Des failed\n");
		return -1;
	}	

   return 0;
}


Uint32
Kernel_Csp1RsaServerVerifyAes (n1_request_type request_type,
			Uint64 context_handle,
			Uint64 * key_handle,
			HashType hash_type,
			SslVersion ssl_version,
			AesType aes_type,
			MasterSecretReturn master_secret_ret,
			Uint16 modlength,
			Uint8 * encrypt_premaster_secret,
			Uint8 * client_random,
			Uint8 * server_random,
			Uint16 handshake_length,
			Uint8 * handshake,
			Uint8 * verify_data,
			Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 hash_size;
	Uint64 key_handle_temp;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((modlength & 0x7) != 0)

    return ERR_ILLEGAL_INPUT_LENGTH;
  if ((modlength >= 64) && (modlength <= 128))
    {
      req->opcode =
	(master_secret_ret << 14) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER;
    }
  else if ((modlength > 128) && (modlength <= 256))
    {
      req->opcode =
	(master_secret_ret << 14) | (global_dma_mode << 7) |
	MAJOR_OP_RSASERVER_LARGE;

#if defined(CSP1_API_DEBUG)
    }
  else
    {

      return ERR_ILLEGAL_INPUT_LENGTH;
#endif
    }

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(8 + modlength + 32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	//*key_handle=htobe64((Uint64)(*key_handle));
	//memcpy(cb_data->input, key_handle, 8);
	key_handle_temp = htobe64((Uint64)(*key_handle));
	memcpy(cb_data->input, &key_handle_temp, 8);
	memcpy(cb_data->input+8, encrypt_premaster_secret, modlength);
	memcpy(cb_data->input+8+modlength, client_random, 32);
	memcpy(cb_data->input+40+modlength, server_random, 32);
	memcpy(cb_data->input+72+modlength, handshake, handshake_length);
	cb_data->in_len=72+modlength+handshake_length;

  if (master_secret_ret == RETURN_ENCRYPTED)
    {
	  cb_data->out_len=48+36;
		cb_data->outcnt=2;
		cb_data->outptr[0]=encrypt_master_secret;
		cb_data->outsize[0]=48;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->outptr[1]=verify_data;
		cb_data->outsize[1]=36;
		cb_data->outunit[1]=UNIT_8_BIT;
    }
  else
    {
	  cb_data->outcnt=1;
	  cb_data->out_len=36;
	  cb_data->outptr[0]=verify_data;
	  cb_data->outsize[0]=36;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
  if(cb_data->output==NULL){
	  printk("no memory\n");
	  cavium_free(req);
	  cavium_free(cb_data->input);
	  cavium_free(cb_data);
	  return -1;
  }

  req->size = modlength;
  req->param =
    (hash_type) | (ssl_version << 2) | (aes_type << 3) | (1 << 7);
  req->dlen = 8 + modlength + 32 + 32 + handshake_length;
  req->rlen=cb_data->out_len;
  req->ctx_ptr = context_handle;
  req->incnt = 1;
  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
   req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin 
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
		printk("Kernel_Csp1RsaServerVerifyAes failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1OtherFullRc4 (n1_request_type request_type,
		  Uint64 context_handle,
		  HashType hash_type,
		  SslVersion ssl_version,
		  Rc4Type rc4_type,
		  MasterSecretReturn master_secret_ret,
		  Uint16 pre_master_length,
		  Uint8 * client_random,
		  Uint8 * server_random,
		  Uint16 handshake_length,
		  Uint8 * handshake,
		  Uint8 * client_finished_message,
		  Uint8 * server_finished_message,
		  Uint8 * encrypt_master_secret, Uint32 * request_id,
		  void (*call_back)(int, void*),
		  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 finished_size;
	Uint16 hash_size;
	Uint16 param;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((ssl_version == VER3_0)
      && (((pre_master_length & 0x3) != 0) || (pre_master_length > 256)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((ssl_version == VER_TLS)
      && (((pre_master_length & 0xf) != 0) || (pre_master_length > 128)))

    return ERR_ILLEGAL_INPUT_LENGTH;
  hash_size = (hash_type == SHA1_TYPE)? 20:16;
  finished_size = 16 + 24 * ssl_version;

  param = (hash_type) | (ssl_version << 2) | (rc4_type << 3) | (1 << 7);	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}		
	memcpy(cb_data->input, client_random, 32);
	memcpy(cb_data->input+32, server_random, 32);
	memcpy(cb_data->input+64, handshake, handshake_length);
	cb_data->in_len=64+handshake_length;
	cb_data->out_len=2 * (finished_size + hash_size);
  if (master_secret_ret == RETURN_ENCRYPTED)
    {
		cb_data->out_len+=48;
		cb_data->outcnt=3;
		cb_data->outptr[0]=encrypt_master_secret;
		cb_data->outsize[0]=48;
		cb_data->outunit[0]=UNIT_8_BIT;
		cb_data->outptr[1]=client_finished_message;
		cb_data->outsize[1]=finished_size+hash_size;
		cb_data->outunit[1]=UNIT_8_BIT;
		cb_data->outptr[2]=server_finished_message;
		cb_data->outsize[2]=finished_size+hash_size;
		cb_data->outunit[2]=UNIT_8_BIT;
    }
  else
    {
	  cb_data->outcnt=2;
	  cb_data->outptr[0]=client_finished_message;
	  cb_data->outsize[0]=finished_size+hash_size;
	  cb_data->outunit[0]=UNIT_8_BIT;
	  cb_data->outptr[1]=server_finished_message;
	  cb_data->outsize[1]=finished_size+hash_size;
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}


  req->opcode = (master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
       MAJOR_OP_OTHER;
  req->size = pre_master_length;



  req->dlen = 32 + 32 + handshake_length;
  req->rlen =cb_data->out_len;
   
  req->param = param;
  
  req->ctx_ptr = context_handle;

  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1OtherFullRc4 failed\n");
		return -1;
	}	

   return 0;
}


Uint32
Kernel_Csp1OtherVerifyRc4 (n1_request_type request_type,
		    Uint64 context_handle,
		    HashType hash_type,
		    SslVersion ssl_version,
		    Rc4Type rc4_type,
		    MasterSecretReturn master_secret_ret,
		    Uint16 pre_master_length,
		    Uint8 * client_random,
		    Uint8 * server_random,
		    Uint16 handshake_length,
		    Uint8 * handshake,
		    Uint8 * verify_data,
		    Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	Uint16 hash_size;
	Uint16 param;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((ssl_version == VER3_0)
      && (((pre_master_length & 0x3) != 0) || (pre_master_length > 256)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((ssl_version == VER_TLS)
      && (((pre_master_length & 0xf) != 0) || (pre_master_length > 128)))

    return ERR_ILLEGAL_INPUT_LENGTH;


  param = (hash_type) | (ssl_version << 2) | (rc4_type << 3) | (1 << 7);

  req->opcode = (master_secret_ret << 14) | (global_dma_mode << 7) |
       MAJOR_OP_OTHER;
  req->size = pre_master_length;

  hash_size = (hash_type == SHA1_TYPE)? 20:16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, client_random, 32);
	memcpy(cb_data->input+32, server_random, 32);
	memcpy(cb_data->input+64, handshake, handshake_length);
	cb_data->in_len=64+handshake_length;
	cb_data->out_len=36;

  if (master_secret_ret == RETURN_ENCRYPTED)
    {
	  cb_data->out_len+=48;
	  cb_data->outcnt=2;
	  cb_data->outptr[0]=encrypt_master_secret;
	  cb_data->outsize[0]=48;
	  cb_data->outunit[0]=UNIT_8_BIT;
	  cb_data->outptr[1]=verify_data;
	  cb_data->outsize[1]=36;
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
  else
    {
	  cb_data->outcnt=1;
	  cb_data->outptr[0]=verify_data;
	  cb_data->outsize[0]=36;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
	
  	cb_data->output = (Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if( cb_data->output == NULL)
	{
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  
  req->dlen = 32 + 32 + handshake_length;
  req->rlen = cb_data->out_len;
  req->param = param;
  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;


  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1OtherVerifyRc4 failed\n");
		return -1;
	}	

   return 0;
}


Uint32
Kernel_Csp1OtherFull3Des (n1_request_type request_type,
		   Uint64 context_handle,
		   HashType hash_type,
		   SslVersion ssl_version,
		   DesType des_type,
		   MasterSecretReturn master_secret_ret,
		   ClientFinishMessageOutput clnt_fin_msg_out,
		   ServerFinishMessageOutput srvr_fin_msg_out,
		   Uint16 client_pad_length,
		   Uint16 server_pad_length,
		   Uint16 pre_master_length,
		   Uint8 * client_random,
		   Uint8 * server_random,
		   Uint16 handshake_length,
		   Uint8 * handshake,
		   Uint8 * client_finished_message,
		   Uint8 * server_finished_message,
		   Uint8 * encrypt_master_secret, Uint32 * request_id,
		   void (*call_back)(int, void*),
		   void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	  Uint16 param;
	  Uint16 finished_size;
	  Uint16 hash_size;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((ssl_version == VER3_0)
      && (((pre_master_length & 0x3) != 0) || (pre_master_length > 256)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((ssl_version == VER_TLS)
      && (((pre_master_length & 0xf) != 0) || (pre_master_length > 128)))

    return ERR_ILLEGAL_INPUT_LENGTH;


  finished_size = 16 + 24 * ssl_version;

  param = (hash_type) | (ssl_version << 2) | (des_type << 3) | (1 << 7);

  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    param |= (1 << 8);

  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    param |= (1 << 9);

  req->opcode = (master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
       MAJOR_OP_OTHER;
  req->size = pre_master_length;

  hash_size = (hash_type == SHA1_TYPE)? 20:16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, client_random, 32);
	memcpy(cb_data->input+32, server_random, 32);
	memcpy(cb_data->input+64, handshake, handshake_length);
	cb_data->in_len=64+handshake_length;
	cb_data->out_len=0;

  if (master_secret_ret == RETURN_ENCRYPTED)
  {
	  cb_data->out_len=48;
	  cb_data->outcnt=3;
      cb_data->outptr[0] = encrypt_master_secret;
      cb_data->outsize[0] = 48;
	  cb_data->outunit[0]=UNIT_8_BIT;
      cb_data->outptr[1] = client_finished_message;

      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);

         cb_data->outsize[1] = ROUNDUP8 (finished_size + hash_size + 1);
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[1] = finished_size;
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
		cb_data->outptr[2] = server_finished_message;

      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);

         cb_data->outsize[2] = ROUNDUP8 (finished_size + hash_size + 1);
		 cb_data->outunit[2]=UNIT_8_BIT;
      }

      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
      {
        cb_data->out_len += finished_size;

         cb_data->outsize[2] = finished_size;
		 cb_data->outunit[2]=UNIT_8_BIT;
      }
    }
  else if (master_secret_ret == NOT_RETURNED)
    {
      cb_data->outcnt = 2;
      cb_data->outptr[0] = client_finished_message;

      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);

         cb_data->outsize[0] = ROUNDUP8 (finished_size + hash_size + 1);
		 cb_data->outunit[0]=UNIT_8_BIT;
      }
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[0] = finished_size;
		 cb_data->outunit[0]=UNIT_8_BIT;
      }
      cb_data->outptr[1] = server_finished_message;
      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);
         cb_data->outsize[1] = ROUNDUP8 (finished_size + hash_size + 1);
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[1] = finished_size;
		 cb_data->outunit[1]=UNIT_8_BIT;
      }

    }
  cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
  if(cb_data->output==NULL){
	  printk("no memory\n");
	  cavium_free(req);
	  cavium_free(cb_data->input);
	  cavium_free(cb_data);
	  return -1;
  }

  req->dlen = 32 + 32 + handshake_length;

  req->rlen = cb_data->out_len;


  req->param = param;
  
  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1OtherFull3Des failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1OtherVerify3Des (n1_request_type request_type,
		     Uint64 context_handle,
		     HashType hash_type,
		     SslVersion ssl_version,
		     DesType des_type,
		     MasterSecretReturn master_secret_ret,
		     Uint16 pre_master_length,
		     Uint8 * client_random,
		     Uint8 * server_random,
		     Uint16 handshake_length,
		     Uint8 * handshake,
		     Uint8 * verify_data,
		     Uint8 * encrypt_master_secret, Uint32 * request_id,
			 void (*call_back)(int, void*),
			 void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	  Uint16 param;
	  Uint16 hash_size;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((ssl_version == VER3_0)
      && (((pre_master_length & 0x3) != 0) || (pre_master_length > 256)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((ssl_version == VER_TLS)
      && (((pre_master_length & 0xf) != 0) || (pre_master_length > 128)))

    return ERR_ILLEGAL_INPUT_LENGTH;


  param = (hash_type) | (ssl_version << 2) | (des_type << 3) | (1 << 7);

  req->opcode = (master_secret_ret << 14) | (global_dma_mode << 7) |
       MAJOR_OP_OTHER;

  req->size = pre_master_length;

  hash_size = (hash_type == SHA1_TYPE)? 20:16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(64+handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, client_random, 32);
	memcpy(cb_data->input+32, server_random, 32);
	memcpy(cb_data->input+64, handshake, handshake_length);
	cb_data->in_len=64+handshake_length;
	cb_data->out_len=36;

  if (master_secret_ret == RETURN_ENCRYPTED)
    {
      cb_data->out_len += 48;
      cb_data->outcnt = 2;

      cb_data->outptr[0] = encrypt_master_secret;
      cb_data->outsize[0] = 48;
	  cb_data->outunit[0]=UNIT_8_BIT;

      cb_data->outptr[1] = verify_data;
      cb_data->outsize[1] = 36;
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
  else
    {
      cb_data->outcnt = 1;
      cb_data->outptr[0] = verify_data;
      cb_data->outsize[0] = 36;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
  if(cb_data->output==NULL){
	  printk("no memory\n");
	  cavium_free(req);
	  cavium_free(cb_data->input);
	  cavium_free(cb_data);
	  return -1;
  }

  req->dlen = 32 + 32 + handshake_length;

  req->rlen = cb_data->out_len;


  req->param = param;

  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1OtherVerify3Des failed\n");
		return -1;
	}	

   return 0;

}

Uint32
Kernel_Csp1OtherFullAes (n1_request_type request_type,
		  Uint64 context_handle,
		  HashType hash_type,
		  SslVersion ssl_version,
		  AesType aes_type,
		  MasterSecretReturn master_secret_ret,
		  ClientFinishMessageOutput clnt_fin_msg_out,
		  ServerFinishMessageOutput srvr_fin_msg_out,
		  Uint16 client_pad_length,
		  Uint16 server_pad_length,
		  Uint16 pre_master_length,
		  Uint8 * client_random,
		  Uint8 * server_random,
		  Uint16 handshake_length,
		  Uint8 * handshake,
		  Uint8 * client_finished_message,
		  Uint8 * server_finished_message,
		  Uint8 * encrypt_master_secret, Uint32 * request_id,
		  void (*call_back)(int, void*),
		  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	  Uint16 param;
	  Uint16 finished_size;
	  Uint16 hash_size;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((ssl_version == VER3_0)
      && (((pre_master_length & 0x3) != 0) || (pre_master_length > 256)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((ssl_version == VER_TLS)
      && (((pre_master_length & 0xf) != 0) || (pre_master_length > 128)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  finished_size = 16 + 24 * ssl_version;

  param = (hash_type) | (ssl_version << 2) | (aes_type << 3) | (1 << 7);

  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    param |= (1 << 8);

  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    param |= (1 << 9);

  req->opcode = (master_secret_ret << 14) | (0x1 << 13) | (global_dma_mode << 7) |
       MAJOR_OP_OTHER;
  req->size = pre_master_length;

  hash_size = (hash_type == SHA1_TYPE)? 20:16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, client_random, 32);
	memcpy(cb_data->input+32, server_random, 32);
	memcpy(cb_data->input+64, handshake, handshake_length);
	cb_data->in_len=64+handshake_length;
	cb_data->out_len=0;
  if (master_secret_ret == RETURN_ENCRYPTED)
  {
      cb_data->out_len = 48;

      cb_data->outcnt = 3;

      cb_data->outptr[0] = encrypt_master_secret;
      cb_data->outsize[0] = 48;
	  cb_data->outunit[0]=UNIT_8_BIT;
      cb_data->outptr[1] = client_finished_message;
      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP16 (finished_size + hash_size + 1);

         cb_data->outsize[1] = ROUNDUP16 (finished_size + hash_size + 1);
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[1] = finished_size;
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
      cb_data->outptr[2] = server_finished_message;

      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP16 (finished_size + hash_size + 1);

         cb_data->outsize[2] = ROUNDUP16 (finished_size + hash_size + 1);
		 cb_data->outunit[2]=UNIT_8_BIT;
      }
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[2] = finished_size;
		 cb_data->outunit[2]=UNIT_8_BIT;
      }
  }
  else if (master_secret_ret == NOT_RETURNED)
  {
      cb_data->outcnt = 2;
      cb_data->outptr[0] = client_finished_message;

      if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP16 (finished_size + hash_size + 1);

         cb_data->outsize[0] = ROUNDUP16 (finished_size + hash_size + 1);
		 cb_data->outunit[0]=UNIT_8_BIT;
      }
      else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[0] = finished_size;
		 cb_data->outunit[0]=UNIT_8_BIT;
      }
      cb_data->outptr[1] = server_finished_message;
      if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
      {
         cb_data->out_len += ROUNDUP16 (finished_size + hash_size + 1);

         cb_data->outsize[1] = ROUNDUP16 (finished_size + hash_size + 1);
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
      else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
      {
         cb_data->out_len += finished_size;

         cb_data->outsize[1] = finished_size;
		 cb_data->outunit[1]=UNIT_8_BIT;
      }
   }
  cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
  if(cb_data->output==NULL){
	  printk("no memory\n");
	  cavium_free(req);
	  cavium_free(cb_data->input);
	  cavium_free(cb_data);
	  return -1;
  }
  
  req->dlen = 32 + 32 + handshake_length;
  req->rlen = cb_data->out_len;
  req->param = param;
  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1OtherFullAes failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1OtherVerifyAes (n1_request_type request_type,
		    Uint64 context_handle,
		    HashType hash_type,
		    SslVersion ssl_version,
		    AesType aes_type,
		    MasterSecretReturn master_secret_ret,
		    Uint16 pre_master_length,
		    Uint8 * client_random,
		    Uint8 * server_random,
		    Uint16 handshake_length,
		    Uint8 * handshake,
		    Uint8 * verify_data,
		    Uint8 * encrypt_master_secret, Uint32 * request_id,
			void (*call_back)(int, void*),
			void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	  Uint16 param;
	  Uint16 hash_size;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((ssl_version == VER3_0)
      && (((pre_master_length & 0x3) != 0) || (pre_master_length > 256)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  if ((ssl_version == VER_TLS)
      && (((pre_master_length & 0xf) != 0) || (pre_master_length > 128)))

    return ERR_ILLEGAL_INPUT_LENGTH;

  param = (hash_type) | (ssl_version << 2) | (aes_type << 3) | (1 << 7);

  req->opcode = (master_secret_ret << 14) | (global_dma_mode << 7) |
       MAJOR_OP_OTHER;

  req->size = pre_master_length;

  hash_size = (hash_type == SHA1_TYPE)? 20:16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(32 + 32 + handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, client_random, 32);
	memcpy(cb_data->input+32, server_random, 32);
	memcpy(cb_data->input+64, handshake, handshake_length);
	cb_data->in_len=64+handshake_length;
	cb_data->out_len=36;
  if (master_secret_ret == RETURN_ENCRYPTED)
    {
      cb_data->out_len += 48;
      cb_data->outcnt = 2;
      cb_data->outptr[0] = encrypt_master_secret;
      cb_data->outsize[0] = 48;
	  cb_data->outunit[0]=UNIT_8_BIT;
      cb_data->outptr[1] = verify_data;
      cb_data->outsize[1] = 36;
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
  else
    {
      cb_data->outcnt = 1;
      cb_data->outptr[0] = verify_data;
      cb_data->outsize[0] = 36;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }	
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}

  req->dlen = 32 + 32 + handshake_length;

  req->rlen = 36;

  req->param = param;

  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;


  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1OtherVerifyAes failed\n");
		return -1;
	}	

   return 0;

}


Uint32
Kernel_Csp1FinishedRc4Finish (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       Uint16 handshake_length,
		       Uint8 * handshake,
		       Uint8 * client_finished_message,
		       Uint8 * server_finished_message, Uint32 * request_id,
			   void (*call_back)(int, void*), 
			   void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
	  Uint16 finished_size;
	  Uint16 hash_size;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
 if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  finished_size = 16 + 24 * ssl_version;
  req->opcode = (global_dma_mode << 7) | MAJOR_OP_FINISHED;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}		
	memcpy(cb_data->input, handshake, handshake_length);
	cb_data->in_len=handshake_length;
	cb_data->out_len=2*(finished_size+hash_size);
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}



 

  req->size = 0;
  req->param = (hash_type) | (ssl_version << 2) | (RC4_128 << 3);
  req->dlen = handshake_length;
  req->rlen =cb_data->out_len;
  req->ctx_ptr = context_handle;

  cb_data->outcnt=2;
  cb_data->outptr[0] = client_finished_message;
  cb_data->outsize[0] = finished_size + hash_size;
  cb_data->outunit[0]=UNIT_8_BIT;
  cb_data->outptr[1] = server_finished_message;
  cb_data->outsize[1] = finished_size + hash_size;
  cb_data->outunit[1]=UNIT_8_BIT;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1FinishedRc4Finish failed\n");
		return -1;
	}	

   return 0;
}


Uint32
Kernel_Csp1Finished3DesFinish (n1_request_type request_type,
			Uint64 context_handle,
			HashType hash_type,
			SslVersion ssl_version,
			ClientFinishMessageOutput clnt_fin_msg_out,
			ServerFinishMessageOutput srvr_fin_msg_out,
			Uint16 client_pad_length,
			Uint16 server_pad_length,
			Uint16 handshake_length,
			Uint8 * handshake,
			Uint8 * client_finished_message,
			Uint8 * server_finished_message, Uint32 * request_id,
			void (*call_back)(int, void *),
			void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 finished_size;
  Uint16 hash_size;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  finished_size = 16 + 24 * ssl_version;
  req->opcode = (global_dma_mode << 7) | MAJOR_OP_FINISHED;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;
	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}		
	memcpy(cb_data->input, handshake, handshake_length);
	cb_data->in_len=handshake_length;
	cb_data->out_len=0;
	cb_data->outcnt=2;
  if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
    {
      cb_data->out_len = ROUNDUP8 (finished_size + hash_size + 1);
      cb_data->outsize[0] = ROUNDUP8 (finished_size + hash_size + 1);
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    {
      cb_data->out_len = finished_size;
      cb_data->outsize[0] = finished_size;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  cb_data->outptr[1] = server_finished_message;
  if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
    {
      cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);
      cb_data->outsize[1] = ROUNDUP8 (finished_size + hash_size + 1);
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
  else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    {
      cb_data->out_len += finished_size;
      cb_data->outsize[1] = finished_size;
	  cb_data->outunit[1]=UNIT_8_BIT;
	}
    cb_data->outptr[0] = client_finished_message;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  req->size = 0;
  req->param = (hash_type) | (ssl_version << 2) | (DES << 3);

  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 8);

  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 9);

  req->dlen = handshake_length;
  req->rlen = cb_data->out_len;


  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1Finished3DesFinish failed\n");
		return -1;
	}	

   return 0;

}

Uint32
Kernel_Csp1FinishedAesFinish (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       AesType aes_type,
		       ClientFinishMessageOutput clnt_fin_msg_out,
		       ServerFinishMessageOutput srvr_fin_msg_out,
		       Uint16 client_pad_length,
		       Uint16 server_pad_length,
		       Uint16 handshake_length,
		       Uint8 * handshake,
		       Uint8 * client_finished_message,
		       Uint8 * server_finished_message, Uint32 * request_id,
			   void (*call_back)(int, void*),
			   void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 finished_size;
  Uint16 hash_size;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  finished_size = 16 + 24 * ssl_version;
  req->opcode = (global_dma_mode << 7) | MAJOR_OP_FINISHED;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(handshake_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, handshake, handshake_length);
	cb_data->in_len=handshake_length;
	cb_data->out_len=0;
  cb_data->outcnt = 2;
  cb_data->outptr[0] = client_finished_message;
  if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
    {
      cb_data->out_len = ROUNDUP16 (finished_size + hash_size + 1);
      cb_data->outsize[0] = ROUNDUP16 (finished_size + hash_size + 1);
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    {
      cb_data->out_len = finished_size;
      cb_data->outsize[0] = finished_size;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  cb_data->outptr[1] = server_finished_message;
  if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
    {
      cb_data->out_len += ROUNDUP16 (finished_size + hash_size + 1);
      cb_data->outsize[1] = ROUNDUP16 (finished_size + hash_size + 1);
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
  else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    {
      cb_data->out_len += finished_size;
      cb_data->outsize[1] = finished_size;
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  req->size = 0;
  req->param = (hash_type) | (ssl_version << 2) | (aes_type << 3);

  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 8);

  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 9);

  req->dlen = handshake_length;
  req->rlen = cb_data->out_len;

  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1FinishedAesFinish failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1ResumeRc4 (n1_request_type request_type,
	       Uint64 context_handle,
	       HashType hash_type,
	       SslVersion ssl_version,
	       Rc4Type rc4_type,
	       MasterSecretInput master_secret_inp,
	       Uint8 * client_random,
	       Uint8 * server_random,
	       Uint8 * encrypt_master_secret,
	       Uint16 handshake_length,
	       Uint8 * handshake,
	       Uint8 * client_finished_message,
	       Uint8 * server_finished_message, Uint32 * request_id,
		   void (*call_back)(int, void*),
		   void* connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 finished_size;
  Uint16 hash_size;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  finished_size = 16 + 24 * ssl_version;


  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;
	cb_data->in_len=32+32+handshake_length;
  if (master_secret_inp == INPUT_ENCRYPTED)
    {
		cb_data->in_len+=48;
		cb_data->input=(Uint8*)cavium_malloc(cb_data->in_len, NULL);
		if(cb_data->input==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data);
			return -1;
		}
		memcpy(cb_data->input, encrypt_master_secret, 48);
		memcpy(cb_data->input+48, client_random, 32);
		memcpy(cb_data->input+80, server_random, 32);
		memcpy(cb_data->input+112, handshake, handshake_length);
    }
  else if (master_secret_inp == READ_FROM_CONTEXT)
    {
		cb_data->input=(Uint8*)cavium_malloc(cb_data->in_len, NULL);
		if(cb_data->input==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data);
			return -1;
		}
		memcpy(cb_data->input, client_random, 32);
		memcpy(cb_data->input+32, server_random, 32);
		memcpy(cb_data->input+64, handshake, handshake_length);
    }
	cb_data->out_len=2*(finished_size+hash_size);
  cb_data->outcnt = 2;

  cb_data->outptr[0] = client_finished_message;
  cb_data->outsize[0] = finished_size + hash_size;
  cb_data->outunit[0]=UNIT_8_BIT;
  cb_data->outptr[1] = server_finished_message;
  cb_data->outsize[1] = finished_size + hash_size;
  cb_data->outunit[1]=UNIT_8_BIT;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}

  req->opcode = (master_secret_inp << 14) | MAJOR_OP_RESUME;
  req->size = 0;
  req->param =
    (hash_type) | (ssl_version << 2) | (rc4_type << 3) | (1 << 7);
  req->dlen = cb_data->in_len;
  req->rlen = cb_data->out_len;
  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1ResumeRc4 failed\n");
		return -1;
	}	

   return 0;
}


Uint32
Kernel_Csp1Resume3Des (n1_request_type request_type,
		Uint64 context_handle,
		HashType hash_type,
		SslVersion ssl_version,
		DesType des_type,
		MasterSecretInput master_secret_inp,
		ClientFinishMessageOutput clnt_fin_msg_out,
		ServerFinishMessageOutput srvr_fin_msg_out,
		Uint16 client_pad_length,
		Uint16 server_pad_length,
		Uint8 * client_random,
		Uint8 * server_random,
		Uint8 * encrypt_master_secret,
		Uint16 handshake_length,
		Uint8 * handshake,
		Uint8 * client_finished_message,
		Uint8 * server_finished_message, Uint32 * request_id,
		void (*call_back)(int, void*),
		void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 finished_size;
  Uint16 hash_size;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  finished_size = 16 + 24 * ssl_version;

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;
	cb_data->in_len=32+32+handshake_length;
	cb_data->out_len=0;
  if (master_secret_inp == INPUT_ENCRYPTED)
    {
		cb_data->in_len+=48;
		cb_data->input=(Uint8*)cavium_malloc(cb_data->in_len, NULL);
		if(cb_data->input==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data);
			return -1;
		}
		memcpy(cb_data->input, encrypt_master_secret, 48);
		memcpy(cb_data->input+48, client_random, 32);
		memcpy(cb_data->input+80, server_random, 32);
		memcpy(cb_data->input+112, handshake, handshake_length);
    }
  else if (master_secret_inp == READ_FROM_CONTEXT)
    {
		cb_data->input=(Uint8*)cavium_malloc(cb_data->in_len, NULL);
		if(cb_data->input==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data);
			return -1;
		}
		memcpy(cb_data->input, client_random, 32);
		memcpy(cb_data->input+32, server_random, 32);
		memcpy(cb_data->input+64, handshake, handshake_length);
    }
  cb_data->outcnt = 2;

  cb_data->outptr[0] = client_finished_message;

  if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
  {
      cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);
      cb_data->outsize[0] = ROUNDUP8 (finished_size + hash_size + 1);
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    {
      cb_data->out_len += finished_size;
      cb_data->outsize[0] = finished_size;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  cb_data->outptr[1] = server_finished_message;
  if (srvr_fin_msg_out == RETURN_SFM_ENCRYPTED)
    {
      cb_data->out_len += ROUNDUP8 (finished_size + hash_size + 1);
      cb_data->outsize[1] = ROUNDUP8 (finished_size + hash_size + 1);
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
  else if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    {

      cb_data->out_len += finished_size;
      cb_data->outsize[1] = finished_size;
	  cb_data->outunit[1]=UNIT_8_BIT;
    }
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  req->opcode = (master_secret_inp << 14) | MAJOR_OP_RESUME;
  req->size = 0;

  req->param =
    (hash_type) | (ssl_version << 2) | (des_type << 3) | (1 << 7);
  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 8);
  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 9);

  req->dlen = cb_data->in_len;
  req->rlen = cb_data->out_len;
  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;


  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1Resume3Des failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1ResumeAes (n1_request_type request_type,
	       Uint64 context_handle,
	       HashType hash_type,
	       SslVersion ssl_version,
	       AesType aes_type,
	       MasterSecretInput master_secret_inp,
	       ClientFinishMessageOutput clnt_fin_msg_out,
	       ServerFinishMessageOutput srvr_fin_msg_out,
	       Uint16 client_pad_length,
	       Uint16 server_pad_length,
	       Uint8 * client_random,
	       Uint8 * server_random,
	       Uint8 * encrypt_master_secret,
	       Uint16 handshake_length,
	       Uint8 * handshake,
	       Uint8 * client_finished_message,
	       Uint8 * server_finished_message, Uint32 * request_id,
		   void (*call_back)(int, void *),
		   void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 finished_size;
  Uint16 hash_size;

	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  finished_size = 16 + 24 * ssl_version;

  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;
	cb_data->in_len=64+handshake_length;
    if (master_secret_inp == INPUT_ENCRYPTED)
    {
		cb_data->in_len+=48;
		cb_data->input=(Uint8*)cavium_malloc(cb_data->in_len, NULL);
		if(cb_data->input==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data);
			return -1;
		}
		memcpy(cb_data->input, encrypt_master_secret, 48);
		memcpy(cb_data->input+48, client_random, 32);
		memcpy(cb_data->input+80, server_random, 32);
		memcpy(cb_data->input+112, handshake, handshake_length);
    }
  else if (master_secret_inp == READ_FROM_CONTEXT)
    {
		cb_data->input=(Uint8*)cavium_malloc(cb_data->in_len, NULL);
		if(cb_data->input==NULL){
			printk("no memory\n");
			cavium_free(req);
			cavium_free(cb_data);
			return -1;
		}
		memcpy(cb_data->input, client_random, 32);
		memcpy(cb_data->input+32, server_random, 32);
		memcpy(cb_data->input+64, handshake, handshake_length);
    }
  cb_data->out_len=0;
  cb_data->outcnt = 2;

  cb_data->outptr[0] = client_finished_message;

  if (clnt_fin_msg_out == RETURN_CFM_ENCRYPTED)
    {
      cb_data->out_len += ROUNDUP16 (finished_size + hash_size + 1);

      cb_data->outsize[0] = ROUNDUP16 (finished_size + hash_size + 1);
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
  else if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    {

      cb_data->out_len += finished_size;
      cb_data->outsize[0] = finished_size;
	  cb_data->outunit[0]=UNIT_8_BIT;
    }
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  req->opcode = (master_secret_inp << 14) | MAJOR_OP_RESUME;
  req->size = 0;

  req->param =
    (hash_type) | (ssl_version << 2) | (aes_type << 3) | (1 << 7);
  if (clnt_fin_msg_out == RETURN_CFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 8);
  if (srvr_fin_msg_out == RETURN_SFM_UNENCRYPTED)
    req->param |= (Uint16) (1 << 9);

  req->dlen = cb_data->in_len;
  req->rlen = cb_data->out_len;

  req->ctx_ptr = context_handle;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
	req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1ResumeAes failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1EncryptRecordRc4 (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      MessageType message_type,
		      Uint16 message_length,
		      Uint8 * message, Uint8 * record, Uint32 * request_id,
			  void (*call_back)(int, void*),
			  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 hash_size;
  Uint64 context_offset;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));

  if (ssl_party == SSL_SERVER)
    context_offset = 8 * 62;
  else
    context_offset = 8 * 22;

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if (message_length > 0x4400)

    return ERR_ILLEGAL_INPUT_LENGTH;

  req->opcode =
    (message_type << 12) | (global_dma_mode << 7) |
    MAJOR_OP_ENCRYPT_DECRYPT_RECORD;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(message_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, message, message_length);
	cb_data->in_len=message_length;
	cb_data->out_len=message_length+hash_size;
	cb_data->outcnt=1;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  cb_data->outptr[0] = record;
  cb_data->outsize[0] = message_length + hash_size;
  cb_data->outunit[0]=UNIT_8_BIT;

  req->param = (hash_type) | (ssl_version << 2) | (RC4_128 << 3);
  req->dlen = cb_data->in_len;
  req->rlen = cb_data->out_len;

  req->size = message_length;
  req->ctx_ptr = context_handle + context_offset;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1EncryptRecordRc4 failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1DecryptRecordRc4 (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      MessageType message_type,
		      Uint16 record_length,
		      Uint8 * record, Uint8 * message, Uint32 * request_id,
			  void (*call_back)(int, void*),
			  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 hash_size;
  Uint64 context_offset;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if (ssl_party == SSL_SERVER)
    context_offset = 8 * 22;
  else
    context_offset = 8 * 62;

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if (record_length > 0x4400)

    return ERR_ILLEGAL_INPUT_LENGTH;

  req->opcode =
    (message_type << 12) | (global_dma_mode << 7) |
    MAJOR_OP_ENCRYPT_DECRYPT_RECORD;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;
	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(record_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, record, record_length);
	cb_data->in_len=record_length;
	cb_data->out_len=record_length;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
	cb_data->outcnt=1;
  cb_data->outptr[0] = message;
  cb_data->outsize[0] = record_length;
  cb_data->outunit[0]=UNIT_8_BIT;

  req->param = (hash_type) | (ssl_version << 2) | (RC4_128 << 3) | (1 << 7);
  req->dlen = record_length;
  req->rlen = record_length;
  req->size = record_length;
  req->ctx_ptr = context_handle + context_offset;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;
  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin
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
		printk("Kernel_Csp1DecryptRecordRc4 failed\n");
		return -1;
	}	

   return 0;

}

Uint32
Kernel_Csp1EncryptRecord3Des (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       SslPartyType ssl_party,
		       MessageType message_type,
		       Uint16 pad_length,
		       Uint16 message_length,
		       Uint8 * message,
		       Uint16 * record_length,
		       Uint8 * record, Uint32 * request_id,
			   void (*call_back)(int, void*),
			   void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 hash_size;
  Uint64 context_offset;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if (ssl_party == SSL_SERVER)
    context_offset = 8 * 34;
  else
    context_offset = 8 * 22;

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if (message_length > 0x4400)

    return ERR_ILLEGAL_INPUT_LENGTH;

  req->opcode =
    (message_type << 12) | (global_dma_mode << 7) |
    MAJOR_OP_ENCRYPT_DECRYPT_RECORD;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;

  *record_length = ROUNDUP8 (message_length + hash_size + 1);	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(message_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, message, message_length);
	cb_data->in_len=message_length;
	cb_data->outcnt=1;
	cb_data->out_len=*record_length;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}	

  cb_data->outptr[0] = record;
  cb_data->outsize[0] = *record_length;
  cb_data->outunit[0]=UNIT_8_BIT;

  req->size = message_length;
  req->param = (hash_type) | (ssl_version << 2) | (DES << 3);
  req->dlen = message_length;
  req->rlen = *record_length;
  req->ctx_ptr = context_handle + context_offset;

  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;

  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1EncryptRecord3Des failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1DecryptRecord3Des (n1_request_type request_type,
		       Uint64 context_handle,
		       HashType hash_type,
		       SslVersion ssl_version,
		       SslPartyType ssl_party,
		       MessageType message_type,
		       Uint16 record_length,
		       Uint8 * record,
		       Uint16 * message_length,
		       Uint8 * message, Uint32 * request_id,
			   void (*call_back)(int, void *),
			   void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 hash_size;
  Uint64 context_offset;
  
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if (ssl_party == SSL_SERVER)
    context_offset = 8 * 22;
  else
    context_offset = 8 * 34;
#if defined(CSP1_API_DEBUG)
  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((record_length > 0x4400) || ((record_length & 0x7) != 0))

    return ERR_ILLEGAL_INPUT_LENGTH;
#endif

  req->opcode =
    (message_type << 12) | (global_dma_mode << 7) |
    MAJOR_OP_ENCRYPT_DECRYPT_RECORD;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(16 + record_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, record+record_length-16, 16);
	memcpy(cb_data->input+16, record, record_length);
	cb_data->in_len=16+record_length;
	cb_data->outcnt=1;
	cb_data->out_len=record_length;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  cb_data->outptr[0] = message;
  cb_data->outsize[0] = record_length;
  cb_data->outunit[0]=UNIT_8_BIT;



  req->size = record_length;
  req->param = (hash_type) | (ssl_version << 2) | (DES << 3) | (1 << 7);
  req->dlen = 16 + record_length;
  req->rlen = record_length;

  req->ctx_ptr = context_handle + context_offset;

  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;



  req->req_type = request_type;

  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
  req->dma_mode = global_dma_mode;
  *message_length = record_length - hash_size
    - *(Uint8 *) (message + record_length - 1) - 1;
   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
  
	if (do_request(&cavium_dev[0], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1DecryptRecord3Des failed\n");
		return -1;
	}	

   return 0;

}

Uint32
Kernel_Csp1EncryptRecordAes (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      AesType aes_type,
		      MessageType message_type,
		      Uint16 pad_length,
		      Uint16 message_length,
		      Uint8 * message,
		      Uint16 * record_length,
		      Uint8 * record, Uint32 * request_id,
			  void (*call_back)(int, void *),
			  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 hash_size;
  Uint64 context_offset;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));
  if (ssl_party == SSL_SERVER)
    context_offset = 8 * 38;
  else
    context_offset = 8 * 22;

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if (message_length > 0x4400)

    return ERR_ILLEGAL_INPUT_LENGTH;

  req->opcode =
    (message_type << 12) | (global_dma_mode << 7) |
    MAJOR_OP_ENCRYPT_DECRYPT_RECORD;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;

  *record_length = ROUNDUP16 (message_length + hash_size + 1);
	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(message_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}	
	memcpy(cb_data->input, message, message_length);
	cb_data->in_len=message_length;
	cb_data->outcnt=1;
	cb_data->out_len=*record_length;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}
  cb_data->outptr[0] = record;
  cb_data->outsize[0] = *record_length;
  cb_data->outunit[0]=UNIT_8_BIT;
	req->size = message_length;
  req->param = (hash_type) | (ssl_version << 2) | (aes_type << 3);
  req->dlen = message_length;
  req->rlen = *record_length;
  req->ctx_ptr = context_handle + context_offset;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;

  req->req_type = request_type;


  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
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
		printk("Kernel_Csp1EncryptRecordAes failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1DecryptRecordAes (n1_request_type request_type,
		      Uint64 context_handle,
		      HashType hash_type,
		      SslVersion ssl_version,
		      SslPartyType ssl_party,
		      AesType aes_type,
		      MessageType message_type,
		      Uint16 record_length,
		      Uint8 * record,
		      Uint16 * message_length,
		      Uint8 * message, Uint32 * request_id,
			  void (*call_back)(int, void *),
			  void *connect)
{
	struct call_back_data *cb_data;
	n1_request_buffer *req;	
  Uint16 hash_size;
  Uint64 context_offset;
	if(CSP1_driver_handle==-1){
		printk("no device\n");
		return -1;
	}
	kernel_call=1;

	req=(n1_request_buffer *)cavium_malloc(sizeof(n1_request_buffer), NULL);
	if(req==NULL){
		printk("no memory %s, %d\n", __FILE__, __LINE__);
		return -1;
	}
	memset(req, 0, sizeof(n1_request_buffer));

  if (ssl_party == SSL_SERVER)
    context_offset = 8 * 22;
  else
    context_offset = 8 * 38;

  if ((context_handle & 0x8000000000000000ULL) != 0)
    {

      if ((context_handle & 0xf) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }
  else
    {

      if ((context_handle & 0x7) != 0)

	return ERR_ILLEGAL_CONTEXT_HANDLE;

    }

  if ((record_length > 0x4400) || ((record_length & 0xf) != 0))

    return ERR_ILLEGAL_INPUT_LENGTH;

  req->opcode =
    (message_type << 12) | (global_dma_mode << 7) |
    MAJOR_OP_ENCRYPT_DECRYPT_RECORD;
  hash_size = (hash_type == SHA1_TYPE) ? 20 : 16;	

	cb_data=(struct call_back_data*)cavium_malloc(sizeof(struct call_back_data), NULL);
	if(cb_data==NULL){
		cavium_free(req);
		printk("no memory\n");
		return -1;
	}

	memset(cb_data, 0, sizeof(struct call_back_data));
	cb_data->context=(void*)connect;

	cb_data->input=(Uint8*)cavium_malloc(32 + record_length, NULL);
	if(cb_data->input==NULL){
		printk("faulse to memory\n");
		cavium_free(req);
		cavium_free(cb_data);
		return -1;
	}
	memcpy(cb_data->input, record+record_length-32, 32);
	memcpy(cb_data->input+32, record, record_length);
	cb_data->in_len=32+record_length;
	cb_data->outcnt=1;
	cb_data->out_len=record_length;
	cb_data->output=(Uint8*)cavium_malloc(cb_data->out_len+8, NULL);
	if(cb_data->output==NULL){
		printk("no memory\n");
		cavium_free(req);
		cavium_free(cb_data->input);
		cavium_free(cb_data);
		return -1;
	}	
  cb_data->outptr[0] = message;
  cb_data->outsize[0] = record_length;
  cb_data->outunit[0]=UNIT_8_BIT;



  req->size = record_length;
  req->param =
    (hash_type) | (ssl_version << 2) | (aes_type << 3) | (1 << 7);
  req->dlen = 32 + record_length;
  req->rlen = record_length;

  req->ctx_ptr = context_handle + context_offset;
  req->incnt = 1;

  req->inptr[0] = cb_data->input;
  req->insize[0] = cb_data->in_len;
  req->inoffset[0] = cb_data->in_len;
  req->inunit[0] = UNIT_8_BIT;
	req->outcnt=1;
	req->outptr[0]=cb_data->output;
	req->outsize[0]=cb_data->out_len;
	req->outoffset[0]=cb_data->out_len;
	req->outunit[0]=UNIT_8_BIT;
  req->req_type = request_type;

  req->req_queue = 0;
  req->res_order = CAVIUM_RESPONSE_UNORDERED;	// added by liqin  
  req->dma_mode = global_dma_mode;

  *message_length = record_length - hash_size
    - *(Uint8 *) (message + record_length - 1) - 1;
   req->callback=call_back;
   req->cb_arg=cb_data;
   cb_data->req=req;
  
	if (do_request(&cavium_dev[0], req, request_id)) 
	{
		cavium_free(cb_data->input);
		cavium_free(cb_data->output);
		cavium_free(cb_data);
		cavium_free(req);
		printk("Kernel_Csp1DecryptRecordAes failed\n");
		return -1;
	}	

   return 0;
}

Uint32
Kernel_Csp1RsaSsl20ServerFullRc4 (n1_request_type request_type,
			   Uint64 context_handle,
			   Uint64 * key_handle,
			   Rc4Type rc4_type,
			   Uint8 * client_master_secret,
			   Uint16 clear_master_secret_length,
			   Uint16 encrypted_master_secret_length,
			   Uint16 modlength,
			   Uint8 * challenge,
			   Uint16 challenge_length,
			   Uint8 * connection_id,
			   Uint8 * session_id,
			   Uint8 * client_finished,
			   Uint8 * server_finished,
			   Uint8 * server_verify,
			   Uint8 * master_secret,
			   Uint16 * master_secret_length, Uint32 * request_id,
			   void (*call_back)(int, void *),
			   void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1RsaSsl20ServerClientAuthRc4 (n1_request_type request_type,
				 Uint64 context_handle,
				 Uint64 * key_handle,
				 Rc4Type rc4_type,
				 Uint8 * client_master_secret,
				 Uint16 clear_master_secret_length,
				 Uint16 encrypted_master_secret_length,
				 Uint16 modlength,
				 Uint8 * challenge,
				 Uint16 challenge_length,
				 Uint8 * connection_id,
				 Uint8 * session_id,
				 Uint8 * client_finished,
				 Uint8 auth_type,
				 Uint8 * cert_challenge,
				 Uint8 * cert_request,
				 Uint8 * server_verify,
				 Uint8 * master_secret,
				 Uint16 * master_secret_length,
				 Uint32 * request_id,
				 void (*call_back)(int, void *),
				 void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1Ssl20ResumeRc4 (n1_request_type request_type,
		    Uint64 context_handle,
		    Uint64 * key_handle,
		    Rc4Type rc4_type,
		    Uint8 * master_secret,
		    Uint16 master_secret_length,
		    Uint8 * challenge,
		    Uint16 challenge_length,
		    Uint8 * connection_id,
		    Uint8 * session_id,
		    Uint8 * client_finished,
		    Uint8 * server_finished,
		    Uint8 * server_verify, Uint32 * request_id,
			void (*call_back)(int, void *),
			void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1Ssl20ResumeClientAuthRc4 (n1_request_type request_type,
			      Uint64 context_handle,
			      Uint64 * key_handle,
			      Rc4Type rc4_type,
			      Uint8 * master_secret,
			      Uint16 master_secret_length,
			      Uint8 * challenge,
			      Uint16 challenge_length,
			      Uint8 * connection_id,
			      Uint8 * session_id,
			      Uint8 * client_finished,
			      Uint8 auth_type,
			      Uint8 * cert_challenge,
			      Uint8 * cert_request,
			      Uint8 * server_verify, Uint32 * request_id,
				  void (*call_back)(int, void *),
				  void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1RsaSsl20ServerFull3Des (n1_request_type request_type,
			    Uint64 context_handle,
			    Uint64 * key_handle,
			    DesType des_type,
			    Uint8 * client_master_secret,
			    Uint16 clear_master_secret_length,
			    Uint16 encrypted_master_secret_length,
			    Uint16 modlength,
			    Uint8 * challenge,
			    Uint16 challenge_length,
			    Uint8 * connection_id,
			    Uint8 * session_id,
			    Uint8 * iv,
			    Uint8 * client_finished,
			    Uint8 * server_finished,
			    Uint8 * server_verify,
			    Uint8 * master_secret,
			    Uint16 * master_secret_length,
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1RsaSsl20ServerClientAuth3Des (n1_request_type request_type,
				  Uint64 context_handle,
				  Uint64 * key_handle,
				  DesType des_type,
				  Uint8 * client_master_secret,
				  Uint16 clear_master_secret_length,
				  Uint16 encrypted_master_secret_length,
				  Uint16 modlength,
				  Uint8 * challenge,
				  Uint16 challenge_length,
				  Uint8 * connection_id,
				  Uint8 * session_id,
				  Uint8 * iv,
				  Uint8 * client_finished,
				  Uint8 auth_type,
				  Uint8 * cert_challenge,
				  Uint8 * cert_request,
				  Uint8 * server_verify,
				  Uint8 * master_secret,
				  Uint16 * master_secret_length,
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1Ssl20Resume3Des (n1_request_type request_type,
		     Uint64 context_handle,
		     Uint64 * key_handle,
		     DesType des_type,
		     Uint8 * master_secret,
		     Uint16 master_secret_length,
		     Uint8 * challenge,
		     Uint16 challenge_length,
		     Uint8 * connection_id,
		     Uint8 * session_id,
		     Uint8 * iv,
		     Uint8 * client_finished,
		     Uint8 * server_finished,
		     Uint8 * server_verify, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1Ssl20ResumeClientAuth3Des (n1_request_type request_type,
			       Uint64 context_handle,
			       Uint64 * key_handle,
			       DesType des_type,
			       Uint8 * master_secret,
			       Uint16 master_secret_length,
			       Uint8 * challenge,
			       Uint16 challenge_length,
			       Uint8 * connection_id,
			       Uint8 * session_id,
			       Uint8 * iv,
			       Uint8 * client_finished,
			       Uint8 auth_type,
			       Uint8 * cert_challenge,
			       Uint8 * cert_request,
			       Uint8 * server_verify, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1Ssl20DecryptRecordRc4 (n1_request_type request_type,
			   Uint64 context_handle,
			   Uint16 record_length,
			   Uint8 * record,
			   Uint8 * message, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}


Uint32
Kernel_Csp1Ssl20EncryptRecordRc4 (n1_request_type request_type,
			   Uint64 context_handle,
			   Uint16 message_length,
			   Uint8 * message,
			   Uint8 * record, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;

}

Uint32
Kernel_Csp1Ssl20DecryptRecord3Des (n1_request_type request_type,
			    Uint64 context_handle,
			    DesType des_type,
			    Uint16 record_length,
			    Uint8 * record,
			    Uint8 * message, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

Uint32
Kernel_Csp1Ssl20EncryptRecord3Des (n1_request_type request_type,
			    Uint64 context_handle,
			    DesType des_type,
			    Uint16 message_length,
			    Uint8 * message,
			    Uint16 * record_length,
			    Uint8 * record, 
			    Uint32 * request_id,
				void (*call_back)(int, void *),
				void *connect)
{
  return ERR_OPERATION_NOT_SUPPORTED;
}

#ifndef MC2
/* some utility functions */

static void
pkp_leftfill (unsigned char input[], int length, unsigned char output[],
	      int finallength)
{
  int i;
  int j;
  memset (output, 0, finallength);
  j = finallength - 1;
  for (i = length - 1; i >= 0; i--)
    {
      output[j] = input[i];
      j = j - 1;
    }
}



static void
swap_word_openssl (unsigned char *d, unsigned char *s, int len)
{
  int i, j;
  Uint64 *ps;
  Uint64 *pd;

  j = 0;

  ps = (Uint64 *) s;
  pd = (Uint64 *) d;

  for (i = (len >> 3) - 1; i >= 0; i--)
    {
      pd[j] = ps[i];
      j++;
    }

}
#endif /* MC2 */

EXPORT_SYMBOL(Kernel_Csp1SetEncryptedMasterSecretKey);
EXPORT_SYMBOL(Kernel_Csp1RsaServerFullRc4);
EXPORT_SYMBOL(Kernel_Csp1RsaServerVerifyRc4);
EXPORT_SYMBOL(Kernel_Csp1RsaServerFull3Des);
EXPORT_SYMBOL(Kernel_Csp1RsaServerVerify3Des);
EXPORT_SYMBOL(Kernel_Csp1RsaServerFullAes);
EXPORT_SYMBOL(Kernel_Csp1RsaServerVerifyAes);
EXPORT_SYMBOL(Kernel_Csp1OtherFullRc4);
EXPORT_SYMBOL(Kernel_Csp1OtherVerifyRc4);
EXPORT_SYMBOL(Kernel_Csp1OtherFull3Des);
EXPORT_SYMBOL(Kernel_Csp1OtherVerify3Des);
EXPORT_SYMBOL(Kernel_Csp1OtherFullAes);
EXPORT_SYMBOL(Kernel_Csp1OtherVerifyAes);
EXPORT_SYMBOL(Kernel_Csp1FinishedRc4Finish);
EXPORT_SYMBOL(Kernel_Csp1Finished3DesFinish);
EXPORT_SYMBOL(Kernel_Csp1FinishedAesFinish);
EXPORT_SYMBOL(Kernel_Csp1ResumeRc4);
EXPORT_SYMBOL(Kernel_Csp1Resume3Des);
EXPORT_SYMBOL(Kernel_Csp1ResumeAes);
EXPORT_SYMBOL(Kernel_Csp1EncryptRecordRc4);
EXPORT_SYMBOL(Kernel_Csp1DecryptRecordRc4);
EXPORT_SYMBOL(Kernel_Csp1EncryptRecord3Des);
EXPORT_SYMBOL(Kernel_Csp1DecryptRecord3Des);
EXPORT_SYMBOL(Kernel_Csp1EncryptRecordAes);
EXPORT_SYMBOL(Kernel_Csp1DecryptRecordAes);
EXPORT_SYMBOL(Kernel_Csp1RsaSsl20ServerFullRc4);
EXPORT_SYMBOL(Kernel_Csp1RsaSsl20ServerClientAuthRc4);
EXPORT_SYMBOL(Kernel_Csp1Ssl20ResumeRc4);
EXPORT_SYMBOL(Kernel_Csp1Ssl20ResumeClientAuthRc4);
EXPORT_SYMBOL(Kernel_Csp1RsaSsl20ServerFull3Des);
EXPORT_SYMBOL(Kernel_Csp1RsaSsl20ServerClientAuth3Des);
EXPORT_SYMBOL(Kernel_Csp1Ssl20Resume3Des);
EXPORT_SYMBOL(Kernel_Csp1Ssl20ResumeClientAuth3Des);
EXPORT_SYMBOL(Kernel_Csp1Ssl20DecryptRecordRc4);
EXPORT_SYMBOL(Kernel_Csp1Ssl20EncryptRecordRc4);
EXPORT_SYMBOL(Kernel_Csp1Ssl20DecryptRecord3Des);
EXPORT_SYMBOL(Kernel_Csp1Ssl20EncryptRecord3Des);

