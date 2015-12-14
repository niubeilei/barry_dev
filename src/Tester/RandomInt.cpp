////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: RandomInt.cpp
// Description:
//	Traveling in SuZhou, China. Nothing to do. So here is the 
//  code I wrote.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/String.h"
#include "Tester/RandomInt.h"

#include "Porting/Random.h"



int
OmnRandomInt::nextValue(bool &status)
{
	status = true;
/*	while (1)
	{
		int value = (int)OmnRandom();

		if (value >= mMin && value <= mMax)
		{
			return value;
		}
	}
*/
	return 0;
}
