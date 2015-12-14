///////////////////////////////////////////////////////////////////////
// Author: Zhang QuanLin
// Creation Time: 07/28/2006
// Description:
//
// Change History
// 07/28/2006 aosKernelFile.h created
// 08/02/2006 is changed
///////////////////////////////////////////////////////////////////////
#ifndef aos_kernel_file_h
#define aos_kernel_file_h

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

//KFILE *aos_fopen(const char *KFILEname, int flags, int mode); 

#define AOSFILE_USERS_INC( us ) us++;
#define AOSFILE_USERS_DEC( us )	us--;
 
extern KFILE *aos_fopen(const char *filename, const char *mode );
extern void aos_fclose(KFILE *filp); 
extern int  aos_fseek(KFILE *filp, int offset, int whence); 
extern int  aos_fread(char *buf, int len, KFILE *filp); 
extern int  aos_fgetc(KFILE *filp); 
extern char *aos_fgets(char *str, int size, KFILE *filp); 
extern int  aos_fwrite(char *buf, int len, KFILE *filp); 
extern int  aos_fputc(int ch, KFILE *filp); 
extern int  aos_fputs(char *str, KFILE *filp); 
extern int  aos_fprintf(KFILE *filp, const char *fmt, ...); 
extern void aos_fscanf(KFILE *filp, const char *fmt, ...);
extern int  aos_file_printk(const char *logfile, const char *fmt, ...); 
#endif
