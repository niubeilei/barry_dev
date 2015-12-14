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
// [1, 2, 3, 4, 5] based on the weights. 
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Util_RandomSelector_h
#define Omn_Util_RandomSelector_h

#include "aosUtil/Types.h"

class AosRandomSelector
{
public:
	enum 
	{
		eMaxSelectors = 100
	};

private:
    int			mIndex[eMaxSelectors];
	u16         mWeights[eMaxSelectors];
	int			mDistribution[100];
	u32 		mNumElems;
	bool		mIsGood;

public:
	AosRandomSelector();
	AosRandomSelector(int * index, u16 * weights, const u32 numElems);
	~AosRandomSelector();

	int 		next() const;	
	bool		setIndex(int *index, u16 *weights, const u32 numElems);
	bool		isGood() const {return mIsGood;}
};

#endif

