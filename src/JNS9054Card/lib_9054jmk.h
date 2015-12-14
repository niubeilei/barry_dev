////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: lib_9054jmk.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef LIB_9054_PCI_HEAD_H
#define LIB_9054_PCI_HEAD_H

#define GAJMK
#define CERT_CA0		0
#define CERT_CA1		1
#define CERT_CA2		2
#define CERT_SERVER_PUB		4
#define CERT_SERVER_PRI		5
#define CERT_OCSP		10

#define CMD_READ_JMKSERIAL			0x00000000
#define CMD_PASSWORD_VERIFY			0x01000001
#define CMD_READ_RSAPRI_FROM_JMK	0x02000002
#define CMD_READ_SEQKEY_FROM_JMK	0x03000003
#define CMD_WRITE_RSAPRI_TO_JMK		0x04000004
#define CMD_WRITE_SEQKEY_TO_JMK		0x05000005
#define CMD_READ_CA_FROM_IC			0x06000006
#define CMD_WRITE_PUB_TO_JMK		0x07000007
#define CMD_WRITE_RSAPAIR_TO_JMK	0x08000008
#define CMD_READ_RSAPAIR_FROM_JMK	0x09000009
#define CMD_SEQKEY_STOPUSE			0x0A00000A
#define CMD_GEN_RSA_OUT_PUB			0x0B00000B
#define CMD_RSA_OSPUB_ENC			0x0C00000C
#define CMD_RSA_INDEXPUB_ENC		0x0D00000D
#define CMD_RSA_OSPUB_DEC			0x0E00000E
#define CMD_RSA_INDEXPUB_DEC		0x0F00000F
#define CMD_RSA_INDEXPRI_ENC		0x10000010
#define CMD_RSA_INDEXPRI_DEC		0x11000011
#define CMD_SEQ_ENC					0x12000012
#define CMD_SEQ_DEC					0x13000013
#define CMD_GEN_RANDOM				0x14000014
#define CMD_CHECK_JMKPART			0x15000015
#define CMD_ICPASSWORD_CHANGE		0x16000016
#define CMD_PCIPASS_CHANGE			0x17000017
#define CMD_PCIJMK_EXIT				0x18000018
#define CMD_IS_IC_READY                      0x19000019
#define CMD_DIRECT_IC_OPERATE		0x1A00001A
#define CMD_PCIPASS_VERIFY                     0x1B00001B//0x17000017//

#define BUFF_MAX			1024*16
#define SMALL_BUFFER_SIZE		512
#define MID_BUFFER_SIZE			8*1024
#define MAXNUM_OF_JMK			10
#define BUFFER_SIZE             1024*48

#define PASSLEN			8
#define HASHLEN			16
#define MKLEN			8
#define BKLEN			8
#define FIRST_CODE_OFFSET	16//20
#define SEQ_ENC_BLOCK_MAX	16*1024 //15*1024
#define SEQ_DEC_BLOCK_MAX	(SEQ_ENC_BLOCK_MAX+8)
#define PRIKEY_BLOCK  0x91*4
#define SEQKEY_BLOCK	(0x88+0x0b)*4
#define MY_MODULUS    0x00010001
#define MODULUS_LEN   4
#define RSA_BLOCK			128
#define RSA_MAX_PLAIN		124
#define RSA_PUB_FLAG		0x5b5a0000
#define MY_RSA_INDEX		9


#define CERTLEN_MAX			4000

#define CERT_OCSP		10

#define JMK_SERIALNUM_LEN		12


typedef struct
{
	int 	open_num;
	int	device_num;
	int	jmk_link_IC;
	int	WorkJmkID;
	int	busy_arr[MAXNUM_OF_JMK];
	volatile unsigned long busy_time[MAXNUM_OF_JMK];
}shared_data;

/*
typedef struct structIoctl
{
	int SubCmd;
	int BuffLen;					//buffer length
	int jnum;					//jmk number
	unsigned char *buff;
}StructIoctl;

enum
{
	P9054_TARGET_WRITE_CMD = 1,
	P9054_TARGET_READ_CMD = 2,
	P9054_DMA_WRITE_CMD = 3,
	P9054_DMA_READ_CMD = 4,
	P9054_GET_DEVICE_NUM = 5,
	P9054_TARGET_INT_CMD = 6,
	P9054_DMA_WRITE_INT_CMD = 7,
	P9054_DMA_READ_INT_CMD = 8,
	P9054_TARGET_QUERY_CMD = 9,
	P9054_DMA_READ_QUERY_CMD = 10,
	AOS_P9054_TARGET_WRITE_CMD_FOR_RSA,
	AOS_P9054_TARGET_WRITE_CMD_FOR_RSA_PUB,
	AOS_P9054_TARGET_QUERY_CMD_FOR_RSA,
	AOS_P9054_TARGET_QUERY_CMD_FOR_RSA_PUB,
	P9054_RESET_DEVICE = 254,
	P9054_RESET_ALL_DEVICE = 255
};
*/
struct k_buff
{
	unsigned char *data;
	unsigned int data_len;
};



struct struct_ioctl;

int Gen_RSA_Out_Pub(int index, unsigned char *pub_key);
int RSA_IndexPub_Enc(int pubindex,int datalen,unsigned char*in,unsigned char*out);
int RSA_IndexPri_Dec(int priindex,int datalen,unsigned char*in,unsigned char*out);
int RSA_IndexPri_Enc(int priindex,int datalen,unsigned char*in,unsigned char*out);
int RSA_IndexPub_Dec(int pubindex,int datalen,unsigned char*in,unsigned char*out);
int Read_CA_Cert(struct k_buff *ca_ca, struct k_buff *ca_pub_key);
int Read_Server_CA(struct k_buff *server_ca, struct k_buff *server_pub_key);
int Seq_Encrypt(int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out);
int Seq_Decrypt(int bflag,unsigned char*bk,int datalen,unsigned char*in,unsigned char*out);
int InitMultiJMK(struct struct_ioctl *io_buf);
int FreeMultiJMK(void);
int Gen_Random(int len,unsigned char*out);
int RSA_OutsidePub_Enc(unsigned char*pubkey,int datalen,unsigned char*in,unsigned char*out);
int RSA_OutsidePub_Dec(unsigned char*pubkey,int datalen,unsigned char*in,unsigned char*out);

extern void AosJNSJMK_getServerCertDer(unsigned char **cert_der, int *cert_der_len);
extern void AosJNSJMK_getServerCertDer(unsigned char **cert_der, int *cert_der_len);
extern void AosJNSJMK_getCA0CertDer(unsigned char **cert_der, int *cert_der_len);
extern void AosJNSJMK_getCA1CertDer(unsigned char **cert_der, int *cert_der_len);
extern void AosJNSJMK_getCA2CertDer(unsigned char **cert_der, int *cert_der_len);
extern void AosJNSJMK_getOCSPCertDer(unsigned char **cert_der, int *cert_der_len);

#endif
