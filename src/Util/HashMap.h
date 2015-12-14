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
//   
//
// Modification History:
// 11/10/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_HashMap_h
#define Omn_Util_HashMap_h

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Porting/ThreadDef.h"
#include "Util/OmnNew.h"


template <class KeyType, class ValueType, class HashFunc, class CompFunc, u64 tableSize>
class AosHashMap 
{
private:
	enum
	{
		eInitTableSize = 10000,
		eInitBucketLength = 5
	};

	struct Bucket
	{
		int			 mNum;
		int			 mLength;
		KeyType 	*mKeys;
		ValueType	*mValues;

		Bucket() : mNum(0), mLength(0), mKeys(0), mValues(0) {}

		Bucket(const int length)
		:
		mNum(0),
		mLength(length)
		{
			mKeys = OmnNew KeyType[length];
			mValues = OmnNew ValueType[length];
		}

		~Bucket()
		{
			OmnDelete [] mKeys;
			mKeys = 0;
			OmnDelete [] mValues;
			mValues = 0;
			mLength = 0;
			mNum = 0;
		}

		void clearMemory()
		{
			OmnDelete [] mKeys;
			mKeys = 0;
			OmnDelete [] mValues;
			mValues = 0;
			mLength = 0;
			mNum = 0;
		}

		bool expandBucket()
		{
			// The bucket is not long enough. This function doubles its size. 
			aos_assert_r(mNum <= mLength, false);
			int len = mLength + mLength;
			KeyType *keys = OmnNew KeyType[len];
			ValueType *values = OmnNew ValueType[len];
			aos_assert_r(keys, false);
			aos_assert_r(values, false);
			for (int i=0; i<mNum; i++)
			{
				keys[i] = mKeys[i];
				values[i] = mValues[i];
			}
			mLength = len;
			OmnDelete [] mKeys;
			OmnDelete [] mValues;
			mKeys = keys;
			mValues = values;
			return true;
		}

		bool appendEntry(const KeyType &key, const ValueType &value)
		{
			if (mNum >= mLength) 
			{
				bool rslt = expandBucket();
				aos_assert_r(rslt, false);
			}

			aos_assert_r(mNum < mLength, false);
			int idx = mNum++;
			mKeys[idx] = key;
			mValues[idx] = value;
			return true;
		}

		bool erase(const int idx)
		{
			aos_assert_r(idx < mNum, false);
			if (idx == mNum-1)
			{
				mNum--;
				return true;
			}

			for (int i=idx; i<mNum-1; i++)
			{
				mKeys[i] = mKeys[i+1];
				mValues[i] = mValues[i+1];
			}
			mNum--;
			return true;
		}
	};

	// Bucket*			mBuckets[tableSize+1];
	Bucket**		mBuckets;
	u64				mTableSize;
	HashFunc		mHashFunc;
	CompFunc		mCompFunc;

public:
	class iterator
	{
	public:
		int				bucket_idx;
		int				entry_idx;
		KeyType			first;
		ValueType		second;
		AosHashMap *	map;

	public:
		iterator()
		{
			bucket_idx = -1;
			entry_idx = -1;
			map = 0;
		}

		iterator *operator -> ()
		{
			return this;
		}

		void operator ++()
		{
			if (!map) return;
			map->getNextValidBucket(*this);
		}

		void operator ++(const int)
		{
			if (!map) return;
			map->getNextValidBucket(*this);
		}

		bool operator == (const iterator &rhs) const
		{
			if (this == &rhs) return true;
			return (bucket_idx == rhs.bucket_idx && entry_idx == rhs.entry_idx);
		}

		inline bool operator != (const iterator &rhs) const
		{
			return !operator == (rhs);
		}

		iterator & operator = (const ValueType &value)
		{
			aos_assert_r(map, *this);
			aos_assert_r(map->isValidBucketIndex(bucket_idx), *this);
			u32 hash_key = map->getHashKey(first);
			aos_assert_r(hash_key == (u64)bucket_idx, *this);
			map->add(first, value);
			return *this;
		}
	};

public:
	AosHashMap()
	:
	mTableSize(tableSize)
	{
		if (mTableSize <= 0) mTableSize = eInitTableSize;
		mBuckets = OmnNew Bucket*[mTableSize+1];
		memset(mBuckets, 0, sizeof(Bucket*) * (mTableSize+1));
	}

	virtual ~AosHashMap()
	{
		for (u64 i=0; i<mTableSize; i++)
		{
			if (mBuckets[i]) OmnDelete mBuckets[i];
		}
	}

	iterator begin()
	{
		iterator itr;
		itr.bucket_idx = -1;
		itr.entry_idx = 0;
		getNextValidBucket(itr);
		return itr;
	}

	static iterator end()
	{
		iterator itr;
		itr.bucket_idx = -2;
		itr.entry_idx = 0;
		return itr;
	}

	inline bool isValidBucketIndex(const int64_t &idx)
	{
		return idx >= 0 && (u64)idx < mTableSize;
	}

	inline u32 getHashKey(const KeyType &key)
	{
		return mHashFunc(key) % mTableSize;
	}

	bool add(const KeyType &key, const ValueType &value)
	{
		// 1. Calculate the hash key. 
		// 2. Check whether the entry is there
		// 3. Store it.

		// 1. Calculate the hash key
		u32 hk = mHashFunc(key) % mTableSize;

		// 2. Check whether it is in the bucket already
		Bucket *bucket = mBuckets[hk];
		if (!bucket)
		{
			// No entry founded. Create the entry
			bucket = OmnNew Bucket(eInitBucketLength);
			if (!bucket)
			{
				OmnAlarm << "Failed allocating node bucket: " << mTableSize << enderr;
				return false;
			}
			bucket->appendEntry(key, value);
			mBuckets[hk] = bucket;
			return true;
		}

		for (int i=0; i<bucket->mNum; i++)
		{
			if (mCompFunc(key, bucket->mKeys[i]))
			{
				// The entry is already there. Override it.
				bucket->mValues[i] = value;
				return true;
			}
		}

		// Not found. Add it
		bool rslt = bucket->appendEntry(key, value);
		aos_assert_r(rslt, false);
		return true;
	}

	ValueType get(const KeyType &key, const ValueType &dft, const bool removeFlag = false)
	{
		// If the entry is found, it returns the entry.
		// If 'removeFlag' is true, it will remove the entry, too.
		// If not found, it returns the default.

		// 1. Calculate the hash key
		u32 hk = mHashFunc(key) % mTableSize;

		// Retrieve it
		Bucket *bucket = mBuckets[hk];
		if (!bucket)
		{
			// Not found. 
			return dft;
		}

		for (int i=0; i<bucket->mNum; i++)
		{
			if (mCompFunc(bucket->mKeys[i], key))
			{
				// Found it. Check whether need to remove it.
				ValueType entry = bucket->mValues[i];
				if (removeFlag)
				{
					// Need to remove it
					bucket->erase(i);
				}
				return entry;
			}
		}

		// Did not find it.
		return dft;
	}

	iterator find(const KeyType &key)
	{
		// 1. Calculate the hash key
		u32 hk = mHashFunc(key) % mTableSize;

		// Retrieve it
		Bucket *bucket = mBuckets[hk];
		if (!bucket)
		{
			// Not found. 
			return end();
		}

		for (int i=0; i<bucket->mNum; i++)
		{
			if (mCompFunc(bucket->mKeys[i], key))
			{
				// Found it. Check whether need to remove it.
				iterator itr;
				itr.bucket_idx = hk;
				itr.entry_idx = i;
				itr.first = key;
				itr.second = bucket->mValues[i];
				itr.map = this;
				return itr;
			}
		}

		// Did not find it.
		return end();
	}

	iterator operator [](const KeyType &key)
	{
		// 1. Calculate the hash key
		u32 hk = mHashFunc(key) % mTableSize;

		// Retrieve it
		Bucket *bucket = mBuckets[hk];
		if (!bucket)
		{
			bucket = OmnNew Bucket(eInitBucketLength);
			if (!bucket)
			{
				OmnAlarm << "Failed allocating node bucket: " << mTableSize << enderr;
				return end();
			}
			mBuckets[hk] = bucket;
		}

		iterator itr;
		itr.bucket_idx = hk;
		itr.entry_idx = -1;
		itr.first = key;
		itr.map = this;
		return itr;
	}

	bool erase(const KeyType &key)
	{
		// 1. Calculate the hash key
		u32 hk = mHashFunc(key) % mTableSize;

		// Retrieve it
		Bucket *bucket = mBuckets[hk];
		if (!bucket)
		{
			// Not found. 
			return false;
		}

		for (int i=0; i<bucket->mNum; i++)
		{
			if (mCompFunc(bucket->mKeys[i], key))
			{
				// Need to remove it
				bucket->erase(i);
				return true;
			}
		}

		// Did not find it.
		return false;
	}

	void reset()
	{
		// It removes all entries in this hash table
		for (unsigned int i=0; i<=mTableSize; i++)
		{
			// Chen Ding, 2013/10/26
			// OmnDelete [] mBuckets[i];
			OmnDelete mBuckets[i];
			mBuckets[i] = 0;
		}
	}

	Bucket *getBucket(const int64_t &idx)
	{
		aos_assert_r(idx >= 0 && (u64)idx < mTableSize, 0);
		return mBuckets[idx];
	}

	bool getNextValidBucket(iterator &itr)
	{
		itr.map = this;
		if (itr.bucket_idx < -1)
		{
			// It is end(). Do nothing.
			return true;
		}

		if (itr.bucket_idx >= 0 && (u64)itr.bucket_idx >= mTableSize)
		{
			itr.entry_idx = -2;
			itr.entry_idx = 0;
			return true;
		}

		int bucket_idx = itr.bucket_idx;
		int entry_idx = itr.entry_idx;
		if (bucket_idx < 0)
		{
			// Find the next valid bucket
			bucket_idx = 0;
			entry_idx = 0;
		}
		else
		{
			entry_idx++;
		}

		while ((u64)bucket_idx < mTableSize)
		{
			if (mBuckets[bucket_idx])
			{
				Bucket *bucket = mBuckets[bucket_idx];
				if (entry_idx < bucket->mNum)
				{
					itr.first = bucket->mKeys[entry_idx];
					itr.second = bucket->mValues[entry_idx];
					itr.bucket_idx = bucket_idx;
					itr.entry_idx = entry_idx;
					return true;
				}
			}
			bucket_idx++;
			entry_idx = 0;
		}
		itr.bucket_idx = -2;
		itr.entry_idx = 0;
		return true;
	}
};
#endif
