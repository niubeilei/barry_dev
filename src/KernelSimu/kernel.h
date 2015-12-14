////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: kernel.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_kernel_h
#define Omn_aos_KernelSimu_kernel_h

#ifdef __KERNEL__
#include <linux/kernel.h>
#else

// Chen Ding, 02/03/2007
#include <linux/stddef.h>

//typedef unsigned int u32;
#include <string.h>
extern const char linux_banner[];

#define NIPQUAD(addr) \
		((unsigned char *)&addr)[0], \
	((unsigned char *)&addr)[1], \
	((unsigned char *)&addr)[2], \
	((unsigned char *)&addr)[3]

// #define INT_MAX		((int)(~0U>>1))
// #define INT_MAX		2147483647
// #define INT_MIN		(-INT_MAX - 1)
// #define UINT_MAX	(~0U)
// #define LONG_MAX	((long)(~0UL>>1))
// #define LONG_MAX	2147483647
// #define LONG_MIN	(-LONG_MAX - 1)
// #define ULONG_MAX	(~0UL)

#define STACK_MAGIC	0xdeadbeef

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

#define	KERN_EMERG	"<0>"	/* system is unusable			*/
#define	KERN_ALERT	"<1>"	/* action must be taken immediately	*/
#define	KERN_CRIT	"<2>"	/* critical conditions			*/
#define	KERN_ERR	"<3>"	/* error conditions			*/
#define	KERN_WARNING	"<4>"	/* warning conditions			*/
#define	KERN_NOTICE	"<5>"	/* normal but significant condition	*/
#define	KERN_INFO	"<6>"	/* informational			*/
#define	KERN_DEBUG	"<7>"	/* debug-level messages			*/

extern int console_printk[];

#define console_loglevel (console_printk[0])
#define default_message_loglevel (console_printk[1])
#define minimum_console_loglevel (console_printk[2])
#define default_console_loglevel (console_printk[3])

#undef offsetof
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)1)->MEMBER - 1)
// #define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#if __cplusplus < 201103L
#define container_of(ptr, type, member) ({			\
        const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#define container_of(ptr, type, member) ({			\
        const decltype( ((type *)0)->member ) *__mptr = (ptr);	\
        (type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#define printk printf

#endif

#endif

