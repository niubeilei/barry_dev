////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Slab.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef aos_aosUtil_slab_h
#define aos_aosUtil_slab_h

#include "aosUtil/Mutex.h"
#include "aosUtil/Types.h"

enum 
{
	eAosTArraySlab = 10,
	eAosCharPTreeSlab
};

typedef struct AosSlab 
{
	int *			head;
	u32				size;
	u32				max_blocks;
	u32				total_allocated;
	u32				idle_blocks;
	u32				min_idle_blocks;
	char			name[20];
	aos_mutex_t		lock;
} AosSlab_t;

#define AOS_SLAB_NEXT	0
#define AOS_SLAB_PREV	1
#define AOS_SLAB_MINIDLEBLOCKS 10

extern int AosSlab_constructor(
					const char *name, 
					const u32 size,
					const u32 maxBlocks, 
					struct AosSlab **theSlab); 
extern int   AosSlab_destructor(struct AosSlab *self);
extern void *AosSlab_get(struct AosSlab *self);
extern int	 AosSlab_release(struct AosSlab *self, void *ptr);
extern int   AosSlab_purge(struct AosSlab *self);
extern int   AosSlab_check(struct AosSlab *self);


#endif
