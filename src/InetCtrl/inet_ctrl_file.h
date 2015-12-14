///////////////////////////////////////////////////////////////////////
// Author: Zhang QuanLin
// Creation Time: 07/28/2006
// Description:
//
// Change History
// 07/28/2006 inetctrlKernelFile.h created
// 08/02/2006 is changed
///////////////////////////////////////////////////////////////////////
#ifndef inetctrl_kernel_file_h
#define inetctrl_kernel_file_h

#ifdef __KERNEL__

#include <linux/kernel.h> 
#include <linux/module.h> 
#include <linux/fs.h> 
#include <linux/sched.h> 
#include <linux/file.h> 
#include <asm/processor.h> 
#include <asm/uaccess.h> 

#define EOF (-1) 
#define SEEK_SET 0 
#define SEEK_CUR 1 
#define SEEK_END 2 

#define LINEMAXLENTH	1024

#define KFILE struct file 

// 
// Function Prototypes 
// 

//KFILE *inetctrl_fopen(const char *KFILEname, int flags, int mode); 

#define INETCTRLFILE_USERS_INC( us ) us++;
#define INETCTRLFILE_USERS_DEC( us )	us--;

extern KFILE *inetctrl_fopen(const char *filename, const char *mode );
extern void inetctrl_fclose(KFILE *filp); 
extern int  inetctrl_fseek(KFILE *filp, int offset, int whence); 
extern int  inetctrl_fread(char *buf, int len, KFILE *filp); 
extern int  inetctrl_fgetc(KFILE *filp); 
extern char *inetctrl_fgets(char *str, int size, KFILE *filp); 
extern int  inetctrl_fwrite(char *buf, int len, KFILE *filp); 
extern int  inetctrl_fputc(int ch, KFILE *filp); 
extern int  inetctrl_fputs(char *str, KFILE *filp); 
extern int  inetctrl_fprintf(KFILE *filp, const char *fmt, ...); 
extern void inetctrl_fscanf(KFILE *filp, const char *fmt, ...);
extern int  inetctrl_file_printk(const char *logfile, const char *fmt, ...); 

#endif

#endif



