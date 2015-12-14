////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: 9054.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef ZZC_9449_H
#define ZZC_9449_H

enum {
	P9054_LAS0RR	= 0x00,
	P9054_LAS0BA	= 0x04,
	P9054_MARBR		= 0x08,
	P9054_BIGEND 	= 0x0c,
	P9054_LMISC 	= 0x0d,
	P9054_PROT_AREA = 0x0e,
	P9054_EROMRR	= 0x10,
	P9054_EROMBA	= 0x14,
	P9054_LBRD0		= 0x18,
	P9054_DMRR		= 0x1c,
	P9054_DMLBAM	= 0x20,
	P9054_DMLBAI	= 0x24,
	P9054_DMPBAM	= 0x28,
	P9054_DMCFGA	= 0x2c,
	P9054_OPQIS		= 0x30,
	P9054_OPQIM		= 0x34,
	P9054_IQP		= 0x40,
	P9054_OQP		= 0x44,
	P9054_MQCR		= 0xc0,
	P9054_QBAR		= 0xc4,
	P9054_MBOX0_OLD	= 0x40,
	P9054_MBOX1_OLD	= 0x44,
	P9054_MBOX0		= 0x78,
	P9054_MBOX1		= 0x7c,
	P9054_MBOX2		= 0x48,
	P9054_MBOX3		= 0x4c,
	P9054_MBOX4		= 0x50,
	P9054_MBOX5		= 0x54,
	P9054_MBOX6		= 0x58,
	P9054_MBOX7		= 0x5c,
	P9054_P2LDBELL	= 0x60,
	P9054_L2PDBELL	= 0x64,
	P9054_INTCSR	= 0x68,
	P9054_CNTRL		= 0x6c,
	P9054_PCIHIDR	= 0x70,
	P9054_PCIHREV	= 0x74,
	P9054_DMAMODE	= 0x80,
	P9054_DMAPADR	= 0x84,
	P9054_DMALADR	= 0x88,
	P9054_DMASIZ	= 0x8c,
	P9054_DMADPR	= 0x90,
	P9054_DMAMODE1	= 0x94,
	P9054_DMAPADR1	= 0x98,
	P9054_DMALADR1	= 0x9c,
	P9054_DMASIZ1	= 0xa0,
	P9054_DMADPR1	= 0xa4,
	P9054_DMACSR	= 0xa8,
	P9054_DMACSR1	= 0xa9,
	P9054_DMAARB	= 0xac,
	P9054_DMATHR	= 0xb0,
	P9054_DMADAC0	= 0xb4,
	P9054_DMADAC1	= 0xb8,
	P9054_IFHPR		= 0xc8,
	P9054_IFTPR		= 0xcc,
	P9054_IPHPR		= 0xd0,
	P9054_IPTPR		= 0xd4,
	P9054_OFHPR		= 0xd8,
	P9054_OFTPR		= 0xdc,
	P9054_OPHPR		= 0xe0,
	P9054_OPTPR		= 0xe4,
	P9054_QSR		= 0xe8,
	P9054_LAS1RR	= 0xf0,
	P9054_LAS1BA	= 0xf4,
	P9054_LBRD1		= 0xf8,
	P9054_DMDAC		= 0xfc
};
enum 
{
    BIT0  = 0x00000001,
    BIT1  = 0x00000002,
    BIT2  = 0x00000004,
    BIT3  = 0x00000008,
    BIT4  = 0x00000010,
    BIT5  = 0x00000020,
    BIT6  = 0x00000040,
    BIT7  = 0x00000080,
    BIT8  = 0x00000100,
    BIT9  = 0x00000200,
    BIT10 = 0x00000400,
    BIT11 = 0x00000800,
    BIT12 = 0x00001000,
    BIT13 = 0x00002000,
    BIT14 = 0x00004000,
    BIT15 = 0x00008000,
    BIT16 = 0x00010000,
    BIT17 = 0x00020000,
    BIT18 = 0x00040000,
    BIT19 = 0x00080000,
    BIT20 = 0x00100000,
    BIT21 = 0x00200000,
    BIT22 = 0x00400000,
    BIT23 = 0x00800000,
    BIT24 = 0x01000000,
    BIT25 = 0x02000000,
    BIT26 = 0x04000000,
    BIT27 = 0x08000000,
    BIT28 = 0x10000000,
    BIT29 = 0x20000000,
    BIT30 = 0x40000000,
    BIT31 = 0x80000000
};

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
	INIT_MULTIJMK,
	P9054_RESET_DEVICE = 254,
	P9054_RESET_ALL_DEVICE = 255
};

typedef struct structIoctl
{
	int SubCmd;
	int BuffLen;					//buffer length
	int jnum;					//jmk number
	unsigned char *buff;
}StructIoctl;

#define JMK_WRITE_BUFF(a)		(jmk_conf_array[a].dmaWRBuffer)
#define JMK_READ_BUFF(a)		(jmk_conf_array[a].dmaRDBuffer)

#define JMK_WRITE_PHYS_ADDR(a)		(jmk_conf_array[a].dmaWRBufAddr)
#define JMK_READ_PHYS_ADDR(a)		(jmk_conf_array[a].dmaRDBufAddr)

#define MIN_LENGTH_CMD			4

#define REG_DMALBAR_9449		0x4B0
#define REG_DMAHBAR_9449		0x4B4
#define REG_DMABSR_9449			0x4B8
#define REG_DMACTRLR_9449		0x4BC
#define REG_HCTRL_9449			0x4E0
#define REG_HICS_9449			0x4E4
#define REG_H2LM_9449			0x4E8
#define REG_LICS_9449			0x4F4
#define REG_L2HM_9449			0x4F8

//#define HICS_DMACLEAR			0xFFFFFFDF
#define HICS_DMAFIN			0x00000020
//#define HICS_MAILCLEAR			0xFFFFFFF7
#define HICS_MAILHAVE			0x00000008
//#define HICS_L2HINTCLEAR		0xFFFFFFEF
#define HICS_L2HINTHAVE			0x00000010

#define LICS_MAILINTABLE		0x00080000
#define LICS_MAILINT            0x00000008

#define H2LM_INTABLE			0x01000000

#define DMALBAR_BASE			0x00
#define DMA_DWMASK			0xFFFFFFFC

#define DMACTRLR_H2L			0x00
#define DMACTRLR_L2H			0x01

#define HCTRL_S				0x02
#define HCTRL_R				0x01
#define HCTRL_SRI			0x00

/*
 * Add by lijing.
 * Use to userland init the jmk
*/

#define MAX_CA_LENGTH 2048

#define SEND_CA_TO_JMK 0
#define RECEIVE_CA_FROM_JMK 1

struct data_buf{
	int len;
	unsigned char buf[MAX_CA_LENGTH];
};

struct io_buf{
	
	struct data_buf ca0;
	struct data_buf ca1;
	struct data_buf ca2;
	struct data_buf server_cert;
	struct data_buf pub2;
	struct data_buf ocsp;
	struct data_buf pri2;
};

struct struct_ioctl{
	unsigned int cmd;
	int send_direct;
	struct io_buf buf;
};

//#define INIT_MULTIJMK	_IOWR(0xbb, 0x64, struct struct_ioctl)

/*
 * Add end
*/

#endif
