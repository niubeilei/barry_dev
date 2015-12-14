////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastLookupTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "Util/Tester/FastLookupTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Random/RandomUtil.h"
// #include "Util/TArray.h"



bool OmnFastLookupTester::start()
{
	// 
	// Test default constructor
	//
	randomTester();
	return true;
}


// 
// It assumes a domain name starts by the first space and in the form:
//	www.www[.www[.www]]     
// where www is a substring that contains no spaces.
//
bool OmnFastLookupTester::randomTester()
{
/* Chen Ding, 11/03/2005
	int rslt;

	OmnBeginTest << "FastLookup Random Tester";
	mTcNameRoot = "RandomFastLookup";

	const int length = 15;
	void **array = 0;
	rslt = OmnTArray_init(&array, length, (void*)0);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	unsigned char value[4] = {'.', '.', '.', ' '};
	int idx = 0;
	int f;
	int endIdx;
	const int maxEntries = 2000;
	unsigned char fields[4];
	unsigned char index[maxEntries][length];
	unsigned char mask[maxEntries][length];

	// 
	// Set the first field
	//
	int numEntries = 0;
	for (int f1=1; f1<=length-8; f1++)
	{
		for (int f2=1; f2<=length-8; f2++)
		{
			for (int f3=1; f3<length-8; f3++)
			{
				for (int f4=1; f4<length-8; f4++)
				{
					if (f1 + f2 + f3 + f4 + 4 <= length)
					{
						fields[0] = f1;
						fields[1] = f2; 
						fields[2] = f3; 
						fields[3] = f4;

						idx = 0;
						for (int p=0; p<4; p++)
						{
							for (f=0; f<fields[p]; f++)
							{
								if (idx >= length)
								{
									cout << "*************" << endl;
								}
								index[numEntries][idx] = 0;
								mask[numEntries][idx++]  = 0;
							}

							if (idx >= length)
							{
								cout << "*************" << endl;
							}
							index[numEntries][idx] = value[p];
							mask[numEntries][idx++] = 255;
						}
						endIdx = idx-2;

						for (; idx<length; idx++)
						{
							index[numEntries][idx] = 0;
							mask[numEntries][idx] = 0;
						}


						numEntries++;
						cout << "numEntries: " << numEntries << endl;
					}
				}
			}
		}
	}

	int nn1, nn2;
	for (int i=0; i<numEntries; i++)
	{
		cout << " [" 
			<< (unsigned int)index[i][0] << ", " << (unsigned int)index[i][1] << ", " << (unsigned int)index[i][2] << ", "
			<< (unsigned int)index[i][3] << ", " << (unsigned int)index[i][4] << ", " << (unsigned int)index[i][5] << ", "
			<< (unsigned int)index[i][6] << ", " << (unsigned int)index[i][7] << ", " << (unsigned int)index[i][8] << ", "
			<< (unsigned int)index[i][9] << ", " << (unsigned int)index[i][10] << ", " << (unsigned int)index[i][11] << ", "
			<< (unsigned int)index[i][12] << ", " << (unsigned int)index[i][13] << ", " << (unsigned int)index[i][14] 
			<< "] " << i << endl;
		cout << " ["
			<< (unsigned int)mask[i][0] << ", " << (unsigned int)mask[i][1] << ", " << (unsigned int)mask[i][2] << ", "
			<< (unsigned int)mask[i][3] << ", " << (unsigned int)mask[i][4] << ", " << (unsigned int)mask[i][5] << ", "
			<< (unsigned int)mask[i][6] << ", " << (unsigned int)mask[i][7] << ", " << (unsigned int)mask[i][8] << ", "
			<< (unsigned int)mask[i][9] << ", " << (unsigned int)mask[i][10] << ", " << (unsigned int)mask[i][11] << ", "
			<< (unsigned int)mask[i][12] << ", " << (unsigned int)mask[i][13] << ", " << (unsigned int)mask[i][14]
			<< "] " << i << endl;

		rslt = OmnTArray_insert1((void***)array, index[i], mask[i], (void*)i, 0);
		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

		for (int j=i; j>=0; j--)
		{
			rslt = OmnTArray_remove1((void ***)array, index[j], mask[j]);
			OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		}
		 
		OmnTC(OmnExpected<int>(0), OmnActual<int>((int)((int*)array)[OMN_TARRAY_ENTRIES])) << endtc;
		OmnTC(OmnExpected<int>(0), OmnActual<int>((int)((int*)array)[OMN_TARRAY_ENTRYHEAD])) << endtc;
 
		int numIdle; 
		int numAllocated;
		OmnTArray_getSlab(&numIdle, &numAllocated);
		OmnTC(OmnExpected<int>(numIdle+14), OmnActual<int>(numAllocated))
			<< "Idle: " << numIdle << ", Allocated: " << numAllocated << endtc;

		for (int k=0; k<=i; k++)
		{
			rslt = OmnTArray_insert1((void***)array, index[k], mask[k], (void*)k, 0);
			OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		}

		nn2 = OmnTArray_getNumEntries((void ***)array);
		cout << "Number of entries: " << nn2 << ", " << nn2 - nn1 << endl;
		nn1 = nn2;
		char c;
		cin >> c;
	}
*/

 		/*
	void * ********************theArray = (void * ********************)array;
	char dn[20] = "www.abc.com";
	void *ptr = theArray
		[(unsigned char)dn[0]]
		[(unsigned char)dn[1]]
		[(unsigned char)dn[2]]
		[(unsigned char)dn[3]]
		[(unsigned char)dn[4]]
		[(unsigned char)dn[5]]
		[(unsigned char)dn[6]]
		[(unsigned char)dn[7]]
		[(unsigned char)dn[8]]
		[(unsigned char)dn[9]]
		[(unsigned char)dn[10]]
		[(unsigned char)dn[11]]
		[(unsigned char)dn[12]]
		[(unsigned char)dn[13]]
		[(unsigned char)dn[14]]
		[(unsigned char)dn[15]]
		[(unsigned char)dn[16]]
		[(unsigned char)dn[17]]
		[(unsigned char)dn[18]]
		[(unsigned char)dn[19]];
	*/

	return true;
}
