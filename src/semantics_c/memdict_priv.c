////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 12/12/2007: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "semantics_c/memdict_priv.h"

#include "alarm_c/alarm.h"
#include "aosUtil/ReturnCode.h"
#include "porting_c/mutex.h"
#include <string.h>

// 
// Temporarily we use an array to implement the binary tree. 
//
#define AOS_MEMBLOCK_ARRAY_SIZE 3000
static struct aos_memblock_t *sg_memblocks[AOS_MEMBLOCK_ARRAY_SIZE];
static u32 sg_memblocks_size = 0;
aos_lock_t sgLock;

int aos_memdict_init()
{
	memset(sg_memblocks, 0, 4 * AOS_MEMBLOCK_ARRAY_SIZE);
	aos_init_lock(&sgLock);
	return 0;
}


int aos_memdict_is_valid_addr(u32 addr)
{
	u32 i;
	struct aos_memblock_t *block = 0;

	aos_lock(&sgLock);
	for (i=0; i<sg_memblocks_size; i++)
	{
		if (sg_memblocks[i]->start_addr >= (u32)addr &&
			(u32)addr < sg_memblocks[i]->start_addr + sg_memblocks[i]->length)
		{
			// 
			// The address is inside the block. Need to check whether 
			// it is an addressable address for that block.
			//
			block = sg_memblocks[i];
			aos_unlock(&sgLock);		
			return aos_memblock_is_addressable(block, addr);
		}
	}

	// 
	// Did not find the address.
	//
	return 0;
}


int aos_memdict_add(struct aos_memblock_t *block)
{
	// 
	// It adds a block into the memory dictionary.
	//
	aos_assert_r(block, -eAosRc_NullPointer);
	if (sg_memblocks_size >= AOS_MEMBLOCK_ARRAY_SIZE)
	{
		aos_alarm("Too many memory blocks. Maximum allowed: %d", 
			AOS_MEMBLOCK_ARRAY_SIZE);
		return -eAosRc_TooManyEntries;
	}

	aos_lock(&sgLock);
	sg_memblocks[sg_memblocks_size++] = block;
	aos_unlock(&sgLock);
	return 0;
}


int aos_memdict_remove(u32 start)
{
	int i;
	struct aos_memblock_t *block = 0;

	aos_lock(&sgLock);
	for (i=0; i<sg_memblocks_size; i++)
	{
		if (sg_memblocks[i]->start_addr == start)
		{
			block = sg_memblocks[i];
			if (i < sg_memblocks_size - 1)
			{
				sg_memblocks[i] = sg_memblocks[sg_memblocks_size-1];
			}
			sg_memblocks_size--;
			aos_unlock(&sgLock);
			return 0;
		}
	}

	aos_unlock(&sgLock);
	aos_alarm("Memory block not found: %d", start);
	return -eAosRc_EntryNotFound;
}


int aos_memblock_is_addressable(struct aos_memblock_t *block, 
								u32 addr)
{
	// 
	// Not implemented yet
	//
	return 0;
}


