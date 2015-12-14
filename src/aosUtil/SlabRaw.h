////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SlabRaw.h
// Description:
//  This is a special slab used by the memory leak catch
//  implementation. Normal users should not use this.
//
//  This file is constructed by copying Slab.h and do the
//  following:
//  1. Global replacement of "AosSlab" with "AosSlabRaw"
//  2. Change "aos_aosUtil_slab_h" to "aos_aosUtil_slabRaw_h"   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aosUtil_slabRaw_h
#define aos_aosUtil_slabRaw_h

#include "aosUtil/Mutex.h"
#include "aosUtil/Types.h"

enum 
{
	eAosTArraySlab = 10,
	eAosCharPTreeSlab
};

typedef struct AosSlabRaw 
{
	int *			head;
	u32				size;
	u32				max_blocks;
	u32				total_allocated;
	u32				idle_blocks;
	u32				min_idle_blocks;
	char			name[20];
	aos_mutex_t		lock;
} AosSlabRaw_t;

#define AOS_SLAB_NEXT	0
#define AOS_SLAB_PREV	1
#define AOS_SLAB_MINIDLEBLOCKS 10

extern int AosSlabRaw_constructor(
					const char *name, 
					const u32 size,
					const u32 maxBlocks, 
					struct AosSlabRaw **theSlab); 
extern int   AosSlabRaw_destructor(struct AosSlabRaw *self);
extern void *AosSlabRaw_get(struct AosSlabRaw *self);
extern int	 AosSlabRaw_release(struct AosSlabRaw *self, void *ptr);
extern int   AosSlabRaw_purge(struct AosSlabRaw *self);
extern int   AosSlabRaw_check(struct AosSlabRaw *self);


#endif
