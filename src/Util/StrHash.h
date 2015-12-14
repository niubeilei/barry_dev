////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrHashNew.h
// Description:
//	Each entry has two values (char * key, T Value).
//  key is the one to be hashed. 
//  Value is the value to be saved in the 
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
//		OmnStrHash<C, 0xfff> table;
//
//  It is extremely import that the second parm must be all 1's. 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_StrHashNew_h
#define Snt_Util_StrHashNew_h

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "aosUtil/Types.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"
#include "Util/UtUtil.h"


template <class C, u32 tableSize>
class OmnStrHash : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxStrLen = 260 
	};

public:
	struct HashEntry
	{
		char		mKey[eMaxStrLen];
		C			mValue;
		HashEntry *	mNext;

		HashEntry() 
		:
		mNext(0)
		{}

		HashEntry(const OmnString &key, C value)
			:
		mKey(key),
		mValue(value),
		mNext(0)
		{
		}
	};

private:
	HashEntry	*   mBuckets[tableSize+1];
	u32				mEntries;
	u32				mTableSize;
	u32				mItrBktIdx;
	HashEntry   *	mItrPtr;

public:
	OmnStrHash()
		:
	mEntries(0),
	mTableSize(tableSize),
	mItrBktIdx(0),
	mItrPtr(0)
	{
		mTableSize = tableSize;
		memset(mBuckets, 0, sizeof(void *) * (tableSize + 1));
	}

	~OmnStrHash()
	{
		HashEntry *next, *ptr;
		for (u32 i=0; i<=mTableSize; i++)
		{
			ptr = mBuckets[i];
			while (ptr)
			{
				next = ptr->mNext;
				OmnDelete ptr;
				ptr = next;
			}
			mBuckets[i] = 0;
		}
	}

	int		getObjCounts() const {return mEntries;}
	bool 	add(const OmnString &key, const C &value) 
			{return add(key.data(), key.length(), value);}
	
	bool 	add(const char *key, const int len, const C &value)
	{
		if(len >= eMaxStrLen) return false;
		// 1. Calculate the hash key
		u32 hk = AosStrHashFunc(key, len) & mTableSize;

		// 2. Check whether it is already in the bucket
		HashEntry *ptr = mBuckets[hk];
		if (!ptr)
		{
			ptr = OmnNew HashEntry;
			mBuckets[hk] = ptr;
		}
		else
		{
			HashEntry *prev = 0;
			while (ptr)
			{
				if (strlen(ptr->mKey) == (u32)len &&
				    strncmp(ptr->mKey, key, len) == 0)
				{
					ptr->mValue = value;
					return true;
				}
				prev = ptr;
				ptr = ptr->mNext;
			}

			aos_assert_r(prev, false);
			ptr = OmnNew HashEntry;
			prev->mNext = ptr;
		}

		memcpy(ptr->mKey, key, len); 
		ptr->mKey[len] = 0;
		ptr->mValue = value;
		ptr->mNext = 0;
		mEntries++;
		return true;
	}

	bool get(const OmnString &hashKey, 
		     C &value,
		     const bool removeFlag)
	{
		return get(hashKey.data(), hashKey.length(), value, removeFlag);
	}

	bool get(const char *data, 
			const int len, 
			C &value, 
			const bool removeFlag)
	{
		if(len >= eMaxStrLen) return false;
					
		u32 hk = AosStrHashFunc(data, len) & mTableSize;

		// Retrieve it
		HashEntry *ptr = mBuckets[hk];
		if (!ptr) return false;
		HashEntry *prev = 0;
		while (ptr)
		{
			if (strlen(ptr->mKey) == (u32)len &&
				memcmp(ptr->mKey, data, len) == 0)
			{
				value = ptr->mValue;
				if (removeFlag)
				{
					if (!prev)
					{
						mBuckets[hk] = ptr->mNext;
					}
					else
					{
						prev->mNext = ptr->mNext;
					}
					OmnDelete ptr;
					mEntries--;
				}
				return true;
			}
			prev = ptr;
			ptr = ptr->mNext;
		}

		// Did not find it.
		return false;
	}

	bool remove(const OmnString &hashKey)
	{
		C value;
		return get(hashKey.data(), hashKey.length(), value, true);
	}

	void resetWordLoop()
	{
		mItrBktIdx = 0;
		mItrPtr = 0;
	}

	bool nextEntry(OmnString &key, C &value)
	{
		if (mItrBktIdx > mTableSize) return false;
		if (mItrPtr)
		{
			key = mItrPtr->mKey;
			value = mItrPtr->mValue;
			mItrPtr = mItrPtr->mNext;
			return true;
		}
		
		mItrBktIdx++;
		while (mItrBktIdx <= mTableSize)
		{
			mItrPtr = mBuckets[mItrBktIdx];
			if (mItrPtr)
			{
				key = mItrPtr->mKey;
				value = mItrPtr->mValue;
				mItrPtr = mItrPtr->mNext;
				return true;
			}
			mItrBktIdx++;
		}
		return false;
	}

	OmnString getRandKey(const OmnString &dft)
	{
		// This function randomly returns a key
		int idx = rand() % mTableSize;

		int guard = 0;
		while (!mBuckets[idx] && guard++ < (int)mTableSize)
		{
			idx++;
			if (idx >= (int)mTableSize) idx = 0;
		}

		if (!mBuckets[idx]) return dft;

		// Determine how many entries in the bucket
		int num = 0;
		HashEntry *bkt = mBuckets[idx];
		guard = 0;
		const int maxEntries = 10000;
		while (bkt && guard++ < maxEntries)
		{
			num++;
			bkt = bkt->mNext;
		}

		aos_assert_r(num > 0, dft);
		int idx2 = rand() % num;
		bkt = mBuckets[idx];
		for(int i = 0;i < idx2;i ++)bkt = bkt->mNext;
		return bkt->mKey;
	}

};

#endif

