////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SlabTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aosUtil/Tester/SlabTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"

#include "aosUtil/Slab.h"



bool AosSlabTester::start()
{
	// 
	// Test default constructor
	//
	basicTest();
	testKernel();
	return true;
}


bool AosSlabTester::basicTest()
{
	bool ret;
	int i;
	const int eMaxBlocks = 9999+1;
	void *ptrs[eMaxBlocks];

	OmnBeginTest << "Test AosSlab";
	mTcNameRoot = "SlabTest";

	// Test the constructor
	AosSlab *theSlab;
	ret = AosSlab_constructor("Test", 999+1, eMaxBlocks, &theSlab);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	// Destroy the slab
	ret = AosSlab_destructor(theSlab);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	for (i=0; i<99999+1; i++)
	{
		ret = AosSlab_constructor("Test", 999+1, 9999+1, &theSlab);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

		ret = AosSlab_destructor(theSlab);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}

	// Test Get and release
	ret = AosSlab_constructor("Test", 999+1, 9999+1, &theSlab);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	
	void *block = AosSlab_get(theSlab);
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(block != 0)) << endtc;
	
	ret = AosSlab_release(theSlab, block);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	for (i=0; i<99999+1; i++)
	{
		void *block = AosSlab_get(theSlab);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(block != 0)) << endtc;
	
		ret = AosSlab_release(theSlab, block);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}

	// Test maximum blocks
	for (i=0; i<eMaxBlocks; i++)
	{
		ptrs[i] = AosSlab_get(theSlab);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptrs[i] != 0)) << endtc;
	}

	// Release all
	for (i=0; i<eMaxBlocks; i++)
	{
		ret = AosSlab_release(theSlab, ptrs[i]);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}

	// Getting maximum blocks again
	for (i=0; i<eMaxBlocks; i++)
	{
		ptrs[i] = AosSlab_get(theSlab);
		OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptrs[i] != 0)) << endtc;
	}
	
	// Release all
	for (i=0; i<eMaxBlocks; i++)
	{
		ret = AosSlab_release(theSlab, ptrs[i]);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	}
	return true;
}


bool AosSlabTester::testKernel()
{
	// int ret;

	OmnBeginTest << "Test AosSlab Kernel";
	mTcNameRoot = "KernelSlabTest";

	// OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
	return true;
}



#if 0
bool OmnSlabTester::start()
{
	// 
	// Test default constructor
	//
	randomTest();
	return true;
}

const int arraysize = 500000;
static void *ptrs[arraysize];

bool OmnSlabTester::randomTest()
{
	struct aosSlab *slab = aosSlab_constructor("Test", 10, 999+1, arraysize+1);

	OmnTC(OmnExpected<int>(999+1), OmnActual<int>(slab->mSize)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)slab->mHead)) << endtc;
	OmnTC(OmnExpected<int>(arraysize+1), OmnActual<int>(slab->mMaxBlocks)) << endtc;
	OmnTC(OmnExpected<int>(10), OmnActual<int>(slab->mId)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(slab->mTotalAllocated)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(slab->mIdleBlocks)) << endtc;
	OmnTC(OmnExpected<int>(AOS_SLAB_MINIDLEBLOCKS), OmnActual<int>(slab->mMinIdleBlocks)) << endtc;
	OmnTC(OmnExpected<OmnString>("Test"), OmnActual<OmnString>(slab->mName)) << endtc;
 
	int index = 0;
	int tries = 0;
	int rslt;
	while (tries++ < 9999+1)
	{
		if ((tries % 999+1) == 0)
		{
			cout << "Tries: " << tries
				<< ", index: " << index << endl;
		}

		if (OmnRandom::nextInt() & 0x11)
		{
			void *ptr = aosSlab_allocate(slab);
			OmnTC(OmnExpected<bool>(true), OmnActual<bool>(ptr != 0))
				<< index << ", tries: " << tries << endtc;

			if (!ptr)
			{
				// 
				// Run out of memory
				//
				continue;
			}

			if (index >= arraysize)
			{
				rslt = aosSlab_release(slab, ptr);
				OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
				continue;
			}
			
			ptrs[index++] = ptr;
		}
		else 
		{
			if (index > 0)
			{
		 		int idx = OmnRandom::nextInt(0, index-1);
				rslt = aosSlab_release(slab, ptrs[idx]);
				OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
				ptrs[idx] = ptrs[index-1];
				index--;
			}
		}

 		rslt = aosSlab_check(slab);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	}

	for (int i=0; i<index; i++)
	{
		rslt = aosSlab_release(slab, ptrs[i]);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	}

	rslt = aosSlab_check(slab);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	OmnTC(OmnExpected<int>(slab->mTotalAllocated), OmnActual<int>(slab->mIdleBlocks)) << endtc;
	
	int expected = 1;
	while (expected)
	{
		if (slab->mIdleBlocks <= slab->mMinIdleBlocks)
		{
			expected = 0;
		}
		else
		{
			expected = slab->mIdleBlocks / 2 * slab->mSize;
		}
		int purged = aosSlab_purge(slab);
		cout << "Expected: " << expected << ", Purged: " << purged << endl;
 		OmnTC(OmnExpected<int>(expected), OmnActual<int>(purged)) 
			<< expected << ", " << purged << endtc;
	}

	return true;
}

#endif

