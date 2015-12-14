////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array4Tester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "Util/Tester/Array4Tester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Array4.h"



bool OmnArray4Tester::start()
{
	// 
	// Test default constructor
	//
//	testInitialization();
	testInsert();
	return true;
}


bool
OmnArray4Tester::testInitialization()
{
	// 
	// Stack objects shall never be deleted by smart pointers. 
	// 
	OmnBeginTest << "Test Init for Array3";
	mTcNameRoot = "Array4-Init";

	int **** array = OmnNew int***[OMN_ARRAY4_TOTAL];
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(OmnArray41Init<int>(array, 10))) << endtc;

	for (int i1=0; i1<256; i1++)
	{
		cout << "i1: " << i1 << endl;

		for (int i2=0; i2<256; i2++)
		{
			cout << "  i2: " << i2 << endl;
			for (int i3=0; i3<256; i3++)
			{
				for (int i4=0; i4<256; i4++)
				{
					OmnTC(OmnExpected<int>(10), OmnActual<int>(array[i1][i2][i3][i4])) << endtc;
				}
			}
		}
	}


	return true;
}


bool
OmnArray4Tester::testInsert()
{
/*	int i;
	OmnBeginTest << "Test Insert for Array3";
	mTcNameRoot = "Array3-Insert";

	int **** array = OmnNew int***[OMN_ARRAY4_TOTAL];
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(OmnArray41Init(array, 10))) << endtc;

	// 32-63, 64-95, 172-175, 0-255
	unsigned char index[4] = {32,   64, 172, 0};
	unsigned char mask[4]  = {0xe0, 0xe0, 0xfc, 0};

	int rslt = OmnArray41Insert(array, index, mask, 11, 10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	for (i=32; i<64; i++)
	{
		verify(array, i, 80, 172, 175, 0, 255, 11);
	}

	verify(array, 31, 80, 172, 175, 0, 255, 10);
	verify(array, 64, 80, 172, 175, 0, 255, 10);
	verify(array, 32, 63, 172, 175, 0, 255, 10);
	verify(array, 32, 96, 172, 175, 0, 255, 10);

	for (i=64; i<96; i++)
	{
		verify(array, 32, i, 172, 175, 0, 255, 11);
	}

	// 32-39, 72-79, 0-255, 0-255
	unsigned char index1[4] = {32, 72, 0, 0};
	unsigned char mask1[4]  = {0xf8, 0xf8, 0, 0};
	rslt = OmnArray41Insert(array, index1, mask1, 12, 10);
	for (i=32; i<40; i++)
	{
		verify(array, i, 71, 0,   171, 0, 255, 10);
		verify(array, i, 71, 172, 175, 0, 255, 11);
		verify(array, i, 71, 176, 255, 0, 255, 10);
		for (int j=72; j<80; j++)
		{
			verify(array, i, j, 0, 255, 0, 255, 12);
		}
	}

	for (i=32; i<40; i++)
	{
		verify(array, i, 71, 172, 175, 0, 255, 11);
	}
*/
	// 32-39, 76-77, 0-255, 0-255
/*	unsigned char index2[4] = {32, 76, 0, 0};
	unsigned char mask2[4]  = {0xf8, 0xfe, 0, 0};
	rslt = OmnArray41Insert(array, index2, mask2, 13, 10);
	for (i=32; i<40; i++)
	{
//		verify(array, i, 71, 172, 175, 0, 255, 11);
//		verify(array, i, 72, 0, 255, 0, 255, 12);
//		verify(array, i, 73, 0, 255, 0, 255, 12);
//		verify(array, i, 74, 0, 255, 0, 255, 12);
//		verify(array, i, 75, 0, 255, 0, 255, 12);
//		verify(array, i, 76, 0, 255, 0, 255, 13);
//		verify(array, i, 77, 0, 255, 0, 255, 13);
//		verify(array, i, 78, 0, 255, 0, 255, 12);
//		verify(array, i, 79, 0, 255, 0, 255, 12);
//		verify(array, i, 80, 172, 175, 0, 255, 11);
	}
*/
	return true;
}


bool
OmnArray4Tester::verify(int **** array, 
						const int index1, const int index2, 
						const int min1,   const int max1,
						const int min2,   const int max2,
						const int value)
{
	int i1 = index1;
	int i2 = index2;
	for (int i3=0; i3<=255; i3++)
	{
		for (int i4=0; i4<=255; i4++)
		{
			if (i3 >= min1 && i3 <= max1 &&	i4 >= min2 && i4 <= max2)
			{
				OmnTC(OmnExpected<int>(value), OmnActual<int>(array[i1][i2][i3][i4])) 
					<< "Index: " << i1 << ", " << i2 << ", " << i3 << ", " << i4 << endtc;
			}
		}
	}

	return true;
}
