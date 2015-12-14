////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: j9054.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#define HAVE_OWN_HEADER_H
#include <linux/pci.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/vmalloc.h>
#include <linux/ioport.h>
#include <linux/errno.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/fcntl.h>
#include <linux/ptrace.h>
#include <linux/serial.h>
#include <linux/delay.h>
#include <asm/system.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/ioctl.h>
#include <asm/irq.h>
#include <asm/segment.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>

#include "JNS9054Card/9054.h"
#include "JNS9054Card/j9054.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"

#define	WJJMK_VERSION			"1.1k"
#define	WJJMK_MAJOR       		65
#define	WJJMK_NAME				"j9054"
#define JMK_BUFFER_SIZE			65*1024
#define	PCI_VENDOR_ID_JMK		0x18b5
#define PCI_DEVICE_ID_JMK		0x9054 //0x9449
#define	JMK_MAXPIECENUM			10
#define BLOCK_LEFT              4
#define BLOCK_BYTES    			32
#define TEMP_CMP_BUFF_LENGTH	24
#define WAIT_DELAYTIME			5000
#define CMD_TRIGER_DELAY 		10 // us
#define AOS_CMD_TRIGER_DELAY 		50 // us

unsigned char TempCmpBuf[TEMP_CMP_BUFF_LENGTH];


struct jmkpci_conf
{
  int jmk_opened;
  int jmk_irq;
  int jmk_ready;

//---------- jmk_dma_times --------------- 
  int jmk_busy; 
  int jmk_dma_times;
  
  wait_queue_head_t int_wait;

  unsigned char *dmaWRBuffer;
  unsigned char *dmaRDBuffer;
  unsigned long dmaWRBufAddr;
  unsigned long dmaRDBufAddr;

  char * kbuf;
  unsigned long csr_base0;
  unsigned long csr_base1;
  unsigned long csr_base2;
  unsigned long csr_base3;
  unsigned long csr_base4;
  struct pci_dev *pcidev;
};

// Gloable Variable
int piece_of_jmk=0;
static struct jmkpci_conf jmk_conf_array[JMK_MAXPIECENUM];

int nRequestedIRQ=0;
int IRQArray[JMK_MAXPIECENUM];
spinlock_t irq_spinlock = SPIN_LOCK_UNLOCKED;

#define jmk_readb0(jnum,addr) (*((byte*)(jmk_conf_array[jnum].csr_base0|addr)))
#define jmk_readw0(jnum,addr) (*((word*)(jmk_conf_array[jnum].csr_base0|addr)))
#define jmk_reg_readl(jnum,addr) (*((unsigned long*)(jmk_conf_array[jnum].csr_base0|addr)))
#define jmk_writeb0(jnum,addr,val) (*((byte*)(jmk_conf_array[jnum].csr_base0|addr)))=val
#define jmk_writew0(jnum,addr,val) (*((word*)(jmk_conf_array[jnum].csr_base0|addr)))=val
#define jmk_reg_writel(jnum,addr,val) (*((unsigned long*)(jmk_conf_array[jnum].csr_base0|addr)))=val

#define jmk_readb(jnum,addr) (*((byte*)(jmk_conf_array[jnum].csr_base0|addr)))
#define jmk_readw(jnum,addr) (*((word*)(jmk_conf_array[jnum].csr_base0|addr)))
#define jmk_readl(jnum,addr) (*((unsigned long*)(jmk_conf_array[jnum].csr_base0|addr)))
#define jmk_writeb(jnum,addr,val) (*((byte*)(jmk_conf_array[jnum].csr_base0|addr)))=val
#define jmk_writew(jnum,addr,val) (*((word*)(jmk_conf_array[jnum].csr_base0|addr)))=val
#define jmk_writel(jnum,addr,val) (*((unsigned long*)(jmk_conf_array[jnum].csr_base0|addr)))=val

#define jmk_readb2(jnum,addr) (*((byte*)(jmk_conf_array[jnum].csr_base2+addr)))
#define jmk_readw2(jnum,addr) (*((word*)(jmk_conf_array[jnum].csr_base2+addr)))
//#define jmk_mem_readl(jnum,addr) inl(jmk_conf_array[jnum].csr_base2|addr)
#define jmk_mem_readl(jnum,addr) (*((unsigned long*)(jmk_conf_array[jnum].csr_base2+addr)))
#define jmk_writeb2(jnum,addr,val) (*((byte*)(jmk_conf_array[jnum].csr_base2+addr)))=val
#define jmk_writew2(jnum,addr,val) (*((word*)(jmk_conf_array[jnum].csr_base2+addr)))=val
//#define jmk_mem_writel(jnum,addr,val) outl(jmk_conf_array[jnum].csr_base2|addr,val)
#define jmk_mem_writel(jnum,addr,val) (*((unsigned long*)(jmk_conf_array[jnum].csr_base2+addr)))=val


void InitDMA(int jnum); 
void InitController(int jnum); 

int init_jmk_module(void);
void cleanup_jmk_module(void);
int DMA_H2L(int jnum,int length);
int DMA_L2H(int jnum,int recvLen); 

//it init_module(void);
//void cleanup_module(void);

/* 
 * static functions: 
 */ 

module_init(init_jmk_module);
module_exit(cleanup_jmk_module);
MODULE_LICENSE("GPL");


//static void jmk_interrupt(int irq,void *dev_id,struct pt_regs *regs);
static irqreturn_t jmk_interrupt(int irq,void *dev_id,struct pt_regs *regs);

static int jmk_open(struct inode *, struct file *); 
static ssize_t jmk_write(struct file *, const char *, size_t, loff_t *); 
static ssize_t jmk_read(struct file *, char *, size_t, loff_t *); 
static int jmk_ioctl(struct inode *,struct file *, unsigned int, unsigned long); 
static int jmk_release(struct inode *, struct file *); 

void WriteMem(int jnum,int mem_offset,void* buffer,int len)
{
	int i,len_d;
	unsigned long *p_ulong;
	unsigned long tmp_l;
	p_ulong=(unsigned long *)buffer;

	len_d=((len+3)/4);
	for (i=0;i<len_d;i++)
	{
//		aos_debug("jnum=0x%x,mem_addr=0x%x,buffer_addr=0x%x\n",jnum,
//			jmk_conf_array[jnum].csr_base2+mem_offset,p_ulong);
		jmk_mem_writel(jnum,mem_offset+i*4,p_ulong[i]);
	}
	tmp_l=jmk_mem_readl(jnum,mem_offset);
//	aos_debug("read mem[0] = 0x%08x\n",tmp_l);
}

void ReadMem(int jnum,int mem_offset,void* buffer,int len)
{
	int i,len_d;
	unsigned long *p_ulong;

	p_ulong=(unsigned long *)buffer;
	len_d=((len+3)/4);
	for (i=0;i<len_d;i++)
	{
//		aos_debug("jnum=0x%x,mem_addr=0x%x,buffer_addr=0x%x\n",jnum,
//			jmk_conf_array[jnum].csr_base2+mem_offset,p_ulong);
		p_ulong[i]=jmk_mem_readl(jnum,mem_offset+i*4);
	}

//	PrintBuff(buffer,len);
}

/********************************************************/ 
/*  Initialize 9449 pci controller:			*/ 
/*  Enable  device  for  operation  --  init.  regs.;	*/ 
/*  clear  extraneous  intr.  bits;  			*/ 
/*  enable  various  interrupts.			*/ 
/*  write command reg,interrupt Control/Status Register */ 
/********************************************************/ 
void InitController(int jnum) 
{
} 
 
/****************************************************************/ 
/* Function: startDMA						*/ 
/* Purpose: Configure the C9449 for PCI initiated DMA		*/ 
/* Inputs: rw_flag,length		 Outputs: None		*/ 
/* The following function assumes the C9449 is configured	*/ 
/* for PCI initiated DMA transfers and sets up the DMA channels */ 
/****************************************************************/ 
int DMA_H2L(int jnum,int length) 
{
	return (0); 
}	// end of startDMA() 
/****************************************************************/ 
/* Function: receiveDMA						*/ 
/****************************************************************/ 
int DMA_L2H(int jnum,int recvLen) 
{ 
	return (0); 
}	// end of receive DMA() 

/********************************************************/ 
/*  Initialize AMCC 5933 DMA transfer			*/ 
/********************************************************/ 
void InitDMA(int jnum) 
{ 
} 

/****************************************************************/ 
/*   	Function Name: jmk_init()				*/ 
/****************************************************************/ 
int jmk_init(void)
{

  struct pci_dev *pdev;

	int bRequested;
  int i;

  struct list_head *n = pci_devices.next;
	aos_debug("function jmk_init begin\n");
  for (;n!=&pci_devices;n=n->next)
  {
      pdev = pci_dev_g(n);
      aos_debug("vendor=%x ; device=%x\n",pdev->vendor,pdev->device);
  }

  pdev=NULL;
  piece_of_jmk=0;
if (1)
  {
    if ((pdev=pci_find_device(PCI_VENDOR_ID_JMK,PCI_DEVICE_ID_JMK,pdev)) != NULL)
    {
      do
      {
      	bRequested=0;
      	for (i=0;i<piece_of_jmk;i++)
      	{
      		if (pdev->irq == jmk_conf_array[i].jmk_irq)
      		{
      			bRequested=1;
      			break;
      		}
      	}
/*      	if (i==1)
      	{
      		pdev=pci_find_device(PCI_VENDOR_ID_JMK,PCI_DEVICE_ID_JMK,pdev);
      		if (pdev)
      			continue;
      		else
      			break;
      	}*/
	if (!bRequested)
	{	// if requested
		if (request_irq(pdev->irq,jmk_interrupt,SA_SHIRQ|SA_INTERRUPT,WJJMK_NAME,NULL))
			bRequested=0;
		else
		{
			bRequested=1;
			IRQArray[nRequestedIRQ]=pdev->irq;
			nRequestedIRQ++;
		}
	}

        if (!bRequested)
        {	// if request failed
           aos_debug("Cann't register IRQ %d\n",pdev->irq);
//           continue;
        }
        else
        {	//if request success
        	//init wait queue for every jmk
        	init_waitqueue_head(&(jmk_conf_array[piece_of_jmk].int_wait));

//           aos_debug("Register IRQ %d OK\n",pdev->irq);
//          jmk_conf_array[piece_of_jmk].dmaWRBuffer=(char*)kmalloc(JMK_BUFFER_SIZE,GFP_KERNEL|GFP_DMA);
          jmk_conf_array[piece_of_jmk].dmaWRBuffer=(char*)__get_dma_pages(GFP_KERNEL, get_order(JMK_BUFFER_SIZE));
          jmk_conf_array[piece_of_jmk].dmaWRBufAddr=virt_to_phys(jmk_conf_array[piece_of_jmk].dmaWRBuffer);
          aos_debug("dmaWRBuffer=0x%x ; dmaWRBufAddr=0x%x\n",
                 (unsigned int)jmk_conf_array[piece_of_jmk].dmaWRBuffer,(unsigned int)jmk_conf_array[piece_of_jmk].dmaWRBufAddr);
//          jmk_conf_array[piece_of_jmk].dmaRDBuffer=(char*)kmalloc(JMK_BUFFER_SIZE,GFP_KERNEL|GFP_DMA);
//          jmk_conf_array[piece_of_jmk].dmaRDBuffer=(char*)__get_dma_pages(GFP_KERNEL, get_order(JMK_BUFFER_SIZE));
//          jmk_conf_array[piece_of_jmk].dmaRDBufAddr=virt_to_phys(jmk_conf_array[piece_of_jmk].dmaRDBuffer);
          jmk_conf_array[piece_of_jmk].dmaRDBuffer=jmk_conf_array[piece_of_jmk].dmaWRBuffer;
          jmk_conf_array[piece_of_jmk].dmaRDBufAddr=jmk_conf_array[piece_of_jmk].dmaWRBufAddr;
          aos_debug("dmaRDBuffer=0x%x ; dmaRDBufAddr=0x%x\n",
                 (unsigned int)jmk_conf_array[piece_of_jmk].dmaRDBuffer,(unsigned int)jmk_conf_array[piece_of_jmk].dmaRDBufAddr);

          jmk_conf_array[piece_of_jmk].pcidev=pdev;

          jmk_conf_array[piece_of_jmk].jmk_irq=pdev->irq;
          jmk_conf_array[piece_of_jmk].csr_base0=0;
          
          pci_read_config_dword(pdev, PCI_BASE_ADDRESS_0, (void*)&(jmk_conf_array[piece_of_jmk].csr_base0));
          pci_read_config_dword(pdev, PCI_BASE_ADDRESS_2, (void*)&(jmk_conf_array[piece_of_jmk].csr_base2));

        aos_debug("JMK csr_base0=0x%x\n",(unsigned int)jmk_conf_array[piece_of_jmk].csr_base0);
	aos_debug("JMK csr_base2=0x%x\n",(unsigned int)jmk_conf_array[piece_of_jmk].csr_base2);

          jmk_conf_array[piece_of_jmk].csr_base0 = (unsigned long)ioremap(jmk_conf_array[piece_of_jmk].csr_base0,0x100);//2*1024);
          jmk_conf_array[piece_of_jmk].csr_base2 = (unsigned long)ioremap(jmk_conf_array[piece_of_jmk].csr_base2,16*1024);//0x1000000);//

//------------- jmk_dma_times -------------------------
	jmk_conf_array[piece_of_jmk].jmk_dma_times=0;
	jmk_conf_array[piece_of_jmk].jmk_busy=0;

        pci_write_config_dword(pdev,4,0x02900117);

#if 0
	//复位找到的加密卡
	jmk_reg_writel(piece_of_jmk,P9054_CNTRL,0x780f767e);
	for (tmpi=0;tmpi<10;tmpi++)
		tmpi=jmk_reg_readl(piece_of_jmk,P9054_CNTRL);
	jmk_reg_writel(piece_of_jmk,P9054_CNTRL,0x380f767e);
#endif

          piece_of_jmk++;
        }

        aos_debug("9449 PCI JMK found %d\n",piece_of_jmk);
        aos_debug("JMK devfn=0x%x ; class=0x%x ; hdr_type=0x%x\n",pdev->devfn,
          pdev->class,pdev->hdr_type);
        aos_debug("JMK vendor=0x%x ; device=0x%x ; int=0x%x ; addr0=0x%x ; addr1=0x%x\n",pdev->vendor,
          (unsigned int)pdev->device,pdev->irq,(unsigned int)jmk_conf_array[piece_of_jmk-1].csr_base0,(unsigned int)jmk_conf_array[piece_of_jmk-1].csr_base1);
        aos_debug("JMK csr_base2=0x%x\n",(unsigned int)jmk_conf_array[piece_of_jmk-1].csr_base2);
        aos_debug("JMK csr_base3=0x%x\n",(unsigned int)jmk_conf_array[piece_of_jmk-1].csr_base3);
        aos_debug("JMK csr_base4=0x%x\n",(unsigned int)jmk_conf_array[piece_of_jmk-1].csr_base4);

        
        pdev=pci_find_device(PCI_VENDOR_ID_JMK,PCI_DEVICE_ID_JMK,pdev);
        
        udelay(40);

      }while(pdev != NULL && piece_of_jmk <= JMK_MAXPIECENUM);
    }
    else
    {
      aos_debug("Cann't find PCI JMK!\n");
      return -1;
    }
  }
  else
  {
    aos_debug("Cann't find PCI BIOS support!\n");
    return -1;
  }

  for (i=0;i<piece_of_jmk;i++)
  {
    aos_debug("JMK %d \n",i+1);
    aos_debug("int = 0x%x ; csr_base1 = 0x%x\n",(unsigned int)jmk_conf_array[i].jmk_irq,(unsigned int)jmk_conf_array[i].csr_base1);
  }

/*	for (i=0;i<piece_of_jmk;i++)
	{
		memset(jmk_conf_array[i].dmaWRBuffer,3,0x100);
		memset(jmk_conf_array[i].dmaRDBuffer,2,0x100);
		WriteMem(i,0,jmk_conf_array[i].dmaWRBuffer,0x100);
//		for (tmpi=0;tmpi<10;tmpi++) nbyte=1;
		PrintBuff(jmk_conf_array[i].dmaWRBuffer,0x100);
		ReadMem(i,0,jmk_conf_array[i].dmaRDBuffer,0x100);
		PrintBuff(jmk_conf_array[i].dmaRDBuffer,0x100);
		if (memcmp(jmk_conf_array[i].dmaRDBuffer,jmk_conf_array[i].dmaWRBuffer,0x100))
			aos_debug("Error\n");
		else
			aos_debug("OK\n");
	}*/
	aos_debug("the piece_of_jmk is %x\n", piece_of_jmk);
	aos_debug("function jmk_init end\n");
	return 0;
}

/****************************************************************/ 
/*   	Function Name: jmk_open()				*/ 
/****************************************************************/ 
int jmk_free(void)
{
	int i;

	for (i=0;i<piece_of_jmk;i++)
	{
		iounmap((void*)jmk_conf_array[piece_of_jmk].csr_base0);
		iounmap((void*)jmk_conf_array[piece_of_jmk].csr_base2);
		free_pages((unsigned int)jmk_conf_array[i].dmaWRBuffer, get_order(JMK_BUFFER_SIZE));
//		free_pages((unsigned int)jmk_conf_array[i].dmaRDBuffer, get_order(JMK_BUFFER_SIZE));
	}

	for (i=0;i<nRequestedIRQ;i++)
	{	//release Requested IRQ
		free_irq(IRQArray[i],NULL);
	}
	
	return 0;
}

/****************************************************************/ 
/*   	Function Name: jmk_open()				*/ 
/****************************************************************/ 
static int jmk_open(struct inode *inode, struct file *filp) 
{ 
//    MOD_INC_USE_COUNT;
    return (0); 
} 

/****************************************************************/ 
/*   	Function Name: jmk_release()				*/ 
/****************************************************************/ 
static int jmk_release(struct inode *inode, struct file *filp) 
{ 
//     MOD_DEC_USE_COUNT;
     return(0); 
} 
 
/****************************************************************/ 
/*   	Function Name: jmk_write()				*/ 
/****************************************************************/ 
static ssize_t jmk_write(struct file *file, const char *buf,size_t count,loff_t *ppos) 
{
  return -1; 
} 
//
//	lijing test
//
int jmk_kernel(unsigned int cmd, void* arg)
{
	return piece_of_jmk;
}
/****************************************************************/ 
/*   	Function Name: jmk_kernel_ioctl()				*/ 
/****************************************************************/ 
int jmk_kernel_ioctl(unsigned int cmd, void* arg)
{
	int tmpi,jnum,len,len_d,i;
	void *pBuff;
	int timeout;
	long RetSleep;
	//struct timeval start, end;

	unsigned char* p=(unsigned char*)arg;

//aos_debug("cmd = %d\n",cmd);

	switch (cmd)
	{
		case P9054_TARGET_WRITE_CMD:
		case P9054_DMA_WRITE_CMD:
		case P9054_TARGET_INT_CMD:
		case P9054_TARGET_QUERY_CMD:
		case AOS_P9054_TARGET_QUERY_CMD_FOR_RSA:
		case AOS_P9054_TARGET_WRITE_CMD_FOR_RSA_PUB:
		case P9054_DMA_WRITE_INT_CMD:
			memcpy(&len, p+4, 4);
			//udelay(5);
			
//aos_debug("len=%d\n",len);

			if (len % 4)
				len-=1;
			if (len < MIN_LENGTH_CMD)
				return -1;
			len_d=(len+3)/4;
			memcpy(&jnum, p+8, 4);
			//udelay(5);
			jnum-=1;
			
//aos_debug("jnum=%d\n",jnum);

			if (jnum < 0 || jnum > piece_of_jmk)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "card no error %d", jnum);
				return -2;
			}
//			pBuff=p+12;
			memcpy(&pBuff, p+12, 4);
			//udelay(5);

//aos_debug("pBuff=%x\n",(unsigned int)pBuff);

			if (!pBuff)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "pBuff is null");
				return -3;
			}
			memcpy(JMK_WRITE_BUFF(jnum)+4, pBuff, len);
			//aos_debug("time is %ld\n", end.tv_usec-start.tv_usec);
			udelay(5);
			memcpy(JMK_WRITE_BUFF(jnum),&len_d,4);
			//aos_debug("time is %ld\n", end.tv_usec-start.tv_usec);
			break;
		case P9054_DMA_READ_CMD:
		case P9054_DMA_READ_INT_CMD:
		case P9054_DMA_READ_QUERY_CMD:
			memcpy(&len, p+4, 4);
		case P9054_TARGET_READ_CMD:
		case P9054_RESET_DEVICE:
			memcpy(&jnum, p+8, 4);
			jnum-=1;
			if (jnum < 0 || jnum >= piece_of_jmk)
				return -2;

			memcpy(&pBuff,p+12,4);
			udelay(50);
			
//aos_debug("pBuff=%x\n",(unsigned int)pBuff);

			if (!pBuff)
				return -3;
			break;
		case P9054_GET_DEVICE_NUM:
			aos_debug("P9054_GET_DEVICE_NUM %d\n", piece_of_jmk);
			return piece_of_jmk;
		case P9054_RESET_ALL_DEVICE:
			for (jnum=0;jnum < piece_of_jmk;jnum++)
			{
				jmk_reg_writel(jnum,P9054_CNTRL,0x780f767e);
				for (tmpi=0;tmpi<10;tmpi++)
					tmpi=jmk_reg_readl(jnum,P9054_CNTRL);
				jmk_reg_writel(jnum,P9054_CNTRL,0x380f767e);
				udelay(50);
			}
			return 0;
		default:break;
	}

	switch (cmd)
	{
		case P9054_TARGET_WRITE_CMD:
			return -1;
			break;
		case P9054_TARGET_READ_CMD:
			return -1;
			break;
		case P9054_DMA_WRITE_CMD:
		case P9054_DMA_WRITE_INT_CMD:
			//aos_debug("Enter DMA WRITE CMD\n");
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT0|BIT1|BIT7|BIT8|BIT10|BIT16|BIT17);
			udelay(2);
			if (cmd == P9054_DMA_WRITE_INT_CMD)
			{
				tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
				//aos_debug("P9054_INTCSR=%08x\n",tmpi);
				tmpi|=(BIT18|BIT9|BIT3);//|BIT11);|BIT8
				jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
			}

			jmk_reg_writel(jnum,P9054_MARBR,0x200000);
			jmk_reg_writel(jnum,P9054_DMAPADR,JMK_WRITE_PHYS_ADDR(jnum));
			jmk_reg_writel(jnum,P9054_DMALADR,0xC0000000);
			jmk_reg_writel(jnum,P9054_DMASIZ,len);	//?????????
			jmk_reg_writel(jnum,P9054_DMADPR,BIT0);
			jmk_reg_writel(jnum,P9054_DMACSR,BIT0|BIT1);

			if (cmd == P9054_DMA_WRITE_INT_CMD)
				interruptible_sleep_on(&jmk_conf_array[jnum].int_wait);
			else
			{
			}
			//aos_debug("Leave DMA WRITE CMD\n");
			break;
		case P9054_DMA_READ_CMD:
		case P9054_DMA_READ_INT_CMD:
			//aos_debug("Enter DMA READ CMD\n");
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT0|BIT1|BIT7|BIT8|BIT10|BIT11|BIT17);//BIT16|

			jmk_reg_writel(jnum,P9054_MARBR,0x200000);
			jmk_reg_writel(jnum,P9054_DMAPADR,JMK_READ_PHYS_ADDR(jnum));
			jmk_reg_writel(jnum,P9054_DMALADR,0xC0000000);
			jmk_reg_writel(jnum,P9054_DMASIZ,len);	//?????????
			memset(JMK_READ_BUFF(jnum),0,TEMP_CMP_BUFF_LENGTH);
			jmk_reg_writel(jnum,P9054_DMADPR,BIT0|BIT3);
			jmk_reg_writel(jnum,P9054_DMACSR,BIT0|BIT1);


#if 1
			if (cmd == P9054_DMA_READ_INT_CMD)
			{
				RetSleep=sleep_on_timeout(&jmk_conf_array[jnum].int_wait,10);
					aos_debug("interruptible_sleep_on_timeout ret=0x%x\n",RetSleep);
				aos_debug("dma waked up!\n");
aos_debug("DMA Size rest=0x%x\n",jmk_reg_readl(jnum,P9054_DMASIZ));
				if (!memcmp(JMK_READ_BUFF(jnum),TempCmpBuf,TEMP_CMP_BUFF_LENGTH))
					return -1;
			}
			else
			{
			}
#endif


			memcpy(pBuff,JMK_READ_BUFF(jnum),len);
			memcpy(p+4,&len,4);

			jmk_conf_array[jnum].jmk_dma_times++;
			//aos_debug("Leave DMA READ CMD\n");
			return len;
			break;
		case P9054_DMA_READ_QUERY_CMD:
			//aos_debug("Enter DMA READ CMD\n");
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT0|BIT1|BIT7|BIT8|BIT11|BIT17);//BIT16|

			jmk_reg_writel(jnum,P9054_MARBR,0x200000);
			jmk_reg_writel(jnum,P9054_DMAPADR,JMK_READ_PHYS_ADDR(jnum));
			jmk_reg_writel(jnum,P9054_DMALADR,0xC0000000);
			jmk_reg_writel(jnum,P9054_DMASIZ,len);	//?????????
			memset(JMK_READ_BUFF(jnum),0,TEMP_CMP_BUFF_LENGTH);
			jmk_reg_writel(jnum,P9054_DMADPR,BIT0|BIT3);
			jmk_reg_writel(jnum,P9054_DMACSR,BIT0|BIT1);


			tmpi=0;
			timeout = jiffies + WAIT_DELAYTIME;
			for (i=0x300000;1;)
			{
				if (jiffies > timeout)
				{
					aos_debug("dma query time out error, i = %x\n", i);
					break;
				}
				tmpi=jmk_reg_readl(jnum,P9054_DMACSR);
				if (tmpi & BIT4)
				{
					//aos_debug("dma query ok\n");
					break;
				}
				i--;
			}

			if (tmpi == 0)// || i <= 0)
				return -2;


			memcpy(pBuff,JMK_READ_BUFF(jnum),len);
			memcpy(p+4,&len,4);

			jmk_conf_array[jnum].jmk_dma_times++;
			//aos_debug("Leave DMA READ CMD\n");
			return len;
			break;
		case P9054_TARGET_INT_CMD:
			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT18|BIT9|BIT8|BIT3);//|BIT11);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		
			//jmk_readl(jnum,P9054_LAS0BA);	liqin invalid statment, so comment it		

			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);	

			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
	
			udelay(100);
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk

			sleep_on_timeout(&jmk_conf_array[jnum].int_wait,1000);

			udelay(5);
			ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
			len=len_d*4;			
			aos_debug("read len=0x%08x",len);
			if (len > 16*1024 || len <0)
				return -1;//len=256;
			memcpy(p+4,&len,4);	
			ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);
			memcpy(pBuff,JMK_READ_BUFF(jnum),len);	

			return len;					//函数返回值是返回结果的字节长度
			break;
		case AOS_P9054_TARGET_WRITE_CMD_FOR_RSA_PUB:
			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			//aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT3);//|BIT11);
			tmpi&=(~(BIT8|BIT9|BIT18));
//			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
//			jmk_reg_writel(jnum,P9054_DMAMODE,BIT7|BIT8|BIT10|BIT17|BIT1|BIT0);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		//使能加密卡 MEM 0 （可写/译码）
			
			//liqin invalid statement, comment it, jmk_readl(jnum,P9054_LAS0BA);				//等待（浪费时间）

			//设置加密卡 MEM 0 为 32 位宽
			//使能 BTERM# 输入信号
			//Prefetch（预取）计数使能
			//预取技术设置为 8 （长字）
			//扩展 ROM 空间局部总线宽度设置为 32 位
			//扩展 ROM 空间 TA#/READY# 输入信号使能
			//加密卡 MEM 0 BURST（突发）使能
			//PCI Target 重试延迟时钟数设置为4（×8）个时钟
			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);		//门铃寄存器清零

			//命令信息写入加密卡的 MEM 0			
			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
			//命令信息的长度写入邮箱寄存器0,引发加密卡内终端处理
			udelay(AOS_CMD_TRIGER_DELAY);
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk

			tmpi=0;
			timeout = jiffies + 4000;//WAIT_DELAYTIME;
			for (i=0x300000;1;)
			{
				//if ((int)time_after(jiffies,  (long)timeout))//i <= 0)
				if (jiffies >  timeout)
				{
					aos_debug("query time out error\n");
					break;
				}
				tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
				if (tmpi)
				{
					//aos_debug("query ok\n");
					len_d=tmpi;
					jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);
					break;
				}
				i--;
			}
			

			if (tmpi == 0)// || i <= 0)
				return -2;

			//获得返回的长字个数
			ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
			len=len_d*4;					//长字个数转换为字节个数

			//aos_debug("tmpi=%d , len_d=0x%08x\n",tmpi,len_d);

			if (len > 16*1024 || len <0)// || len_d != tmpi)
				return -1;//len=256;
			memcpy(p+4,&len,4);			//字节个数返回到结构中
			ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);	//读加密卡内存中的返回结果
			memcpy(pBuff,JMK_READ_BUFF(jnum),len);	//返回结果拷贝到用户空间

			//aos_trace_hex("JMK_READ_BUFF",(char*)(JMK_READ_BUFF(jnum)),len);

			return len;					//函数返回值是返回结果的字节长度
			break;
			
		case AOS_P9054_TARGET_QUERY_CMD_FOR_RSA:
			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			//aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT3);//|BIT11);
			tmpi&=(~(BIT8|BIT9|BIT18));
//			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
//			jmk_reg_writel(jnum,P9054_DMAMODE,BIT7|BIT8|BIT10|BIT17|BIT1|BIT0);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		//使能加密卡 MEM 0 （可写/译码）
			jmk_readl(jnum,P9054_LAS0BA);				//等待（浪费时间）

			//设置加密卡 MEM 0 为 32 位宽
			//使能 BTERM# 输入信号
			//Prefetch（预取）计数使能
			//预取技术设置为 8 （长字）
			//扩展 ROM 空间局部总线宽度设置为 32 位
			//扩展 ROM 空间 TA#/READY# 输入信号使能
			//加密卡 MEM 0 BURST（突发）使能
			//PCI Target 重试延迟时钟数设置为4（×8）个时钟
			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);		//门铃寄存器清零

			//命令信息写入加密卡的 MEM 0			
			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
			//命令信息的长度写入邮箱寄存器0,引发加密卡内终端处理
			udelay(AOS_CMD_TRIGER_DELAY);
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk

			tmpi=0;
			timeout = jiffies + 4000;//WAIT_DELAYTIME;
			//added by lijing
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(4);
			for (i=0x300000;1;)
			{
				//if ((int)time_after(jiffies,  (long)timeout))//i <= 0)
				if (jiffies > timeout)
				{
					aos_debug("query time out error\n");
					break;
				}
				tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
				if (tmpi)
				{
					//aos_debug("query ok\n");
					len_d=tmpi;
					jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);
					break;
				}
				i--;
			}
			

			if (tmpi == 0)// || i <= 0)
				return -2;

			//获得返回的长字个数
			ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
			len=len_d*4;					//长字个数转换为字节个数

			//aos_debug("tmpi=%d , len_d=0x%08x\n",tmpi,len_d);

			if (len > 16*1024 || len <0)// || len_d != tmpi)
				return -1;//len=256;
			memcpy(p+4,&len,4);			//字节个数返回到结构中
			ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);	//读加密卡内存中的返回结果
			memcpy(pBuff,JMK_READ_BUFF(jnum),len);	//返回结果拷贝到用户空间

			//aos_trace_hex("JMK_READ_BUFF",(char*)(JMK_READ_BUFF(jnum)),len);

			return len;					//函数返回值是返回结果的字节长度
			break;
			
		case P9054_TARGET_QUERY_CMD:
			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			//aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT3);//|BIT11);
			tmpi&=(~(BIT8|BIT9|BIT18));
//			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
//			jmk_reg_writel(jnum,P9054_DMAMODE,BIT7|BIT8|BIT10|BIT17|BIT1|BIT0);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		//使能加密卡 MEM 0 （可写/译码）
			jmk_readl(jnum,P9054_LAS0BA);				//等待（浪费时间）

			//设置加密卡 MEM 0 为 32 位宽
			//使能 BTERM# 输入信号
			//Prefetch（预取）计数使能
			//预取技术设置为 8 （长字）
			//扩展 ROM 空间局部总线宽度设置为 32 位
			//扩展 ROM 空间 TA#/READY# 输入信号使能
			//加密卡 MEM 0 BURST（突发）使能
			//PCI Target 重试延迟时钟数设置为4（×8）个时钟
			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);		//门铃寄存器清零

			//命令信息写入加密卡的 MEM 0			
			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
			//命令信息的长度写入邮箱寄存器0,引发加密卡内终端处理
			udelay(AOS_CMD_TRIGER_DELAY);
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk

			tmpi=0;
			timeout = jiffies + 4000;//WAIT_DELAYTIME;
			//added by lijing
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(2);// comment by liqin
			//for (i=0x300000;1;)
			while(1)
			{
				//if ((int)time_after(jiffies,  (long)timeout))//i <= 0)
				if (jiffies > timeout)
				{
					aos_debug("query time out error\n");
					break;
				}
				tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
				if (tmpi)
				{
					//aos_debug("query ok\n");
					len_d=tmpi;
					jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);
					break;
				}
				//i--;
			}
			

			if (tmpi == 0)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "jmk reg real error, card no %d", jnum);
				return -2;
			}
			//获得返回的长字个数
			ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
			len=len_d*4;					//长字个数转换为字节个数

			//aos_debug("tmpi=%d , len_d=0x%08x\n",tmpi,len_d);

			if (len > 16*1024 || len <0)// || len_d != tmpi)
			{
				aos_alarm(eAosMD_SSL, eAosAlarm_ProgErr, "return len %d error, card no %d", len, jnum);
				return -1;//len=256;
			}
			memcpy(p+4,&len,4);			//字节个数返回到结构中
			ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);	//读加密卡内存中的返回结果
			memcpy(pBuff,JMK_READ_BUFF(jnum),len);	//返回结果拷贝到用户空间

			//aos_trace_hex("JMK_READ_BUFF",(char*)(JMK_READ_BUFF(jnum)),len);

			return len;					//函数返回值是返回结果的字节长度
			break;
		case P9054_RESET_DEVICE:
			jmk_reg_writel(jnum,P9054_CNTRL,0x780f767e);
			for (tmpi=0;tmpi<10;tmpi++)
				tmpi=jmk_reg_readl(jnum,P9054_CNTRL);
			udelay(5);
			jmk_reg_writel(jnum,P9054_CNTRL,0x380f767e);
			break;
		default:break;
	}
	
	return 0;
}



/****************************************************************/ 
/*   	Function Name: jmk_ioctl()				*/ 
/****************************************************************/ 
static int jmk_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg) 
{
	int tmpi,jnum,len,len_d,i;
	void *pBuff;
	int timeout;
	long RetSleep;
	int ret;

	unsigned char* p=(unsigned char*)arg;

	aos_debug("cmd = %d\n",cmd);

	switch (cmd)
	{
		case INIT_MULTIJMK:
			ret=InitMultiJMK((struct struct_ioctl*)arg);
			if(ret>0)
			{
				aos_debug("init jmk return %d\n", ret);
			}
			else
			{
				aos_debug("init jmk error! return value is %d\n", ret);
			}
			return ret;
		case P9054_TARGET_WRITE_CMD:
		case P9054_DMA_WRITE_CMD:
		case P9054_TARGET_INT_CMD:
		case P9054_TARGET_QUERY_CMD:
		case P9054_DMA_WRITE_INT_CMD:
			copy_from_user(&len,p+4,4);
			//udelay(5);
			aos_debug("len=%d\n",len);
			if (len % 4)
				len-=1;
			if (len < MIN_LENGTH_CMD)
				return -1;
			len_d=(len+3)/4;
			copy_from_user(&jnum,p+8,4);
			//udelay(5);
			jnum-=1;
			aos_debug("jnum=%d\n",jnum);
			if (jnum < 0 || jnum > piece_of_jmk)
				return -2;
//			pBuff=p+12;
			copy_from_user(&pBuff,p+12,4);
			//udelay(5);
			aos_debug("pBuff=%x\n",(unsigned int)pBuff);
			if (!pBuff)
				return -3;
			copy_from_user(JMK_WRITE_BUFF(jnum)+4,pBuff,len);
			udelay(5);
//			PrintBuff(JMK_WRITE_BUFF(jnum),len_d*4);
			memcpy(JMK_WRITE_BUFF(jnum),&len_d,4);
			break;
		case P9054_DMA_READ_CMD:
		case P9054_DMA_READ_INT_CMD:
		case P9054_DMA_READ_QUERY_CMD:
			copy_from_user(&len,p+4,4);
			//udelay(5);
		case P9054_TARGET_READ_CMD:
		case P9054_RESET_DEVICE:
			copy_from_user(&jnum,p+8,4);
			//udelay(5);
			jnum-=1;
			if (jnum < 0 || jnum >= piece_of_jmk)
				return -2;
//			copy_from_user(&pBuff,p+12,4);
//			pBuff=p+12;
			copy_from_user(&pBuff,p+12,4);
			udelay(50);
			aos_debug("pBuff=%x\n",(unsigned int)pBuff);
			if (!pBuff)
				return -3;
			break;
		case P9054_GET_DEVICE_NUM:
			return piece_of_jmk;
		case P9054_RESET_ALL_DEVICE:
			for (jnum=0;jnum < piece_of_jmk;jnum++)
			{
				jmk_reg_writel(jnum,P9054_CNTRL,0x780f767e);
//				udelay(5);
				for (tmpi=0;tmpi<10;tmpi++)
					tmpi=jmk_reg_readl(jnum,P9054_CNTRL);
				jmk_reg_writel(jnum,P9054_CNTRL,0x380f767e);
				udelay(50);
			}
			return 0;
		default:break;
	}

	switch (cmd)
	{
		case P9054_TARGET_WRITE_CMD:
/*			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT18|BIT3);//|BIT11);|BIT9|BIT8
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT7|BIT8|BIT10|BIT17|BIT1|BIT0);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		//使能加密卡 MEM 0 （可写/译码）
			jmk_readl(jnum,P9054_LAS0BA);				//等待（浪费时间）

			//设置加密卡 MEM 0 为 32 位宽
			//使能 BTERM# 输入信号
			//Prefetch（预取）计数使能
			//预取技术设置为 8 （长字）
			//扩展 ROM 空间局部总线宽度设置为 32 位
			//扩展 ROM 空间 TA#/READY# 输入信号使能
			//加密卡 MEM 0 BURST（突发）使能
			//PCI Target 重试延迟时钟数设置为4（×8）个时钟
			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//			aos_debug("1 P9054_L2PDBELL=%08x\n",tmpi);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);		//门铃寄存器清零
//			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//			aos_debug("2 P9054_L2PDBELL=%08x\n",tmpi);

			//命令信息写入加密卡的 MEM 0			
			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
			//命令信息的长度写入邮箱寄存器0,引发加密卡内终端处理
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk
*/
			return -1;
			break;
		case P9054_TARGET_READ_CMD:
/*//			aos_debug("jnum=%d\n",jnum);
			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);//tmpi=jmk_reg_readl(0,P9054_INTCSR);
//			if (tmpi & BIT13)			//if pci doorbell is active
			if (tmpi)
			{
				aos_debug("P9054_L2PDBELL=0x%08x\n",tmpi);
				jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);	//门铃寄存器清零，避免其它不良影响

				//获得返回的长字个数
				ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
				len=len_d*4;					//长字个数转换为字节个数
				if (len > 16*1024 || len <0)
					return -1;//len=256;
				copy_to_user(p+4,&len,4);			//字节个数返回到结构中
				ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);	//读加密卡内存中的返回结果
				copy_to_user(pBuff,JMK_READ_BUFF(jnum),len);	//返回结果拷贝到用户空间
				return len;					//函数返回值是返回结果的字节长度
			}
			else
			{
//				aos_debug("P9054_L2PDBELL=0x%08x\n",tmpi);
//				ReadMem(jnum,0,JMK_READ_BUFF(jnum),16);
//				PrintBuff(JMK_READ_BUFF(jnum),16);
				return -4;
			}
*/			return -1;
			break;
		case P9054_DMA_WRITE_CMD:
		case P9054_DMA_WRITE_INT_CMD:
			aos_debug("Enter DMA WRITE CMD\n");
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT0|BIT1|BIT7|BIT8|BIT10|BIT16|BIT17);
			udelay(2);
			if (cmd == P9054_DMA_WRITE_INT_CMD)
			{
				tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
				aos_debug("P9054_INTCSR=%08x\n",tmpi);
				tmpi|=(BIT18|BIT9|BIT3);//|BIT11);|BIT8
				jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
//				udelay(5);
			}

			jmk_reg_writel(jnum,P9054_MARBR,0x200000);
			jmk_reg_writel(jnum,P9054_DMAPADR,JMK_WRITE_PHYS_ADDR(jnum));
			jmk_reg_writel(jnum,P9054_DMALADR,0xC0000000);
			jmk_reg_writel(jnum,P9054_DMASIZ,len);	//?????????
//			udelay(5);
			jmk_reg_writel(jnum,P9054_DMADPR,BIT0);
//			udelay(5);
			jmk_reg_writel(jnum,P9054_DMACSR,BIT0|BIT1);

			if (cmd == P9054_DMA_WRITE_INT_CMD)
				interruptible_sleep_on(&jmk_conf_array[jnum].int_wait);
			else
			{
			}
			aos_debug("Leave DMA WRITE CMD\n");
			break;
		case P9054_DMA_READ_CMD:
		case P9054_DMA_READ_INT_CMD:
			aos_debug("Enter DMA READ CMD\n");
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT0|BIT1|BIT7|BIT8|BIT10|BIT11|BIT17);//BIT16|
/*			if (((jmk_conf_array[jnum].jmk_dma_times+1) % 2) == 0)
			{
				udelay(80);
				if (jmk_conf_array[jnum].jmk_dma_times & 0x8000)
				{
					aos_debug("jmk_dma_times = 0x%x",jmk_conf_array[jnum].jmk_dma_times);
					jmk_conf_array[jnum].jmk_dma_times &= 0xffff0000;
				}
			}*/

			jmk_reg_writel(jnum,P9054_MARBR,0x200000);
			jmk_reg_writel(jnum,P9054_DMAPADR,JMK_READ_PHYS_ADDR(jnum));
			jmk_reg_writel(jnum,P9054_DMALADR,0xC0000000);
			jmk_reg_writel(jnum,P9054_DMASIZ,len);	//?????????
			memset(JMK_READ_BUFF(jnum),0,TEMP_CMP_BUFF_LENGTH);
			jmk_reg_writel(jnum,P9054_DMADPR,BIT0|BIT3);
			jmk_reg_writel(jnum,P9054_DMACSR,BIT0|BIT1);

//			interruptible_sleep_on(&jmk_conf_array[jnum].int_wait);

#if 1
			if (cmd == P9054_DMA_READ_INT_CMD)
			{
				RetSleep=sleep_on_timeout(&jmk_conf_array[jnum].int_wait,10);
//				RetSleep=interruptible_sleep_on_timeout(&jmk_conf_array[jnum].int_wait,10);
//				interruptible_sleep_on(&jmk_conf_array[jnum].int_wait);
//				if (RetSleep)
//				{
					aos_debug("interruptible_sleep_on_timeout ret=0x%x\n",RetSleep);
//				}
				aos_debug("dma waked up!\n");
aos_debug("DMA Size rest=0x%x\n",jmk_reg_readl(jnum,P9054_DMASIZ));
				if (!memcmp(JMK_READ_BUFF(jnum),TempCmpBuf,TEMP_CMP_BUFF_LENGTH))
					return -1;
			}
			else
			{
			}
#endif

/*			len = len/4;
			pbyte=JMK_READ_BUFF(jnum);
			for (i=0;i<len;i++)
				pbyte[i]=pbyte[i*4];*/

			copy_to_user(pBuff,JMK_READ_BUFF(jnum),len);
			copy_to_user(p+4,&len,4);

			jmk_conf_array[jnum].jmk_dma_times++;
			aos_debug("Leave DMA READ CMD\n");
			return len;
			break;
		case P9054_DMA_READ_QUERY_CMD:
			aos_debug("Enter DMA READ CMD\n");
			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
			jmk_reg_writel(jnum,P9054_DMAMODE,BIT0|BIT1|BIT7|BIT8|BIT11|BIT17);//BIT16|

			jmk_reg_writel(jnum,P9054_MARBR,0x200000);
			jmk_reg_writel(jnum,P9054_DMAPADR,JMK_READ_PHYS_ADDR(jnum));
			jmk_reg_writel(jnum,P9054_DMALADR,0xC0000000);
			jmk_reg_writel(jnum,P9054_DMASIZ,len);	//?????????
			memset(JMK_READ_BUFF(jnum),0,TEMP_CMP_BUFF_LENGTH);
			jmk_reg_writel(jnum,P9054_DMADPR,BIT0|BIT3);
			jmk_reg_writel(jnum,P9054_DMACSR,BIT0|BIT1);

//			interruptible_sleep_on(&jmk_conf_array[jnum].int_wait);

			tmpi=0;
			timeout = jiffies + WAIT_DELAYTIME;
			for (i=0x300000;1;)
			{
				//if (time_after(jiffies,  (long)timeout))//i <= 0)
				if (jiffies > timeout)
				{
					aos_debug("dma query time out error\n");
					break;
				}
				tmpi=jmk_reg_readl(jnum,P9054_DMACSR);
//				tmpi=jmk_reg_readl(jnum,P9054_DMACSR);
//				tmpi=jmk_reg_readl(jnum,P9054_DMACSR);
				if (tmpi & BIT4)
				{
					aos_debug("dma query ok\n");
					break;
				}
				i--;
			}

			if (tmpi == 0)// || i <= 0)
				return -2;

/*			len = len/4;
			pbyte=JMK_READ_BUFF(jnum);
			for (i=0;i<len;i++)
				pbyte[i]=pbyte[i*4];*/

			copy_to_user(pBuff,JMK_READ_BUFF(jnum),len);
			copy_to_user(p+4,&len,4);

			jmk_conf_array[jnum].jmk_dma_times++;
			aos_debug("Leave DMA READ CMD\n");
			return len;
			break;
		case P9054_TARGET_INT_CMD:
			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT18|BIT9|BIT8|BIT3);//|BIT11);
//			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
//			jmk_reg_writel(jnum,P9054_DMAMODE,BIT7|BIT8|BIT10|BIT17|BIT1|BIT0);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		//使能加密卡 MEM 0 （可写/译码）
			jmk_readl(jnum,P9054_LAS0BA);				//等待（浪费时间）

			//设置加密卡 MEM 0 为 32 位宽
			//使能 BTERM# 输入信号
			//Prefetch（预取）计数使能
			//预取技术设置为 8 （长字）
			//扩展 ROM 空间局部总线宽度设置为 32 位
			//扩展 ROM 空间 TA#/READY# 输入信号使能
			//加密卡 MEM 0 BURST（突发）使能
			//PCI Target 重试延迟时钟数设置为4（×8）个时钟
			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//			aos_debug("1 P9054_L2PDBELL=%08x\n",tmpi);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);		//门铃寄存器清零
//			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//			aos_debug("2 P9054_L2PDBELL=%08x\n",tmpi);

			//命令信息写入加密卡的 MEM 0			
			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
			//命令信息的长度写入邮箱寄存器0,引发加密卡内终端处理
			udelay(100);
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk

			sleep_on_timeout(&jmk_conf_array[jnum].int_wait,800);
//			interruptible_sleep_on_timeout(&jmk_conf_array[jnum].int_wait,800);
//			interruptible_sleep_on(&jmk_conf_array[jnum].int_wait);

			//获得返回的长字个数
			udelay(5);
			ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
			len=len_d*4;					//长字个数转换为字节个数
			aos_debug("read len=0x%08x",len);
			if (len > 16*1024 || len <0)
				return -1;//len=256;
			copy_to_user(p+4,&len,4);			//字节个数返回到结构中
			ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);	//读加密卡内存中的返回结果
			copy_to_user(pBuff,JMK_READ_BUFF(jnum),len);	//返回结果拷贝到用户空间

			return len;					//函数返回值是返回结果的字节长度
			break;
		case P9054_TARGET_QUERY_CMD:
			tmpi=jmk_reg_readl(jnum,P9054_INTCSR);
			aos_debug("P9054_INTCSR=%08x\n",tmpi);
			tmpi|=(BIT3);//|BIT11);
			tmpi&=(~(BIT8|BIT9|BIT18));
//			jmk_reg_writel(jnum,P9054_DMATHR,0x0f);
//			jmk_reg_writel(jnum,P9054_DMAMODE,BIT7|BIT8|BIT10|BIT17|BIT1|BIT0);
			jmk_reg_writel(jnum,P9054_INTCSR,tmpi);
		
			jmk_reg_writel(jnum,P9054_LAS0BA,0x40000001);		//使能加密卡 MEM 0 （可写/译码）
			jmk_readl(jnum,P9054_LAS0BA);				//等待（浪费时间）

			//设置加密卡 MEM 0 为 32 位宽
			//使能 BTERM# 输入信号
			//Prefetch（预取）计数使能
			//预取技术设置为 8 （长字）
			//扩展 ROM 空间局部总线宽度设置为 32 位
			//扩展 ROM 空间 TA#/READY# 输入信号使能
			//加密卡 MEM 0 BURST（突发）使能
			//PCI Target 重试延迟时钟数设置为4（×8）个时钟
			jmk_reg_writel(jnum,P9054_LBRD0,0x41434483);

			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//			aos_debug("1 P9054_L2PDBELL=%08x\n",tmpi);
			jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);		//门铃寄存器清零
//			tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//			aos_debug("2 P9054_L2PDBELL=%08x\n",tmpi);

			//命令信息写入加密卡的 MEM 0			
			WriteMem(jnum,0,JMK_WRITE_BUFF(jnum),(len_d+1)*4);		//write mem
			//命令信息的长度写入邮箱寄存器0,引发加密卡内终端处理
			udelay(5);
			jmk_reg_writel(jnum,P9054_MBOX0,len_d);			//write reg to start jmk

			tmpi=0;
			timeout = jiffies + 4000;//WAIT_DELAYTIME;
			for (i=0x300000;1;)
			{
				//if (time_after(jiffies,  (unsigned long)timeout))//i <= 0)
				if (jiffies >  timeout)
				{
					aos_debug("query time out error\n");
					break;
				}
				tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//				tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
//				tmpi=jmk_reg_readl(jnum,P9054_L2PDBELL);
				if (tmpi)
				{
					aos_debug("query ok\n");
					len_d=tmpi;
					jmk_reg_writel(jnum,P9054_L2PDBELL,tmpi);
					break;
				}
				i--;
			}

			if (tmpi == 0)// || i <= 0)
				return -2;

			//获得返回的长字个数
			ReadMem(jnum,0,&len_d,4);//len_d=tmpi;//jmk_reg_readl(0,P9054_L2PDBELL);
			len=len_d*4;					//长字个数转换为字节个数

			aos_debug("tmpi=%d , len_d=0x%08x\n",tmpi,len_d);

			if (len > 16*1024 || len <0)// || len_d != tmpi)
				return -1;//len=256;
			copy_to_user(p+4,&len,4);			//字节个数返回到结构中
			ReadMem(jnum,4,JMK_READ_BUFF(jnum),len);	//读加密卡内存中的返回结果
			copy_to_user(pBuff,JMK_READ_BUFF(jnum),len);	//返回结果拷贝到用户空间

			aos_trace_hex("JMK_READ_BUFF",(char*)(JMK_READ_BUFF(jnum)),len);

			return len;					//函数返回值是返回结果的字节长度
			break;
		case P9054_RESET_DEVICE:
			jmk_reg_writel(jnum,P9054_CNTRL,0x780f767e);
			for (tmpi=0;tmpi<10;tmpi++)
				tmpi=jmk_reg_readl(jnum,P9054_CNTRL);
			udelay(5);
			jmk_reg_writel(jnum,P9054_CNTRL,0x380f767e);
			break;
		default:break;
	}
	
	return 0; 
} 

/****************************************************************/ 
/*   	Function Name: jmk_read()				*/ 
/****************************************************************/ 
static ssize_t jmk_read(struct file *file, char *buf,size_t count,loff_t *ppos) 
{
  return -2;
} 

/****************************************************************/ 
/*   	Function Name: jmk_interrupt()				*/ 
/****************************************************************/ 
//static void jmk_interrupt(int irq,void* dev_id,struct pt_regs* regs)
static irqreturn_t jmk_interrupt(int irq,void* dev_id,struct pt_regs* regs)
{
	int i,nbyte;

	int card_no=0;

	spin_lock_irq(&irq_spinlock);
	card_no=-1;
	for(i=0;i<piece_of_jmk;i++){
		nbyte=jmk_reg_readl(i, P9054_INTCSR);
		if((nbyte & BIT13) || (nbyte & BIT21)){
			card_no=i;
			break;
		}
	}
	if(card_no==-1){
		spin_unlock_irq(&irq_spinlock);
		return IRQ_NONE;
	}
	spin_unlock_irq(&irq_spinlock);
	//aos_debug("enter jmk_interrupt irq=%d\n", irq);

	if (nbyte & BIT13)
	{	//门铃寄存器清零，即清除了中断
		spin_lock_irq(&irq_spinlock);
		nbyte=jmk_reg_readl(card_no,P9054_L2PDBELL);
		//aos_debug("P9054_L2PDBELL=%08x\n",nbyte);
		
		for (;1;)
		{
			jmk_reg_writel(card_no,P9054_L2PDBELL,nbyte);
//			udelay(5);
//				nbyte=jmk_reg_readl(i,P9054_L2PDBELL);
//				aos_debug("P9054_L2PDBELL=%08x\n",nbyte);
			nbyte=jmk_reg_readl(card_no,P9054_INTCSR);
			if (!(nbyte & BIT13))
				break;
		}
		spin_unlock_irq(&irq_spinlock);
//			udelay(5);
		//aos_debug("will wake up in jmk %d, in line %d\n", card_no, __LINE__);
		wake_up(&jmk_conf_array[card_no].int_wait);
		//wake_up_interruptible(&jmk_conf_array[card_no].int_wait);
	}
	if (nbyte & BIT21)
	{
		//aos_debug("Enter JMK=%d DMA irq\n",card_no);
		spin_lock_irq(&irq_spinlock);
		for (;1;)
		{
			jmk_reg_writel(card_no,P9054_DMACSR,BIT3);
			nbyte=jmk_reg_readl(i,P9054_INTCSR);
			if (!(nbyte & BIT21))
				break;
		}
		spin_unlock_irq(&irq_spinlock);
		//udelay(2);
		//aos_debug("will wake up in jmk %d, in line %d\n", card_no, __LINE__);
		wake_up(&jmk_conf_array[card_no].int_wait);
		//wake_up_interruptible(&jmk_conf_array[i].int_wait);
		//aos_debug("Leave JMK=%d DMA irq\n",card_no);
	}
	return IRQ_HANDLED;

}

/*
static struct file_operations jmk_fops = { 
	NULL,			// jmk_lseek
	jmk_read,		// jmk_read	
	jmk_write,		// jmk_write
	NULL,			// jmk_readdir
	NULL,			// jmk_select
	jmk_ioctl,		// jmk_ioctl
	NULL,			// jmk_mmap
	jmk_open,		// jmk_open
	NULL,			// flush
	jmk_release,		// jmk_release
}; 
*/

static struct file_operations jmk_fops =
{
	.owner		=THIS_MODULE,
	.read		=jmk_read,
	.write		=jmk_write,
	.ioctl		=jmk_ioctl,
	.mmap		=NULL,
	.open		=jmk_open,
	.release	=jmk_release,
};

/****************************************************************
****int initmodule
****************************************************************/

int init_jmk_module(void)
{

  int result=register_chrdev(WJJMK_MAJOR,WJJMK_NAME,&jmk_fops);
  if (result < 0)
  {
    aos_debug("Failed when register char device \"JiaMiKa\" !\n");
    result=-1;
  }
  else
  {
    aos_debug("Successed when register char device \"JiaMiKa\" !\n");
    result=0;
  }
  jmk_init();

//---------------- 2003-08-24 ---------------------
  memset(TempCmpBuf,0,TEMP_CMP_BUFF_LENGTH);
//---------------- 2003-08-24 ---------------------

  return result;

}


void cleanup_jmk_module(void)
{
  unregister_chrdev(WJJMK_MAJOR,WJJMK_NAME);
  jmk_free();
  aos_debug("Successed when exit char device \"JiaMiKa\" !\n");
}
/*
int ApplyJmk(int CardNo)
{
	int i,iret;

	iret=-1;
	do
	{
		for (i=0;i<piece_of_jmk;i++)
		{
			if (jmk_conf_array[i].jmk_busy == 0)
			{
				jmk_conf_array[i].jmk_busy=1;
				iret=i+1;
				break;
			}
		}
	} while (iret <= 0);

	return iret;
}

void FreeJmk()
{
}*/
EXPORT_SYMBOL(jmk_init);
EXPORT_SYMBOL(jmk_free);
EXPORT_SYMBOL(jmk_kernel_ioctl);
EXPORT_SYMBOL(jmk_kernel);
/***************************************************************/
/*				End of the program		*/
/****************************************************************/



