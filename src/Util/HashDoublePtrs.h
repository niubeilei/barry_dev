////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashDoublePtrs.h
// Description:
//	It hashes on two pointers (or integers). It assumes the class
//	C supports operator ==.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_HashDoublePtrs_h
#define Omn_Util_HashDoublePtrs_h

#include "Obj/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/HashMgr.h"
#include "Util/HashUtil.h"

#define OmnDoublePtrHashFunc(ptr1, ptr2) \
	(( \
		(ptr1 & 0xff) + \
		((ptr1 >> 8) & 0xff)  + \
		((ptr1 >> 16) & 0xff) + \
		((ptr1 >> 24) & 0xff) + \
		(ptr2 & 0xff) + \
		((ptr2 >> 8) & 0xff) + \
		((ptr2 >> 16) & 0xff) + \
		((ptr2 >> 24) & 0xff) \
	) & mBitmask)

template <class ObjType, class ObjIdType, int tableSize>
class OmnDoublePtrHash 
{
private:
	enum 
	{
		eMaxBits = 20
	};

	OmnDynArray<ObjType, 2, 2>	mBuckets[tableSize+1];
	unsigned int			mBitmask;
	ObjType					mDefault;

	friend class OmnDoublePtrHashItr;

public:
	OmnDoublePtrHash(const ObjType &d)
		:
	mDefault(d)
	{
		mBitmask = OmnHashUtil::getNearestBitmask(tableSize);
	}

	virtual ~OmnDoublePtrHash()
	{
	}


	bool add(const void *ptr1, const void *ptr2, const ObjType &obj)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		OmnAssert(!obj.isNull(), "Null pointer passed in", false);

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = OmnDoublePtrHashFunc((int)ptr1, (int)ptr2);

		//
		// 2. Check whether it is in the bucket already
		//
		OmnDynArray<ObjType, 2, 2> &bucket = mBuckets[hk];
		for (int i=0; i<bucket.entries(); i++)
		{
			if (bucket[i] == obj)
			{
				//
				// The entry is already there. Override it.
				//
				bucket[i] = obj;
				return true;
			}
		}

		//
		// Not found. Add it
		//
		bucket.append(obj);
		return true;
	}

	ObjType get(const void *ptr1, 
				  const void *ptr2, 
				  const ObjIdType &objId, 
				  const bool removeFlag = false)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = OmnDoublePtrHashFunc((int)ptr1, (int)ptr2);

		//
		// Retrieve it
		//
		OmnDynArray<ObjType, 2, 2> &bucket = mBuckets[hk];
		for (int i=0; i<bucket.entries(); i++)
		{
			if (bucket[i]->getHashId() == objId)
			{
				// 
				// Found it.
				// 
				return bucket[i];
			}
		}

		//
		// Did not find it.
		//
		return mDefault;
	}

	ObjType remove(const ObjType &obj)
	{
		return get(obj, true);
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
		for (uint i=0; i<=mBitmask; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};


#endif
