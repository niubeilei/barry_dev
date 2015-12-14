////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Random.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Random.h"

#include "Util/String.h"
#include "Porting/TimeOfDay.h"

// 
// This function returns a string of random values. 
// The function uses gettimeofday() to generate
// the first four bytes. If 'bufflen' needs more than 
// four bytes, the function uses these values as the seeds
// in calling srand() to generate more random values. 
//
// Parameters:
//	'buff': OUT
//		The random string to be generated.
//
//	'bufflen': IN
//		The length of the random string to be generated.
//		The caller must be sure 'buff' enough memory.
//
void AosRandom_get(char *buff, u32 bufflen)
{
	u32 index = 0;
	unsigned int seedIndex = 0;
	char seeds[4];
	struct timeval tv;
	int finished;

	OmnGetTimeOfDay(&tv);
	seeds[0] = (tv.tv_sec >> 5) + tv.tv_usec;
	seeds[1] = (tv.tv_sec >> 9) + (tv.tv_usec >> 7);
	seeds[2] = (tv.tv_sec >> 3) + (tv.tv_usec >> 9);
	seeds[3] = (tv.tv_sec >> 11) + (tv.tv_usec >> 3);

	if (seeds[0] == 0) seeds[0] = (tv.tv_sec >> 1);
	if (seeds[1] == 0) seeds[1] = (tv.tv_sec >> 2);
	if (seeds[2] == 0) seeds[2] = (tv.tv_sec >> 3);
	if (seeds[3] == 0) seeds[3] = (tv.tv_sec >> 4);

	// 
	// The first four bytes will use the seeds.
	//
	while (index < bufflen && index < 4)
	{
		buff[index] = seeds[index];
		index++;
	}

	if (index >= bufflen)
	{
		// 
		// That's all. Go home!
		//
		return;
	}

	while (index < bufflen)
	{
		seedIndex = (seedIndex & 0x03);
		seedIndex++;
//		aos_random(buff[seedIndex]);	

		finished = 0;
		while (index < bufflen && !finished)
		{
			int value = aos_random();
			u32 shift = 3;
			int bytes = 0;

			while (index < bufflen && bytes < 4)
			{
				buff[index] = (value >> shift);
				if (buff[index] == 0)
				{
					finished = 1;
					break;
				}

				index++;
				bytes++;
				shift += 2;

			}
		}
	}
}

