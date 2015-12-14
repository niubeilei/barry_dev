////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: EnumInt.cpp
// Description:
//	Traveling in SuZhou, China. Nothing to do. So here is the 
//  code I wrote.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/String.h"
#include "Tester/EnumInt.h"

#include "aosUtil/Memory.h"
#include "Porting/Random.h"


OmnEnumInt::OmnEnumInt(const int *values, 
					   const int num_values, 
					   const int defaultValue)
:
mDefault(defaultValue),
mCrt(0)
{
	mValues = (int *)aos_malloc(sizeof(int) * num_values);

	memcpy(mValues, values, sizeof(int) * num_values);
	mNumValues = num_values;
}


int
OmnEnumInt::nextValue(bool &status)
{
	if (!mValues || mCrt >= mNumValues)
	{
		status = false;
		return mDefault;
	}

	int value = mValues[mCrt++];
	status = true;
	return value;
}
