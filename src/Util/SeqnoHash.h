////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SeqnoHash.h
// Description:
// 	This hash table assumes the key is a seqno. It takes the least
// 	x number of bits as the hash key. It is very important to 
// 	ensure that the key is truly a seqno.
//
// 	If we take 12 number of the least significant bits and 
// 	the seqno is sequentially generated, keys should be evenly
// 	distributed across 4096 buckets.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_SeqnoHash_h
#define Snt_Util_SeqnoHash_h

#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"
#include "Util/RCObjImp.h"


template <class C, unsigned int tableSize>
class OmnSeqnoHash : public OmnHashTable 
{
	OmnDefineRCObject;

public:
	enum {eMaxTableSize = 0xffff};

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
		{
		}
	};

	OmnValList<HashEntry>	mBuckets[tableSize];
	unsigned int			mTableSize;
	C						mDefault;
	unsigned int			mBitMask;

public:
	OmnSeqnoHash()
		:
	mTableSize(tableSize)
	{
		OmnHashMgr::addHashTable(this);

		init();
	}


	void init()
	{
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
			mBitMask = eMaxTableSize;
			while (mBitMask > mTableSize)
			{
				//
				// Shift 'size' one bit lest to reduce the mBitMask
				//
				mBitMask = (mBitMask >> 1);
			}
		}
	}


	~OmnSeqnoHash()
	{
		OmnHashMgr::removeHashTable(this);
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
		unsigned int hk = (key & mBitMask);

		//
		// 2. Check whether it is in the bucket already
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

	C get(const int key, const bool removeFlag = false)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (key & mBitMask);

		//
		// Retrieve it
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
				// Found it. Check whether need to remove it.
				//
				if (removeFlag)
				{
					//
					// Need to remove it
					// (This statement will cause C++ to generate warning.)
					//
					bucket.eraseCrt1();
				}

				return entry.value;
			}

			bucket.next();
		}

		//
		// Did not find it.
		//
		return mDefault;
	}

	C remove(const int key)
	{
		return get(key, true);
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
