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
// 	It hashes on two strings: 
// 		[key1, key2, value]
//
// Modification History:
// 	2010/02/21	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_StrPairHash_h
#define Snt_Util_StrPairHash_h

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "aosUtil/Types.h"
#include "Util/String.h"
#include "Util/DynArray.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"
#include "Util/UtUtil.h"


template <class C, u32 tableSize>
class OmnStrPairHash : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	struct HashEntry
	{
		OmnString	mKey1;
		OmnString	mKey2;
		C			mValue;
		HashEntry *	mNext;

		HashEntry() 
		:
		mNext(0)
		{}

		HashEntry(const OmnString &key1, 
				const OmnString &key2, 
				C value)
			:
		mKey1(key1),
		mKey2(key2),
		mValue(value),
		mNext(0)
		{
		}
	};

private:
	enum
	{
		eInitSize = 10, 
		eIncSize = 5,
		eMaxSize = 1000
	};

	HashEntry	*   mBuckets[tableSize+1];
	u32				mEntries;
	u32				mTableSize;
	u32				mItrBktIdx;
	HashEntry   *	mItrPtr;
	bool			mSupportDuplicate;

public:
	OmnStrPairHash()
		:
	mEntries(0),
	mTableSize(tableSize),
	mItrBktIdx(0),
	mItrPtr(0),
	mSupportDuplicate(false)
	{
		mTableSize = tableSize;
		memset(mBuckets, 0, sizeof(void*) * (tableSize + 1));
	}


	~OmnStrPairHash()
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
	bool 	add(const OmnString &key1, 
				const OmnString &key2, 
				const C &value) 
			{
				u32 hk = AosStrHashFunc(key1.data(), key1.length());
				hk = AosStrHashFunc(hk, key2.data(), key2.length()) & mTableSize;
				return add(hk, key1, key2, value);
			}
	
	bool 	add(const u32 hk, 
				const OmnString &key1, 
				const OmnString &key2,
				const C &value)
			{
				HashEntry *ptr = mBuckets[hk];
				if (!ptr)
				{
					ptr = OmnNew HashEntry;
					mBuckets[hk] = ptr;
				}
				else if(mSupportDuplicate)
				{
					// add to the head of the bucket directly
					ptr = OmnNew HashEntry;
					ptr->mNext = mBuckets[hk];
					mBuckets[hk] = ptr;
				}else
				{
					HashEntry *prev = 0;
					while (ptr)
					{
						if (ptr->mKey1 == key1 && ptr->mKey2 == key2)
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

				ptr->mKey1 = key1;
				ptr->mKey2 = key2;
				ptr->mValue = value;

				// Chen Ding, 2011/03/02. This is a bug. It may drop all the entries
				// in the list (if mSupportDuplicate is true).
				// ptr->mNext = 0;
				mEntries++;
				return true;
			}

	bool get(const OmnString &key1, 
			 const OmnString &key2,
		     C &value,
		     const bool removeFlag)
	{
		u32 hk = AosStrHashFunc(key1.data(), key1.length());
		hk = AosStrHashFunc(hk, key2.data(), key2.length()) & mTableSize;

		// Retrieve it
		HashEntry *ptr = mBuckets[hk];
		if (!ptr) return false;
		HashEntry *prev = 0;
		while (ptr)
		{
			if (ptr->mKey1 == key1 && ptr->mKey2 == key2)
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

	bool remove(const OmnString &key1, const OmnString &key2)
	{
		C value;
		return get(key1, key2, value, true);
	}

	void reset()
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
		resetWordLoop();
	}

	void resetWordLoop()
	{
		mItrBktIdx = 0;
		mItrPtr = mBuckets[0];
	}

	void setSupportDuplicate(const bool support){mSupportDuplicate = support;}

	bool nextEntry(OmnString &key1, OmnString &key2, C &value)
	{
		if (mItrBktIdx > mTableSize) return false;
		if (mItrPtr)
		{
			key1 = mItrPtr->mKey1;
			key2 = mItrPtr->mKey2;
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
				key1 = mItrPtr->mKey1;
				key2 = mItrPtr->mKey2;
				value = mItrPtr->mValue;
				mItrPtr = mItrPtr->mNext;
				return true;
			}
			mItrBktIdx++;
		}
		return false;
	}
};

#endif

