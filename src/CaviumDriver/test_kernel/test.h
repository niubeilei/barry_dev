////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: test.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef TEST_HEAD_H
#define TEST_HEAD_H

#define TEST_COUNT 1000000
#define DATA_LENGTH 16*1024

#define test_malloc(x) kmalloc(x, GFP_ATOMIC)
#define free(x) kfree(x)
#define pos printk("%s %d\n", __FILE__, __LINE__)

#define P(x...) printk(x)

struct data_ctrl{
	long request_id;
	unsigned char *result;
};

enum{
	test_rc4=100,
	test_3des
};

#define MAX_INCNT   8
#define MAX_OUTCNT   8
#define MAX_BUFCNT   MAX_INCNT

static inline int print_buf(char *title, unsigned char *buffer, int len)
{
	int i;
	
	P("%s/%d\n", title, len);
	for(i=1;i<=len;i++){
		P("%02x ", buffer[i-1]);
		if(i%16==0){
			P("\n");
		}
	}
	P("\n");
	return 0;	
}

typedef enum
{ CAVIUM_DIRECT = 0, CAVIUM_SCATTER_GATHER = 1 }
Csp1DmaMode, n1_dma_mode, DmaMode;

typedef enum
{ CONTEXT_SSL = 0, CONTEXT_IPSEC = 2 }
ContextType;

typedef enum
{ CAVIUM_BLOCKING = 0, CAVIUM_NON_BLOCKING = 1, CAVIUM_SIGNAL = 2 }
Csp1RequestType, n1_request_type;

typedef enum
{ CAVIUM_NO_UPDATE = 0, CAVIUM_UPDATE = 1 }
ContextUpdate;

typedef enum
{ AES_128 = 0, AES_192 = 1, AES_256 = 2 }
AesType;

typedef enum
{ BT1 = 0, BT2 = 1 }
RsaBlockType;

typedef unsigned char		Uint8;
typedef unsigned short		Uint16;
typedef unsigned long		Uint32;
typedef unsigned long long	Uint64;


struct call_back_data{
	void *context;
	unsigned char *input;
	unsigned char *output;
	void* req;
	Uint32 outcnt;
	Uint8* outptr[MAX_OUTCNT];
	Uint32 outsize[MAX_OUTCNT];
	int in_len;
	int out_len;
};

struct rc4_struct{
	Uint8 input[DATA_LENGTH];
	Uint8 output[DATA_LENGTH];
	Uint8* outout;
	Uint8 key[16];
};

struct rsa_struct{
	Uint8 input[124];
	Uint8 output[128];
	Uint8 out_out[1284];
	Uint16 outlength;
};

#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 1024
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)























int new_none_blocking(void);
void test_initrc4_call_back(int flag, void* arg);
void test_encrypt_call_back(int flag, void* arg);
void test_decrypt_call_back(int flag, void* arg);
void test_aes_decrypt_callback(int flag, void *arg);
void test_aes_call_back(int flag, void *arg);
int test_aes(void);
void test_rsa_call_back(int flag, void *arg);
void test_rsa_de_call_back(int flag, void *arg);
int test_rsa(void);



#endif
