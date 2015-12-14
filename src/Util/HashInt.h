////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashInt.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_HashInt_h
#define Omn_Util_HashInt_h

#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashUtil.h"
#include "Util/HashMgr.h"

#define OmnHashIntHashFunc(x) \
	(((x & 0xff) + \
	((x >> 8) & 0xff)  + \
	((x >> 16) & 0xff) + \
	((x >> 24) & 0xff)) & mBitmask)

template <class C, unsigned int tableSize>
class OmnHashInt 
{
private:
	struct Ety
	{
		int		mKey;
		C 		mValue;

		Ety(const int key, const C &v)
			:
		mKey(key),
		mValue(v)
		{
		}

		Ety()
			:
		mKey(0)
		{
		}
	};

	OmnVList<Ety>	mBuckets[tableSize+1];
	unsigned int			mTableSize;
	C						mDefault;

public:
	OmnHashInt()
		:
	mTableSize(tableSize)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}


	OmnHashInt(const C &d)
		:
	mTableSize(tableSize),
	mDefault(d)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}


	virtual ~OmnHashInt()
	{
	}


	bool add(const unsigned int key, const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (key & mTableSize);

		//
		// 2. Check whether it is in the bucket already
		//
		OmnVList<Ety> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		for (int i=0; i<s; i++, bucket.next())
		{
			if (bucket.crtValue().mKey == (int)key)
			{
				//
				// The entry is already there. Override it.
				//
				bucket.crtValue().mValue = value;
				return true;
			}
		}

		//
		// Not found. Add it
		//
		bucket.append(Ety(key, value));
		return true;
	}

	C get(const unsigned int key, const bool removeFlag = false)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (key & mTableSize);

		//
		// Retrieve it
		//
		OmnVList<Ety> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		Ety entry;
		for (int i=0; i<s; i++, bucket.next())
		{
			if (bucket.crtValue().mKey == (int)key)
			{
				//
				// Found it. Check whether need to remove it.
				//
				entry = bucket.crtValue();
				if (removeFlag)
				{
					//
					// Need to remove it
					// (This statement will cause C++ to generate warning.)
					//
					bucket.eraseCrt();
				}

				return entry.mValue;
			}
		}

		//
		// Did not find it.
		//
		return mDefault;
	}

	C remove(const unsigned int key)
	{
		return get(key, true);
	}

	void reset()
	{
		//
		// It removes all entries in this hash table
		//
		for (unsigned int i=0; i<=mTableSize; i++)
		{
			mBuckets[i].clear();
		}
	}

	virtual int getHashedObj() const
	{
		int objs = 0;
		for (uint i=0; i<=mTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};


#endif
