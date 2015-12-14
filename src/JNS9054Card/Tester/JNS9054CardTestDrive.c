////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: JNS9054CardTestDrive.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/param.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/init.h>
#include <linux/random.h>
#include <linux/time.h>
#include <asm/uaccess.h>




#include "aos/aosKernelApi.h"
#include "KernelSimu/netdevice.h"
#include "KernelUtil/KernelStr.h"

#include "card_test.h"


struct semaphone thread_sem;

static int time;
static int success;
static int speed;
static int end_time;
static int thread_count_t;
static int user_thread_count;
static int random_test_size;
static int start_time;
static int success;
static struct _result_ result;
//jmk API
extern int InitMultiJMK(void);
extern int FreeMultiJMK(void);
extern int jmk_kernel(unsigned int cmd, void* arg);
extern int Seq_Decrypt(int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out);
extern int Seq_Encrypt(int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out);
extern int RSA_IndexPub_Enc(int pubindex,int datalen,unsigned char*in,unsigned char*out);
extern int RSA_IndexPri_Dec(int priindex,int datalen,unsigned char*in,unsigned char*out);
extern int RSA_IndexPri_Enc(int priindex,int datalen,unsigned char*in,unsigned char*out);
extern int RSA_IndexPub_Dec(int pubindex,int datalen,unsigned char*in,unsigned char*out);
extern int Gen_Random(int len,unsigned char*out);
//kernel thread function



//card_test <option> <thread_count> <buffer_size>
//<option> is a string, it is one of the following strings
//	test_seq_speed
//	test_rsa_pri_encrypt
//	test_rsa_pri_decrypt
//	test_rsa_pub_operation
//	test_all
//	test_mix


int aos_card_test(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    char *tester_select = parms->mStrings[0];
    char *thread_count = parms->mStrings[1];
    char *buffer_size = parms->mstrings[2];
    //struct net_device *dev = dev_get_by_name(name);
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;
    PEXTENSION pdx=(PEXTENSION)kmalloc(sizeof(EXTENSION), GFP_KERNEL);
	
    if(pdx==NULL){
    	printk("alloc memory fail! in %s\n", __FUNCTION__);
    	return 0;//eAosRc_MemErr;
    }
	
	sscanf(thread_count, "%d", &pdx->thread_count);
	sscanf(buffer_size, "%d", &pdx->buffer_size);
	if(memcmp(tester_select, "test_seq_speed", strlen(tester_select))==0){
		pdx->select_tester=test_seq_speed;
	}
	if(memcmp(tester_select, "test_rsa_pri_encrypt", strlen(tester_select))==0){
		pdx->select_tester=test_rsa_pri_encrypt;
	}
	if(memcmp(tester_select, "test_rsa_pri_decrypt", strlen(tester_select))==0){
		pdx->select_tester=test_rsa_pri_decrypt;
	}
	if(memcmp(tester_select, "test_rsa_pub_operation", strlen(tester_select))==0){
		pdx->select_tester=test_rsa_pub_operation;
	}
	if(memcmp(tester_select, "test_all", strlen(tester_select))==0){
		pdx->select_tester=test_all;
	}	
	if(memcmp(tester_select, "test_seq_random_length", strlen(tester_select))==0){
		pdx->select_tester=test_seq_random_length;
	}	
	if(memcmp(tester_select, "test_mix", strlen(tester_select))==0){
		pdx->select_tester=test_mix;
	}	

	InitMultiJMK();
	card_tester(pdx);
	
	aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, (struct _result_*)&pdx->reslut, sizeof(struct _result_));
	
	*length = rsltIndex;
	kfree(pdx);
	FreeMultiJMK();
    return 0;

}

int card_tester(PEXTENSION pdx){
	//init semaphone
	sema_init(&thread_sem, 1);	
	multi_thread_test(pdx);	

	return 0;
}

int multi_thread_test(PEXTENSION pdx){
	int i;

	thread_count_t=0;
	time=0;
	start_time=0;
	end_time=0;
	success=0;
	random_test_size=0;
	user_thread_count=pdx->thread_count;
	success=0;
	donw(&thread_sem);
	for(i=0;i<pdx->thread_count;i++){
		if(kernel_thread(thread_tester, (void*)pdx, CLONE_VM | CLONE_FS | CLONE_FILES)<0){
			printk("kernel thread create fail!\n");
		}
	}
	if(kernel_thread(thread_result, (void*)pdx, CLONE_VM | CLONE_FS | CLONE_FILES)<0){
		printk("result thread create fail!\n");
	}
	start_time=jiffies;
	down(&thread_sem);
	up(&thread_sem);
	return 0;
}

static int thread_tester(void *argv){

	tester((PEXTENSION)argv);
	return 0;
}

static int thread_result(void *argv){
	PEXTENSION pdx=(PEXTENSION)argv;

	while(1){
		if(thread_count_t<pdx->thread_count){
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(5*1000);
		}else{
			break;
		}
	}
	switch(pdx->select_tester){
		case test_seq_speed:
			pdx->reslut.data_length=pdx->buffer_size*pdx->thread_count*SEQ_TEST_TIMES;
			pdx->reslut.run_time=end_time-start_time;
			pdx->reslut.run_times=SEQ_TEST_TIMES*pdx->thread_count;
			pdx->reslut.speed=pdx->reslut.data_length/(pdx->reslut.run_time/HZ)*8;
			pdx->reslut.thread_count=pdx->thread_count;
			pdx->reslut.success=success;
			break;
		case test_seq_random_length:
			pdx->reslut.data_length=random_test_size;
			pdx->reslut.run_time=end_time-start_time;
			pdx->reslut.speed=random_test_size/(pdx->reslut.run_time/HZ)*8;
			pdx->reslut.run_times=pdx->thread_count*SEQ_TEST_TIMES;
			pdx->reslut.thread_count=pdx->thread_count;
			pdx->reslut.success=success;
			break;
		case test_rsa_pri_encrypt:
		case test_rsa_pri_decrypt:
		case test_rsa_pub_operation:
			pdx->reslut.data_length=128*pdx->thread_count*RSA_TEST_TIMES;
			pdx->reslut.run_time=end_time-start_time;
			pdx->reslut.run_times=pdx->thread_count*RSA_TEST_TIMES;
			pdx->reslut.speed=pdx->reslut.run_times/(pdx->reslut.run_time/HZ);
			pdx->reslut.thread_count=pdx->thread_count;
			pdx->reslut.success=success;
			break;
		case test_mix:
			break;
		case test_all:
			break;
		default:
	}

	result.data_length=pdx->reslut.data_length;
	result.run_time=pdx->reslut.run_time;
	result.run_times=pdx->reslut.run_times;
	result.speed=pdx->reslut.speed;
	result.thread_count=pdx->reslut.thread_count;
	result.success=pdx->reslut.success;
	
	up(&thread_sem);
	return 0;
}

int tester(PEXTENSION pdx){
	unsigned char *in;
	unsigned char *out;
	int length;
	char buf[4];
	int i;

	
	switch(pdx->select_tester){
		case test_seq_speed:
			//only seq encrypt test	
			in=(unsigned char *)kmalloc(pdx->buffer_size, GFP_KERNEL);
			out=(unsigned char *)kmalloc(pdx->buffer_size+8, GFP_KERNEL);	
			for(i=0;i<pdx->buffer_size;i++){
				in[i]&=0xff;
			}
			for(i=0;i<SEQ_TEST_TIMES;i++){
				success+=only_seq_enc_test(in, out, pdx->buffer_size);
			}
			kfree(in);
			kfree(out);
			//tester end
			break;
		case test_seq_random_length:
			//random_test_size=0;
			for(i=0;i<SEQ_TEST_TIMES;i++){
				get_random_bytes((void*)buf, sizeof(int));
				sscanf(buf, "%d", &length);
				length=length%(MAX_RANDOM_TEST_SIZE);
				if(length<100){
					i--;
					continue;
				}
				success+=seq_random_test(length);
				random_test_size+=length;
			}
			break;
		case test_rsa_pri_encrypt:
			in=(unsigned char *)kmalloc(124, GFP_KERNEL);
			for(i=0;i<124;i++){
				in[i]=i;
			}
			for(i=0;i<RSA_TEST_TIMES;i++){
				success+=only_rsa_pri_enc_test(in);
			}
			kfree(in);
			break;
		case test_rsa_pri_decrypt:
			in=(unsigned char *)kmalloc(124, GFP_KERNEL);
			out=(unsigned char *)kmalloc(128, GFP_KERNEL);
			for(i=0;i<124;i++){
				in[i]=i;
			}
			length=RSA_IndexPub_Enc(9, 124, in, out);
			if(length!=128){
				printk("encrypt error!\n");
				kfree(in);
				kfree(out);
				return 0;
			}
			for(i=0;i<RSA_TEST_TIMES;i++){
				success+=only_rsa_pri_dec_test(out);
			}
			kfree(in);
			kfree(out);		
			break;
		case test_rsa_pub_operation:
			in=(unsigned char *)kmalloc(124, GFP_KERNEL);
			for(i=0;i<124;i++){
				in[i]=i;
			}
			for(i=0;i<RSA_TEST_TIMES;i++){
				success+=only_rsa_pub_enc_test(in);
			}
			kfree(in);
			break;
		case test_mix:
			mix_tester();
			break;
		case test_all:
			break;
		default:
			return 0;
	}
	end_time=jiffies;
	thread_count_t++;
	return 0;
}

int only_seq_enc_test(unsigned char *in, unsigned char *out, int length){
	int length;

	length=Seq_Encrypt(0, (unsigned char*)"12345678", length, in, out);
	if(length==length+8){
		return 1;
	}else{
		return 0;
	}
	return 1;
}	

int seq_random_test(int length){
	int i;
	unsigned char *in, *out;
	int len;

	in=kmalloc(length, GFP_KERNEL);
	if(in==NULL){
		printk("malloc memory error!, in %s\n", __FUNCTION__);
		return 0;
	}
	out=kmalloc(length+8, GFP_KERNEL);
	if(out==NULL){
		kfree(in);
		printk("malloc memory error! in %s\n", __FUNCTION__);
		return 0;
	}
	for(i=0;i<length;i++){
		in[i]=i&0xff;
	}
	len=Seq_Encrypt(0, (unsigned char*)"12345678", length, in, out);
	if(len!=length+8){
		printk("encrypt error!\n");
		kfree(in);
		kfree(out);
		return 0;
	}
	kfree(in);
	kfree(out);
	return 1;
}

int only_rsa_pri_enc_test(unsigned char *in){
	int length=0;
	unsigned char *out;


	out=(unsigned char *)kmalloc(128, GFP_KERNEL);

	length=RSA_IndexPri_Enc(9, 124, in, out);
	if(length==128){
		kfree(out);
		return 1;
	}else{
		kfree(out);
		return 0;
	}
}

int only_rsa_pri_dec_test(unsigned char *in){
	int length=0;
	unsigned char *out;
	
	out=(unsigned char *)kmalloc(124, GFP_KERNEL);
	length=RSA_IndexPri_Dec(9, 128, in, out);
	if(length==124){
		kfree(out);
		return 1;
	}else{
		kfree(out);
		return 0;
	}
}

int only_rsa_pub_enc_test(unsigned char *in){
	int length=0;
	unsigned char *out;
	
	out=(unsigned char *)kmalloc(128, GFP_KERNEL);
	length=RSA_IndexPub_Enc(9, 124, in, out);
	if(length==128){
		kfree(out);
		return 1;
	}else{
		kfree(out);
		return 0;
	}
}

int mix_tester(void){
#if 0
	int i;

	for(i=0;i<9999;i++){
		if(kernel_thread(thread_mix_tester, NULL, CLONE_VM | CLONE_FS | CLONE_FILES)<0){
			printk("test thread create error!\n");
		}
	}
	run_time=jiffies;
#endif
	return 0;
}

static int thread_mix_tester(void*  argv){
	mix_test();
	return 0;
}

int mix_test(void){

	
	return 0;
}

int aos_get_card_test_result(char *data,
                       unsigned int *length,
                       struct aosKernelApiParms *parms,
                       char *errmsg,
                       const int errlen)
{
    //struct net_device *dev = dev_get_by_name(name);
    char *rsltBuff = aosKernelApi_getBuff(data);
    unsigned int rsltIndex = 0;
    unsigned int optlen = *length;

	char *rslt;

	if(thread_count_t<user_thread_count){
		rslt=(char*)kmalloc(50, GFP_KERNEL);
		if(rslt==NULL){
			printk("alloc memory error! in %s \n", __FUNCTION__);
			return 0;
		}
		memset(rslt, 0, 50);
		sprintf(rslt, "The card_test is running, thread_count remain %d\n", user_thread_count-thread_count_t);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, (char *)rslt, 50);
	}else{
		rslt=(char*)kmalloc(500, GFP_KERNEL);
		if(rslt==NULL){
			printk("alloc memory error! in %s\n", __FUNCTION__);
			return 0;
		}
		sprintf(rslt, "Data length is %d\n			\
						Run time is %d\n			\
						Run %d times, %d times success\n	\
						Thread count is %d\n		\
						The speed is %d\n", result.data_length, result.run_time, result.run_times, result.success, result.thread_count, result.speed);
		aosCheckAndCopy(rsltBuff, &rsltIndex, optlen, (char*)rslt, 500);
	}

	
	
	*length = rsltIndex;
	free(rslt);
    return 0;

}

