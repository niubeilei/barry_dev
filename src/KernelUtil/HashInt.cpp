////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashInt.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "KernelUtil/HashInt.h"

#include "aos/aosKernelAlarm.h"
#include "aosUtil/Memory.h"

// 
// This function will add one block of idle entries to the list.
// Each block contains eAosHashIntAllocSize number of entries.
// All the idle entries are added to the idle list "idleList".
// 
// Parameters:
//	struct AosHashInt *self:
//		The table to which a block of idle entries is to be added.
//
// Errors:
//	1. eAosAlarm_HashTableBlockOverflow:
//		Too many blocks (maximum blocks: eAosHashIntBlockArraySize.
//
//	2. eAosAlarm_HashTableOverflow:
//		Too many entries in the table. 
//	
//	3. eAosAlarm_HashTableBlockAllocErr:
//		Failed to allocate memory for the block.
//
static int AosHashInt_addIdleEntries(struct AosHashInt *self)
{
	struct AosHashInt_entry *idle;
	int i;

	// 
	// Check whether it can add more blocks
	//
	if (self->numBlocks >= eAosHashIntBlockArraySize)
	{
		self->errorBlockOverflow++;
		return aosAlarm(eAosAlarm_HashTableBlockOverflow);
	}

	if (self->totalEntries > self->maxEntries)
	{
		self->errorTableOverflow++;
		return aosAlarm(eAosAlarm_HashTableOverflow);
	}

	// 
	// Allocate the memory
	//
	idle = (struct AosHashInt_entry *)
		aos_malloc(sizeof(struct AosHashInt_entry) * eAosHashIntAllocSize);
	if (!idle)
	{
		self->errorBlockAllocError++;
		return aosAlarm(eAosAlarm_HashTableBlockAllocErr);
	}

	// 
	// Create the idle list
	//
	for (i=0; i<eAosHashIntAllocSize; i++)
	{
		idle[i].next = self->idleList;
		idle[i].key = 0;
		idle[i].value = 0;
		self->idleList = &idle[i];
	}

	// 
	// Add the idle to the block list
	//
	self->totalEntries += eAosHashIntAllocSize;
	self->blocks[(self->numBlocks)++] = idle;
	return 0;
}


//
// This function returns an idle entry. It is called when one needs to 
// add an entry into the table. If the idle list "idleList" is not empty,
// it returns the first entry in the list. Otherwise, it will call 
// AosHashInt_addIdleEntries(...) to create more idle entries. 
// 
// Parameters:
//	"struct AosHashInt *self"
//		The table itself.
//
// Errors:
//	The only errors are returned by calling AosHashInt_addIdleEntries(...).
//  Refer to that function for more information.
//
static int
AosHashInt_getEntry(struct AosHashInt *self, 
					struct AosHashInt_entry **entry)
{
	int ret;

	(*entry) = 0;
	if (!self->idleList)
	{
		if ((ret = AosHashInt_addIdleEntries(self)))
		{
			// 
			// Failed to add more idle entries. This is probably because
			// the list grows too big.
			//
			self->errorGetEntryFail++;
			return ret;
		}
	}

	*entry = self->idleList;
	self->idleList = (*entry)->next;

	return 0;
}


// 
// This function frees all blocks and sets "idleList" to null. 
//
// Parameters:
//	"self"
//		The table.
//
// Errors:
//	No errors.
//
static int AosHashInt_releaseAllBlocks(struct AosHashInt *self)
{
	int i;
	for (i=0; i<self->numBlocks; i++)
	{
		aos_free(self->blocks[i]);
		self->blocks[i] = 0;
	}

	self->numBlocks = 0;
	self->idleList = 0;
	self->numIdleEntries = 0;
	return 0;
}


// 
// This is the constructor of "AosHashInt". It is important that one should
// always call this constructor to create an instance of "struct AosHashInt". 
// Do not create the table directly. 
//
// Parameters:
//	1. "self"
//		This is the pointer to the pointer of the hash table to be created. 
//		This function will malloc memory for the table and returns the 
//		member through this parameter.
//	2. "tableSize"
//		This is the number of buckets the table should have. The minimum 
//		is 255 and maximum 0xffff. 
//	3. "maxEntries"
//		This is the maximum number of entries the table is allowed to create.
//		When the table reaches this number, no new entries can be added to the
//		table. "maxEntries == 0" means there is no limit on the maximum.
// 
// Errors:
//	1. eAosAlarm_FailedToAllocTable:
//		Failed to allocate memory for the table. "self" is set to 0.
//
//	2. eAosAlarm_FailedToAllocBuckets:
//		Failed to allocate memory for the buckets. (*self)->buckets
//		is set to 0.
//	3. eAosAlarm_HashTableBlockOverflow:
//		Too many blocks (maximum blocks: eAosHashIntBlockArraySize.
//
//	4. eAosAlarm_HashTableOverflow:
//		Too many entries in the table. 
//	
//	5. eAosAlarm_HashTableBlockAllocErr:
//		Failed to allocate memory for the block.
//
int AosHashInt_constructor(struct AosHashInt **self, 
						   unsigned int tableSize,
						   unsigned int maxEntries)
{
	unsigned int i;

	// 
	// Allocate the memory
	//
	*self = (struct AosHashInt *)aos_malloc(sizeof(struct AosHashInt));
	if (!(*self))
	{
		return aosAlarmStr(eAosAlarm_FailedToAllocTable, "Failed to allocate the table");
	}

	// 
	// Determine table size
	//
	unsigned int size = 0xff;		// minimum 256
	while (tableSize > size && size < eAosMaxHashIntTableSize)
	{
		size = (size << 1) + 1;
	}

	// 
	// Init block array
	//
	(*self)->numBlocks = 0;
	for (i=0; i<eAosHashIntBlockArraySize; i++)
	{
		(*self)->blocks[i] = 0;
	}

	// 
	// Initialize other member data
	//
	(*self)->tableSize = size;
	(*self)->maxEntries = maxEntries;
	(*self)->totalEntries = 0;
	(*self)->errorBlockOverflow = 0;
	(*self)->errorTableOverflow = 0;
	(*self)->errorBlockAllocError = 0;
	(*self)->errorGetEntryFail = 0;

	// 
	// Allocate the memory for the buckets
	//
	(*self)->buckets = (struct AosHashInt_entry **)
		aos_malloc(sizeof(struct AosHashInt_entry *) * size);
	if (!(*self)->buckets)
	{
		// 
		// This is a serious problem. 
		// 
		return aosAlarmStr(eAosAlarm_FailedToAllocBuckets, "Failed to allocate buckets");
	}

	for (i=0; i<size; i++)
	{
		(*self)->buckets[i] = 0;
	}

	// 
	// Initialize the idle list. 
	//
	(*self)->idleList = 0;
	return AosHashInt_addIdleEntries(*self);
}


// 
// This function adds an entry to the table "self". The entry's key is
// "key" and the entry's value is "value". This function does not care
// the value of "value". If the entry is already in the table: if 'replace'
// is non-zero, it will replace the entry with the new value. Otherwise, 
// it is an error. 
// 
// Parameters:
//	"self"
//		The table 
//
//  "key"
//		The key of the entry to be added
//
//  "value"
//		The value of the entry
//
//	"replace"
//		Indicates whether to replace (non-zero) or not to replace (zero)
//		the entry is the entry is already in the list.
//
// Errors:
//	1. eAosRc_EntryAlreadyInList:
//		The entry is already in the list.
//
//	2. eAosRc_MemErr:
//		Failed to allocate memory or
//
//	3. eAosAlarm_HashTableBlockOverflow:
//		Too many blocks (maximum blocks: eAosHashIntBlockArraySize).
//
//	4. eAosAlarm_HashTableOverflow:
//		Too many entries in the table. 
//	
//	5. eAosAlarm_HashTableBlockAllocErr:
//		Failed to allocate memory for the block.
//
int AosHashInt_add(struct AosHashInt *self, 
				   unsigned int key, 
				   void *value, 
				   const char replace)
{
	//
	// 1. Calculate the hash key. 
	// 2. Check whether the entry is there
	// 3. Store it.
	//

	//
	// 1. Calculate the hash key
	//
	int ret;
	unsigned int hk = (key & self->tableSize);

	//
	// 2. Check whether it is in the bucket already
	//
	struct AosHashInt_entry *bucket = self->buckets[hk];
	while (bucket)
	{
		if (bucket->key == key)
		{
			//
			// The entry is already there.
			//
			if (replace)
			{
				// 
				// Override it
				//
				bucket->value = value;
				return 0;
			}
			else
			{
				// 
				// This is an error.
				//
				return eAosRc_EntryAlreadyInList;
			}
		}

		bucket = bucket->next;
	}

	//
	// Not found. Add it
	//
	ret = AosHashInt_getEntry(self, &bucket);
	aosKernelAssert(!ret, ret);

	bucket->key = key;
	bucket->value = value;
	bucket->next = self->buckets[hk];
	self->buckets[hk] = bucket;
	return 0;
}


// 
// This function retrieves an entry from the table. If the entry is found,
// it returns 0. Otherwise, it returns an error.
//
// Parameters:
//	"self"
//		The table.
//
//	"key"
//		The key to be retrieved.
//
//	"value"
//		The value to be retrieved. If the entry is found, the value is returned
//		through this parameter.
//
//	"removeFlag"
//		If it is non-zero and if the entry is found, the entry will be removed.
//
// Errors:
//	1. eAosRc_EntryNotFound:
//		The entry is not found. "value" is set to 0.
//
int AosHashInt_get(struct AosHashInt *self,
		const unsigned int key, 
		void **value, 
		const int removeFlag)
{
	//
	// If the entry is found, it returns the entry.
	// If 'removeFlag' is true, it will remove the entry, too.
	// If not found, it returns the default.
	//

	//
	// 1. Calculate the hash key
	//
	unsigned int hk = (key & self->tableSize);

	//
	// Retrieve it
	//
	struct AosHashInt_entry *bucket = self->buckets[hk];
	while (bucket)
	{
		if (bucket->key == key)
		{
			//
			// Found it. Check whether need to remove it.
			//
			*value = bucket->value;
			if (removeFlag)
			{
				//
				// Need to remove it
				//
				AosHashInt_returnEntry(self, bucket);
			}

			return 0;
		}

		bucket = bucket->next;
	}

	*value = 0;
	return eAosRc_EntryNotFound;
}


// 
// This function removes all entries from the list and returns all the 
// blocks allocated. 
//
// Parameters:
//	"self"
//		The table.
//
// Errors:
//	None
//
int AosHashInt_reset(struct AosHashInt *self)
{
	unsigned int i;

	for (i=0; i<self->tableSize; i++)
	{
		if (self->buckets[i])
		{
			struct AosHashInt_entry *entry = self->buckets[i];
			while (entry)
			{
				entry->next = self->idleList;
				entry->key = 0;
				entry->value = 0;
				self->idleList = entry;

				entry = entry->next;
			}
		}
	}

	AosHashInt_releaseAllBlocks(self);

	return 0;
}


// 
// This function releases all the resources a hash table allocated, 
// including itself. One should always call this function to 
// deallocate a hash table instead of calling the free function
// directly.
//
int AosHashInt_destructor(struct AosHashInt *self)
{
	// 
	// Not implemented yet
	//
	AosHashInt_releaseAllBlocks(self);
	aos_free(self);
	return 0;
}
