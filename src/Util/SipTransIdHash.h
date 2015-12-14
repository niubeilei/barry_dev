////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SipTransIdHash.h
// Description:
//	This is a copy of SipTransIdHash. Sip TransId is in the form
//  of:
//		z9hG4bKdddddddd
//
//  where 'd' is a hex digit. For instance, following are trans IDs:
//		z9hG4bK00000001
//		z9hG4bK26345b6a
//  
//	and we assume SipTransIdMgr will increment sip transaction Ids
//  continuously. This means that we can use the last two bytes 
//  to calculate the hash id. It is very fast and distributed 
//  very evenly.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_SipTransIdHash_h
#define Snt_Util_SipTransIdHash_h

#include "Alarm/Alarm.h"
#include "Debug/Debug.h"
#include "SipMsg/SipMethod.h"
#include "Util/String.h"
#include "Util/ValList.h"
#include "Util/HashTable.h"
#include "Util/HashMgr.h"


template <class C, unsigned int tableSize>
class OmnSipTransIdHash : public OmnHashTable 
{
	OmnDefineRCObject;

public:
	enum 
	{
		eMaxTableSize = 0x0fff,
		eKeyLength = 15
	};

public:
	struct HashEntry
	{
		OmnString	mKey;
		C			mValue;

		HashEntry()
		{
		}

		HashEntry(const OmnString key, C value)
			:
		mKey(key),
		mValue(value)
		{
		}
	};

private:
	OmnValList<HashEntry>	mBuckets[tableSize];
	int						mTableSize;
	int						mBitMask;
	int						mItrIndex;
	int						mItrBlockSize;

public:
	OmnSipTransIdHash()
		:
	mTableSize(tableSize),
	mItrIndex(0),
	mItrBlockSize(5)
	{
		OmnHashMgr::addHashTable(this);

		//
		// Need to calculate how many bits for the table. For example,
		// if the table size is 1024, we need to use the last 10 bits
		// when calculating hash keys. 
		//
		if (mTableSize >= eMaxTableSize)
		{
			mBitMask = eMaxTableSize;
		}
		else
		{
			mBitMask = 0;
			unsigned int size = mTableSize;
			while (size != 0)
			{
				//
				// Shift 'size' one bit right, shift mBitMask one bit left, 
				// and then sets mBitMask last bit to 1.
				//
				size = size >> 1;
				mBitMask = mBitMask << 1;
				mBitMask += 1;
			}
		}
	}


	~OmnSipTransIdHash()
	{
		OmnHashMgr::removeHashTable(this);
	}

	void	setNullValue(const C c) {mNullValue.mValue = c;}
	int		getObjCounts() const
			{
				//
				// This function is intended for NMS. It should not 
				// be called while there are calls running since it is	
				// not thread safe. 
				//
				int counts = 0;
				for (int i=0; i<tableSize; i++)
				{
					counts += mBuckets[i].entries();
				}
				return counts;
			}

	void	resetItr(const int blockSize) 
			{
				mItrIndex = 0;
				mItrBlockSize = blockSize;
			}

	bool	hasMoreBuckets() const {return mItrIndex < mTableSize;}
	void	getBuckets(OmnValList<HashEntry> **buckets, int &numBuckets)
			{
				//
				// From the current position (i.e., mItrIndex), it
				// retrieves either mItrBlockSize or the remaining
				// whichever smaller. 
				//
				numBuckets = 0;
				while (mItrIndex < mTableSize && numBuckets < mItrBlockSize)
				{
					buckets[numBuckets++] = &mBuckets[mItrIndex++];
				}
			}

	bool add(const OmnString &key, const C &value)
	{
		//
		// 'key' is exactly eKeyLength (should be 15) bytes long
		// in the form of:
		//
		// 	z9hG4bKhhhhhhhh
		//
		// We will get the last four characters to calculate the key.
		//

		if (key.length() != eKeyLength)
		{
			OmnAlarm << OmnErrId::eAlarmProgramError
				<< "Incorrect transaction ID length: " 
				<< key << enderr;
			return false;
		}

		//
		// Calculate the hash key
		//
		const char *data = key.data();
		unsigned int hk = (data[eKeyLength-1] - 'a') & 0xf +
						  (((data[eKeyLength-2] - 'a') << 4) & 0xf0) +
						  (((data[eKeyLength-3] - 'a') << 8) & 0xf00) + 
						  (((data[eKeyLength-4] - 'a') << 12) & 0xf000);

		//
		// Take the remaining eBitMask bits
		//
		hk = (hk & mBitMask);

		//
		// Add to the bucket
		//
		mBuckets[hk].append(HashEntry(key, value));

		//OmnTrace << "BucketSize: " << mBuckets[hk].entries() << endl;
		//	<< ". Key = " << key 
		//	<< ". Converted: " << hk << endl;

		return true;
	}

	bool get(const OmnString &key, 
			 const OmnSipMethod::E method,
		     C &value,
		     const bool removeFlag)
	{
		//
		// 'key' is exactly eKeyLength bytes long (we only care about
		//  the last 2 bytes
		//
		if (key.length() != eKeyLength)
		{
			OmnAlarm << OmnErrId::eAlarmProgramError
				<< "Incorrect transaction ID length: " 
				<< key << enderr;
			return false;
		}

		//
		// Calculate the hash key
		//
		const char *data = key.data();
		unsigned int hk = (data[eKeyLength-1] - 'a') & 0xf +
						  (((data[eKeyLength-2] - 'a') << 4) & 0xf0) +
						  (((data[eKeyLength-3] - 'a') << 8) & 0xf00) + 
						  (((data[eKeyLength-4] - 'a') << 12) & 0xf000);

		//
		// Take the remaining eBitMask bits
		//
		hk = (hk & mBitMask);

		//
		// Retrieve it
		//
		OmnValList<HashEntry> &bucket = mBuckets[hk];
		bucket.reset();
		while (bucket.hasMore())
		{
			HashEntry &entry = bucket.crtValue();
			bucket.next();
			if (entry.mKey == key && (entry.mValue)->getMethod() == method)
			{
				value = entry.mValue;
				if (removeFlag)
				{
					bucket.eraseCrt1();
				}

				return true;
			}
		}

		//
		// Did not find it.
		//
		return false;
	}

	void reset()
	{
		//
		// It removes all entries in this hash table
		//
		for (uint i=0; i<mTableSize; i++)
		{
			mBuckets[i].clear();
		}
	}

	int getHashedObj() const
	{
		int objs = 0;
		for (int i=0; i<mTableSize; i++)
		{
			objs += mBuckets[i].entries();
		}

		return objs;
	}
};

#endif

