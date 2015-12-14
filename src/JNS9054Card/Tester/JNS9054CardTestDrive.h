////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTestDrive.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifndef JNS_9054CARD_TEST_HEAD_H
#define JNS_9054CARD_TEST_HEAD_H


#define SEQ_TEST_TIMES 10000
#define RSA_TEST_TIMES 10000
#define MAX_RANDOM_TEST_SIZE 16*1024

struct _result_{
	int run_time;
	int run_times;
	int data_length;
	int thread_count;
	int speed;
	int success;
};

typedef struct _EXTENSION_{
	int thread_count;
	int select_tester;
	int buffer_size;
	struct _result_ reslut;
}EXTENSION, *PEXTENSION;

enum{
	test_seq_speed=1,
	test_seq_random_length,
	test_rsa_pri_encrypt,
	test_rsa_pri_decrypt,
	test_rsa_pub_operation,
	test_mix,
	test_all
};

int aos_card_test(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen);
int card_tester(PEXTENSION pdx);
int multi_thread_test(PEXTENSION pdx);
static int thread_tester(void *argv);
static int thread_result(void *argv);
int tester(PEXTENSION pdx);
int only_seq_enc_test(unsigned char *in, unsigned char *out, int length);
int seq_random_test(int length);
int only_rsa_pri_enc_test(unsigned char *in);
int only_rsa_pri_dec_test(unsigned char *in);
int only_rsa_pub_enc_test(unsigned char *in);
int mix_tester(void);
static int thread_mix_tester(void*  argv);
int mix_test(void);

#endif 
