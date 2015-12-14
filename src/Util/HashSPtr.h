////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashSPtr.h
// Description:
// 	This is a hash table for Smart Pointers. The pointed to object
//	must support a function: getHashKey() and isSameObj(const C &obj).   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_HashSptr_h
#define Omn_Util_HashSptr_h

#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashUtil.h"
#include "Util/HashMgr.h"


template <class C, unsigned int tableSize>
class OmnHashSPtr 
{
private:
	OmnVList<C>			mBuckets[tableSize+1];
	unsigned int		mTableSize;
	C					mDefault;

public:
	OmnHashSPtr()
		:
	mTableSize(tableSize)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}


	OmnHashSPtr(const C &d)
		:
	mTableSize(tableSize),
	mDefault(d)
	{
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}


	virtual ~OmnHashSPtr()
	{
	}


	bool add(const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (value->getHashKey() & mTableSize);

		//
		// 2. Check whether it is in the bucket already
		//
		OmnVList<C> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		for (int i=0; i<s; i++, bucket.next())
		{
			if ((bucket.crtValue())->hasSameObjId(value))
			{
				//
				// The entry is already there. Override it.
				//
				bucket.crtValue() = value;
				return true;
			}
		}

		//
		// Not found. Add it
		//
		bucket.append(value);
		return true;
	}

	C get(const C &value, const bool removeFlag = false)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (value->getHashKey() & mTableSize);

		//
		// Retrieve it
		//
		OmnVList<C> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		for (int i=0; i<s; i++, bucket.next())
		{
			if ((bucket.crtValue())->hasSameObjId(value))
			{
				//
				// Found it. Check whether need to remove it.
				//
				C entry = bucket.crtValue();
				if (removeFlag)
				{
					//
					// Need to remove it
					// (This statement will cause C++ to generate warning.)
					//
					bucket.eraseCrt();
				}

				return entry;
			}
		}

		//
		// Did not find it.
		//
		return mDefault;
	}

	C remove(const C &value)
	{
		return get(value, true);
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
