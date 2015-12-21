////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array3Tester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/Array3Tester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Array3.h"



bool OmnArray3Tester::start()
{
	// 
	// Test default constructor
	//
//	testInitialization();
	testInsert();
	return true;
}


bool
OmnArray3Tester::testInitialization()
{
	// 
	// Stack objects shall never be deleted by smart pointers. 
	// 
	OmnBeginTest << "Test Init for Array3";
	mTcNameRoot = "Array4-Init";

	OmnArray3<char> array;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(OmnArray3Init(&array, 'a'))) << endtc;

	for (int i1=0; i1<256; i1++)
	{
		cout << "i1: " << i1 << endl;

		for (int i2=0; i2<256; i2++)
		{
			for (int i3=0; i3<256; i3++)
			{
				OmnTC(OmnExpected<char>('a'), OmnActual<char>(array.mData[i1][i2][i3])) << endtc;
			}
		}
	}


	return true;
}


bool
OmnArray3Tester::testInsert()
{
	OmnBeginTest << "Test Insert for Array3";
	mTcNameRoot = "Array3-Insert";

	OmnArray3<char> array;
	OmnTC(OmnExpected<bool>(true), OmnActual<bool>(OmnArray3Init(&array, 'a'))) << endtc;

	unsigned char index[3];
	index[0] = 5;
	index[1] = 10;
	index[2] = 20;
	unsigned char mask[3];
	mask[0] = (unsigned char)0;
	mask[1] = (unsigned char)0xfc;
	mask[2] = (unsigned char)0xff;

	int rslt = OmnArray3Insert(&array, index, mask, 'b');

	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	
	int i1, i2, i3;
	for (i1=0; i1<256; i1++)
	{
		cout << "i1 = " << i1 << endl;
		for (i2=0; i2<256; i2++)
		{
			for (i3=0; i3<256; i3++)
			{
				// int k = (i2 & 0xfc);
				// int l = (10 & 0xfc);
				if ( i1 == 20 && (i2 & 0xfc) == (10 & 0xfc))
				{
					OmnTC(OmnExpected<char>('b'), OmnActual<char>(array.mData[i1][i2][i3])) << endtc;
				}
				else
				{
					OmnTC(OmnExpected<char>('a'), OmnActual<char>(array.mData[i1][i2][i3])) << endtc;
				}
			}
		}
	}

	return true;
}
#endif
