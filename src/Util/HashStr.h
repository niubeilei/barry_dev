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
#ifndef Omn_Util_HashStr_h
#define Omn_Util_HashStr_h

#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashUtil.h"
#include "Util/DynArray.h"
#include "Util/HashMgr.h"


template <class C, unsigned int tableSize>
class OmnHashStr 
{
public:
	enum
	{
		eArrayInitSize = 50,
		eArrayIncSize = 10,
		eArrayMaxSize = 10000
	};

private:
	OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize>	mBuckets[tableSize+1];
	unsigned int	mTableSize;
	C				mDefault;
	u32				mTotal;
	u32				mMaxBktLen;

public:
	OmnHashStr()
		:
	mTableSize(tableSize),
	mTotal(0),
	mMaxBktLen(0)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}

	OmnHashStr(const C &d)
		:
	mTableSize(tableSize),
	mDefault(d),
	mTotal(0),
	mMaxBktLen(0)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}


	virtual ~OmnHashStr()
	{
	}

	u32	getMaxBktLen() const {return mMaxBktLen;}
	bool add(const C &value)
	{
		// 1. Calculate the hash key
		u32 hk = (value.getHashKey() & mTableSize);

		// 2. Check whether it is in the bucket already
		OmnDynArray<C, eArrayInitSize, eArrayIncSize, eArrayMaxSize> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == value) return false;
		}
		bucket.append(value);
		mTotal++;
		if (mMaxBktLen < (u32)bucket.entries())
			mMaxBktLen = bucket.entries();
		return true;
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
		const int s = bucket.entries();
		for (int i=0; i<s; i++)
		{
			if (bucket[i] == value)
			{
				bucket.remove(i, false);
				mTotal--;
				return true;
			}
		}

		// Did not find it.
		return false;
	}
};


#endif
