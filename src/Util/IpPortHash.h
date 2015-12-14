////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: IpPortHash.h
// Description:
// 	This hash table is used to hash MGCP endpoints based on 
//
//       (IP Address, Port)
//
//	The hash function is to add the bytes in ip address and 
//  ports, then take the 12 bits from the results as the key.
//  
//  Since in real networks, endpoint signaling ports are normally
//  invariant (2427), it's no good to use ports as the key, this
//  hashtable can be conditionally compiled to use or not to use
//  the ports.
//
//  To simplify things, it assumes table size is 4096 (13 bits)   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_IpPortHash_h
#define Snt_Util_IpPortHash_h

#include "Debug/Debug.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"
#include "Util/IpAddr.h"

#define OmnIpPortHashKeyFunc(bubbleId, ipAddr, eid) \
 				(ipAddr & 0xff) \
 			  + ((ipAddr >> 8) & 0xff) \
 			  + ((ipAddr >> 16) & 0xff) \
 			  + ((ipAddr >> 24) & 0xff) \
			  + bubbleId \
			  + eid[3] \
			  + eid[4] \
			  + eid[5] \
			  + eid[6] \
			  + eid[7] \
			  + eid[8] \
			  + eid[9]

template <class C>
class OmnIpPortHash : public OmnHashTable
{
	OmnDefineRCObject;

public:
	enum
	{
		eTableSize = 4096,
		eBitMask = 0x0fff
	};

	struct HashEntry
	{
		int			mBubbleId;
		int			mIpAddr;
		//int			mPort;
		OmnString	mEid;
		C			mValue;

		HashEntry()
			:
		mBubbleId(-1),
		mIpAddr(0)
		{
		}

		HashEntry(const int bubbleId, const int ip, 
			const OmnString &eid, const C &value)
			:
		mBubbleId(bubbleId),
		mIpAddr(ip),
		//mPort(port),
		mEid(eid),
		mValue(value)
		{
		}
	};

private:
	OmnValList<HashEntry>	mBuckets[eTableSize];

public:
	OmnIpPortHash()
	{
		OmnHashMgr::addHashTable(this);
	}


	~OmnIpPortHash()
	{
		OmnHashMgr::removeHashTable(this);
	}


	bool add(const int bubbleId, 
			 const int ipAddr, 
			 // const int port,
			 const OmnString &eid, 
			 const C &value)
	{
		//
		// 1. Calculate the hash key. 
		// 2. Store it.
		//

		int hashValue = OmnIpPortHashKeyFunc(bubbleId, ipAddr, eid.data());
		//int s = eid.length();
		//const char *data = eid.data();
		//for (int i=0; i<s; i++)
		//{
		//	hashValue += data[i];
		//}
		//hashValue &= eBitMask;
			

		//
		// Take the remaining eBitMask bits
		//
		hashValue &= eBitMask;

		//
		// 2. Add to the bucket
		//
		mBuckets[hashValue].append(HashEntry(bubbleId, ipAddr, 
								eid, value));
		//OmnTraceCP << "EpHash: " << mBuckets[hashValue].entries() << endl;
		return true;
	}

	bool get(const int bubbleId, 
			 const int ipAddr, 
			 //const int port,
			 const OmnString &eid, 
		     C &value,
		     const bool removeFlag)
	{
		//
		// If the entry is found, it sets to 'value' and return true.
		// If 'removeFlag' is true, it will remove the entry.
		// If not found, it returns false.
		// If hashkey is empty, it is an error. Return false.
		//

		int hashValue = OmnIpPortHashKeyFunc(bubbleId, ipAddr, eid.data());

        //
        // Take the remaining eBitMask bits
        //
        hashValue &= eBitMask;

		//
		// Retrieve it
		//
		OmnValList<HashEntry> &bucket = mBuckets[hashValue];
		bucket.reset();
		while (bucket.hasMore())
		{
			HashEntry &entry = bucket.crtValue();
			if (entry.mIpAddr == ipAddr &&
				entry.mEid == eid &&
				entry.mBubbleId == bubbleId)
			{
				value = entry.mValue;
				if (removeFlag)
				{
					bucket.eraseCrt1();
				}

				return true;
			}
			bucket.next();
		}

		//
		// Did not find it.
		//
		return false;
	}

	bool get(const int bubbleId, 
			 const int ipAddr, 
		     C &value,
		     const bool removeFlag)
	{
		int hashValue = OmnIpPortHashKeyFunc(bubbleId, ipAddr, eid.data());
        hashValue &= eBitMask;

		//
		// Retrieve it
		//
		OmnValList<HashEntry> &bucket = mBuckets[hashValue];
		bucket.reset();
		while (bucket.hasMore())
		{
			HashEntry &entry = bucket.crtValue();
			if (entry.mIpAddr == ipAddr && entry.mBubbleId == bubbleId)
			{
				value = entry.mValue;
				if (removeFlag)
				{
					bucket.eraseCrt1();
				}

				return true;
			}
			bucket.next();
		}

		//
		// Did not find it.
		//
		return false;
	}

	bool remove(const int bubbleId, 
			 	const int ipAddr, 
			 	const OmnString &eid) 
	{
		C value;
		if (get(bubbleId, ipAddr, eid, value, true))
		{
			return true;
		}
		return false;
	}

	void reset()
	{
		//
		// It removes all entries in this hash table
		//
		for (uint i=0; i<eTableSize; i++)
		{
			mBuckets[i].clear();
		}
	}


	virtual int getHashedObj() const
	{
		int objs = 0;
		for (uint i=0; i<eTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};

#endif

