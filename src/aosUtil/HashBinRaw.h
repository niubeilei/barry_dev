////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashBinRaw.h
// Description:
//  This is a special hash table used by the memory leak catch
//  implementation. Normal users should not use this table.
//
//  This file is created by copying "HashBin.h" and do the
//  following changes:
//  1. Global replacement of "AosHashBin" with "AosHashBinRaw"
//  2. Global replacement of "AosSlab" with "AosSlabRaw"
//  3. Change "Aos_aosUtil_HashBin_h" to "Aos_aosUtil_HashBinRaw_h"   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_aosUtil_HashBinRaw_h
#define Aos_aosUtil_HashBinRaw_h

#include "aosUtil/Types.h"
#include "aosUtil/List.h"

#include "aosUtil/Tracer.h"
#include "aosUtil/AosPlatform.h"

enum
{
    eAosMaxHashBinTableSize = 0xffff,
	eAosHashBinRaw_MaxKeyLen = 32,
};

struct AosHashBinRaw_entry
{
	struct AosHashBinRaw_entry *next;
	struct AosHashBinRaw_entry *prev;

	struct AosHashBinRaw_entry *hash_next;
	char *		 key1[eAosHashBinRaw_MaxKeyLen];
	u32 		 key_len;
	void 		*value;
};

struct AosSlabRaw;
typedef struct AosHashBinRaw
{
	struct AosHashBinRaw_entry **	buckets;
	struct aos_list_head		list;
	struct AosSlabRaw			   *slab;
	u32							tableSize;

	u32							errorBlockOverflow;
	u32							errorTableOverflow;
	u32							errorBlockAllocError;
	u32							errorGetEntryFail;
} AosHashBinRaw_t;


static inline u16 AosHashBinRaw_hash_func(char *key, u32 key_len)
{
	// 
	// 
	//u16 hashkey  = 1234321;
	u16 hashkey  = 34321;

	while (key_len)
	{
// aos_printk("key: %u, %d\n", (unsigned int)key[key_len-1], (unsigned int)key_len);

		hashkey += (key_len==1)?key[0] : key[key_len-1] + (key[key_len-2] << 6);
		key_len--;
	}

// aos_printk("hashkey: %u\n", (unsigned int)hashkey);
	return hashkey;
}


extern int AosHashBinRaw_constructor(struct AosHashBinRaw **self, u32 tableSize);
extern int AosHashBinRaw_destructor(struct AosHashBinRaw *self);
extern int AosHashBinRaw_add(AosHashBinRaw_t *self, 
							char *key, 
							u32 key_len, 
							void *value, 
							const char replace);
extern int AosHashBinRaw_get(AosHashBinRaw_t *self,
							char *key, 
							u32 key_len,
							void **value, 
							const int removeFlag);
extern int AosHashBinRaw_reset(AosHashBinRaw_t *self);
extern int AosHashBinRaw_dump(AosHashBinRaw_t *self, char *key, u32 key_len);

static inline int AosHashBinRaw_remove(AosHashBinRaw_t *self, char *key, u32 key_len)
{
	void *value;
	return AosHashBinRaw_get(self, key, key_len, &value, 1);
}


static inline int AosHashBinRaw_returnEntry(AosHashBinRaw_t *self, 
										 struct AosHashBinRaw_entry *bucket)
{
	return 0;
}


#endif
