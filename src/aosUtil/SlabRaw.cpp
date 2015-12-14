////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SlabRaw.cpp
// Description:
//  This is a special slab used by the memory leak catch
//  implementation. Normal users should not use this.
//
//  This file is constructed by copying Slab.cpp and do the
//  following:
//  1. Global replacement of "AosSlab" with "AosSlabRaw"
//  2. Global replacement of "aos_malloc" with "aos_malloc_raw"
//  3. Global replacement of "aos_free" with "aos_free_raw"
//  4. Change "Slab.h" to "SlabRaw.h"   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "aosUtil/SlabRaw.h"

#include "alarm_c/alarm.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Mutex.h"
#include "KernelSimu/aosKernelDebug.h"
#include "KernelSimu/gfp.h"
#include "KernelSimu/core.h"
#include "aos/aosKernelAlarm.h"
#include "aos/aosReturnCode.h"

#include "KernelSimu/string.h"
#include "KernelSimu/gfp.h"


int AosSlabRaw_constructor(const char *name, 
					const u32 size,
					const u32 maxBlocks, 
					struct AosSlabRaw **slab)
{
	*slab = (struct AosSlabRaw *)
		aos_malloc_raw(sizeof(struct AosSlabRaw),GFP_KERNEL);
	if (!(*slab))
	{
		aos_alarm("Failed to allocate slab: %s\n", name);
		return -1;
	}

	strcpy((*slab)->name, name);
	(*slab)->size = size;
	(*slab)->max_blocks = maxBlocks;
	(*slab)->head = 0;
	(*slab)->idle_blocks = 0;
	(*slab)->min_idle_blocks = 10;
	(*slab)->total_allocated = 0;
	AOS_INIT_MUTEX((*slab)->lock);
	return 0;
}


int AosSlabRaw_destructor(struct AosSlabRaw *self)
{
	aos_lock(self->lock);
	while (self->head)
	{
		int *next = (int*)self->head[AOS_SLAB_NEXT];
		aos_free_raw(self->head);
		self->head = next;
	}
	aos_free_raw(self);
	aos_unlock(self->lock);
	return 0;
}


void *
AosSlabRaw_get(struct AosSlabRaw *self)
{
    //
    // When needs a block from this memory pool, call this function to get
    // one. If there are idle blocks, it returns directly. Otherwise,
    // it news a block and returns it. It is important to return the
    // block when not needed. Otherwise, it is a memory leak.
    //
	// If it runs out of memory, it returns 0 and an alarm is raised.
	//
	void *ptr;

    //
    // Check whether the size is correct
    //
    aos_lock(self->lock);

    //
    // If the idle list is not empty, get one from the list.
    //
    if (!self->head)
    {
		//
		// It is empty. Need to allocate more. Check whether it is possible
		//
		void *mem;
		// if (self->total_allocated >= self->max_blocks)
		// {
		// 	aos_alarm(eAosMD_Platform, eAosAlarm_MemErr, 
		// 		("Failed to allocate memory: %d, %d, %s", 
		// 		self->total_allocated, self->max_blocks, self->name));
		// 	aos_unlock(self->lock);
		// 	return 0;
		// }

		mem = aos_malloc_raw(self->size,GFP_KERNEL);

		if (!mem)
		{
			aos_alarm("Alarm");
		}

		self->total_allocated++;
		aos_unlock(self->lock);
		return mem;
    }

    //
    // It is not empty.
    //
    ptr = (void *)self->head;
    self->head = (int*)self->head[AOS_SLAB_NEXT];
    self->idle_blocks--;
	aos_unlock(self->lock);
	return ptr;
}


int		
AosSlabRaw_release(struct AosSlabRaw *self, void *ptr)
{
	//
	// If there are too many, delete it
	//
	// if (mNumElem >= max_blocks)
	//{
	//	//
	//	// There are too many. Not inserted into the list.
	//	//
	//	total_allocated--;
	//	mLock->unlock();
	//	//_operator OmnDelete(ptr);
	//	OmnDelete(ptr);
	//	return;
	//}

	//
	// Add it to the list.
	//
	int *block = (int*)ptr;

	aos_lock(self->lock);
	block[AOS_SLAB_PREV] = 0;
	block[AOS_SLAB_NEXT] = (unsigned long)self->head;
	if (self->head)
	{
		self->head[AOS_SLAB_PREV] = (unsigned long)block;
	}

	self->head = (int *)ptr;
	self->idle_blocks++;
	aos_unlock(self->lock);
	return 0;
}
 

int AosSlabRaw_check(struct AosSlabRaw *self)
{
	// 
	// This is to make sure the slab is maintained correctly so far.
	//
	if ((!self->head && self->idle_blocks) || 
		(self->head && !self->idle_blocks))
	{
		aos_alarm("Program error: %d", self->idle_blocks);
		return -1;
	}

	if (self->idle_blocks)
	{
		// 
		// Since there are idle blocks, this number should be the same as
		// the number of entries in head.
		//
		u32 num = 0;
		int *ptr = self->head;
		while (ptr)
		{
			ptr = (int *)ptr[AOS_SLAB_NEXT];
			num++;
		}
		
		if (num != self->idle_blocks)
		{
			return aos_alarm("Alarm");
		}
	}

	if (self->idle_blocks > self->max_blocks)
	{
		return aos_alarm("Alarm");
	}

	if (self->total_allocated > self->max_blocks)
	{
		return aos_alarm("Alarm");
	}

	return 0;
}


int
AosSlabRaw_purge(struct AosSlabRaw *self)
{
	//
	// This function may be called to release some memory, if any.
	// This may be due to the system failed to allocate memory.
	// If the idle list is not empty, it releases half of the blocks.
	// It reutrns the total number of bytes released. If no bytes 
	// are released, return 0.
	//
	int blocksToRelease;
	int bytesReleased = 0;
	int *next;
	int i;

	aos_lock(self->lock);
	if (self->idle_blocks <= self->min_idle_blocks)
	{
		//
		// Will not purge
		//
		aos_unlock(self->lock);
		return 0;
	}

	blocksToRelease = self->idle_blocks >> 1;	// Equivalent to mNumElem/2
	for (i=0; i<blocksToRelease; i++)
	{
		if (!self->head)
		{
			//
			// This is a program error. Should never happen.
			//
			aos_unlock(self->lock);
			aos_alarm("Alarm");
			return bytesReleased;
		}

		next = (int *)self->head[AOS_SLAB_NEXT];
		aos_free_raw(self->head);
		self->head = next;
		bytesReleased += self->size;
		self->idle_blocks--;
	}

	aos_unlock(self->lock);
	return bytesReleased;
}

#endif
