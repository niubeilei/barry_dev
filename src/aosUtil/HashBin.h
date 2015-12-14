////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashBin.h
// Description:
//   
//
// Modification History:
// Copied back from HashBinRaw.h
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_aosUtil_HashBin_h
#define Aos_aosUtil_HashBin_h

#include "aosUtil/Types.h"
#include "aosUtil/List.h"

#include "aosUtil/Tracer.h"
#include "aosUtil/AosPlatform.h"

enum
{
    eAosMaxHashBinTableSize = 0xffff,
	eAosHashBin_MaxKeyLen = 32,
};

struct AosHashBin_entry
{
	struct AosHashBin_entry *next;
	struct AosHashBin_entry *prev;

	struct AosHashBin_entry *hash_next;
	char *		 key1[eAosHashBin_MaxKeyLen];
	u32 		 key_len;
	void 		*value;
};

struct AosSlab;
typedef struct AosHashBin
{
	struct AosHashBin_entry **	buckets;
	struct aos_list_head		list;
	struct AosSlab			   *slab;
	u32							tableSize;
	u32							numEntries;
	u32							maxSize;

	u32							errorBlockOverflow;
	u32							errorTableOverflow;
	u32							errorBlockAllocError;
	u32							errorGetEntryFail;
} AosHashBin_t;


static inline u16 AosHashBin_hash_func(char *key, u32 key_len)
{
	// 
	// 
	//u16 hashkey  = 1234321;
	u16 hashkey  = 34321;

	while (key_len)
	{
		hashkey += (key_len==1)?key[0] : key[key_len-1] + (key[key_len-2] << 6);
		key_len--;
	}

	return hashkey;
}


extern int AosHashBin_constructor(
			struct AosHashBin **self, 
			u32 tableSize);
extern int AosHashBin_destructor(struct AosHashBin *self);
extern int AosHashBin_add(AosHashBin_t *self, 
							char *key, 
							u32 key_len, 
							void *value, 
							const char replace);
extern int AosHashBin_get(AosHashBin_t *self,
							char *key, 
							u32 key_len,
							void **value, 
							const int removeFlag);
extern int AosHashBin_reset(AosHashBin_t *self);
extern int AosHashBin_dump(AosHashBin_t *self, char *key, u32 key_len);

static inline int AosHashBin_remove(AosHashBin_t *self, char *key, u32 key_len)
{
	void *value;
	return AosHashBin_get(self, key, key_len, &value, 1);
}


static inline int AosHashBin_returnEntry(AosHashBin_t *self, 
										 struct AosHashBin_entry *bucket)
{
	return 0;
}


#endif
