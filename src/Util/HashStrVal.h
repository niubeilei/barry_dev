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
// Modification History:
//	12/05/2009	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_HashStrVal_h
#define Omn_Util_HashStrVal_h

#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashUtil.h"
#include "Util/DynArray.h"
#include "Util/HashMgr.h"


template <class C, class V, unsigned int tableSize>
class OmnHashStrVal
{
	enum
	{
		eArrayInitSize = 50,
		eArrayIncSize = 10,
		eArrayMaxSize = 10000
	};

private:
	OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mBuckets[tableSize+1];
	OmnDynArray<V, eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mValues[tableSize+1];
	unsigned int	mTableSize;
	u32				mTotal;
	u32				mMaxBktLen;

public:
	OmnHashStrVal()
		:
	mTableSize(tableSize),
	mTotal(0),
	mMaxBktLen(0)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}

	virtual ~OmnHashStrVal()
	{
	}

	u32	getMaxBktLen() const {return mMaxBktLen;}
	bool add(const C &key, const V &value)
	{
		// 1. Calculate the hash key
		u32 hk = (key.getHashKey() & mTableSize);

		// 2. Check whether it is in the bucket already
		OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &bucket = mBuckets[hk];
		OmnDynArray<V, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values = mValues[hk];
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == key) return false;
		}
		bucket.append(key);
		values.append(value);
		mTotal++;
		if (mMaxBktLen < (u32)bucket.entries())
			mMaxBktLen = bucket.entries();
		return true;
	}

	bool set(const C &key, const V &value)
	{
		// 1. Calculate the hash key
		u32 hk = (key.getHashKey() & mTableSize);

		// 2. Check whether it is in the bucket already
		OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &bucket = mBuckets[hk];
		OmnDynArray<V, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values = mValues[hk];
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == key) 
			{
				values[i] = value;
				return true;
			}
		}
		return false;
	}

	bool exist(const C &value)
	{
		// 1. Calculate the hash key
		u32 hk = (value.getHashKey() & mTableSize);

		// 2. Check whether it is in the bucket already
		OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == value) return true;
		}
		return false;
	}

	V get(const C &value, const V &dft)
	{
		// 1. Calculate the hash key
		u32 hk = (value.getHashKey() & mTableSize);

		// 2. Check whether it is in the bucket already
		OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &bucket = mBuckets[hk];
		OmnDynArray<V, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values = mValues[hk];
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == value) 
			{
				return values[i];
			}
		}
		return dft;
	}

	bool remove(const C &value)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		// 1. Calculate the hash key
		u32 hk = (value->getHashKey() & mTableSize);

		// Retrieve it
		OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &bucket = mBuckets[hk];
		OmnDynArray<V, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &values = mValues[hk];
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == value)
			{
				bucket.remove(i, false);
				values.remove(i, false);
				mTotal--;
				return true;
			}
		}

		// Did not find it.
		return false;
	}
};


#endif
