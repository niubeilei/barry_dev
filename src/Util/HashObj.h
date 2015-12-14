////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashObj.h
// Description:
// 	This is an OmnObj hash table. It is hashed on OmnObjId. 
//	The table size "tableSize" is the number of buckets. It can
//  be in the thousands. 
//
//  This table assumes the stored objects are pointers (either
//  raw pointers or smart pointers. The object must support the 
//  following member functions:
//  	u32 	getHashKey();
//  	bool	isSameObj(const C &rhs);
//  	
//
//	Non-Existing Object Processing:
// 
// Non-Existed Object was created when one tried to retrieve an 
// object that is not defined in the system. Since it is a non-existed
// object, it will not be in the hash table. In order to determine
// whether it is an existing object, we have to check the database,
// which can be very costly. It is for this reason that if it is
// the first time an non-existed object is queried, we check the 
// database. Once determined, we create a Non-Exist object (identified
// by the object ID), and put it in the hash table so that if it
// is retrieved the next time, we can quickly determine whether 
// it is an existing object. Non-exist object will expire, which 
// is controlled by eNonExistObjTTL.    
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_HashObj_h
#define Omn_Util_HashObj_h

#include "alarm_c/alarm.h"
#include "Obj/Ptrs.h"
#include "Util/DynArray.h"
#include "Util/HashMgr.h"
#include "Util/HashUtil.h"
#include <list>

template <class C, unsigned int tableSize>
class OmnObjHash 
{
private:
	OmnDynArray<C, 100, 100, 10000>	mBuckets[tableSize+1];
	unsigned int	mTableSize;
	C				mDefault;

	friend class OmnObjHashItr;

public:
	OmnObjHash()
		:
	mTableSize(tableSize)
	{
	}


	OmnObjHash(const C &d)
		:
	mTableSize(tableSize),
	mDefault(d)
	{
		aos_assert(OmnHashUtil::verifyBitmask(tableSize));
		mTableSize = OmnHashUtil::getNearestBitmask(tableSize);
	}

	virtual ~OmnObjHash()
	{
	}


	bool add(const C &obj, const bool override = true)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		aos_assert_r(!obj.isNull(), false);

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = (obj->getHashKey() & mTableSize);

		//
		// 2. Check whether it is in the bucket already
		//
		OmnDynArray<C, 100, 100, 10000> &bucket = mBuckets[hk];
		for (int i=0; i<bucket.entries(); i++)
		{
			// 
			// Chen Ding, 12/05/2007
			//
			// if (bucket[i]->getObjId() == obj->getObjId())
			if (bucket[i]->isSameObj(obj))
			{
				//
				// The entry is already there. 
				//
				if (!override)
				{
					// 
					// Object already exist in the table. 
					//
					return false;
				}

				// 
				// Override it.
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

	C get(const C &obj, const bool removeFlag = false)
	{
		//
		// 1. Calculate the hash key
		//
		u32 hk = (obj->getHashKey() & mTableSize);

		//
		// Retrieve it
		//
		OmnDynArray<C, 100, 100, 10000> &bucket = mBuckets[hk];
		for (int i=0; i<bucket.entries(); i++)
		{
			if (bucket[i]->isSameObj(obj))
			{
				// 
				// Found it. Check whether it is an Non-Exist Object. 
				// 
				// Chen Ding, 12/05/2007
				// Turn it off for the time being.
				//
				// if ((bucket[i].mObj)->getObjType() == OmnObjType::eNonExistObj)
				// {
				// 	// 
				// 	// It is a non-exist object. Check whether it expires.
				// 	//
				// 	if (((OmnNonExistObj*)bucket[i].mObj)->expired())
				// 	{
				// 		// 
				// 		// The object expired. Remove it.
				// 		//
				// 		bucket.remove(i);
				// 		return mDefault;
				// 	}
				// }

				//
				// Otherwise, it is either not a non-exist object, or it has not
				// expired yet. Considered being found. Check whether need to remove it.
				//
				if (removeFlag)
				{
					//
					// Need to remove it
					// (This statement will cause C++ to generate warning.)
					//
					bucket.remove(i);
				}

				return bucket[i];
			}
		}

		//
		// Did not find it.
		//
		return mDefault;
	}

	C remove(const C &obj)
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
		for (uint i=0; i<=mTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}


	// 
	// This function retrieves all objects whose key matches 
	// the key of 'obj'. 
	//
	u32 get(const C &obj, std::list<C> &list) 
	{
		list.clear();
		u32 hk = (obj->getHashKey() & mTableSize);
		OmnDynArray<C, 100, 100, 10000> &bucket = mBuckets[hk];
		for (int i=0; i<bucket.entries(); i++)
		{
			list.push_back(bucket[i]);
		}
		return list.size();
	}
};


#endif
