////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: jmk_lib_internal.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef JMK_LIB_INTERNAL_HEAD_H
#define JMK_LIB_INTERNAL_HEAD_H
#include "aosUtil/Mutex.h"
#define DEBUG


#define OmnKernelMemcpy(x, y, z)  memcpy(x, y, z)
#define OmnKernelMemset(x, y, z) memset(x, y, z)


#define QUERY_CMD_WAY				1
#define INT_CMD_WAY				2


extern int piece_of_jmk;
extern shared_data *shared_memory;
extern AOS_DECLARE_MUTEX(jmk_mut[MAXNUM_OF_JMK]);//add by lijing 20060224
extern AOS_DECLARE_SEMAPHORE(card_sem);

static inline int GetFreeJmk(void)
{
	int i;
	aos_lock(card_sem);
	for(i=0;i<piece_of_jmk;i++)
	{
		if(shared_memory->busy_arr[i]==0)
		{
aos_prod_log(eAosMD_SSL,"pid %d, card no %d", current->pid, i);
			aos_lock(jmk_mut[i]);//add by lijing 20060224
			return i;
		}
	}
	return -1;
}

#define set_jmk_busy(jnum)	{shared_memory->busy_arr[(jnum)] = 1;}
//#define set_jmk_free(jnum)	{shared_memory->busy_arr[(jnum)] = 0; up(&card_sem);}
#define set_jmk_free(jnum)	{shared_memory->busy_arr[(jnum)] = 0; aos_unlock(jmk_mut[jnum]); aos_unlock(card_sem);}//add by lijing 20060224
#define free_certain_jmk(jnum)	{shared_memory->busy_arr[(jnum)] = 0;}

//int GetFreeJmk(void);
int my_jmk_wait(struct structIoctl * p,int cmd);
void InvertBuff(unsigned char*buf,int len);
int Fill_Enc_Block(int bt,int datalen,unsigned char*in,unsigned char*eb);
void Get_Pseudo_Random(int len,unsigned char*out);
int Get_Plain(int datalen,unsigned char*in,unsigned char*out);
int Read_CA_From_IC(int caIndex,unsigned char* ca);
int GetCertainJmk(int CardNo);
int Get_DerPubKey_From_CA(unsigned char*ca,unsigned char*der_pubkey);
int Get_PubKey_From_DerPubKey(unsigned char*der_pubkey,unsigned char*pubkey);
int Seq_Enc(int jnum,int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out);
int Seq_Dec(int jnum,int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out);
int PCIPassVerify(char* pwd);
void SetJmkBusy(int jnum);
int GetJmkSerial(int jmkIndex,char* SerialStr);
int PasswordVerify(char* pwd);
int Get_DerKey_From_CERT(unsigned char*ca,unsigned char*der_pubkey,int kType);
int Write_RSAKey_To_JMK(int cmd,int RSA_JMK_Index,unsigned char*RSAKey,int jnum);
//void set_jmk_busy(int jnum);
//void set_jmk_free(int jnum);

#endif

