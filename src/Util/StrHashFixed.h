///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StrHashNew.h
// Description:
//	Each entry has two values (char * key, T Value).
//  key is the one to be hashed. 
//  Value is the value to be saved in the 
//  table. Normally, Value should be a smart pointer. 
//
//  The hash function calculates hash value by adding up to 
//  x number of bytes:
//
//	int hashvalue = byte[0] + byte[1] + ... = byte[x]
//
//  and then take the last 12 bits from hashvalue to obtain the 
//  final hash key. This means that every hash type of this 
//  type will have 4096 buckets.
//
//	The table maximum size should not be bigger than 65000. 
//  Otherwise it limits to that.
//
//  To create an instance of this class:
//
//		OmnStrHash<C, 0xfff> table;
//
//  It is extremely import that the second parm must be all 1's. 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Snt_Util_StrHashFixed_h
#define Snt_Util_StrHashFixed_h

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "aosUtil/Types.h"
#include "Util/String.h"
#include "Util/UtUtil.h"


template <class C, u32 strlen, u32 bucketlen>
class OmnStrHashFixed
{
private:
	enum
	{
		eTableSize = 0x3ff
	};

	char	mBuckets[eTableSize+1][bucketlen][strlen+1];
	u16		mStrLens[eTableSize+1][bucketlen];
	C		mValues[eTableSize+1][bucketlen];
	u32		mNumElems[eTableSize+1];
	u32		mEntries;
	u32		mBucketLen;
	u32		mStrLen;
	u32		mItrBktIdx;
	u32		mItrEntryIdx;
	vector<int>	mDirtyBuckets;

public:
	OmnStrHashFixed()
		:
	mEntries(0),
	mBucketLen(bucketlen),
	mStrLen(strlen),
	mItrBktIdx(0),
	mItrEntryIdx(0)
	{
		memset(mNumElems, 0, sizeof(u32) * (eTableSize + 1));
	}

	~OmnStrHashFixed()
	{
	}

	void reset() 
	{
		memset(mNumElems, 0, sizeof(u32) * (eTableSize+1)); 
		mEntries = 0;
		resetWordLoop();
	}

	int		getObjCounts() const {return mEntries;}
	bool 	add(const OmnString &key, const C &value) 
			{return add(key.data(), key.length(), value);}

	bool 	add(const char *key, const int len, const C &value)
	{
		// Make sure the length is in the range: [1, mStrLen]
		if (len <= 0 || (u32)len >= mStrLen)
		{
			OmnAlarm << "Too long key : " << len << enderr;
			return false;
		}
		// Calculate the hash key
		u32 hk = AosStrHashFunc(key, len) % eTableSize;
		// Check whether it is already in the bucket
		for (u32 i=0; i<mNumElems[hk]; i++)
		{
			if (mStrLens[hk][i] == (u32)len && strncmp(mBuckets[hk][i], key, len) == 0)
			{
				mValues[hk][i] = value;
				return true;
			}
		}

		if (mNumElems[hk] >= mBucketLen)
		{
			OmnAlarm << "Too many elements: " << mBucketLen << enderr;
			return false;
		}

		u32 num = mNumElems[hk]++;
		if (num == 0)
		{
			mDirtyBuckets.push_back(hk);
		}

		memcpy(mBuckets[hk][num], key, len); 
		mBuckets[hk][num][len] = 0;
		mStrLens[hk][num] = (u32)len;
		mValues[hk][num] = value;
		mEntries++;
		return true;
	}
	

	bool get(const OmnString &hashKey, C &value, const bool removeFlag)
	{
		return get(hashKey.data(), hashKey.length(), value, removeFlag);
	}

	bool get(const char *data, 
			const int len, 
			C &value, 
			const bool removeFlag)
	{
		if (len <= 0 || (u32)len >= mStrLen) return false;

		u32 hk = AosStrHashFunc(data, len) % eTableSize;

		// Retrieve it
		u32 num = mNumElems[hk];
		for (u32 i=0; i<num; i++)
		{
			if (mStrLens[hk][i] == (u32)len && memcmp(mBuckets[hk][i], data, len) == 0)
			{
				value = mValues[hk][i];
				if (removeFlag)
				{
					mEntries--;
					if (num == 1)
					{
						mNumElems[hk] = 0;
						return true;
					}

					mValues[hk][i] = mValues[hk][num-1];
					mStrLens[hk][i] = mStrLens[hk][num-1];
					
					strcpy(mBuckets[hk][i], mBuckets[hk][num-1]);
					mNumElems[hk]--;
					return true;
				}
				return true;
			}
		}

		// Did not find it.
		return false;
	}

	bool remove(const OmnString &hashKey)
	{
		C value;
		return get(hashKey.data(), hashKey.length(), value, true);
	}

	void resetWordLoop()
	{
		mItrBktIdx = 0;
		mItrEntryIdx = 0;
		mDirtyBuckets.clear();
	}

	bool nextEntry(OmnString &key, C &value)
	{
		u32 size = mDirtyBuckets.size();
		if (size == 0 || mItrBktIdx >= size) return false;

		u32 bkt_idx = mDirtyBuckets[mItrBktIdx];
		if (mItrEntryIdx < mNumElems[bkt_idx])
		{
			key   = mBuckets[bkt_idx][mItrEntryIdx]; 
			value = mValues[bkt_idx][mItrEntryIdx]; 
			mItrEntryIdx++;
			return true;
		}
		
		mItrBktIdx++;
		
		mItrEntryIdx = 0;
		while(mItrBktIdx < size)
		{
			bkt_idx = mDirtyBuckets[mItrBktIdx];
			if(mItrEntryIdx < mNumElems[bkt_idx])
			{
				key =   mBuckets[bkt_idx][mItrEntryIdx]; 
				value = mValues[bkt_idx][mItrEntryIdx]; 
				mItrEntryIdx++;
				return true;
			}
			mItrBktIdx++;
		}
		return false;
	}

	OmnString getRandKey(const OmnString &dft)
	{
		// This function randomly returns a key
		int idx = rand() % eTableSize;

		// Find a non-empty bucket
		int guard = 0;
		while (!mNumElems[idx] && guard++ < (int)eTableSize)
		{
			idx++;
			if (idx >= (int)eTableSize) idx = 0;
		}

		if (!mNumElems[idx]) return dft;

		// Randomly pick one from the bucket
		int entryidx = rand() % mNumElems[idx];
		return mBuckets[idx][entryidx];
	}

	void printKeyDistribution()
	{
		u32 max = 0;
		u32 min = 1000;
		int group = 0;
		for (int i=0; i<eTableSize; i++)
		{
			if (group == 30)
			{
				cout << endl;
				group = 0;
			}
			group++;
			if (mNumElems[i] > max) max = mNumElems[i];
			if (mNumElems[i] < min) min = mNumElems[i];

			cout << mNumElems[i] << "\t";
		}
		cout << endl;
		cout << "max: " << max << endl;
		cout << "min: " << min << endl;
	}

	// Chen Ding, 2014/08/13
	char * nextEntryNew(C &value, int &len)
	{
		u32 size = mDirtyBuckets.size();
		if (size == 0 || mItrBktIdx >= size) return 0;

		u32 bkt_idx = mDirtyBuckets[mItrBktIdx];
		if (mItrEntryIdx < mNumElems[bkt_idx])
		{
			char *data = mBuckets[bkt_idx][mItrEntryIdx]; 
			value = mValues[bkt_idx][mItrEntryIdx]; 
			len = mStrLens[bkt_idx][mItrEntryIdx];
			mItrEntryIdx++;
			return data;
		}
		
		mItrBktIdx++;
		if (mItrBktIdx >= size) return 0;

		mItrEntryIdx = 0;
		while(mItrBktIdx < size)
		{
			bkt_idx = mDirtyBuckets[mItrBktIdx];
			if(mItrEntryIdx < mNumElems[bkt_idx])
			{
				char *data =   mBuckets[bkt_idx][mItrEntryIdx]; 
				value = mValues[bkt_idx][mItrEntryIdx]; 
				len = mStrLens[bkt_idx][mItrEntryIdx];
				mItrEntryIdx++;
				return data;
			}
			mItrBktIdx++;
		}
		return 0;
	}
};

#endif

