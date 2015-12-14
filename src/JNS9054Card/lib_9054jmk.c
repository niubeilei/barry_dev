////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: lib_9054jmk.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/param.h>
#include <linux/slab.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>

#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"

#include "JNS9054Card/9054.h"


#include "JNS9054Card/lib_9054jmk.h"
#include "JNS9054Card/jmk_lib_internal.h"
#include "JNS9054Card/j9054.h"
#include "Porting/TimeOfDay.h"

//
// Yang Tao, 02/21/2006, #83
//
#include "PKCS/CertMgr.h"


extern int piece_of_jmk;				// the number of JMK in system

extern int InitOk;
extern unsigned char ca0_cert[CERTLEN_MAX],ca1_cert[CERTLEN_MAX];
extern unsigned char ca2_cert[CERTLEN_MAX],server_cert[CERTLEN_MAX],ocsp_cert[CERTLEN_MAX];
extern unsigned char server_pub[RSA_BLOCK];
extern int ca0_cert_len,ca1_cert_len,ca2_cert_len;
extern int server_cert_len,ocsp_cert_len;
extern int jmk_link_IC;
extern shared_data 	*shared_memory;
extern char JmkSerials[MAXNUM_OF_JMK][JMK_SERIALNUM_LEN+1];
extern int jmk_busy_arr[MAXNUM_OF_JMK];		// work busy table for choose JMK
extern int JmkFreeNum;
extern struct semaphore card_sem;
extern unsigned char *gjmk_buffer[MAXNUM_OF_JMK];
//extern volatile unsigned long jiffies;
//
//	Make a pair of RSA Key
//
int Gen_RSA_Out_Pub(int index, unsigned char *pub_key)
{
	int iret=0;
	long cmd=CMD_GEN_RSA_OUT_PUB;
	int offset;
	//unsigned char buf[MID_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;
	int card_no;

	buf=(unsigned char *)kmalloc(MID_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL){
		aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "alloc memory error!\n");
		return -1;
	}
			
	sio.buff=buf;

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;
	OmnKernelMemcpy(buf+offset,&index,4);
	offset+=4;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);
	OmnKernelMemcpy(buf+offset,&card_no,1);
	
	//sio.jnum=cmd;
	sio.jnum = card_no+1;
	sio.BuffLen=offset+1;
	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	//FreeJmk(cmd);

	if (iret >= 0 && buf[0] == 0x0a)
	{
		iret-=4;
		OmnKernelMemcpy(pub_key, buf+4,iret);
		//#ifndef GAJMK
		InvertBuff(pub_key, iret);
		//#endif
	}
	else
		iret=-8;
	set_jmk_free(card_no);
	return iret;
}

//
//	RSA Public Key Encrypt
//
int RSA_IndexPub_Enc(int pubindex,int datalen,unsigned char*in,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_RSA_INDEXPUB_ENC;
	int offset;
	unsigned char buf[SMALL_BUFFER_SIZE];
	struct structIoctl sio;
	int card_no;

	sio.buff=buf;

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;
	OmnKernelMemcpy(buf+offset,&pubindex,4);
	offset+=4;

	iret=Fill_Enc_Block(2,datalen,in,buf+offset);

#ifdef NOT_CIPHER
	OmnKernelMemcpy(out,buf+offset,128);
	return 128;
#endif

#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
#endif
	if (iret < 0)
		return -10;
	offset+=RSA_BLOCK;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);
	OmnKernelMemcpy(buf+offset,&card_no,1);

	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);
	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	if (iret >= 0 && buf[0] == 0x0a)
	{
		OmnKernelMemcpy(out,buf+4,iret-4);//RSA_BLOCK);
		//#ifndef GAJMK
		InvertBuff(out,iret-4);//RSA_BLOCK);
		//#endif
		iret=iret-4;//RSA_BLOCK;
	}
	else
		iret=-10;
	set_jmk_free(card_no);
	return iret;
}

//
//	RSA Private Key Decrypt
//

int RSA_IndexPri_Dec(int priindex,int datalen,unsigned char*in,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_RSA_INDEXPRI_DEC;
	int offset;
	unsigned char buf[SMALL_BUFFER_SIZE];
	struct structIoctl sio;
	int card_no;

	sio.buff=buf;

#ifdef NOT_CIPHER
	return Get_Plain(RSA_BLOCK,in,out);
#endif

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;
	OmnKernelMemcpy(buf+offset,&priindex,4);
	offset+=4;

	OmnKernelMemcpy(buf+offset,in,RSA_BLOCK);
	//#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
	//#endif
	//    if (iret < 0)
	//        return -1401;
	offset+=RSA_BLOCK;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);
	OmnKernelMemcpy(buf+offset,&card_no,1);
	

	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);
	iret=my_jmk_wait(&sio,AOS_P9054_TARGET_WRITE_CMD_FOR_RSA);

	if (iret >= 0 && buf[0] == 0x0a)
	{
	#ifndef GAJMK
		InvertBuff(buf+4,RSA_BLOCK);
	#endif
		iret=Get_Plain(iret-4,buf+4,out);
		if (iret < 0)
			iret=-1402;
	}
	else
		iret=-1403;
	set_jmk_free(card_no);
	return iret;
}

//
//	RSA IndexPri Encrypt
//
int RSA_IndexPri_Enc(int priindex,int datalen,unsigned char*in,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_RSA_INDEXPRI_ENC;
	int offset;
	unsigned char buf[SMALL_BUFFER_SIZE];
	struct structIoctl sio;
	int card_no;

	sio.buff=buf;

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;
	OmnKernelMemcpy(buf+offset,&priindex,4);
	offset+=4;

	iret=Fill_Enc_Block(1,datalen,in,buf+offset);

#ifdef NOT_CIPHER
	OmnKernelMemcpy(out,buf+offset,128);
	return 128;
#endif

#ifdef RSATEST
	//    memcpy(out,buf+offset,RSA_BLOCK);
	//    return RSA_BLOCK;
#endif

#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
#endif
	if (iret < 0)
		return -13;
	offset+=RSA_BLOCK;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);
	OmnKernelMemcpy(buf+offset,&card_no,1);
	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);
	iret=my_jmk_wait(&sio,AOS_P9054_TARGET_WRITE_CMD_FOR_RSA);

	if (iret >= 0 && buf[0] == 0x0a)
	{
		OmnKernelMemcpy(out,buf+4,RSA_BLOCK);
		//#ifndef GAJMK
		InvertBuff(out,RSA_BLOCK);
		//#endif
		iret=RSA_BLOCK;
	}
	else
		iret=-14;
	set_jmk_free(card_no);
	return iret;
}

//
//	RSA IndexPub Decrypt
//
int RSA_IndexPub_Dec(int pubindex,int datalen,unsigned char*in,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_RSA_INDEXPUB_DEC;
	int offset;
	unsigned char buf[SMALL_BUFFER_SIZE];
	struct structIoctl sio;
	int card_no;

	sio.buff=buf;

#ifdef NOT_CIPHER
	return Get_Plain(RSA_BLOCK,in,out);
#endif

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;
	OmnKernelMemcpy(buf+offset,&pubindex,4);
	offset+=4;

	OmnKernelMemcpy(buf+offset,in,RSA_BLOCK);
	//#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
	//#endif

#ifdef RSATEST
	//	return Get_Plain(RSA_BLOCK,buf+offset,out);
#endif

	offset+=RSA_BLOCK;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);
	OmnKernelMemcpy(buf+offset,&card_no,1);

	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);
	iret=my_jmk_wait(&sio,AOS_P9054_TARGET_WRITE_CMD_FOR_RSA_PUB);

	if (iret >= 0 && buf[0] == 0x0a)
	{
	#ifndef GAJMK
		InvertBuff(buf+4,RSA_BLOCK);
	#endif
		iret=Get_Plain(iret-4,buf+4,out);
	}
	else
		iret=-12;
	set_jmk_free(card_no);
	return iret;
}

//
//	Read CA's CA
//
int Read_CA_Cert(struct k_buff *ca_ca, struct k_buff *ca_pub_key)
{
	unsigned char ca[2048];
	unsigned char ret[BUFFER_SIZE];
	long len;
	
	len=Read_CA_From_IC(0,ca);
	OmnKernelMemcpy(ca_ca->data, ca, len);
	ca_ca->data_len=len;
	len=Get_DerPubKey_From_CA(ca,ret);
	len=Get_PubKey_From_DerPubKey(ret,ca);
	OmnKernelMemcpy(ca_pub_key->data, ca, len);
	ca_pub_key->data_len=len;

	return 0;
}

//
//	Read Server's CA
//
int Read_Server_CA(struct k_buff *server_ca, struct k_buff *server_pub_key)
{
	unsigned char ca[2048];
	unsigned char ret[BUFFER_SIZE];
	long len;
	
	len=Read_CA_From_IC(2,ca);
	OmnKernelMemcpy(server_ca->data, ca, len);
	server_ca->data_len=len;
	len=Get_DerPubKey_From_CA(ca,ret);
	len=Get_PubKey_From_DerPubKey(ret,ca);
	OmnKernelMemcpy(server_pub_key->data, ca, len);
	server_pub_key->data_len=len;

	return 0;	
}

//
//	SEQ Encrypt
//
int Seq_Encrypt(int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out)
{
	int in_len,in_offset,out_offset,rest,iret;
	int jnum;

	//aos_trace("Seq Encrypt entry");	
	jnum = GetFreeJmk();
	aos_assert1(jnum >= 0);
	set_jmk_busy(jnum);

	if (bflag == 0)
	{
		in_len = (datalen > SEQ_ENC_BLOCK_MAX)? SEQ_ENC_BLOCK_MAX : datalen;
		if ((iret = Seq_Enc(jnum+1,0,bk,in_len,in,out)) <= 0)
		{ 
			aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Seq Encryption failed %d, card no. %d", iret, jnum);
			set_jmk_free(jnum);
			return iret;
		}
		in_offset = in_len;
		out_offset = in_len+MKLEN;
		rest = datalen-in_len;
	}
	else
	{
		in_offset = 0;
		out_offset = 0;
		rest = datalen;
	}
	for (;rest > 0;)
	{
		in_len = (rest > SEQ_ENC_BLOCK_MAX)? SEQ_ENC_BLOCK_MAX: rest;
		if ((iret = Seq_Enc(jnum+1,1,bk,in_len,in+in_offset,out+out_offset)) <= 0)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Seq Encryption failed %d, card no. %d", iret, jnum);
			set_jmk_free(jnum);
			return iret;
		}
		in_offset += in_len;
		out_offset += in_len;
		rest = rest-in_len;
	}

	set_jmk_free(jnum);

	return out_offset;
}

//
//	SEQ Decrypt
//

int Seq_Decrypt(int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out)
{
	int in_len,in_offset,out_offset,rest,iret,jnum;

	//aos_trace("Seq Decrypt entry usec %u", AosGetUsec());	

	jnum = GetFreeJmk();
	aos_assert1(jnum >= 0);	
	set_jmk_busy(jnum);

	if (bflag == 0)
	{
		in_len = (datalen>SEQ_DEC_BLOCK_MAX)? SEQ_DEC_BLOCK_MAX : datalen;

		//aos_trace("Seq Decrypt call Seq Dec usec %u", AosGetUsec());
		//aos_prod_log(eAosMD_SSL,"Seq_Dec jnum %d", jnum);	
		if ((iret = Seq_Dec(jnum+1,0,bk,in_len,in,out)) <= 0)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr,"Seq Decryption failed %d, card no. %d", iret, jnum);
			//aos_debug("JMK %d Seq_Dec failed!\n",jnum);
			set_jmk_free(jnum);
			return iret;
		}
		in_offset = in_len;
		out_offset = in_len - MKLEN;
		rest = datalen - in_len;
	}
	else
	{
		in_offset=0;
		out_offset=0;
		rest=datalen;
	}

	for (;rest > 0;)
	{
		in_len = (rest > SEQ_ENC_BLOCK_MAX)? SEQ_ENC_BLOCK_MAX: rest;
		if ((iret = Seq_Dec(jnum+1,1,bk,in_len,in+in_offset,out+out_offset)) <= 0)
		{
			aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "Seq Decryption failed %d, card no. %d", iret, jnum);
			//aos_debug("JMK %d Seq_Dec failed!\n",jnum);
			set_jmk_free(jnum);
			return iret;
		}
		in_offset += in_len;
		out_offset += in_len;
		rest = rest-in_len;
	}

	set_jmk_free(jnum);
	
	return out_offset;
}

//
//	Init card
//
//------------------------------------------------------------
// Init threads to drive Multi JMK
//------------------------------------------------------------
int InitMultiJMK(struct struct_ioctl *io_buf)
{
	int i,len;
	//unsigned char mypub[RSA_BLOCK],tmpstr[CERTLEN_MAX];
	//unsigned char ca[4000];
	//unsigned char buf[SEQ_DEC_BLOCK_MAX*4+FIRST_CODE_OFFSET];
	struct structIoctl sio;
	//sio.buff=buf;

	DECLARE_MUTEX(init_sem);

	unsigned char *mypub;
	unsigned char *tmpstr;
	unsigned char *buf;
	unsigned char *ca;

	init_MUTEX(&init_sem);
	down(&init_sem);
	mypub=(unsigned char *)kmalloc(RSA_BLOCK, GFP_KERNEL);
	if(mypub==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	tmpstr=(unsigned char *)kmalloc(CERTLEN_MAX, GFP_KERNEL);
	if(tmpstr==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	buf=(unsigned char *)kmalloc(SEQ_DEC_BLOCK_MAX*4+FIRST_CODE_OFFSET, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	ca=(unsigned char *)kmalloc(4000, GFP_KERNEL);
	if(ca==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	sio.buff=buf;
	aos_debug("INFO: OpenJmk begin!\n");
	JmkFreeNum=jmk_kernel_ioctl(P9054_GET_DEVICE_NUM,NULL);//??????????????
	aos_debug("Find %d block of JMK\n",JmkFreeNum);
	aos_debug("JMK number is %d\n", piece_of_jmk);
	if (piece_of_jmk > MAXNUM_OF_JMK || piece_of_jmk < 1)
	{
		kfree(buf);
		kfree(mypub);
		kfree(ca);
		kfree(tmpstr);
		return -6;
	}
	
	shared_memory=(shared_data*)kmalloc(sizeof(shared_data), GFP_KERNEL);
	if(shared_memory==NULL)
	{
		kfree(buf);
		kfree(mypub);
		kfree(ca);
		kfree(tmpstr);
		aos_debug("the shared_memory kmalloc error!\n");
		return -1;
	}
	aos_debug("INFO: Memory attached at address %X \n", (int)shared_memory);			
	shared_memory->open_num = 0;
	shared_memory->WorkJmkID=1;

	aos_debug("INFO: first open wjjmk!\n");

	shared_memory->device_num =piece_of_jmk;		
	for (i=0;i<piece_of_jmk;i++)
	{
		aos_debug("init kmk busy flag. jmk number is %d\n", i);
		shared_memory->busy_arr[i]=0;
		shared_memory->busy_time[i] = 0;
	}
	shared_memory->open_num++;
	shared_memory->jmk_link_IC=-1;

	sema_init(&card_sem, piece_of_jmk);
	if (PCIPassVerify("00000000") < 0)//12345678")<0)
	{
		kfree(buf);
		kfree(mypub);
		kfree(ca);
		kfree(tmpstr);
		aos_debug("%d time error!",i+1);
		return -7;
	}

	aos_debug("Start read JMK SerialNums\n");
	for (i=0;i<piece_of_jmk;i++)
	{
		GetJmkSerial(i+1,JmkSerials[i]);
		aos_debug("%s\n",JmkSerials[i]);
	}
	aos_debug("Read JMK SerialNums End\n");

//
//	Add by lijing
//	userland call
//
	if(io_buf!=NULL)
	{
		
		if(io_buf->send_direct==SEND_CA_TO_JMK)
		{
			aos_debug("io_buf send_direct is SEND_CA_TO_JMK\n");
			copy_from_user(ca0_cert, io_buf->buf.ca0.buf ,io_buf->buf.ca0.len);
			ca0_cert_len=io_buf->buf.ca0.len;
			copy_from_user(ca1_cert, io_buf->buf.ca1.buf ,io_buf->buf.ca1.len);
			ca1_cert_len=io_buf->buf.ca1.len;
			copy_from_user(ca2_cert, io_buf->buf.ca2.buf ,io_buf->buf.ca2.len);
			ca2_cert_len=io_buf->buf.ca2.len;
			copy_from_user(server_cert,io_buf->buf.server_cert.buf ,io_buf->buf.server_cert.len);
			server_cert_len=io_buf->buf.server_cert.len;
			if(Get_DerKey_From_CERT(server_cert,(unsigned char*)tmpstr,1) < 0)
			{
				return -8;
			}
			if (Get_PubKey_From_DerPubKey((unsigned char*)tmpstr, io_buf->buf.server_cert.buf) < 0)
			{
				return -9;
			}
			copy_from_user(mypub, io_buf->buf.server_cert.buf ,128);
			copy_from_user(server_pub, io_buf->buf.pub2.buf, io_buf->buf.pub2.len);
			copy_from_user(tmpstr, &io_buf->buf.pri2.len ,4);
			copy_from_user(tmpstr+4,io_buf->buf.pri2.buf ,io_buf->buf.pri2.len);
			if(io_buf->buf.ocsp.len>0)
			{
				ocsp_cert_len=io_buf->buf.ocsp.len;
				copy_from_user(ocsp_cert, io_buf->buf.ocsp.buf ,ocsp_cert_len);

#ifdef CONFIG_CERT_VERIFY
				// 
				// Yang Tao, 02/21/2006, #83
				//
				AosCertMgr_setOcspCert(ocsp_cert, ocsp_cert_len);
#endif
			}
			else
			{
				ocsp_cert_len=0;
				return -12;
			}
			for (i=0;i<piece_of_jmk;i++)
			{
				if (Write_RSAKey_To_JMK(CMD_WRITE_RSAPRI_TO_JMK,MY_RSA_INDEX,tmpstr,i+1) < 0)
					return -13;
			}
		}
		if(io_buf->send_direct==RECEIVE_CA_FROM_JMK)
		{
			aos_debug("send_direct is RECEIVE_CA_FROM_JMK\n");
			//验证IC卡口令
			jmk_link_IC=-1;
			strcpy((char*)tmpstr,"4F47FD");
			tmpstr[6]=tmpstr[7]=0xff;
			tmpstr[8]=0;

			// 验证IC卡口令函数没有调用GetFreeJmk(...)和GetCertainJmk(...)，必须独占所有加密卡
			if (PasswordVerify((char*)tmpstr) < 0)	//"00000000") < 0)
			{
				return -77;
			}
			//验证IC卡口令后记住连接IC卡的加密卡编号
			shared_memory->jmk_link_IC=jmk_link_IC;

			//从IC卡读出设备公钥证书，然后从中取出设备公钥并写入各加密卡
			aos_debug("\nStart Read Certs!\n");

			if ((len=Read_CA_From_IC(CERT_CA0,ca)) > 0)
			{
				copy_to_user(io_buf->buf.ca0.buf, ca, len);
				io_buf->buf.ca0.len=len;
				OmnKernelMemcpy(ca0_cert,ca,len);
				ca0_cert_len=len;
			}
			aos_debug("ca0 len : %d\n",len);

			if ((len=Read_CA_From_IC(CERT_CA1,ca)) > 0)
			{
				copy_to_user(io_buf->buf.ca1.buf, ca, len);
				io_buf->buf.ca1.len=len;			
				OmnKernelMemcpy(ca1_cert,ca,len);
				ca1_cert_len=len;
			}
			aos_debug("ca1 len : %d\n",len);

			if ((len=Read_CA_From_IC(CERT_CA2,ca)) > 0)
			{
				copy_to_user(io_buf->buf.ca2.buf, ca, len);
				io_buf->buf.ca2.len=len;			
				OmnKernelMemcpy(ca2_cert,ca,len);
				ca2_cert_len=len;
			}
			aos_debug("ca2 len : %d\n",len);

			if ((len=Read_CA_From_IC(CERT_SERVER_PUB,ca)) < 0 || Get_DerKey_From_CERT(ca,(unsigned char*)tmpstr,1) < 0)
			{
				return -8;
			}

			if (len > 0)
			{
			#ifdef GAJMK
				copy_to_user(io_buf->buf.server_cert.buf, ca, len);
				io_buf->buf.server_cert.len=len;
				OmnKernelMemcpy(server_cert,ca,len);
				server_cert_len=len;
			#endif
			}
			aos_debug("pub2 len : %d\n",len);

			if (Get_PubKey_From_DerPubKey((unsigned char*)tmpstr,ca) < 0)
			{
				return -9;
			}

			OmnKernelMemcpy(mypub,ca,128);
			copy_to_user(io_buf->buf.pub2.buf, ca, 128);
			io_buf->buf.pub2.len=128;
			OmnKernelMemcpy(server_pub,ca,128);

			//从IC卡读出设备私钥证书，然后从中取出设备公钥并写入各加密卡
			if ((len=Read_CA_From_IC(CERT_SERVER_PRI,ca)) < 0)	// || Get_DerKey_From_CERT(ca,(unsigned char*)tmpstr,2) < 0)
			{
				return -11;
			}

			aos_debug("pri2 len : %d\n",len);

			copy_to_user(io_buf->buf.pri2.buf, ca, len);
			io_buf->buf.pri2.len=len;
			OmnKernelMemcpy(tmpstr,&len,4);
			OmnKernelMemcpy(tmpstr+4,ca,len);

			if ((len=Read_CA_From_IC(CERT_OCSP,ca)) < 0)
			{
				aos_debug("Read OCSP cert failed!\n");
				ocsp_cert_len=0;
				return -12;
			}
			if (len > 0)
			{
				copy_to_user(io_buf->buf.ocsp.buf, ca, len);
				io_buf->buf.ocsp.len=len;
				ocsp_cert_len=len;
				OmnKernelMemcpy(ocsp_cert,ca,ocsp_cert_len);
				aos_debug("ocsp len : %d\n",len);
			}


			for (i=0;i<piece_of_jmk;i++)
			{
				if (Write_RSAKey_To_JMK(CMD_WRITE_RSAPRI_TO_JMK,MY_RSA_INDEX,tmpstr,i+1) < 0)
					return -13;
				else
					aos_debug("write rsakey to jmk success\n");
			}
			

		}
	}
	else
	{
		aos_debug("ioctl argument is NULL\n");
		//验证IC卡口令
		jmk_link_IC=-1;
		strcpy((char*)tmpstr,"4F47FD");
		tmpstr[6]=tmpstr[7]=0xff;
		tmpstr[8]=0;

		// 验证IC卡口令函数没有调用GetFreeJmk(...)和GetCertainJmk(...)，必须独占所有加密卡
		if (PasswordVerify((char*)tmpstr) < 0)	//"00000000") < 0)
		{
			return -77;
		}
		//验证IC卡口令后记住连接IC卡的加密卡编号
		shared_memory->jmk_link_IC=jmk_link_IC;

		//从IC卡读出设备公钥证书，然后从中取出设备公钥并写入各加密卡
		aos_debug("\nStart Read Certs!\n");

		if ((len=Read_CA_From_IC(CERT_CA0,ca)) > 0)
		{
			OmnKernelMemcpy(ca0_cert,ca,len);
			ca0_cert_len=len;
		}
		aos_debug("ca0 len : %d\n",len);

		if ((len=Read_CA_From_IC(CERT_CA1,ca)) > 0)
		{
			OmnKernelMemcpy(ca1_cert,ca,len);
			ca1_cert_len=len;
		}
		aos_debug("ca1 len : %d\n",len);

		if ((len=Read_CA_From_IC(CERT_CA2,ca)) > 0)
		{
			OmnKernelMemcpy(ca2_cert,ca,len);
			ca2_cert_len=len;
		}
		aos_debug("ca2 len : %d\n",len);

		if ((len=Read_CA_From_IC(CERT_SERVER_PUB,ca)) < 0 || Get_DerKey_From_CERT(ca,(unsigned char*)tmpstr,1) < 0)
		{
			return -8;
		}

		if (len > 0)
		{
		#ifdef GAJMK
			OmnKernelMemcpy(server_cert,ca,len);
			server_cert_len=len;
		#endif
		}
		aos_debug("pub2 len : %d\n",len);

		if (Get_PubKey_From_DerPubKey((unsigned char*)tmpstr,ca) < 0)
		{
			return -9;
		}

		OmnKernelMemcpy(mypub,ca,128);
		OmnKernelMemcpy(server_pub,ca,128);

		//从IC卡读出设备私钥证书，然后从中取出设备公钥并写入各加密卡
		if ((len=Read_CA_From_IC(CERT_SERVER_PRI,ca)) < 0)	// || Get_DerKey_From_CERT(ca,(unsigned char*)tmpstr,2) < 0)
		{
			return -11;
		}

		aos_debug("pri2 len : %d\n",len);


		OmnKernelMemcpy(tmpstr,&len,4);
		OmnKernelMemcpy(tmpstr+4,ca,len);

		if ((len=Read_CA_From_IC(CERT_OCSP,ca)) < 0)
		{
			aos_debug("Read OCSP cert failed!\n");
			ocsp_cert_len=0;
			return -12;
		}
		if (len > 0)
		{
			ocsp_cert_len=len;
			OmnKernelMemcpy(ocsp_cert,ca,ocsp_cert_len);
			aos_debug("ocsp len : %d\n",len);
		}

		for (i=0;i<piece_of_jmk;i++)
		{
			if (Write_RSAKey_To_JMK(CMD_WRITE_RSAPRI_TO_JMK,MY_RSA_INDEX,tmpstr,i+1) < 0)
				return -13;
		}

	}	
//
//	Added end by lijing
//
	aos_debug("InitMultiJmk ok!\n");

	InitOk=1;

	aos_debug("JMK Num=%d, link IC=%d",shared_memory->device_num,shared_memory->jmk_link_IC);

	aos_debug("piece_of_jmk is %d\n", piece_of_jmk);

	memset(gjmk_buffer, 0, MAXNUM_OF_JMK);
	for(i=0;i<piece_of_jmk;i++)
	{
		free_certain_jmk(i);
		//gjmk_buffer[i] = (unsigned char *)aos_malloc(SEQ_DEC_BLOCK_MAX*4+FIRST_CODE_OFFSET);
		gjmk_buffer[i] = (unsigned char *)kmalloc(SEQ_DEC_BLOCK_MAX*4+FIRST_CODE_OFFSET, GFP_KERNEL);
		//aos_assert1(gjmk_buffer);
	}

	kfree(buf);
	kfree(mypub);
	kfree(ca);
	kfree(tmpstr);
	up(&init_sem);
	return (shared_memory->open_num);		
}

//
//	Free card
//
//------------------------------------------------------------
// Free threads and memory to leave
//------------------------------------------------------------
int FreeMultiJMK(void)
{
	//int ret;
	int i;

	aos_debug("INFO: CloseJmk begin!\n");	
	if (!shared_memory->open_num)
		return 0;
	shared_memory->open_num--;
	if (shared_memory->open_num <= 0)
	{		
		kfree(shared_memory);
	}	
	else
	{
		kfree(shared_memory);
	}
	aos_debug("INFO: CloseJmk end!\n");

	for(i=0;i<piece_of_jmk;i++)
	{
		free_certain_jmk(i);
	}
	return 0;
}

//
//	Generate random bytes
//
int Gen_Random(int len,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_GEN_RANDOM;
	int offset;
	//unsigned char buf[MID_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;
	int card_no;

	buf=(unsigned char *)kmalloc(MID_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	sio.buff=buf;

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;
	iret=len/4;
	OmnKernelMemcpy(buf+offset,&iret,4);
	offset+=4;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);

	OmnKernelMemcpy(buf+offset,&card_no,1);

	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);
	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	//FreeJmk(cmd);

	if (iret >= 0 && buf[0] == 0x0a)
	{
		OmnKernelMemcpy(out,buf+4,len);
		iret=len;
	}
	else
	{
		iret=-7;
	}
	set_jmk_free(card_no);
	return len;
}

//
//	RSA out pub key encrypt
//
int RSA_OutsidePub_Enc(unsigned char*pubkey,int datalen,unsigned char*in,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_RSA_OSPUB_ENC;
	int offset;
	unsigned char buf[SMALL_BUFFER_SIZE];
	struct structIoctl sio;
	int card_no;

	sio.buff=buf;

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;

	OmnKernelMemcpy(buf+offset,pubkey,RSA_BLOCK);
	//#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
	//#endif
	offset+=RSA_BLOCK;

	iret=Fill_Enc_Block(2,datalen,in,buf+offset);

#ifdef NOT_CIPHER
	memcpy(out,buf+offset,128);
aos_trace("1");
	return 128;
#endif

	if (iret < 0)
	{
aos_trace("1");
		return -9;
	}
#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
#endif
	offset+=RSA_BLOCK;
	
	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);

	OmnKernelMemcpy(buf+offset,&card_no,1);

	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;

	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	//FreeJmk(cmd);

	if (iret >= 0 && buf[0] == 0x0a)
	{
		OmnKernelMemcpy(out,buf+4,RSA_BLOCK);
		//#ifndef GAJMK
		InvertBuff(out,RSA_BLOCK);
		//#endif
		iret=RSA_BLOCK;
	}
	else
		iret=-9;
	set_jmk_free(card_no);
	return iret;
}

//
//	RSA out pub key encrypt
//
int RSA_OutsidePub_Dec(unsigned char*pubkey,int datalen,unsigned char*in,unsigned char*out)
{
	int iret=0;
	long cmd=CMD_RSA_OSPUB_DEC;
	int offset;
	unsigned char buf[SMALL_BUFFER_SIZE];
	struct structIoctl sio;
	int card_no;

	sio.buff=buf;

#ifdef NOT_CIPHER
	return Get_Plain(RSA_BLOCK,in,out);
#endif

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;

	OmnKernelMemcpy(buf+offset,pubkey,RSA_BLOCK);
	//#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
	//#endif
	offset+=RSA_BLOCK;

	memcpy(buf+offset,in,RSA_BLOCK);
	//#ifndef GAJMK
	InvertBuff(buf+offset,RSA_BLOCK);
	//#endif

#ifdef RSATEST
	//	return Get_Plain(RSA_BLOCK,buf+offset,out);
#endif

	offset+=RSA_BLOCK;

	card_no=GetFreeJmk();
	if(card_no==-1)
	{
		return -1111;
	}
	set_jmk_busy(card_no);

	OmnKernelMemcpy(buf+offset,&card_no,1);

	sio.jnum=card_no+1;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);
	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	//FreeJmk(cmd);

	if (iret >= 0 && buf[0] == 0x0a)
	{
	#ifndef GAJMK
		InvertBuff(buf+4,RSA_BLOCK);
	#endif
		iret=Get_Plain(iret-4,buf+4,out);
	}
	else
		iret=-11;
	set_jmk_free(card_no);
	return iret;
}

void AosJNSJMK_getServerCertDer(unsigned char **cert_der, int *cert_der_len)
{
aos_trace("get server cert from jns jmk");
	*cert_der = server_cert;
	*cert_der_len = server_cert_len;
}
void AosJNSJMK_getCA0CertDer(unsigned char **cert_der, int *cert_der_len)
{
aos_trace("get CA0 cert from jns jmk");
	*cert_der = ca0_cert;
	*cert_der_len = ca0_cert_len;
}

void AosJNSJMK_getCA1CertDer(unsigned char **cert_der, int *cert_der_len)
{
aos_trace("get CA1 cert from jns jmk");
	*cert_der = ca1_cert;
	*cert_der_len = ca1_cert_len;
}
void AosJNSJMK_getCA2CertDer(unsigned char **cert_der, int *cert_der_len)
{
aos_trace("get CA2 cert from jns jmk");
	*cert_der = ca2_cert;
	*cert_der_len = ca2_cert_len;
}

void AosJNSJMK_getOCSPCertDer(unsigned char **cert_der, int *cert_der_len)
{
aos_trace("get OCSP cert from jns jmk");
	*cert_der = ocsp_cert;
	*cert_der_len = ocsp_cert_len;
}


EXPORT_SYMBOL(Gen_RSA_Out_Pub);
EXPORT_SYMBOL(RSA_IndexPub_Enc);
EXPORT_SYMBOL(RSA_IndexPri_Dec);
EXPORT_SYMBOL(RSA_IndexPri_Enc);
EXPORT_SYMBOL(RSA_IndexPub_Dec);
EXPORT_SYMBOL(Read_CA_Cert);
EXPORT_SYMBOL(Read_Server_CA);
EXPORT_SYMBOL(Seq_Encrypt);
EXPORT_SYMBOL(Seq_Decrypt);
EXPORT_SYMBOL(InitMultiJMK);
EXPORT_SYMBOL(FreeMultiJMK);
EXPORT_SYMBOL(Gen_Random);
EXPORT_SYMBOL(RSA_OutsidePub_Enc);
EXPORT_SYMBOL(RSA_OutsidePub_Dec);


