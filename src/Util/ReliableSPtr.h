////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ReliableSPtr.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Snt_Util_ReliableSPtr_h
#define Snt_Util_ReliableSPtr_h

/*
#include "Util/SPtrInterface.h"
#include "Util/ValList.h"



class SntReliableSPtr
{
	SntSPtrInterface				mPrimary;
	SntValList<SntSPtrInterface>	mSecondary;
	int								mCount;
	int								mMaxSize;

public:
	SntReliableSPtr(const int maxSize = 5)
				:
	mPrimary(0),
	mCount(0),
	mMaxSize(maxSize),
	mSecondary(0)
	{
	}
	
	~SntReliableSPtr()
	{
		delete mPrimary; mPrimary = 0;
		delete [] mSecondary; mSecondary = 0;
	}

	C * operator -> () {return mPrimary;}
	bool	isFull() const {return mCount == mMaxSize;}
	C * operator [] (const int index) const
	{
		if (index == 0)
		{
			return mPrimary;
		}

		if (index > mCount)
		{
			return 0;
		}

		return mSecondary[index-1];
	}

	size_t entries() const 
	{
		if (!mPrimary)
		{
			return 0;
		}

		return mCount+1;
	}

	bool isNull() const {return (!mPrimary && !mSecondary);}

	bool removePtr(const C * ptr)
	{
		//
		// Check whether it is the primary.
		//
		if (mPrimary == ptr)
		{
			delete mPrimary;
			if (mCount>0 && mSecondary[0])
			{
				mPrimary = mSecondary[0];
				for (int i=mCount-1; i>0; i--)
				{
					mSecondary[i-1] = mSecondary[i];
				}
				mSecondary[mCount-1] = 0;
				return true;
			}
		}

		//
		// Check whether it is in the secondary.
		//
		for (int i=0; i<mCount; i++)
		{
			if (mSecondary[i] == ptr)
			{
				delete mSecondary[i];
				for (int j=mCount-1; j>i; j--)
				{
					mSecondary[j-1] = mSecondary[j];
				}
				mSecondary[mCount-1] = 0;
				return true;
			}
		}

		//
		// This means it is not found.
		//
		return false;
	}

	bool append(C * ptr)
	{
		//
		// Check whether the mPrimary is null
		//
		if (!mPrimary)
		{
			mPrimary = ptr;
			return true;
		}

		if (mCount == 0)
		{
			mSecondary = OmnNew C*[mMaxSize];
			for (int i=0; i<mMaxSize; i++)
			{
				mSecondary[i] = 0;
			}
			mSecondary[0] = ptr;
			mCount = 1;
			return true;
		}

		//
		// Search for an empty slot
		//
		for (int i=0; i<mMaxSize; i++)
		{
			if (!mSecondary[i])
			{
				mSecondary[i] = ptr;
				mCount++;
				return true;
			}
		}

		//
		// This means it is overflowed. Not inserted.
		//
		return false;
	}


	C * prepend(C * ptr)
	{
		//
		// It insert the pointer to the beginning of the list. If it overflows,
		// the last one is popped up and returned. Otherwise, NULL is returned.
		//

		//
		// Check whether the mPrimary is null
		//
		if (!mPrimary)
		{
			mPrimary = ptr;
			return 0;
		}

		//
		// This means that the primary is not null. 
		// If the secondary is null, initialize it, and move the primary into it
		// and set the primary to the new one.
		//
		if (mCount == 0)
		{
			mSecondary = OmnNew C*[mMaxSize];
			for (int i=0; i<mMaxSize; i++)
			{
				mSecondary[i] = 0;
			}
			mSecondary[0] = mPrimary;
			mPrimary = ptr;
			mCount = 1;
			return 0;
		}

		//
		// This means that the secondary is not null. Need to make a room. 
		//
		C *result = mSecondary[mMaxSize-1];
		for (int i=mMaxSize-1; i>0; i--)
		{
			mSecondary[i] = mSecondary[i-1];
		}
		mSecondary[0] = mPrimary;
		mPrimary = ptr;
		if (!result)
		{
			//
			// This means nothing popped out. Need to increment the count
			//
			mCount++;
		}
		return result;
	}
};

*/
#endif
