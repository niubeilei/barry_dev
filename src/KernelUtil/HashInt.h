////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashInt.h
// Description:
// 	This is the same as OmnHashInt except that it does not use
//  template. This can be used in kernel.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_HashIntC_h
#define Omn_Util_HashIntC_h

// #include "Util/ValList.h"
// #include "Util/HashTable.h"
// #include "Util/HashUtil.h"
// #include "Util/HashMgr.h"


enum
{
	eAosMaxHashIntTableSize = 0xffff, 
	eAosHashIntAllocSize = 1000,
	eAosHashIntBlockArraySize = 200
};

struct AosHashInt_entry
{
	struct AosHashInt_entry *next;
	unsigned int key;
	void *value;
};


struct AosHashInt
{
	struct AosHashInt_entry **	buckets;
	struct AosHashInt_entry *	idleList;
	int							numIdleEntries;
	void *						blocks[eAosHashIntBlockArraySize];
	int							numBlocks;
	unsigned int				tableSize;
	unsigned int				maxEntries;
	unsigned int				totalEntries;

	unsigned int				errorBlockOverflow;
	unsigned int				errorTableOverflow;
	unsigned int				errorBlockAllocError;
	unsigned int				errorGetEntryFail;
};


extern int AosHashInt_constructor(struct AosHashInt **self, 
						   unsigned int tableSize,
						   unsigned int maxEntries);
extern int AosHashInt_destructor(struct AosHashInt *self);
extern int AosHashInt_add(struct AosHashInt *self, unsigned int key, void *value, const char replace);
extern int AosHashInt_get(
		struct AosHashInt *self,
		const unsigned int key, 
		void **value, 
		const int removeFlag);
extern int AosHashInt_reset(struct AosHashInt *self);

static inline int AosHashInt_remove(struct AosHashInt *self, const unsigned int key)
{
	void *value;
	return AosHashInt_get(self, key, &value, 1);
}


static inline int AosHashInt_returnEntry(struct AosHashInt *self, 
										 struct AosHashInt_entry *bucket)
{
	bucket->next = self->idleList;
	bucket->key = 0;
	bucket->value = 0;
	self->idleList = bucket;
	return 0;
}


#endif
