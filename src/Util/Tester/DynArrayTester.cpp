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
// 02/21/2007 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/DynArrayTester.h"

#include "Debug/Debug.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/DynArray.h"
#include <limits.h>
#include <string.h>

const int sgArrayInitSize = 100;
const int sgArrayIncSize = 50;
const int sgArrayMaxSize = 10000;

static int sgTestData[sgArrayMaxSize];
static u32 sgNumElems = 0;
static OmnDynArray<int, sgArrayInitSize, sgArrayIncSize, sgArrayMaxSize> sgArray;


bool OmnDynArrayTester::start()
{
	/* Commented out by Chen Ding, 04/10/2012
	// 
	// Test default constructor
	//
	memset(sgTestData, 0, sizeof(sgTestData));
	sgNumElems = 0;

	u32 tries = sgArrayMaxSize + 10;
	// u32 tries = 5;

	torture();

	while (tries)
	{
		int idx = aos_next_int(0, 1);
		switch (idx)
		{
		case 0:
			 testAppend();
			 break;

		case 1:
			 testSetValue();
			 break;
		}

		check();
		tries--;
	}
	*/

	return true;
}

int iii = 0;

bool
OmnDynArrayTester::torture()
{
	/* Commented out by Chen Ding, 04/10/2012
	int tries = 10000000;
	for (int i=0; i<tries; i++)
	{
		OmnDynArray<int, 10, 10, 1000> thearray;

		int innertries = 100;
		for (int k=0; k<innertries; k++)
		{
			thearray[k] = k;
		}
	}
	*/
	return true;
}


bool
OmnDynArrayTester::testAppend()
{
	/* Commented out by Chen Ding, 04/10/2012
	int num = aos_next_int(0, 10);
	for (int i=0; i<num; i++)
	{
		int value = aos_next_int(0, INT_MAX);

		if (sgArray.reachedCapacity())
		{
			OmnTC(OmnExpected<int>(-1), OmnActual<int>(sgArray.append(value))) << endtc;
		}
		else
		{
			OmnTC(OmnExpected<int>(sgNumElems), OmnActual<int>(sgArray.append(value))) << endtc;
			sgTestData[sgNumElems++] = value;
		}
	}
	*/

	return true;
}


bool
OmnDynArrayTester::testSetValue()
{
	/*
	int num = aos_next_int(0, 30);
	for (int i=0; i<num; i++)
	{
		int value = aos_next_int(0, INT_MAX);
		u32 delta = (u32)aos_next_int(0, 100);
		u32 pos = (u32)aos_next_int(0, sgNumElems+delta);

		if (pos >= sgNumElems)
		{
			OmnTC(OmnExpected<bool>(false), 
				OmnActual<bool>(sgArray.setValue(pos, value))) << endtc;

			if (sgArray.setValue(pos, value))
			{
				cout << "Failed" << endl;
			}
		}
		else
		{
			OmnTC(OmnExpected<bool>(true), 
				OmnActual<bool>(sgArray.setValue(pos, value))) << endtc;
			sgTestData[pos] = value;
		}
	}
	*/

	return true;
}


bool
OmnDynArrayTester::check()
{
	int ret = memcmp((void *)sgTestData, sgArray.getArrayPointer(), sgNumElems*4);
	if (ret)
	{
		cout << "sgNumElems: " << sgNumElems << endl;
		for (u32 i=0; i<sgNumElems; i++)
		{
			if (sgTestData[i] != sgArray[i])
			{
				cout << i << ":" << sgTestData[i] << ":" << sgArray[i] << endl;
			}
		}
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;

	return true;
}

