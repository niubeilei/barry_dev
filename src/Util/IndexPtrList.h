////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IndexPtrList.h
// Description:
//	An Indexed Pointer List is a pointer list that indexes on 
//  a type. The index data type is 'IndType'. It assumes that
// 
//	It assumes the type 'IndexT' defines the following functions:
//		unsigned int getHashKey()
//		bool operator ==()   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_IndexPtrList_h
#define Omn_Util_IndexPtrList_h

#include "Util/ValList.h"
#include "Util/HashUtil.h"



template <class IndexT, class C, int tableSize>
class OmnIdxPList
{
private:
	struct Ety
	{
		IndexT	mKey;
		C 		mValue;

		Ety(const IndexT &key, const C &v)
			:
		mKey(key),
		mValue(v)
		{
		}

		Ety()
		{
		}
	};

	OmnVList<Ety>	mBuckets[tableSize+1];
	unsigned int	mTableSize;
	C				mDefault;

public:
	OmnIdxPList()
		:
	mTableSize(tableSize),
	mDefault(0)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}

	OmnIdxPList(const C &d)
		:
	mTableSize(tableSize),
	mDefault(d)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}


	virtual ~OmnIdxPList()
	{
	}


	bool add(const IndexT &key, const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (key.getHashKey() & mTableSize);

		//
		// 2. Check whether it is in the bucket already
		//
		OmnVList<Ety> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		Ety entry;
		for (int i=0; i<s; i++, bucket.next())
		{
			if (bucket.crtValue().mKey == key)
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

	C get(const IndexT &key, const bool removeFlag = false)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (key.getHashKey() & mTableSize);

		//
		// Retrieve it
		//
		OmnVList<Ety> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		Ety entry;
		for (int i=0; i<s; i++, bucket.next())
		{
			if (bucket.crtValue().mKey == key)
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

	C remove(const int key)
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

