////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashAddr.h
// Description:
// 	This class hashes on IpAddr. It adds the four bytes and then
// 	take the last x number of bits as the key.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_HashAddr_h
#define Snt_Util_HashAddr_h

#include "Util/ValList.h"
#include "Util/IpAddr.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"

#define OmnHashAddrHashFunc(x) \
	(((x & 0xff) + \
	((x >> 8) & 0xff)  + \
	((x >> 16) & 0xff) + \
	((x >> 24) & 0xff)) & mBitmask)  \

template <class C, unsigned int bitmask>
class OmnHashAddrPort 
{
private:
	struct HashEntry
	{
		OmnIpAddr	mIpAddr;
		C 			mValue;

		HashEntry(const OmnIpAddr &addr, const C &v)
			:
		mIpAddr(addr),
		mValue(v)
		{
		}

		HashEntry()
		{
		}
	};

	OmnValList<HashEntry>	mBuckets[bitmask+1];
	unsigned int			mBitMask;
	unsigned int			mTableSize;
	C						mDefault;

public:
	OmnHashAddrPort()
		:
	mBitMask(bitmask),
	mTableSize(mBitMask+1)
	{
	}


	OmnHashAddrPort(const C &d)
		:
	mBitMask(bitmask),
	mTableSize(mBitMask+1),
	mDefault(d)
	{
	}


	virtual ~OmnHashAddrPort()
	{
	}


	bool add(const OmnIpAddr &addr, const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = OmnHashAddrHashFunc(addr);

		//
		// 2. Check whether it is in the bucket already
		//
		OmnValList<HashEntry> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		HashEntry entry;
		for (int i=0; i<s; i++, bucket.next())
		{
			if (bucket.crtValue().mIpAddr == addr)
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
		bucket.append(HashEntry(addr, value));
		return true;
	}

	C get(const OmnIpAddr &addr, const bool removeFlag = false)
	{
		//
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.
		//

		//
		// 1. Calculate the hash key
		//
		unsigned int hk = OmnHashAddrHashFunc(addr);

		//
		// Retrieve it
		//
		OmnValList<HashEntry> &bucket = mBuckets[hk];
		const int s = bucket.entries();
		bucket.reset();
		HashEntry entry;
		for (int i=0; i<s; i++, bucket.next())
		{
			if (bucket.crtValue().mIpAddr == addr)
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
					bucket.eraseCrt1();
				}

				return entry.mValue;
			}
		}

		//
		// Did not find it.
		//
		return mDefault;
	}

	C remove(const OmnIpAddr &addr)
	{
		return get(addr, true);
	}

	void reset()
	{
		//
		// It removes all entries in this hash table
		//
		for (unsigned int i=0; i<mTableSize; i++)
		{
			mBuckets[i].clear();
		}
	}

	virtual int getHashedObj() const
	{
		int objs = 0;
		for (uint i=0; i<mTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};


#endif
