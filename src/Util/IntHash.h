////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IntHash.h
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
//  To make things simpler, we make this hash table fixed size:
//  		4096
//
//  To create an instance of this class:
//
//		OmnIntHash<C> table;   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_IntHash_h
#define Snt_Util_IntHash_h

#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"
#include "Util/RCObjImp.h"


template <class C>
class OmnIntHash : public OmnHashTable 
{
	OmnDefineRCObject;

private:
	struct HashEntry
	{
		int			key;
		C 			value;

		HashEntry(const int k, const C &v)
			:
		key(k),
		value(v)
		{
		}

		HashEntry()
		:
		key(0)
		{
		}
	};

	enum
	{
		eBitMask = 0xfff
	};

	OmnValList<HashEntry>	mBuckets[eBitMask+1];
	unsigned int			mTableSize;
	C						mDefault;
	unsigned int			mItrIndex;
	int						mItrBlockSize;

public:
	OmnIntHash()
		:
	mTableSize(eBitMask+1),
	mItrIndex(0),
	mItrBlockSize(5)
	{
		OmnHashMgr::addHashTable(this);
	}


	OmnIntHash(const C &d)
		:
	mTableSize(eBitMask+1),
	mDefault(d),
	mItrIndex(0),
	mItrBlockSize(5)
	{
		OmnHashMgr::addHashTable(this);
	}


	~OmnIntHash()
	{
		OmnHashMgr::removeHashTable(this);
	}


	void resetItr(const int blockSize)
	{
		mItrIndex = 0;
		mItrBlockSize = blockSize;
	}


	bool hasMoreBuckets() const {return mItrIndex < mTableSize;}

	void getBuckets(OmnValList<HashEntry> **buckets, int &numBuckets)
	{
		//
		// From the current position (i.e., mItrIndex), it
		// retrieves either mItrBlockSize or the remaining
		// whichever smaller.
		//
		numBuckets = 0;
		while (mItrIndex < mTableSize && numBuckets < mItrBlockSize)
		{
			buckets[numBuckets++] = &mBuckets[mItrIndex++];
		}
	}


	bool add(const int key, const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = key & eBitMask;

		//
		// 2. Check whether it is already in the bucket
		//
		OmnValList<HashEntry> &bucket = mBuckets[hk];
		bucket.reset();
		HashEntry entry;
		while (bucket.hasMore())
		{
			entry = bucket.crtValue();
			if (entry.key == key)
			{
				//
				// The entry is already there. Override it.
				//
				entry.value = value;
				return true;
			}

			bucket.next();
		}

		//
		// Not found. Add it
		//
		bucket.append(HashEntry(key, value));
		return true;
	}

	bool get(const int key, 
			 C &value, 
			 const bool removeFlag)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = key & eBitMask;

		//
		// Retrieve it
		//
		OmnValList<HashEntry> &bucket = mBuckets[hk];
		bucket.reset();
		HashEntry entry;
		while (bucket.hasMore())
		{
			if (bucket.crtValue().key == key)
			{
				//
				// Found it. Check whether need to remove it.
				//
				entry = bucket.crtValue();
				value = entry.value;
				if (removeFlag)
				{
					//
					// Need to remove it
					// (This statement will cause C++ to generate warning.)
					//
					bucket.eraseCrt1();
				}

				return true;
			}

			bucket.next();
		}

		//
		// Did not find it.
		//
		return false;
	}

	C remove(const int key)
	{
		C c;
		get(key, c, true);
		return c;
	}

	void reset()
	{
		//
		// It removes all entries in this hash table
		//
		for (unsigned int i=0; i<mTableSize; i++)
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
