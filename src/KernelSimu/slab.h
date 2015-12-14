////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: slab.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_aos_KernelSimu_slab_h
#define Omn_aos_KernelSimu_slab_h

#ifdef AOS_KERNEL_SIMULATE

#include <stdlib.h>

typedef struct kmem_cache_s kmem_cache_t;


extern void * __cache_alloc (kmem_cache_t *cachep, int flags);

inline void * kmem_cache_alloc (kmem_cache_t *cachep, int flags)
{
	return __cache_alloc(cachep, flags);
}


inline void kmem_cache_free (kmem_cache_t *cachep, void *objp)
{
	free(objp);
}



#else
#include <linux/slab.h>
#endif

#endif

