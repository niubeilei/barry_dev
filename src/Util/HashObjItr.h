////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HashObjItr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_Util_HashObjItr_h
#define Omn_Util_HashObjItr_h


template <class C, unsigned int cTableSize>
class OmnObjHashItr 
{
private:
	OmnDynArray<C>	mBuckets[cTableSize+1];
	int				mBucketIdx;
	int				mElemIdx;
	C				mDefault;
	int				mObjCnt;

public:
	OmnObjHashItr(const OmnObjHash<C, size> &hash)
		:
	mBucketIdx(0),
	mElemIdx(0),
	mDefault(hash.getDefault()),
	mObjCnt(0)
	{
		for (int i=0; i<tableSize+1; i++)
		{
			mBuckets[i] = hash.mBuckets[i];
		}

		reset();
	}

	virtual ~OmnObjHashItr()
	{
	}

	void	reset()
	{
		// 
		// Need to move the indexes to the first valid element.
		//
		mBucketIdx = 0;
		mElemIdx = 0;
		mObjCnt = 0;
		while (mBucketIdx < cTableSize+1)
		{
			if (mBuckets[mBucketIdx].entries() > 0)
			{
				// 
				// Found the first bucket that is not empty. 
				//
				return;
			}
			
			mBucketIdx++;
		}
	}

	C		crt() 
	{
		if (mBucketIdx >= cTableSize+1)
		{
			// 
			// There is no current. This is normally the case that the hash
			// table is empty.
			//
			return mDefault;
		}

		return mBuckets[mBucketIdx][mElemIdx];
	}

	C		next()
	{
		C c;
		if (mBucketIdx < cTableSize+1)
		{
			// 
			// mBucketIdx must be pointing to a non-empty bucket. 
			//
			c = mBuckets[mBucketIdx][mElemIdx];
		}
		else 
		{
			c = mDefault;
		}

		// 
		// Move to the next element
		//
		mElemIdx++;
		while (mElemIdx >= mBuckets[mBucketIdx].entries())
		{
			// 
			// Need to move to the next bucket
			//
			mElemIdx = 0;
			mBucketIdx++;
			if (mBucketIdx >= cTableSize+1)
			{
				// 
				// All buckets have been traversed. 
				//
				break;
			}

		}
		
		return c;
	}

	bool	hasMore() const
	{
		return (mBucketIdx < cTableSize+1);
	}

	int		objCount() const {return mItr.objCount();}

};


#endif
