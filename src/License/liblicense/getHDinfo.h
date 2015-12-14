////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: getHDinfo.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef GET_HD_INFO_HEAD_H
#define GET_HD_INFO_HEAD_H

struct dmi_header{
	unsigned char type;
	unsigned char length;
	unsigned short handle;
};
//typedef unsigned char unsigned char

#define FLAG_VERSION            (1<<0)
#define FLAG_HELP               (1<<1)
#define FLAG_DUMP               (1<<2)
#define FLAG_QUIET              (1<<3)

#define VERSION "2.8"

#ifdef __ia64__
#define USE_EFI
#endif /* __ia64__ */

/* Use mmap or not */
#ifndef __BEOS__
#define USE_MMAP
#endif

#ifdef ALIGNMENT_WORKAROUND
#define HANDLE(x) WORD((unsigned char *)&(x->handle))
#else
#define HANDLE(x) x->handle
#endif

#ifdef USE_MMAP
#include <sys/mman.h>
#ifndef MAP_FAILED
#define MAP_FAILED ((void *) -1)
#endif /* !MAP_FAILED */
#endif /* USE MMAP */

/*
#define strlen(x) strlen(x)
#define printf(x...) printf(x)
#define malloc(x) malloc(x)
#define free(x) free(x)
#define memcpy(x, y, z) memcpy(x, y, z)
*/

#ifdef ALIGNMENT_WORKAROUND
#       ifdef BIGENDIAN
#       define WORD(x) (unsigned short)((x)[1]+((x)[0]<<8))
#       define DWORD(x) (unsigned int)((x)[3]+((x)[2]<<8)+((x)[1]<<16)+((x)[0]<<24))
#       else /* BIGENDIAN */
#       define WORD(x) (unsigned short)((x)[0]+((x)[1]<<8))
#       define DWORD(x) (unsigned int)((x)[0]+((x)[1]<<8)+((x)[2]<<16)+((x)[3]<<24))
#       endif /* BIGENDIAN */
#else /* ALIGNMENT_WORKAROUND */
#define WORD(x) (unsigned short)(*(const unsigned short *)(x))
#define DWORD(x) (unsigned int )(*(const unsigned int *)(x))
#endif /* ALIGNMENT_WORKAROUND */

#ifdef __BEOS__
	#define DEFAULT_MEM_DEV "/dev/misc/mem"
#else
	#define DEFAULT_MEM_DEV "/dev/mem"
#endif
struct string_keyword{
	const char *keyword;
	unsigned char type;
	unsigned char offset;
	const char *(*lookup)(unsigned char);
	void (*print)(unsigned char *);
};

struct opt
{
	const char *devmem;
	unsigned int flags;
	unsigned char *type;
	const struct string_keyword *string;
};
//extern struct opt opt;

//#define P(x...) printf(x)
#define P(x...)



void *mem_chunk(size_t base, size_t len, const char *devmem);
int checksum(const unsigned char *buf, size_t len);
void dmi_dump(struct dmi_header *h, const char *prefix);
char *dmi_string(struct dmi_header *dm, unsigned char s);
void dmi_decode(unsigned char *data, unsigned short ver);
void dmi_table(unsigned int base, unsigned short len, unsigned short num, unsigned short ver, const char *devmem);
int smbios_decode(unsigned char *buf, const char *devmem);
int get_main_sn(void);
int get_hardware_info(unsigned char *serial_number, unsigned char *mac);
int get_mac_address(void);

#endif
