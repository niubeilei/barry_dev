////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AsciiHexHash.h
// Description:
//	AsciiHexHash is a hash table. Its key is a string, which 
//  are encoded as ascii hex. For instance, given a string:
//	   
//		12345678
//  the last two digits ('78') is converted as 0x78, as the 
//  least significant byte of an unsigned integer;
//  the next two digits ('56') is converted to 0x56, as the
//  next least significant byte of an unsigned inter, and so on.
// 
//  After calculated the key, the class then takes the least
//  number of bits as the hash key. If the number of bits 
//  is 12, it is a mod of 4096, for instance.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_AsciiHexHash_h
#define Snt_Util_AsciiHexHash_h

#include "Debug/Debug.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"
#include "Util/ValList.h"


template <class C, unsigned int tableSize>
class OmnAsciiHexHash : public OmnHashTable 
{
	OmnDefineRCObject;

public:
	enum {eMaxTableSize = 0x0fff};

public:
	struct HashEntry
	{
		OmnString	mKey;
		C			mValue;

		HashEntry()
		{
		}

		HashEntry(const OmnString key, C value)
			:
		mKey(key),
		mValue(value)
		{
		}
	};

private:
	OmnVList<HashEntry>	mBuckets[tableSize];
	int					mTableSize;
	int					mBitMask;
	int					mItrIndex;
	int					mItrBlockSize;

public:
	OmnAsciiHexHash()
		:
	mTableSize(tableSize),
	mItrIndex(0),
	mItrBlockSize(5)
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
			// 
			// BitMask must smaller than (TableSize-1)
			// 
			mBitMask = mBitMask >> 1;
		}
	}


	~OmnAsciiHexHash()
	{
		OmnHashMgr::removeHashTable(this);
	}

	// void	setNullValue(const C c) {mNullValue.mValue = c;}
	int		getObjCounts() const
			{
				//
				// This function is intended for NMS. It should not 
				// be called while there are calls running since it is	
				// not thread safe. 
				//
				int counts = 0;
				for (int i=0; i<tableSize; i++)
				{
					counts += mBuckets[i].entries();
				}
				return counts;
			}

	void	resetItr(const int blockSize) 
			{
				mItrIndex = 0;
				mItrBlockSize = blockSize;
			}

	bool	hasMoreBuckets() const {return mItrIndex < mTableSize;}
	void	getBuckets(OmnVList<HashEntry> **buckets, int &numBuckets)
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

	bool add(const char *key, const C &value)
	{
		//
		// For efficiency purpose, we do not check the parameters, 
		// trusting the caller.
		// 
		// For the efficient purpose, we do not check paramters. We 
		// assume the following:
		// 1. 'hashKey' is at least 8 bytes long (we only care about
		//    the first 8 bytes
		// 2. The first 8 bytes contains only digits (0-9).
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = 0;
		hk = (key[7] - '0')	
			 + ((key[6] - '0') << 4)
			 + ((key[5] - '0') << 8)
			 + ((key[4] - '0') << 12);

		//
		// Take the remaining eBitMask bits
		//
		hk = (hk & mBitMask);

		//
		// 2. Add to the bucket
		//
		mBuckets[hk].append(HashEntry(key, value));

		//OmnTrace << "BucketSize: " << mBuckets[hk].entries() << endl;
			//<< ". Key = " << key 
			//<< ". Converted: " << hk << endl;

		return true;
	}

	bool get(const char *key, 
		     C &value,
		     const bool removeFlag)
	{
		//
		// If the entry is found, it sets to 'value' and return true.
		// If 'removeFlag' is true, it will remove the entry.
		// If not found, it returns false.
		// 
		// For the efficient purpose, we do not check paramters. We 
		// assume the following:
		// 1. 'hashKey' is at least 8 bytes long (we only care about
		//    the first 8 bytes
		// 2. The first 8 bytes contains only digits (0-9).
		//

        //
        // 1. Calculate the hash key
        //
        unsigned int hk = 0;
		hk = (key[7] - '0')	
			 + ((key[6] - '0') << 4)
			 + ((key[5] - '0') << 8)
			 + ((key[4] - '0') << 12);

		//
		// Take the remaining eBitMask bits
		//
		hk &= mBitMask;

		//
		// Retrieve it
		//
		OmnVList<HashEntry> &bucket = mBuckets[hk];
		bucket.reset();

		while (bucket.hasMore())
		{
			HashEntry &entry = bucket.crtValue();
			bucket.next();
			if (entry.mKey == key)
			{
				value = entry.mValue;
				if (removeFlag)
				{
					bucket.eraseCrt();
				}

				return true;
			}
		}

		//
		// Did not find it.
		//
		return false;
	}

	bool remove(const char *hashKey, const OmnString &entryKey)
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

	int getHashedObj() const
	{
		int objs = 0;
		for (int i=0; i<mTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};

#endif

