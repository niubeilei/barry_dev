////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TSArrayTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/TSArrayTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Random/RandomUtil.h"
#include "Util/TSArray.h"


const int sgDefaultValue = 10;
static OmnRandom sgRandom;


bool OmnTSArrayTester::start()
{
	// 
	// Test default constructor
	//
//	testInitialization();
	randomTester();
//	specialTest1();
//	specialTest2();
	return true;
}


bool
OmnTSArrayTester::testInitialization()
{
	/*
	// 
	// Stack objects shall never be deleted by smart pointers. 
	// 
	OmnBeginTest << "Test Init for Array3";
	mTcNameRoot = "TSArray-Init";


	void **array = 0;
	int ret = OmnTSArray_init(&array, 4, (void*)10);
	if (ret)
	{
		OmnTrace << "Failed" << endl;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		return false;
	}

	void * ****theArray = (void * ****)array;
	int v = (int)theArray[0][0][0][0];
	v = 1;
	for (int i1=0; i1<25; i1++)
	{
		cout << "i1: " << i1 << endl;

		for (int i2=0; i2<25; i2++)
		{
			cout << "  i2: " << i2 << endl;
			for (int i3=0; i3<25; i3++)
			{
				for (int i4=0; i4<25; i4++)
				{
					OmnTC(OmnExpected<int>(10), OmnActual<int>((int)theArray[i1][i2][i3][i4])) << endtc;
				}
			}
		}
	}

	*/
	return true;
}


bool
OmnTSArrayTester::randomAddDelete()
{
	struct OmnEntryDef1		entryList[eMaxEntry];
	int entries = 0;
	int rslt;

	OmnBeginTest << "TSArray Random Tester";
	mTcNameRoot = "Random4";

	void **array = 0;
	rslt = OmnTSArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	while (entries < eMaxEntry)
	{
		// 
		// Create the next entry
		//
		for (int i=0; i<eDimension; i++)
		{
			entryList[entries].mIndex[i] = sgRandom.nextInt(0, 255);
			entryList[entries].mValue = sgRandom.nextInt();
			entryList[entries].mIsInsert = (sgRandom.nextInt() & 0x03) != 0;
		}

		// 
		// Insert it. 
		//
		cout << "	addEntry(theArray, "
			<< (int)entryList[entries].mIndex[0] << ", " 
			<< (int)entryList[entries].mIndex[1] << ", " 
			<< (int)entryList[entries].mIndex[2] << ", " 
			<< (int)entryList[entries].mIndex[3] << ", " 
			<< entryList[entries].mValue << ", "
			<< entryList[entries].mIsInsert << ");";

		if (entryList[entries].mIsInsert)
		{
			rslt = OmnTSArray_insert((void***)array, 
				entryList[entries].mIndex, 
				(void*)entryList[entries].mValue, 0);
		}
		else
		{
			rslt = OmnTSArray_remove((void ***)array, 
				entryList[entries].mIndex);
		}

		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

		entries++;

		if ((entries % 20) == 0)
		{
			cout << "// Entry : " << entries;
		}

		cout << endl; 
	}

	cout << "To remove" << endl;
	
	for (int i=entries-1; i>=0; i--)
	{
		if (entryList[i].mIsInsert)
		{
		cout << "	removeEntry(theArray, "
			<< (int)entryList[i].mIndex[0] << ", " 
			<< (int)entryList[i].mIndex[1] << ", " 
			<< (int)entryList[i].mIndex[2] << ", " 
			<< (int)entryList[i].mIndex[3] << ", " << endl; 

			rslt = OmnTSArray_remove((void ***)array, entryList[i].mIndex);
			OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		}
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)array[OMN_TSARRAY_ENTRIES])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)array[OMN_TSARRAY_ENTRYHEAD])) << endtc;

	OmnTSArray_dump((void ***)array);

	return true;
}


bool
OmnTSArrayTester::randomTester()
{
	struct OmnEntryDef1 entryList[eMaxEntry];
	int entries = 0;
	int rslt;

	OmnBeginTest << "TSArray Random Tester";
	mTcNameRoot = "Random4";

	void **array = 0;
	rslt = OmnTSArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	while (entries < eMaxEntry)
	{
		// 
		// Create the next entry
		//
		for (int i=0; i<eDimension; i++)
		{
			entryList[entries].mIndex[i]	= sgRandom.nextInt(0, 255);
		}
		entryList[entries].mValue		= sgRandom.nextInt();
		entryList[entries].mIsInsert	= (sgRandom.nextInt() & 0x03) != 0;

		// 
		// Insert it. 
		//
		cout << "    addEntry(theArray, "
			<< (int)entryList[entries].mIndex[0] << ", " 
			<< (int)entryList[entries].mIndex[1] << ", " 
			<< (int)entryList[entries].mIndex[2] << ", " 
			<< (int)entryList[entries].mIndex[3] << ", " 
			<< entryList[entries].mValue << ", "
			<< entryList[entries].mIsInsert << ");" << endl;

		if (entryList[entries].mIsInsert)
		{
			rslt = OmnTSArray_insert((void***)array, 
				entryList[entries].mIndex, 
				(void*)entryList[entries].mValue, 0);
		}
		else
		{
			rslt = OmnTSArray_remove((void ***)array, 
				entryList[entries].mIndex);
		}

		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

		entries++;
		verify(array, entryList, entries);
	}

	return true;
}


bool
OmnTSArrayTester::verify(void **arr, struct OmnEntryDef1 *entryList, const int entries)
{
	unsigned char index[eDimension];
	void * ****array = (void *****)arr;
	static int numHit = 0;

	for (int i=0; i<eTriesPerVerification; i++)
	{
		for (int j=0; j<eDimension; j++)
		{
			index[j] = (unsigned char)sgRandom.nextInt(0, 255); 
		}

		check(array, index[0], index[1], index[2], index[3], entryList, entries, &numHit);
	}

	// 
	// Find the right value
	//
	unsigned char v1 = entryList[entries-1].mIndex[0];
	unsigned char v2 = entryList[entries-1].mIndex[1];
	unsigned char v3 = entryList[entries-1].mIndex[2];
	unsigned char v4 = entryList[entries-1].mIndex[3];

	unsigned char tries[4][3] = 
	{
		{v1-1, v1, v1+1},
		{v2-1, v2, v2+1},
		{v3-1, v3, v3+1},
		{v4-1, v4, v4+1}
	};

	for (int m1=0; m1<3; m1++)
	{
		for (int m2=0; m2<3; m2++)
		{
			for (int m3=0; m3<3; m3++)
			{
				for (int m4=0; m4<3; m4++)
				{
					check(array, 
						tries[0][m1], 
						tries[1][m2],
						tries[2][m3],
						tries[3][m4],
						entryList, entries, &numHit);
				}
			}
		}
	}

	if (entries % 50 == 0)
	{
		OmnTrace << "  Hit: " << numHit << ", entries: " << entries << endl;
	}

	return true;
}


bool
OmnTSArrayTester::check(void * ****array, 
					   const unsigned char i1, 
					   const unsigned char i2, 
					   const unsigned char i3, 
					   const unsigned char i4, 
					   struct OmnEntryDef1 *entryList, 
					   const int entries, 
					   int *hit)
{
	unsigned char index[4];
	index[0] = i1;
	index[1] = i2;
	index[2] = i3;
	index[3] = i4;
	bool found = false;

	for (int k=entries-1; k>=0; k--)
	{
		int expected;

		// 
		// Check whether it matches the entry
		//
		int m;
		for (m=0; m<eDimension; m++)
		{
			if (index[m] != entryList[k].mIndex[m])
			{
				break;
			}
		}

		// 
		// A match is found
		//
		if (m >= eDimension)
		{
			(*hit)++;
			found = true;
			if (entryList[k].mIsInsert)
			{
				expected = entryList[k].mValue;
			}
			else
			{
				expected = sgDefaultValue;
			}
		}

		if (found)
		{
			int actual = (int)array[i1][i2][i3][i4];
			if (actual != expected)
			{
				OmnTrace << "Not the same: " 
					<< "Actual: " << actual << ", Expected: " << expected
					<< "index: " << i1 << ", " << i2 << ", " << i3 << ", " << i4 << endl;
				exit(0);
			}

			OmnTC(OmnExpected<int>(expected), OmnActual<int>(actual))
				<< "Index: " << i1 << ", " << i2 << ", " 
				<< i3 << ", " << i4 << endtc;

			break;
		}
	}

	// 
	// Didn't find the match. 
	//
	if (!found)
	{
		int actual = (int)array[i1][i2][i3][i4];
		if (actual != sgDefaultValue)
		{
			OmnTrace << "not the same" << endl;
		}

		OmnTC(OmnExpected<int>(sgDefaultValue), OmnActual<int>((int)array[i1][i2][i3][i4])) 
			<< "Index: " << i1 << ", " << i2 << ", " 
			<< i3 << ", " << i4 << endtc;
	}

	return true;
}


int
OmnTSArrayTester::addEntry(void * ****array, 
		 unsigned char idx1, 
		 unsigned char idx2, 
		 unsigned char idx3, 
		 unsigned char idx4, 
		 int value, 
		 bool isInsert)
{
	cout << "To add entry: "
		<< "[" << (unsigned int)idx1 << ", "
		<< (unsigned int)idx2 << ", "
		<< (unsigned int)idx3 << ", "
		<< (unsigned int)idx4 << "]"
		<< ", value = " << value << ", is insert: " << isInsert << endl;

	unsigned char index[4] = {idx1, idx2, idx3, idx4};
	int rslt;
	if (isInsert)
	{
		rslt = OmnTSArray_insert((void***)array, index, (void*)value, 0);
	}
	else
	{
		rslt = OmnTSArray_remove((void ***)array, index);
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(OmnTSArray_checkArray((void ***)array))) <<endtc;
	return rslt;
}


bool
OmnTSArrayTester::specialTest1()
{
	void **array = 0;
	int rslt = OmnTSArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

    addEntry(theArray, 54, 207, 27, 110, 3868, 0);
    addEntry(theArray, 170, 68, 45, 248, 7477, 0);
    addEntry(theArray, 199, 240, 92, 150, 12929, 0);
    addEntry(theArray, 203, 95, 164, 137, 27450, 1);

	OmnTC(OmnExpected<int>(10), OmnActual<int>((int)theArray[227][83][239][165])) 
		<< "Index: 227, 83, 239, 165" << endtc;
	return true;
}


bool
OmnTSArrayTester::specialTest2()
{
	void **array = 0;
	int rslt = OmnTSArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

	addEntry(theArray, 234, 121, 147, 1, 18967, 0);
	addEntry(theArray, 30, 55, 122, 183, 11105, 1);

	OmnTC(OmnExpected<int>(11105), OmnActual<int>((int)theArray[30][55][122][183])) 
		<< "Index: 30, 55, 122, 183" << endtc;
	return true;
}


#endif
