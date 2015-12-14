////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomSelector.h
// RandomSelector randomly selects an element from a list based on a
// random distribution model. As an example, below is a random distribution
// model:
//     Index      Weight
//     1          10%
//     2          5%
//     3          15%
//     4          20%
//     5          50%
//
// Given this random selection model, this class will randomly select 
// [1, 2, 3, 4, 5] based on the weights. Note that weights are always 
// represented as an integer in the range of [0, 100].
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Util/RandomSelector.h"

#include "Alarm/Alarm.h"
#include "Random/RandomUtil.h"


AosRandomSelector::AosRandomSelector()
	:
mNumElems(0),
mIsGood(false)
{
	memset(mIndex, 0, sizeof(int) * eMaxSelectors);
	memset(mWeights, 0, sizeof(u16) * eMaxSelectors);
	memset(mDistribution, 0, sizeof(int) * 100);
}


AosRandomSelector::AosRandomSelector(int * index, u16 * weights, const u32 numElems)
{
	mIsGood = setIndex(index, weights, numElems);
}


bool
AosRandomSelector::setIndex(int * index, u16 * weights, const u32 numElems)
{
	// Make sure the array is not too big
	if (numElems >= eMaxSelectors)
	{
		OmnAlarm << "Too many elements: " << (int)numElems << enderr;
		return false;
	}

	mNumElems = numElems;

	// Copy the two arrays
	memcpy(mIndex, index, sizeof(int) * numElems);
	memcpy(mWeights, weights, sizeof(u16) * numElems);

	// The weights must add to 100. If not, we will modify the last one to make it 100
	u16 sum = 0;
	for (u32 i=0; i<numElems; i++)
	{
		sum += weights[i];
	}

	if (sum != 100)
	{
		OmnWarn << "The total of the weights is not 100: " << sum 
			<< ". The last weight will be adjusted to make it 100!" << enderr;
	}

	if (mWeights[0] > 100)
	{
		mWeights[0] = 100;
	}

	sum = 0;
	for (u32 i=0; i<numElems; i++)
	{
		sum += weights[i];
		if (sum == 100)
		{
			// Set the remaining weights to be 0
			for (u32 j=i+1; j<numElems; j++)
			{
				mWeights[j] = 0;
			}
			break;
		}

		if (sum > 100)
		{
			sum -= weights[i];
			mWeights[i] = 100 - sum;

			for (u32 j=i+1; j<numElems; j++)
			{
				mWeights[j] = 0;
			}
			break;
		}
	}

	int idx = 0;
	int m = 0;
	for (u32 i=0; i<100; i++)
	{
		u32 w = mWeights[idx];
		for (u32 j=0; j<w; j++)
		{
			if (m >= 100)
			{
				OmnAlarm << "Index out of bound: " << (int)m << enderr;
				return false;
			}

			mDistribution[m++] = mIndex[idx]; 
		}
		idx++;
	}

	mIsGood = true;
	return true;
}


AosRandomSelector::~AosRandomSelector()
{
}


// 
// This function selects an index based on the weights
//
int 		
AosRandomSelector::next() const
{
	int v = OmnRandom::nextInt1(0, 99);
	return mDistribution[v];
}

