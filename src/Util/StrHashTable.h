////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrHashTable.h
// Description:
//	Each entry has three values (HashedKey, EntryKey, Value).
//  HashedKey is the one to be hashed. EntryKey is unique
//  to all entries. Value is the value to be saved in the 
//  table. Normally, Value should be a smart pointer. 
//
//  The hash function calculates hash value by adding up to 
//  x number of bytes:
//
//	int hashvalue = byte[0] + byte[1] + ... = byte[x]
//
//  and then take the last 12 bits from hashvalue to obtain the 
//  final hash key. This means that every hash type of this 
//  type will have 4096 buckets.
//
//	The table maximum size should not be bigger than 65000. 
//  Otherwise it limits to that.
//
//  To create an instance of this class:
//
//		OmnStrHashTable<C, 1024> table;
//		OmnStrHashTable<C, 4096> table(25);   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#ifndef Snt_Util_StrHashTable_h
#define Snt_Util_StrHashTable_h

#include "Util/ValList.h"
#include "Util/String.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"


template <class C, unsigned int tableSize>
class OmnStrHashTable : public OmnHashTable 
{
public:
	enum {eMaxTableSize = 0xffff};

private:
	struct HashEntry
	{
		OmnString	hashedKey;
		OmnString	entryKey;
		C 			value;

		HashEntry() {}
		HashEntry(const OmnString &k, const OmnString e, const C &v)
			:
		hashedKey(k),
		entryKey(e),
		value(v)
		{
		}
	};

	int						mBytesToAdd;			
	OmnVList<HashEntry>	mBuckets[tableSize];
	unsigned int			mTableSize;
	unsigned int			mBitMask;

public:
	OmnStrHashTable(const int bytesToAdd = 15)
		:
	mBytesToAdd(bytesToAdd),
	mTableSize(tableSize)
	{
		OmnHashMgr::addHashTable(this);

		//
		// Need to calculate how many bits for the table. For example,
		// if the table size is 1024, we need to use the last 10 bits
		// when calculating hash keys. 
		//
		if (mTableSize >= eMaxTableSize)
		{
			mBitMask = eMaxTableSize;
		}
		else
		{
			mBitMask = 0;
			unsigned int size = mTableSize;
			while (size != 0)
			{
				//
				// Shift 'size' one bit right, shift mBitMask one bit left, 
				// and then sets mBitMask last bit to 1.
				//
				size = size >> 1;
				mBitMask = mBitMask << 1;
				mBitMask += 1;
			}
		}
	}


	~OmnStrHashTable()
	{
		OmnHashMgr::removeHashTable(this);
	}


	bool add(const OmnString &hashKey, 
			 const OmnString &entryKey, 
			 const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//
		// hashKey is an empty string, return false. 
		// Otherwise, it should not generate errors.
		//

		int length = hashKey.length();
		if (length <= 0)
		{
			//
			// This is an error.
			//
			return false;
		}


		//
		// 1. Calculate the hash key
		//
		unsigned int hk = 0;
		int index = 0;
		const char *data = hashKey.data();
		while (index < length && index < mBytesToAdd)
		{
			hk += data[index++];
		}

		//
		// Take the remaining eBitMask bits
		//
		hk &= mBitMask;

		//
		// 2. Check whether it is in the bucket already
		//
		OmnVList<HashEntry> &bucket = mBuckets[hk];

		bucket.reset();
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket.crtValue().entryKey == entryKey)
			{
				//
				// The entry is already there. Override it.
				//
				bucket.crtValue().value = value;
				return true;
			}
		}

		//
		// Not found. Add it
		//
		bucket.append(HashEntry(hashKey, entryKey, value));
		return true;
	}

	bool get(const OmnString &hashKey, 
		     const OmnString &entryKey, 
		     C &value,
		     const bool removeFlag)
	{
		//
		// If the entry is found, it sets to 'value' and return true.
		// If 'removeFlag' is true, it will remove the entry.
		// If not found, it returns false.
		// If hashkey is empty, it is an error. Return false.
		//

		const int length = hashKey.length();
		if (length <= 0)
		{
			return false;
		}

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = 0;
		int index = 0;
		const char *data = hashKey.data();
		while (index < length && index < mBytesToAdd)
		{
			hk += data[index++];
		}

		//
		// Take the remaining eBitMask bits
		//
		hk &= mBitMask;

		//
		// Retrieve it
		//
		OmnVList<HashEntry> &bucket = mBuckets[hk];
		bucket.reset();
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket.crtValue().entryKey == entryKey)
			{
				//
				// Found it.
				//
				value = bucket.crtValue().value;

				//
				// Check whether need to remove it.
				//
				if (removeFlag)
				{
					//
					// Need to remove it
					//
					bucket.eraseCrt1();
				}

				return true;
			}
		}

		//
		// Did not find it.
		//
		return false;
	}

	bool remove(const OmnString hashKey, const OmnString &entryKey)
	{
		C value;
		if (get(hashKey, entryKey, value, true))
		{
			return true;
		}
		return false;
	}

	void reset()
	{
		//
		// It removes all entries in this hash table
		//
		for (uint i=0; i<mTableSize; i++)
		{
			mBuckets[i].clear();
		}
	}

	virtual int getHashedObj() const
	{
		int objs = 0;
		for (uint i=0; i<mTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};

#endif

