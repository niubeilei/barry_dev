////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashBinRaw.cpp
// Description:
//  This is a special hash table used by the memory leak catch
//  implementation. Normal users should not use this table.
//
//  This file is constructed by copying HashBin.cpp and do the
//  following:
//  1. Global replacement of "AosHashBin" with "AosHashBinRaw".
//  2. Global replacement of "aos_malloc" with "aos_malloc_raw"
//  3. Global replacement of "aos_free" with "aos_free_raw"
//  4. Global replacement of "AosSlab" with "AosSlabRaw"
//  5. Change "Slab.h" to "SlabRaw.h"
//  6. Change "HashBin.h" to "HashBinRaw.h"   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/HashBinRaw.h"

#include "alarm_c/alarm.h"
#include "aosUtil/SlabRaw.h"
#include "aosUtil/Memory.h"
#include "aosUtil/ReturnCode.h"
#include "KernelSimu/string.h"
#include <stdio.h>

// 
// This is the constructor of "AosHashBinRaw". It is important that one should
// always call this constructor to create an instance of "struct AosHashBinRaw". 
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
// 
// Errors:
//	1. eAosAlarm_FailedToAllocTable:
//		Failed to allocate memory for the table. "self" is set to 0.
//
//	2. eAosAlarm_FailedToAllocBuckets:
//		Failed to allocate memory for the buckets. (*self)->buckets
//		is set to 0.
//	3. eAosAlarm_HashTableBlockOverflow:
//		Too many blocks (maximum blocks: eAosHashBinRawBlockArraySize.
//
//	4. eAosAlarm_HashTableOverflow:
//		Too many entries in the table. 
//	
//	5. eAosAlarm_HashTableBlockAllocErr:
//		Failed to allocate memory for the block.
//
int AosHashBinRaw_constructor(struct AosHashBinRaw **self, u32 tableSize)
{
	u32 i;
	int ret;
	unsigned int size = 0xff;		// minimum 256

	// 
	// Allocate the memory
	//
	*self = (struct AosHashBinRaw *)aos_malloc_raw(sizeof(struct AosHashBinRaw),GFP_KERNEL);
	if (!(*self))
	{
		return aos_alarm("Failed to allocate the table");
	}

	ret = AosSlabRaw_constructor("", sizeof(struct AosHashBinRaw_entry), 
			0, &(*self)->slab);
	if (!(*self)->slab)
	{
		aos_free_raw(*self);
		return aos_alarm("Failed to allocate memory");
	}

	// 
	// Determine table size
	//
	while (tableSize > size && size < eAosMaxHashBinTableSize)
	{
		size = (size << 1) + 1;
	}

	// 
	// Initialize other member data
	//
	(*self)->tableSize = size;
	(*self)->errorBlockOverflow = 0;
	(*self)->errorTableOverflow = 0;
	(*self)->errorBlockAllocError = 0;
	(*self)->errorGetEntryFail = 0;

	// 
	// Allocate the memory for the buckets
	//
	(*self)->buckets = (struct AosHashBinRaw_entry **)
		aos_malloc_raw(sizeof(struct AosHashBinRaw_entry *) * (size + 1),GFP_KERNEL);
	if (!(*self)->buckets)
	{
		// 
		// This is a serious problem. 
		// 
		return aos_alarm("Failed to allocate buckets: %u", (unsigned int)size);
	}

	for (i=0; i<=size; i++)
	{
		(*self)->buckets[i] = 0;
	}

	// 
	// Initialize the list
	//
	AOS_INIT_LIST_HEAD(&(*self)->list);
	return 0;
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
//		Too many blocks (maximum blocks: eAosHashBinRawBlockArraySize).
//
//	4. eAosAlarm_HashTableOverflow:
//		Too many entries in the table. 
//	
//	5. eAosAlarm_HashTableBlockAllocErr:
//		Failed to allocate memory for the block.
//
int AosHashBinRaw_add(struct AosHashBinRaw *self, 
				   char *key, 
				   u32 key_len,
				   void *value, 
				   const char replace)
{
	//
	// 1. Calculate the hash key. 
	// 2. Check whether the entry is there
	// 3. Store it.
	//
	u16 hk;
	struct AosHashBinRaw_entry *entry;
	if (key_len > eAosHashBinRaw_MaxKeyLen)
	{
		aos_alarm("Key too long: %d\n", key_len);
		return -1;
	}

	//
	// 1. Calculate the hash key
	//
	hk = AosHashBinRaw_hash_func(key, key_len) & self->tableSize;


	//
	// 2. Check whether it is in the bucket already
	//
	entry= self->buckets[hk];
//aos_printk("%s:%d: %u\n", __FILE__, __LINE__, (unsigned int)hk);
	while (entry)
	{
/*char *tmp1 = (char *)&key;
char *tmp2 = (char *)&entry->key;
aos_printk("%s:%d: %u %u %u %u %u %u %u %u %u %u\n", 
	__FILE__, __LINE__, 
	(unsigned int)key_len, (unsigned int)entry->key_len,
	tmp1[0], tmp2[0], 
	tmp1[1], tmp2[1], 
	tmp1[2], tmp2[2], 
	tmp1[3], tmp2[3]); 
*/
		if (entry->key_len == key_len && memcmp(entry->key1, key, key_len) == 0)
		{
			//
			// The entry is already there.
			//
//aos_printk("%s;%d found\n",__FILE__, __LINE__);
			if (replace)
			{
				// 
				// Override it
				//
				entry->value = value;
				return 0;
			}
			else
			{
				// 
				// This is an error.
				//
				return -eAosRc_EntryAlreadyInList;
			}
		}

		entry = entry->hash_next;
	}

	//
	// Not found. Add it
	//
	entry = (struct AosHashBinRaw_entry *)AosSlabRaw_get(self->slab);
	aos_assert_r(entry, -1);

	memcpy(entry->key1, key, key_len);
	entry->key_len = key_len;
	entry->value = value;
	entry->hash_next = self->buckets[hk];

	entry->prev = 0;
	entry->next = 0;
	self->buckets[hk] = entry;

	aos_list_add_tail((struct aos_list_head*)entry, &self->list);

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
// Return Values:
//	0: 
//		The entry is found
//
//	-eAosRc_EntryNotFound:
//		The entry is not found
//
// Errors:
//	1. eAosRc_EntryNotFound:
//		The entry is not found. "value" is set to 0.
//
int AosHashBinRaw_get(struct AosHashBinRaw *self,
		char *key, 
		u32 key_len,
		void **value, 
		const int removeFlag)
{
	//
	// If the entry is found, it returns the entry.
	// If 'removeFlag' is true, it will remove the entry, too.
	// If not found, it returns the default.
	//
	u16 hk;
	struct AosHashBinRaw_entry *entry, *prev;
	if (key_len > eAosHashBinRaw_MaxKeyLen)
	{
		aos_alarm("Key too long: %d\n", key_len);
		return -1;
	}

	//
	// 1. Calculate the hash key
	//
	hk = AosHashBinRaw_hash_func(key, key_len) & self->tableSize;
	entry= self->buckets[hk];
	prev = 0;

	while (entry)
	{
		if (entry->key_len == key_len && memcmp(entry->key1, key, key_len) == 0)
		{
			//
			// Found it. Check whether need to remove it.
			//
			*value = entry->value;
			if (removeFlag)
			{
				//
				// Need to remove it
				//
				if (prev)
				{
					prev->hash_next = entry->hash_next;
				}
				else
				{
					self->buckets[hk] = entry->hash_next;
				}

//aos_printk("Entry deleted: %x\n", (unsigned int)entry->value);

				aos_list_del((struct aos_list_head*)entry);

				AosSlabRaw_release(self->slab, entry);				
			}

			return 0;
		}

		prev = entry;
		entry = entry->hash_next;
	}

	*value = 0;
	return -eAosRc_EntryNotFound;
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
int AosHashBinRaw_reset(struct AosHashBinRaw *self)
{
	unsigned int i;

	for (i=0; i<self->tableSize; i++)
	{
		if (self->buckets[i])
		{
			struct AosHashBinRaw_entry *next;
			struct AosHashBinRaw_entry *entry = self->buckets[i];
			while (entry)
			{
				next = entry->hash_next;
				AosSlabRaw_release(self->slab, entry);
				entry = next;
			}
		}
	}

	AOS_INIT_LIST_HEAD(&self->list);	

	return 0;
}


// 
// This function releases all the resources a hash table allocated, 
// including itself. One should always call this function to 
// deallocate a hash table instead of calling the free function
// directly.
//
int AosHashBinRaw_destructor(struct AosHashBinRaw *self)
{
	AosHashBinRaw_reset(self);
	aos_free_raw(self);
	return 0;
}


int AosHashBinRaw_dump(
			struct AosHashBinRaw *self, 
			char *key, 
			u32 key_len)
{
	u16 hk = AosHashBinRaw_hash_func(key, key_len) & self->tableSize;
	struct AosHashBinRaw_entry *entry= self->buckets[hk];
	int index=0;
	unsigned int i;

	while (entry)
	{
		char *kkk = (char *)entry->key1;
		aos_printk("    char key%d[] = {", index);
		for (i=0; i<entry->key_len-1; i++)
		{
			aos_printk("0x%02x, ", ((unsigned int)kkk[i]) & 0xff);
		}

		aos_printk("0x%02x};\n", ((unsigned int)kkk[entry->key_len-1]) & 0xff);

		aos_printk("    ret = AosHashBinRaw_add(hash, (void *)key%d, %u, (void *)value, 0);\n", index, (unsigned int)entry->key_len);

		index++;
		entry = entry->hash_next;
	}

	return 0;
}

