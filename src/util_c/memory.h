////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Memory.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_AosUtil_Memory_h
#define Omn_AosUtil_Memory_h

#include "util_c/types.h"
#include <string.h>

// #include "KernelSimu/string.h"

enum
{
    eAosMemory_FileNameLen = 20,

    eAosMemoryStatus_NotSet = 0,
    eAosMemoryStatus_Set = 1,
    eAosMemoryStatus_InitFailed = 2,
    eAosMemory_NumCounts = 3,
};  

// extern int AosMemory_Status;
extern int aos_memcmp_equal(void *mem, char value, const int size);
extern int AosMemory_init(char *);
extern int AosMemory_setStatus(void);
extern void * AosMemory_malloc(char *file, int line, u32 size,int gfp);
extern int AosMemory_free(void *ptr);

#ifdef CONFIG_AOS_TEST
struct aosKernelApiParms;
extern int AosMemory_checkMemory(char *ptr);
extern int AosMemory_initCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen);
extern int AosMemory_setStatusCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen);
extern int AosMemory_showCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen);
extern int AosMemory_getRsltCli(char *data,
                      unsigned int *length,
                      struct aosKernelApiParms *parms,
                      char *errmsg,
                      const int errlen);
#endif

#ifdef __KERNEL__
// 
// The kernel portion
//
#include <linux/slab.h>
#include "aosUtil/Tracer.h"


#define aos_malloc_atomic(size) aos_malloc_general(size,GFP_ATOMIC)
//#define aos_malloc(size) aos_malloc_general(size,GFP_KERNEL)


static inline void *aos_zmalloc_func(u32 size,int gfp)
{
	void *m = kmalloc(size, gfp);
	if (!m) return m;

	memset(m, 0, size);
	return m;
}

static inline void *AosMemory_zmalloc(char *file, int line, u32 size,int gfp)
{
	void *m = AosMemory_malloc(file, line, size, gfp);
	if (m)
	{
		memset(m, 0, size);
	}
	return m;
}


#ifndef aos_zmalloc

#ifdef CONFIG_AOS_MEMORY_CATCHER
#define aos_zmalloc(size) AosMemory_zmalloc(__FILE__, __LINE__, (size),GFP_KERNEL)
#else
#define aos_zmalloc(size) aos_zmalloc_func(size,GFP_KERNEL)
#endif

#endif


// #define aos_malloc(size) kmalloc((size), GFP_KERNEL)

#ifndef aos_malloc_general

#ifdef CONFIG_AOS_MEMORY_CATCHER
#define aos_malloc_general(size,gfp) AosMemory_malloc(__FILE__, __LINE__, size,gfp)
#define aos_malloc(size) AosMemory_malloc(__FILE__, __LINE__, size,GFP_KERNEL)
#else
#define aos_malloc_general(size,gfp) kmalloc((size), gfp)
#define aos_malloc(size) kmalloc((size),GFP_KERNEL)
#endif

#endif

#ifndef aos_free 

#ifdef CONFIG_AOS_MEMORY_CATCHER
#define aos_free(x) AosMemory_free(x)
#else
#define aos_free(x) kfree((x))
#endif

#endif


#ifndef aos_malloc_raw
#define aos_malloc_raw(size,gfp) kmalloc((size), gfp)
#endif

#ifndef aos_free_raw
#define aos_free_raw(x) kfree(x)
#endif


#else

#ifdef OMN_PLATFORM_MICROSOFT
#include <malloc.h>

#ifndef aos_malloc
#define aos_malloc(size) malloc((size))
#endif

#ifndef aos_malloc_atomic
#define aos_malloc_atomic(size) malloc(size)
#endif

#ifndef aos_free
#define aos_free(x) free((x));
#endif

#ifndef aos_malloc_raw
#define aos_malloc_raw(size, gfp) malloc((size))
#endif

#ifndef aos_free_raw
#define aos_free_raw(x) free((x));
#endif

#elif OMN_PLATFORM_UNIX
#include <malloc.h>

static inline void *aos_zmalloc_func(u32 size)
{
	void *m = malloc(size);
	if (!m) return m;

	memset(m, 0, size);
	return m;
}

#ifndef aos_malloc
// #define aos_malloc(size) malloc((size))
#define aos_malloc(size) AosMemory_malloc(__FILE__, __LINE__, (size), 0)
#endif

#ifndef aos_zmalloc
#define aos_zmalloc(size) aos_zmalloc_func(size)
// #define aos_zmalloc(size) AosMemory_zmalloc(__FILE__, __LINE__, (size))
#endif

#ifndef aos_malloc_raw
#define aos_malloc_raw(size, gfp) malloc((size))
#endif

#ifndef aos_free 
#define aos_free(x) free((x));
// #define aos_free(x) AosMemory_free(x)
#endif

#ifndef aos_free_raw
#define aos_free_raw(x) free((x));
#endif

#ifndef aos_malloc_atomic
#define aos_malloc_atomic(size) malloc(size)
#endif

#endif

#endif

#endif

