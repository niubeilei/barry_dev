////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: jmk_lib_internal.c
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
#include "aosUtil/Mutex.h"
//#include "aosUtil/Semaphore.h"
#include "aosUtil/Tracer.h"
#include "JNS9054Card/9054.h"
#include "JNS9054Card/lib_9054jmk.h"
#include "JNS9054Card/jmk_lib_internal.h"


#include "JNS9054Card/j9054.h"

#include "Porting/TimeOfDay.h"



unsigned char jmk_pwd_hash[17]=
{0xDD, 0x4B, 0x21, 0xE9, 0xEF, 0x71, 0xE1, 0x29, 0x11, 0x83, 0xA4, 0x6B, 0x91, 0x3A, 0xE6, 0xF2, 0x00};
int InitOk=0;
unsigned char ca0_cert[CERTLEN_MAX],ca1_cert[CERTLEN_MAX];
unsigned char ca2_cert[CERTLEN_MAX],server_cert[CERTLEN_MAX],ocsp_cert[CERTLEN_MAX];
unsigned char server_pub[RSA_BLOCK];
int ca0_cert_len=0,ca1_cert_len=0,ca2_cert_len=0;
int server_cert_len=0,ocsp_cert_len=0;
int jmk_link_IC;
shared_data 	*shared_memory;
char JmkSerials[MAXNUM_OF_JMK][JMK_SERIALNUM_LEN+1];
int jmk_busy_arr[MAXNUM_OF_JMK];		// work busy table for choose JMK
int JmkFreeNum;
unsigned char *gjmk_buffer[MAXNUM_OF_JMK];
AOS_DECLARE_MUTEX(jmk_mut[MAXNUM_OF_JMK]);
AOS_DECLARE_SEMAPHORE(card_sem);

//
//
//
int my_jmk_wait(struct structIoctl * p,int cmd)
{
	int iret;
	int WorkWay;


//	if (p->jnum == 0x0)//p->jnum)//
		WorkWay=QUERY_CMD_WAY;
//	else
//		WorkWay=INT_CMD_WAY;

	iret=-1;
	switch (cmd)
	{
	case P9054_TARGET_WRITE_CMD:
		if (WorkWay == QUERY_CMD_WAY)
			iret=jmk_kernel_ioctl(P9054_TARGET_QUERY_CMD,p);
		else
			iret=jmk_kernel_ioctl(P9054_TARGET_INT_CMD,p);
		break;
	case P9054_DMA_WRITE_CMD:
		break;
	case AOS_P9054_TARGET_WRITE_CMD_FOR_RSA_PUB:
		if (WorkWay == QUERY_CMD_WAY)
			iret=jmk_kernel_ioctl(AOS_P9054_TARGET_QUERY_CMD_FOR_RSA_PUB,p);
		else
			iret=jmk_kernel_ioctl(P9054_TARGET_INT_CMD,p);
		break;	case AOS_P9054_TARGET_WRITE_CMD_FOR_RSA:
		if (WorkWay == QUERY_CMD_WAY)
			iret=jmk_kernel_ioctl(AOS_P9054_TARGET_QUERY_CMD_FOR_RSA,p);
		else
			iret=jmk_kernel_ioctl(P9054_TARGET_INT_CMD,p);
		break;
	case P9054_DMA_READ_INT_CMD:
		if (WorkWay == QUERY_CMD_WAY)
			iret=jmk_kernel_ioctl(P9054_DMA_READ_QUERY_CMD,p);
		else
			iret=jmk_kernel_ioctl(P9054_DMA_READ_INT_CMD,p);
		break;
		default:break;
	}
	return iret;
}

//
//	Invert the buffer
//
void InvertBuff(unsigned char*buf,int len)
{
	int i,n;
	unsigned char tmp;

	n=len/4;

	for (i=0;i<n;i++)
	{
		tmp=buf[4*i];
		buf[4*i]=buf[4*i+3];
		buf[4*i+3]=tmp;
		tmp=buf[4*i+1];
		buf[4*i+1]=buf[4*i+2];
		buf[4*i+2]=tmp;
	}

	return;
}

//
//	Fill the block of encrypt
//
int Fill_Enc_Block(int bt,int datalen,unsigned char*in,unsigned char*eb)
{
	int iret;

	if (datalen > RSA_MAX_PLAIN && datalen != RSA_BLOCK)
		return -1;

	OmnKernelMemset(eb,0,RSA_BLOCK);
	switch (bt)
	{
	case 0:
		if (in[0] == 0)
			iret=-2;
		else
			iret=1;
		break;
	case 1:
		eb[1]=1;
		OmnKernelMemset(eb+2,0xff,RSA_BLOCK-datalen-3);
		iret=2;
		break;
	case 2:
		eb[1]=2;
		Get_Pseudo_Random(RSA_BLOCK-datalen-3,eb+2);
		iret=3;
		break;
		default:iret=-4;break;
	}
	if (iret > 0)
		OmnKernelMemcpy(eb+RSA_BLOCK-datalen,in,datalen);

#ifdef GAJMK
	InvertBuff(eb,RSA_BLOCK);
#endif

	return iret;
}

//
//	Generate random of all 0xff
//
void Get_Pseudo_Random(int len,unsigned char*out)
{
	OmnKernelMemset(out, 0xff, len);
	return;
}

//
//
//
int Get_Plain(int datalen,unsigned char*in,unsigned char*out)
{
	int iret,i;

#ifdef GAJMK
	InvertBuff(in,RSA_BLOCK);
#endif

	if (datalen > RSA_MAX_PLAIN && datalen != RSA_BLOCK)
	{
		aos_debug("Get Plain input wrong:datalen=%d",datalen);
		return -1;
	}
	else
	if (datalen <= RSA_MAX_PLAIN)
	{
		OmnKernelMemcpy(out,in,datalen);
		return datalen;
	}
	if (in[0] != 0)
		return -2;
	switch (in[1])
	{
	case 0:
		for (i=0;i<RSA_BLOCK;i++)
		if (in[i] != 0)
			break;
		if (i >= RSA_BLOCK)
			iret = -3;
		else
			iret=RSA_BLOCK-i;
		break;
	case 1:
		for (i=0;i<RSA_BLOCK-3;i++)
		if (in[2+i] != 0xff)
			break;
		if (in[2+i] != 0)
			iret=-4;
		else if (i >= RSA_BLOCK-3)
			iret=-5;
		else
			iret=RSA_BLOCK-i-3;
		break;
	case 2:
		for (i=0;i<RSA_BLOCK;i++)
		if (in[i+2] == 0)
			break;
		if (i >= RSA_BLOCK-2)
			iret=-6;
		else
			iret=RSA_BLOCK-i-2-1;
		break;
		default:iret=-7;
	}
	if (iret > 0)
		OmnKernelMemcpy(out,in+RSA_BLOCK-iret,iret);

#ifdef GAJMK
	InvertBuff(in,RSA_BLOCK);
#endif

	return iret;
}

//
//
//
int Read_CA_From_IC(int caIndex,unsigned char* ca)
{
	int iret=0;
	long cmd=CMD_READ_CA_FROM_IC;
	//unsigned char buf[MID_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;

	buf=(unsigned char *)kmalloc(MID_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	sio.buff=buf;

	OmnKernelMemcpy(buf,&cmd,4);
	OmnKernelMemcpy(buf+4,&caIndex,4);
	buf[4+4]=jmk_link_IC;

	sio.jnum=jmk_link_IC;
	sio.BuffLen=8+1;

	iret=GetCertainJmk(jmk_link_IC);

	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	if (iret >= 0 && buf[0] == 0x0a)
	{
		iret=buf[9]*256 + buf[8];
		if (iret > 0 && iret <= 1534)
			OmnKernelMemcpy(ca,buf+8+2+2,iret);
	}
	else
		iret=-103;
	kfree(buf);
	return iret;
}

//
//
//
int GetCertainJmk(int CardNo)
{
	return CardNo;
}


//
//	Get CA to der code
//
//---------------------------  CA  --------------------------------
int Get_DerPubKey_From_CA(unsigned char*ca,unsigned char*der_pubkey)
{
	int offset,len;

	offset=0;
	if (ca[offset] != 0x30)
		return -1;
	offset+=1;
	switch (ca[offset])
	{	// Total Length
	case 0x81:offset+=2;break;
	case 0x82:offset+=3;break;
		default:offset+=1;break;
	}
	if (ca[offset] != 0x30)
		return -2;
	offset+=1;
	switch (ca[offset])
	{    // tbsCert Length
	case 0x81:offset+=2;break;
	case 0x82:offset+=3;break;
		default:offset+=1;break;
	}
	if (ca[offset] != 0xa0)		//Have Version Information
		return -3;
	offset+=1;
	switch (ca[offset])
	{    // Version Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x02)		//Serial Number
		return -4;
	offset+=1;
	switch (ca[offset])
	{    // Serial Number Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Signature Algorithm
		return -5;
	offset+=1;
	switch (ca[offset])
	{    // Signature Algorithm Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Issuer
		return -6;
	offset+=1;
	switch (ca[offset])
	{    // Issuer Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Validity
		return -7;
	offset+=1;
	switch (ca[offset])
	{    // Validity Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Subject
		return -8;
	offset+=1;
	switch (ca[offset])
	{    // Subject Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //DER Public Key
		return -9;
	offset+=1;
	switch (ca[offset])
	{    // DER Public Key Length
	case 0x81:
		len=ca[offset+1]+3;
		//        offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2]+4;
		//        offset+=3+len;
		break;
	default:
		len=ca[offset]+2;
		//        offset+=1+len;
		break;
	}
	OmnKernelMemcpy(der_pubkey,ca+offset-1,len);
	return len;
}

//
//
//
int Get_PubKey_From_DerPubKey(unsigned char*der_pubkey,unsigned char*pubkey)
{
	int offset,len;

	/*    for (len=0;len<162;len++)
	printf("%02x ",der_pubkey[len]);     */

	if (der_pubkey[0] != 0x30)             //Total
		return -1;
	offset=1;
	switch (der_pubkey[offset])
	{    // Total Length
	case 0x81:
		offset+=2;
		break;
	case 0x82:
		offset+=3;
		break;
	default:
		offset+=1;
		break;
	}
	if (der_pubkey[offset] != 0x30)             //Algorithm id
		return -2;
	offset+=1;
	switch (der_pubkey[offset])
	{    // Algorithm Length
	case 0x81:
		len=der_pubkey[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=der_pubkey[offset+1]*256+der_pubkey[offset+2];
		offset+=3+len;
		break;
	default:
		len=der_pubkey[offset];
		offset+=1+len;
		break;
	}
	if (der_pubkey[offset] != 0x03)             //Total_Pub
		return -3;
	offset+=1;
	switch (der_pubkey[offset])
	{    // Total_Pub Length
	case 0x81:
		offset+=2;
		break;
	case 0x82:
		offset+=3;
		break;
	default:
		offset+=1;
		break;
	}
	if (der_pubkey[offset] != 0x00)             //Gap Code
		return -4;
	offset+=1;
	if (der_pubkey[offset] != 0x30)             //PubKey
		return -5;
	offset+=1;
	switch (der_pubkey[offset])
	{    // PubKey Length
	case 0x81:
		len=der_pubkey[offset+1];
		offset+=2;
		break;
	case 0x82:
		len=der_pubkey[offset+1]*256+der_pubkey[offset+2];
		offset+=3;
		break;
	default:
		len=der_pubkey[offset];
		offset+=1;
		break;
	}
	if (der_pubkey[offset] != 0x02)             //PubKey
		return -6;
	offset+=1;
	switch (der_pubkey[offset])
	{    // PubKey Length
	case 0x81:
		len=der_pubkey[offset+1];
		offset+=2;
		break;
	case 0x82:
		len=der_pubkey[offset+1]*256+der_pubkey[offset+2];
		offset+=3;
		break;
	default:
		len=der_pubkey[offset];
		offset+=1;
		break;
	}
	if (len > 0x80)
		offset+=len-0x80;
	OmnKernelMemcpy(pubkey,der_pubkey+offset,0x80);
	return 0x80;
}

//
//	Seq encrypt
//
int Seq_Enc(int jnum,int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out)
{
	int iret,i;
	unsigned char mk[MKLEN];
	long cmd=CMD_SEQ_ENC;
	int offset,out_offset;
	struct structIoctl sio;
	sio.buff = gjmk_buffer[jnum-1];

	//aos_debug("Enter Seq_Enc cmd datalen %d, jiffies %u, usec %u\n", datalen, jiffies, AosGetUsec());
	
	sio.jnum = jnum;
	if (bflag == 0 || bflag == 3)
	{
		OmnKernelMemcpy(sio.buff,&cmd,4);
		offset = 4;

		OmnKernelMemcpy(sio.buff+offset,bk,BKLEN);
		InvertBuff(sio.buff+offset,BKLEN);
		offset += BKLEN;

		cmd = sio.buff[offset] = jnum;
		sio.BuffLen = offset+1;

		iret = my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);
		if (iret >= 0 && sio.buff[0] == 0x0a)
		{
			OmnKernelMemcpy(mk,sio.buff+4,MKLEN);
		}
		else
			return -1;

		offset = FIRST_CODE_OFFSET;
		out_offset = 8;

	}
	else
	{
		offset = 0;
		out_offset = 0;
	}

	sio.BuffLen = datalen*4+offset;
	iret = my_jmk_wait(&sio,P9054_DMA_READ_INT_CMD);
	if (iret == sio.BuffLen)
	{
		for (i = datalen-1; i >= 0; i--)
		{
			out[i+out_offset] = in[i]^sio.buff[offset+4*i];
		}
		if (bflag == 0 || bflag == 3)
		{
			OmnKernelMemcpy(out,mk,MKLEN);
			InvertBuff(out,MKLEN);
		}
	}

	//aos_debug("Enter Seq_Enc exit jiffies %u, usec %u\n", jiffies, AosGetUsec());
	return iret;
}

//
//	Seq decrypt
//
int Seq_Dec(int jnum,int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out)
{
	int iret = 0;
	int out_offset,i;
	long cmd=CMD_SEQ_DEC;
	int offset;
	struct structIoctl sio;

	sio.buff = gjmk_buffer[jnum-1];

	//aos_debug("Enter Seq_Dec cmd datalen %d, jiffies %u, usec %u\n", datalen, jiffies, AosGetUsec());

	sio.jnum = jnum;
//aos_prod_log(eAosMD_SSL,"sio.jnum %d", sio.jnum);
	if (bflag == 0 || bflag == 3)
	{
		OmnKernelMemcpy(sio.buff,&cmd,4);
		offset = 4;

		OmnKernelMemcpy(sio.buff+offset,bk,BKLEN);
		InvertBuff(sio.buff+offset,BKLEN);
		offset += BKLEN;

		OmnKernelMemcpy(sio.buff+offset,in,MKLEN);
		InvertBuff(sio.buff+offset,MKLEN);
		offset += MKLEN;

		cmd = sio.buff[offset] = jnum;
		sio.BuffLen = offset+1;
		iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);
//aos_prod_log(eAosMD_SSL,"Seq_Dec %d", iret);
		if (iret >= 0 && sio.buff[0] == 0x0a)
		{
			;
		}
		else
			return -1;

		offset = FIRST_CODE_OFFSET;
		out_offset = 8;
	}
	else
	{
		offset = 0;
		out_offset = 0;
	}

	sio.BuffLen = (datalen-out_offset)*4 + offset;

//aos_prod_log(eAosMD_SSL,"Seq_Dec %d", iret);
	iret = my_jmk_wait(&sio,P9054_DMA_READ_INT_CMD);
	if (iret == sio.BuffLen)
	{
//aos_prod_log(eAosMD_SSL,"Seq_Dec %d", iret);
		for (i = 0; i < (datalen-out_offset); i++)
		{
			out[i] = in[i+out_offset]^sio.buff[offset+4*i];
		}
	}
//aos_prod_log(eAosMD_SSL,"Seq_Dec %d", iret);
	//aos_debug("Enter Seq_Dec exit jiffies %u, usec %u\n", jiffies, AosGetUsec());
	return iret;
}

//
//	Verify password of JMK
//
int PCIPassVerify(char* pwd)
{
	int iret=-1;
	int offset=0;
	long cmd;
	int i;
	//unsigned char buf[SMALL_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;

	aos_debug("function PCIPassVerify begin\n");
	buf=(unsigned char*)kmalloc(SMALL_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	sio.buff=buf;

	aos_debug("%d piece of JMK is ready!\n",piece_of_jmk);
	iret=-1;

	for (i=1;i<=piece_of_jmk;i++)
	{
		cmd=CMD_PCIPASS_VERIFY;
		OmnKernelMemset(buf,0,SMALL_BUFFER_SIZE);
		OmnKernelMemcpy(buf,&cmd,4);
		offset=4;
		OmnKernelMemcpy(buf+offset,pwd,strlen(pwd));

		offset+=PASSLEN;
		OmnKernelMemcpy(buf+offset,jmk_pwd_hash,16);

	#ifndef GAJMK
		InvertBuff(buf+offset,PASSLEN+HASHLEN);
	#endif
		offset+=PASSLEN+HASHLEN;

		buf[offset]=i;

		sio.jnum=i;
		sio.BuffLen=offset+1;
		//PrintBuff(sio.buff,sio.BuffLen);

		iret=GetCertainJmk(i);

		iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

		if (buf[0] == 0x0a)
		{
			iret=1;
			aos_debug("JMK %d PCI verify password success!\n",i);
		}
		else
		{
			aos_debug("JMK %d PCI verify password failed!\n",i);
			SetJmkBusy(i);
			iret=-1;
		}
	}
	aos_debug("function PCIPassVerify end\n");
	kfree(buf);
	return iret;
}

//
//	Set jmk busy
//
void SetJmkBusy(int jnum)
{
	shared_memory->busy_arr[jnum-1] = 1;
	//??????????????????
	shared_memory->busy_time[jnum-1]=jiffies;
//	time( &(shared_memory->busy_time[jnum-1]) );
	return;
}

//
//	Get jmk serial
//
int GetJmkSerial(int jmkIndex,char* SerialStr)
{
	int iret=0;
	long cmd=CMD_READ_JMKSERIAL;
	int offset;
	//unsigned char buf[SMALL_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;

	aos_debug("function GetJmkSerial begin\n");
	buf=(unsigned char *)kmalloc(SMALL_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	sio.buff=buf;

	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;

	//GetCertainJmk(jmkIndex);
	set_jmk_busy(jmkIndex);
	cmd=jmkIndex;

	OmnKernelMemcpy(buf+offset,&cmd,1);

	sio.jnum=cmd;
	sio.BuffLen=offset+1;

	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);
	aos_debug("SerialStr address is %x", (unsigned int)SerialStr);
	if (iret >= 0 && buf[0] == 0x0a)
	{
		iret-=4;
		OmnKernelMemcpy(SerialStr,buf+4,JMK_SERIALNUM_LEN);
		SerialStr[JMK_SERIALNUM_LEN]=0;
	}
	else
		iret=-8;
	free_certain_jmk(jmkIndex);
	kfree(buf);
	aos_debug("function GetJmkSerial end\n");
	return iret;
}

//
//	Password verify
//
int PasswordVerify(char* pwd)
{
	int iret=-1;
	int offset=0;
	long cmd;
	int i;
	//unsigned char buf[SMALL_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;

	buf=(unsigned char *)kmalloc(SMALL_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}
	sio.buff=buf;

	aos_debug("%d piece of JMK is ready!\n",piece_of_jmk);
	iret=-1;

	for (i=piece_of_jmk;i>=1;i--)
	{

		cmd=CMD_PASSWORD_VERIFY;
		OmnKernelMemset(buf,0,SMALL_BUFFER_SIZE);
		OmnKernelMemcpy(buf,&cmd,4);
		offset=4;
		OmnKernelMemcpy(buf+4,pwd,strlen(pwd));
		InvertBuff(buf+4,PASSLEN);
		offset+=PASSLEN;

		buf[offset]=i;

		sio.jnum=i;
		sio.BuffLen=offset+1;

		iret=GetCertainJmk(i);

		iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);
		if (buf[0] == 0x0a)
		{
			iret=1;
			jmk_link_IC=i;
			break;
		}
		else
		{
			OmnKernelMemcpy(&iret,buf+4,4);
			iret=0-iret;
		}
	}
	if (iret < 0)
	{
		if (iret == -5)
			aos_debug("Please ensure your IC link ready!\n");
		else
			aos_debug("IC password error %d times now!\n",-iret);
	}
	aos_debug("function PasswordVerify end");
	kfree(buf);
	return iret;
}

//
//	Get key from der certificate
//
int Get_DerKey_From_CERT(unsigned char*ca,unsigned char*der_pubkey,int kType)
{
	int offset,len;
	unsigned char * t_pk;

	if (kType != 1 && kType != 2 && kType != 3)
		return -100;
	offset=0;
	if (ca[offset] != 0x30)
		return -1;
	offset+=1;
	switch (ca[offset])
	{	// Total Length
	case 0x81:offset+=2;break;
	case 0x82:offset+=3;break;
		default:offset+=1;break;
	}
	if (ca[offset] != 0x30)
		return -2;
	offset+=1;
	switch (ca[offset])
	{    // tbsCert Length
	case 0x81:offset+=2;break;
	case 0x82:offset+=3;break;
		default:offset+=1;break;
	}
	if (ca[offset] != 0xa0)		//Have Version Information
		return -3;
	offset+=1;
	switch (ca[offset])
	{    // Version Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x02)		//Serial Number
		return -4;
	offset+=1;
	switch (ca[offset])
	{    // Serial Number Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Signature Algorithm
		return -5;
	offset+=1;
	switch (ca[offset])
	{    // Signature Algorithm Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Issuer
		return -6;
	offset+=1;
	switch (ca[offset])
	{    // Issuer Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Validity
		return -7;
	offset+=1;
	switch (ca[offset])
	{    // Validity Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //Subject
		return -8;
	offset+=1;
	switch (ca[offset])
	{    // Subject Length
	case 0x81:
		len=ca[offset+1];
		offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2];
		offset+=3+len;
		break;
	default:
		len=ca[offset];
		offset+=1+len;
		break;
	}
	if (ca[offset] != 0x30)             //DER Public Key
		return -9;
	offset+=1;
	switch (ca[offset])
	{    // DER Public Key Length
	case 0x81:
		len=ca[offset+1]+3;
		//        offset+=2+len;
		break;
	case 0x82:
		len=ca[offset+1]*256+ca[offset+2]+4;
		//        offset+=3+len;
		break;
	default:
		len=ca[offset]+2;
		//        offset+=1+len;
		break;
	}
	if (kType == 2 || kType == 3)
	{
		offset+=len-1;
		if (ca[offset] != 0x30)             //DER Private Key
			return -10;
		offset+=1;
		switch (ca[offset])
		{    // DER Key struct Length
		case 0x81:
			len=ca[offset+1]+3;
			offset+=2;
			break;
		case 0x82:
			len=ca[offset+1]*256+ca[offset+2]+4;
			offset+=3;
			break;
		default:
			len=ca[offset]+2;
			offset+=1;
			break;
		}
		if ((kType == 2 && ca[offset] != 0x81) ||
			(kType == 3 && ca[offset] != 0xa2))             //DER Private Key or Sequence Key
			return -11;
		offset+=1;
		switch (ca[offset])
		{    // DER Private Key Length OR CommuniticationKey Length
		case 0x81:
			len=ca[offset+1];
			offset+=2;
			break;
		case 0x82:
			len=ca[offset+1]*256+ca[offset+2];
			offset+=3;
			break;
		default:
			len=ca[offset];
			offset+=1;
			break;
		}
		if (kType == 2)
		{   // Copy cipher Private Key
			memcpy(der_pubkey,ca+offset+1,len-1);
			return (len-1);
		}
		else    //if (kType == 3)
		{
			if (ca[offset] != 0x03)
				return -12;
			offset+=3;
			t_pk=der_pubkey;
			OmnKernelMemcpy(t_pk,ca+offset,14);    // Copy Device ID
			offset+=14;
			t_pk+=14;
			if (ca[offset] != 0x03)
				return -13;
			offset+=3;
			OmnKernelMemcpy(t_pk,ca+offset,14);    // Copy Code of Algo
			offset+=14;
			t_pk+=14;
			if (ca[offset] != 0x02)
				return -14;
			offset+=1;
			switch (ca[offset])
			{    // DER Key version Length
			case 0x81:
				len=ca[offset+1];
				offset+=2;
				break;
			case 0x82:
				len=ca[offset+1]*256+ca[offset+2];
				offset+=3;
				break;
			default:
				len=ca[offset];
				offset+=1;
				break;
			}
			OmnKernelMemcpy(t_pk,ca+offset,len);     // Copy the Verion of SeqKey
			offset+=len;
			t_pk+=len;
			if (ca[offset] != 0x03)
				return -15;
			offset+=1;
			switch (ca[offset])
			{    // DER Key version Length
			case 0x81:
				len=ca[offset+1];
				offset+=2;
				break;
			case 0x82:
				len=ca[offset+1]*256+ca[offset+2];
				offset+=3;
				break;
			default:
				len=ca[offset];
				offset+=1;
				break;
			}
			OmnKernelMemcpy(t_pk,ca+offset+1,len-1);     // Copy the cipher SeqKey
			return ((int)(t_pk - der_pubkey + len - 1));
		}
	}
	OmnKernelMemcpy(der_pubkey,ca+offset-1,len);
	return len;
}

//
//	Write RSA key to jmk
//
int Write_RSAKey_To_JMK(int cmd,int RSA_JMK_Index,unsigned char*RSAKey,int jnum)
{
	int iret,offset;
	//unsigned char buf[MID_BUFFER_SIZE];
	unsigned char *buf;
	struct structIoctl sio;

	buf=(unsigned char *)kmalloc(MID_BUFFER_SIZE, GFP_KERNEL);
	if(buf==NULL)
	{
		aos_debug("alloc memory error!\n");
		return -1;
	}

	sio.buff=buf;

	if (jnum == 0)
		jnum=jmk_link_IC;
	else
	if (jnum > piece_of_jmk || jnum < 0)
		return -1;

	//memset(buf,0,BUFF_MAX);
	OmnKernelMemcpy(buf,&cmd,4);
	offset=4;

	if (cmd == CMD_WRITE_PUB_TO_JMK)
	{
		OmnKernelMemcpy(buf+offset,&RSA_JMK_Index,4);
		offset+=4;
		OmnKernelMemcpy(buf+offset,RSAKey,RSA_BLOCK);

		offset+=RSA_BLOCK;
	}
	else
	if (cmd == CMD_WRITE_RSAPRI_TO_JMK)
	{
		iret=RSAKey[1]*256+RSAKey[0]+4;
		OmnKernelMemcpy(buf+offset,RSAKey,iret);//PRIKEY_BLOCK);
		iret=0x180;//((iret+3)/4)*4;

		iret=iret*4;

		offset+=iret+4;//PRIKEY_BLOCK;
	}
	else
	if (cmd == CMD_WRITE_SEQKEY_TO_JMK)
	{
		OmnKernelMemcpy(buf+offset,RSAKey,SEQKEY_BLOCK+4);
		//#ifndef GAJMK
		InvertBuff(buf+offset+4,SEQKEY_BLOCK);
		//#endif
		offset+=SEQKEY_BLOCK+4;
	}
	else
		return -1;

	buf[offset]=jnum;

	sio.jnum=jnum;
	sio.BuffLen=offset+1;
	//PrintBuff(sio.buff,sio.BuffLen);

	iret=GetCertainJmk(jnum);

	iret=my_jmk_wait(&sio,P9054_TARGET_WRITE_CMD);

	//FreeJmk(jnum);

	if (iret >= 0 && buf[0] == 0x0a)
		iret=1;
	else
	{
		aos_debug("Error occured when write sth to JMK\n");
		//sleep(2);
		iret=-4;
	}
	kfree(buf);
	return iret;
}

