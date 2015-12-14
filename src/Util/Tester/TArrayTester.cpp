////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TArrayTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Util/Tester/TArrayTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Random/RandomUtil.h"
#include "KernelUtil/TArray.h"
 
unsigned char sgMasks[8] = 
{
	0xff,
	0xfe,
	0xfc,
	0xf8, 
	0xf0, 
	0xe0,
	0xc0,
	0x80
};

int aosTotalUtilTestAlarms = 0;

int sgDefaultValue = 10;
int tmpMaxListSize = 0;
static OmnRandom sgRandom;


bool OmnTArrayTester::start()
{
	// 
	// Test default constructor
	//
//	testInitialization();
//	testInsert();
	randomTester();
//	randomAddDelete();
//	specialTest1();
//	specialTest2();
//	specialTest3();
//	specialTest4();
//	specialTest5();
//	specialTest6();
//	specialTest7();
//	specialTest8();
//	specialTest9();
//	specialTest10();
//	specialTest11();
//	dim13Test();
	return true;
}


bool
OmnTArrayTester::testInitialization()
{
	// 
	// Stack objects shall never be deleted by smart pointers. 
	// 
	OmnBeginTest << "Test Init for Array3";
	mTcNameRoot = "TArray-Init";

	void **array = 0;
	int ret = OmnTArray_init(&array, 4, (void*)10);
	if (ret)
	{
		OmnTrace << "Failed" << endl;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		return false;
	}

	void * ****theArray = (void * ****)array;
	int vv = (int)theArray[0][0][0][0];
	vv = vv;
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


	return true;
}


bool
OmnTArrayTester::testInsert()
{
	OmnBeginTest << "Test Insert for Array3";
	mTcNameRoot = "Array3-Insert";

	void **array = 0;
	int rslt;
	int ret = OmnTArray_init(&array, 4, (void*)10);
	if (ret)
	{
		OmnTrace << "Failed" << endl;
		OmnTC(OmnExpected<int>(0), OmnActual<int>(ret)) << endtc;
		return false;
	}

 	unsigned char index[4] = {32,   64, 172, 0};
	unsigned char mask[4]  = {0xe0, 0xe0, 0xfc, 0};

	rslt = OmnTArray_insert1((void***)array, index, mask, (void*)11, 0);

	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTArray_remove1((void ***)array, index, mask);

	int i;
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
	rslt = OmnTArray_insert1((void***)array, index1, mask1, (void *)12, 0);
	OmnTArray_remove1((void ***)array, index1, mask1);

	for (i=32; i<40; i++)
	{
		cout << "i = " << i << endl;
		verify(array, i, 71, 0,   171, 0, 255, 10);
		verify(array, i, 71, 172, 175, 0, 255, 11);
		verify(array, i, 71, 176, 255, 0, 255, 10);
		for (int j=72; j<80; j++)
		{
			cout << "   j = " << j << endl;
			verify(array, i, j, 0, 255, 0, 255, 12);
		}
	}

	for (i=32; i<40; i++)
	{
		cout << "ii = " << i << endl;
		verify(array, i, 71, 172, 175, 0, 255, 11);
	}

	// 32-39, 76-77, 0-255, 0-255
	unsigned char index2[4] = {32, 76, 0, 0};
	unsigned char mask2[4]  = {0xf8, 0xfe, 0, 0};
	rslt = OmnTArray_insert1((void ***)array, index2, mask2, (void *)13, 0);
	OmnTArray_remove1((void ***)array, index2, mask2);
	for (i=32; i<40; i++)
	{
		cout << "i = " << i << endl;
		verify(array, i, 71, 172, 175, 0, 255, 11);
		verify(array, i, 72, 0, 255, 0, 255, 12);
		verify(array, i, 73, 0, 255, 0, 255, 12);
		verify(array, i, 74, 0, 255, 0, 255, 12);
		verify(array, i, 75, 0, 255, 0, 255, 12);
		verify(array, i, 76, 0, 255, 0, 255, 13);
		verify(array, i, 77, 0, 255, 0, 255, 13);
		verify(array, i, 78, 0, 255, 0, 255, 12);
		verify(array, i, 79, 0, 255, 0, 255, 12);
		verify(array, i, 80, 172, 175, 0, 255, 11);
	}

	rslt = OmnTArray_insert1((void***)array, index, mask, (void*)11, 0);
	rslt = OmnTArray_insert1((void***)array, index1, mask1, (void *)12, 0);
	rslt = OmnTArray_insert1((void ***)array, index2, mask2, (void *)13, 0);
	OmnTArray_remove1((void ***)array, index, mask);
	
	OmnTArray_remove1((void ***)array, index1, mask1);
	OmnTArray_remove1((void ***)array, index2, mask2);

	return true;
}


bool
OmnTArrayTester::verify(void ** arr, 
						const int index1, const int index2, 
						const int min1,   const int max1,
						const int min2,   const int max2,
						const int value)
{
	int ****array = (int ****)arr;

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


bool
OmnTArrayTester::randomAddDelete()
{
	struct OmnEntryDef		entryList[eMaxEntry];
	int entries = 0;
	int rslt;

	OmnBeginTest << "TARRAY Random Tester";
	mTcNameRoot = "Random4";

	void **array = 0;
	rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	while (entries < eMaxEntry)
	{
		// 
		// Create the next entry
		//
		for (int i=0; i<eDimension; i++)
		{
			entryList[entries].mIndex[i] = sgRandom.nextInt(0, 255);
			entryList[entries].mMask[i] = sgMasks[sgRandom.nextInt(0, 7)];
			entryList[entries].mValue = sgRandom.nextInt();
			entryList[entries].mIsInsert = (sgRandom.nextInt() & 0x03) != 0;
		}

		// 
		// Insert it. 
		//
		cout << "	addEntry(theArray, "
			<< (int)entryList[entries].mIndex[0] << ", " << (int)entryList[entries].mMask[0] << ", "
			<< (int)entryList[entries].mIndex[1] << ", " << (int)entryList[entries].mMask[1] << ", "
			<< (int)entryList[entries].mIndex[2] << ", " << (int)entryList[entries].mMask[2] << ", "
			<< (int)entryList[entries].mIndex[3] << ", " << (int)entryList[entries].mMask[3] << ", "
			<< entryList[entries].mValue << ", "
			<< entryList[entries].mIsInsert << ");";

		if (entryList[entries].mIsInsert)
		{
			rslt = OmnTArray_insert1((void***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask, 
				(void*)entryList[entries].mValue, 0);
		}
		else
		{
			rslt = OmnTArray_remove1((void ***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask);
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
			<< (int)entryList[i].mIndex[0] << ", " << (int)entryList[i].mMask[0] << ", "
			<< (int)entryList[i].mIndex[1] << ", " << (int)entryList[i].mMask[1] << ", "
			<< (int)entryList[i].mIndex[2] << ", " << (int)entryList[i].mMask[2] << ", "
			<< (int)entryList[i].mIndex[3] << ", " << (int)entryList[i].mMask[3] << ");" << endl; 

			rslt = OmnTArray_remove1((void ***)array, entryList[i].mIndex, entryList[i].mMask);
			OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		}
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)array[OMN_TARRAY_ENTRIES])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)array[OMN_TARRAY_ENTRYHEAD])) << endtc;

	OmnTArray_dump((void ***)array);

	return true;
}


bool
OmnTArrayTester::randomTester()
{
	struct OmnEntryDef entryList[eMaxEntry];
	int entries = 0;
	int rslt;

	OmnBeginTest << "TARRAY Random Tester";
	mTcNameRoot = "Random4";

	void **array = 0;
	rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	while (entries < eMaxEntry)
	{
		// 
		// Create the next entry
		//
		for (int i=0; i<eDimension; i++)
		{
			entryList[entries].mIndex[i]	= sgRandom.nextInt(0, 255);
			entryList[entries].mMask[i]		= sgMasks[sgRandom.nextInt(0, 7)];
			entryList[entries].mValue		= sgRandom.nextInt();
			// entryList[entries].mIsInsert	= (sgRandom.nextInt() & 0x03) != 0;
			entryList[entries].mIsInsert	= true;
		}

		// 
		// Insert it. 
		//
		cout << "    addEntry(theArray, "
			<< (int)entryList[entries].mIndex[0] << ", " << (int)entryList[entries].mMask[0] << ", "
			<< (int)entryList[entries].mIndex[1] << ", " << (int)entryList[entries].mMask[1] << ", "
			<< (int)entryList[entries].mIndex[2] << ", " << (int)entryList[entries].mMask[2] << ", "
			<< (int)entryList[entries].mIndex[3] << ", " << (int)entryList[entries].mMask[3] << ", "
			<< entryList[entries].mValue << ", "
			<< entryList[entries].mIsInsert << ");" << entries << "of"<<eMaxEntry<<endl;

		if (entryList[entries].mIsInsert)
		{
			rslt = OmnTArray_insert1((void***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask, 
				(void*)entryList[entries].mValue, 0);
		}
		else
		{
			rslt = OmnTArray_remove1((void ***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask);
		}

		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

		entries++;
		std::cout<<1<<endl;
		verify(array, entryList, entries);
		std::cout<<2<<endl;
	}

	return true;
}


bool
OmnTArrayTester::verify(void **arr, struct OmnEntryDef *entryList, const int entries)
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
	unsigned char min1 = (entryList[entries-1].mIndex[0] & entryList[entries-1].mMask[0]);
	unsigned char max1 = (entryList[entries-1].mIndex[0] | (~entryList[entries-1].mMask[0]));
	unsigned char min2 = (entryList[entries-1].mIndex[1] & entryList[entries-1].mMask[1]);
	unsigned char max2 = (entryList[entries-1].mIndex[1] | (~entryList[entries-1].mMask[1]));
	unsigned char min3 = (entryList[entries-1].mIndex[2] & entryList[entries-1].mMask[2]);
	unsigned char max3 = (entryList[entries-1].mIndex[2] | (~entryList[entries-1].mMask[2]));
	unsigned char min4 = (entryList[entries-1].mIndex[3] & entryList[entries-1].mMask[3]);
	unsigned char max4 = (entryList[entries-1].mIndex[3] | (~entryList[entries-1].mMask[3]));

	int tries[4][4] = 
	{
		{min1-1, min1, max1, max1+1},
		{min2-1, min2, max2, max2+1},
		{min3-1, min3, max3, max3+1},
		{min4-1, min4, max4, max4+1}
	};

	for (int m1=0; m1<4; m1++)
	{
		for (int m2=0; m2<4; m2++)
		{
			for (int m3=0; m3<4; m3++)
			{
				for (int m4=0; m4<4; m4++)
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
OmnTArrayTester::check(void * ****array, 
					   const unsigned char i1, 
					   const unsigned char i2, 
					   const unsigned char i3, 
					   const unsigned char i4, 
					   struct OmnEntryDef *entryList, 
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
		unsigned char min, max;

		// 
		// Check whether it matches the entry
		//
		int m;
		for (m=0; m<eDimension; m++)
		{
			min = (entryList[k].mIndex[m] & entryList[k].mMask[m]);
			max = (entryList[k].mIndex[m] | (~entryList[k].mMask[m]));
			if (index[m] < min || index[m] > max)
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

				// 
				// Dump all the affected entries
				//
				for (int p=0; p<entries; p++)
				{
					if ((entryList[p].mIndex[0] & entryList[p].mMask[0]) <= i1 &&
						i1 <= (entryList[p].mIndex[0] | (~entryList[p].mMask[0])) &&
						(entryList[p].mIndex[1] & entryList[p].mMask[1]) <= i2 &&
						i2 <= (entryList[p].mIndex[1] | (~entryList[p].mMask[1])) &&
						(entryList[p].mIndex[2] & entryList[p].mMask[2]) <= i3 &&
						i3 <= (entryList[p].mIndex[2] | (~entryList[p].mMask[2])) &&
						(entryList[p].mIndex[3] & entryList[p].mMask[3]) <= i4 &&
						i4 <= (entryList[p].mIndex[3] | (~entryList[p].mMask[3])))
					{
						OmnTrace << "    addEntry(theArray, "
							<< (int)entryList[p].mIndex[0] << ", " << (int)entryList[p].mMask[0] << ", "
							<< (int)entryList[p].mIndex[1] << ", " << (int)entryList[p].mMask[1] << ", "
							<< (int)entryList[p].mIndex[2] << ", " << (int)entryList[p].mMask[2] << ", "
							<< (int)entryList[p].mIndex[3] << ", " << (int)entryList[p].mMask[3] << ", "
							<< entryList[p].mValue << ", "
							<< entryList[p].mIsInsert << ");" << endl;
					}
				}

			
				std::cout<<"Bugs???"<<endl;
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


bool
OmnTArrayTester::specialTest()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

	addEntry(theArray, 89, 248, 172, 224, 28, 224, 241, 224, 20867, 1);
	addEntry(theArray, 72, 128, 70, 252, 72, 192, 158, 254, 20966, 0);
	addEntry(theArray, 106, 192, 136, 128, 202, 192, 70, 248, 20755, 1);

	int value = (int)theArray[125][167][31][231];
	cout << "Value = " << value << endl;
	return true;
}


bool
OmnTArrayTester::specialTest1()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;
	addEntry(theArray, 169, 128, 125, 128, 26, 248, 218, 192, 7622, 1);
	addEntry(theArray, 193, 224, 119, 240, 187, 254, 237, 224, 13376, 1);	// Must
	addEntry(theArray, 250, 128, 113, 248, 143, 192, 170, 192, 14099, 1);
	addEntry(theArray, 138, 128, 55, 224, 99, 224, 176, 248, 29832, 1);		// Must
	OmnTArray_dump((void ***)array);

	OmnTC(OmnExpected<int>(13376), OmnActual<int>((int)theArray[212][114][186][241])) 
		<< "Index: 212, 114, 186, 241" << endtc;
	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


bool
OmnTArrayTester::specialTest2()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;
	addEntry(theArray, 169, 128, 125, 128, 26, 248, 218, 192, 7622, 1);
	addEntry(theArray, 193, 224, 119, 240, 187, 254, 237, 224, 13376, 1);
	addEntry(theArray, 250, 128, 113, 248, 143, 192, 170, 192, 14099, 1);

	OmnTArray_dump((void ***)array);

	OmnTC(OmnExpected<int>(13376), OmnActual<int>((int)theArray[212][114][186][241])) 
		<< "Index: 212, 114, 186, 241" << endtc;
	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


int
OmnTArrayTester::addEntry(void * ****array, 
		 unsigned char idx1, 
		 unsigned char mask1, 
		 unsigned char idx2, 
		 unsigned char mask2, 
		 unsigned char idx3, 
		 unsigned char mask3, 
		 unsigned char idx4, 
		 unsigned char mask4, 
		 int value, 
		 bool isInsert)
{
	unsigned char min1 = (unsigned char)(idx1 & mask1); 
	unsigned char min2 = (unsigned char)(idx2 & mask2); 
	unsigned char min3 = (unsigned char)(idx3 & mask3); 
	unsigned char min4 = (unsigned char)(idx4 & mask4); 

	unsigned char max1 = (unsigned char)(idx1 | (~mask1));
	unsigned char max2 = (unsigned char)(idx2 | (~mask2));
	unsigned char max3 = (unsigned char)(idx3 | (~mask3));
	unsigned char max4 = (unsigned char)(idx4 | (~mask4));
	
	cout << "To add entry: "
		<< "[" << (unsigned int)min1 << "-" << (unsigned int)max1 << "]"
		<< "[" << (unsigned int)min2 << "-" << (unsigned int)max2 << "]"
		<< "[" << (unsigned int)min3 << "-" << (unsigned int)max3 << "]"
		<< "[" << (unsigned int)min4 << "-" << (unsigned int)max4 << "]"
		<< ", value = " << value << ", is insert: " << isInsert << endl;

	unsigned char index[4] = {idx1, idx2, idx3, idx4};
	unsigned char mask[4]  = {mask1, mask2, mask3, mask4};
	int rslt;
	if (isInsert)
	{
		rslt = OmnTArray_insert1((void***)array, index, mask, (void*)value, 0);
	}
	else
	{
		rslt = OmnTArray_remove1((void ***)array, index, mask);
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(OmnTArray_checkArray((void ***)array))) <<endtc;
	return rslt;
}



bool
OmnTArrayTester::specialTest3()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

	addEntry(theArray, 155, 248, 54, 128, 46, 224, 37, 254, 11676, 1);
    addEntry(theArray, 242, 128, 23, 192, 32, 224, 99, 254, 10301, 1);
    addEntry(theArray, 149, 240, 111, 128, 97, 254, 28, 240, 17042, 1);

	OmnTArray_dump((void ***)theArray);
	OmnTC(OmnExpected<int>(11676), OmnActual<int>((int)theArray[158][121][56][37])) 
		<< "Index: 158, 121, 56, 37" << endtc;
	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


bool
OmnTArrayTester::specialTest4()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

	addEntry(theArray, 170, 128, 56, 128, 61, 254, 29, 255, 15736, 1);
	addEntry(theArray, 187, 192, 247, 128, 250, 192, 211, 224, 13868, 1);

	OmnTArray_dump((void ***)theArray);
	OmnTC(OmnExpected<int>(13868), OmnActual<int>((int)theArray[176][216][248][201])) 
		<< "Index: 176, 216, 248, 201" << endtc;
	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


bool
OmnTArrayTester::specialTest5()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

    addEntry(theArray, 75, 224, 6, 252, 55, 240, 198, 192, 22362, 1);
	addEntry(theArray, 116, 128, 115, 252, 65, 240, 27, 248, 24573, 1);
	addEntry(theArray, 20, 192, 220, 192, 37, 192, 115, 248, 9871, 1);
	addEntry(theArray, 122, 128, 63, 192, 71, 252, 215, 128, 28622, 1);

	OmnTArray_dump((void ***)theArray);
	OmnTC(OmnExpected<int>(10), OmnActual<int>((int)theArray[108][6][62][232])) 
		<< "Index: 108, 6, 62, 232" << endtc;
	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


bool
OmnTArrayTester::specialTest6()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;
  
    addEntry(theArray, 31, 128, 30, 252, 85, 240, 187, 128, 25983, 1);
    addEntry(theArray, 59, 248, 29, 128, 88, 192, 143, 240, 5533, 0);

	OmnTArray_dump((void ***)theArray);
	OmnTC(OmnExpected<int>(25983), OmnActual<int>((int)theArray[22][29][86][132])) 
		<< "Index: 22, 29, 86, 132" << endtc;
	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


bool
OmnTArrayTester::specialTest7()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

    addEntry(theArray, 62, 192, 179, 252, 70, 255, 162, 254, 14984, 1);
    addEntry(theArray, 247, 254, 41, 255, 114, 240, 180, 224, 21437, 0);
    addEntry(theArray, 132, 248, 69, 128, 86, 224, 103, 252, 4372, 0);
    addEntry(theArray, 52, 254, 28, 192, 7, 240, 72, 128, 12639, 1);
    addEntry(theArray, 158, 255, 178, 128, 207, 248, 190, 255, 9512, 1);
    addEntry(theArray, 160, 254, 34, 224, 158, 240, 23, 240, 1850, 1);
    addEntry(theArray, 177, 255, 89, 254, 159, 254, 115, 255, 11847, 0);
    addEntry(theArray, 226, 255, 150, 248, 253, 254, 123, 248, 5518, 1);
    addEntry(theArray, 7, 252, 163, 252, 224, 240, 234, 254, 5439, 0);
    addEntry(theArray, 220, 252, 171, 128, 23, 224, 119, 254, 7752, 1);
    addEntry(theArray, 161, 248, 142, 224, 78, 254, 255, 192, 19426, 1);
    addEntry(theArray, 3, 240, 120, 255, 93, 252, 255, 248, 13915, 1);
    addEntry(theArray, 109, 240, 167, 248, 67, 240, 205, 248, 1943, 0);
    addEntry(theArray, 130, 248, 9, 192, 49, 255, 138, 254, 16202, 1);
    addEntry(theArray, 121, 192, 58, 252, 71, 192, 62, 248, 3024, 1);
    addEntry(theArray, 102, 128, 221, 192, 102, 254, 13, 192, 20762, 1);
    addEntry(theArray, 21, 248, 42, 252, 34, 240, 205, 255, 19701, 1);
    addEntry(theArray, 153, 224, 177, 224, 129, 240, 248, 255, 1683, 1);
    addEntry(theArray, 59, 224, 84, 254, 155, 192, 160, 248, 20641, 1);
    addEntry(theArray, 194, 240, 173, 252, 242, 128, 36, 255, 18579, 0);
    addEntry(theArray, 78, 240, 121, 240, 83, 254, 82, 255, 7048, 1);
    addEntry(theArray, 85, 192, 78, 255, 183, 192, 98, 192, 32011, 1);
    addEntry(theArray, 63, 255, 96, 240, 181, 192, 229, 224, 9252, 1);
    addEntry(theArray, 19, 254, 78, 255, 148, 192, 13, 254, 28692, 1);
    addEntry(theArray, 115, 255, 220, 240, 87, 254, 46, 252, 24118, 1);
    addEntry(theArray, 46, 128, 0, 240, 210, 240, 105, 240, 12505, 1);
    addEntry(theArray, 42, 192, 28, 224, 56, 240, 233, 240, 27718, 1);
    addEntry(theArray, 138, 254, 165, 192, 184, 224, 146, 192, 8767, 1);
    addEntry(theArray, 93, 254, 148, 128, 219, 240, 245, 192, 12553, 0);
    addEntry(theArray, 54, 128, 75, 254, 8, 240, 57, 254, 21325, 1);
    addEntry(theArray, 243, 240, 252, 252, 209, 248, 91, 128, 26019, 1);
    addEntry(theArray, 126, 248, 168, 254, 226, 252, 126, 254, 31298, 1);
    addEntry(theArray, 14, 248, 97, 252, 249, 224, 145, 255, 5644, 1);
    addEntry(theArray, 237, 255, 150, 252, 118, 255, 58, 128, 7547, 1);
    addEntry(theArray, 226, 254, 137, 128, 162, 128, 184, 224, 15910, 1);
    addEntry(theArray, 175, 254, 178, 255, 80, 224, 79, 254, 5780, 0);
    addEntry(theArray, 173, 255, 157, 192, 145, 240, 106, 252, 24052, 1);
    addEntry(theArray, 193, 254, 40, 252, 11, 192, 173, 192, 2441, 1);
    addEntry(theArray, 46, 255, 126, 252, 3, 255, 112, 255, 30256, 1);
    addEntry(theArray, 43, 224, 250, 240, 48, 128, 201, 192, 20034, 0);
    addEntry(theArray, 120, 252, 204, 192, 218, 128, 112, 192, 23503, 1);
    addEntry(theArray, 46, 255, 91, 248, 197, 224, 177, 240, 9417, 1);
    addEntry(theArray, 22, 240, 38, 254, 84, 254, 197, 255, 16387, 1);
    addEntry(theArray, 158, 254, 237, 252, 17, 224, 117, 252, 498, 0);
    addEntry(theArray, 248, 224, 3, 255, 128, 254, 159, 252, 13295, 0);
    addEntry(theArray, 139, 192, 181, 192, 126, 224, 251, 224, 72, 0);
    addEntry(theArray, 140, 192, 157, 248, 28, 254, 146, 224, 7885, 1);
    addEntry(theArray, 19, 254, 32, 252, 118, 255, 225, 128, 26922, 1);
    addEntry(theArray, 248, 254, 178, 128, 51, 248, 215, 224, 28785, 1);
    addEntry(theArray, 147, 224, 34, 254, 177, 254, 246, 254, 9060, 0);
    addEntry(theArray, 60, 192, 138, 248, 219, 128, 206, 254, 31390, 0);
    addEntry(theArray, 138, 252, 117, 240, 139, 128, 34, 252, 1201, 1);
    addEntry(theArray, 125, 240, 216, 255, 47, 224, 162, 192, 22446, 1);
    addEntry(theArray, 205, 252, 134, 240, 59, 252, 236, 128, 14424, 1);
    addEntry(theArray, 68, 192, 21, 224, 85, 248, 192, 240, 928, 1);
    addEntry(theArray, 195, 240, 26, 252, 181, 254, 104, 248, 7821, 0);
    addEntry(theArray, 150, 248, 241, 255, 106, 255, 210, 128, 2417, 1);
    addEntry(theArray, 49, 252, 150, 128, 45, 252, 179, 128, 18318, 0);
    addEntry(theArray, 163, 254, 123, 248, 76, 252, 165, 128, 11830, 1);
    addEntry(theArray, 80, 192, 126, 254, 108, 128, 137, 192, 19341, 1);
    addEntry(theArray, 182, 240, 37, 255, 208, 240, 222, 254, 7616, 0);
    addEntry(theArray, 211, 255, 124, 248, 220, 248, 8, 128, 28347, 1);
    addEntry(theArray, 1, 252, 47, 248, 70, 255, 191, 192, 21165, 1);
    addEntry(theArray, 75, 252, 53, 224, 97, 192, 149, 255, 23082, 1);
    addEntry(theArray, 130, 192, 122, 128, 234, 252, 248, 254, 22089, 0);
    addEntry(theArray, 34, 192, 25, 128, 86, 240, 110, 252, 26524, 1);
    addEntry(theArray, 114, 252, 140, 224, 226, 192, 10, 255, 27899, 1);
    addEntry(theArray, 133, 128, 87, 248, 214, 240, 173, 128, 24582, 0);
    addEntry(theArray, 184, 255, 58, 252, 179, 192, 173, 224, 1386, 1);
    addEntry(theArray, 107, 254, 104, 248, 117, 240, 132, 254, 21434, 1);
    addEntry(theArray, 115, 255, 179, 240, 9, 128, 191, 254, 29447, 1);
    addEntry(theArray, 30, 224, 254, 240, 12, 248, 31, 252, 29756, 1);
    addEntry(theArray, 209, 252, 141, 192, 0, 224, 181, 192, 17710, 0);
    addEntry(theArray, 251, 254, 45, 192, 237, 254, 114, 224, 8032, 0);
    addEntry(theArray, 140, 252, 33, 224, 111, 128, 42, 128, 27046, 1);
    addEntry(theArray, 66, 254, 182, 254, 248, 224, 121, 128, 6708, 1);
    addEntry(theArray, 210, 192, 174, 255, 189, 128, 43, 255, 10498, 1);
    addEntry(theArray, 7, 255, 132, 248, 122, 240, 195, 255, 9275, 0);
    addEntry(theArray, 9, 248, 137, 224, 49, 224, 149, 254, 30263, 1);
    addEntry(theArray, 44, 128, 137, 248, 164, 224, 110, 128, 24274, 1);
    addEntry(theArray, 205, 254, 97, 240, 120, 192, 53, 224, 23706, 0);
    addEntry(theArray, 228, 128, 95, 128, 55, 192, 133, 254, 17458, 0);
    addEntry(theArray, 227, 240, 203, 255, 187, 248, 66, 192, 29802, 1);
    addEntry(theArray, 142, 252, 158, 248, 144, 192, 134, 255, 2646, 1);
    addEntry(theArray, 50, 224, 186, 252, 92, 248, 212, 224, 3091, 1);
    addEntry(theArray, 129, 128, 53, 254, 32, 252, 233, 192, 22997, 1);
    addEntry(theArray, 199, 224, 201, 192, 220, 224, 0, 192, 23050, 0);
    addEntry(theArray, 154, 254, 152, 248, 31, 254, 203, 248, 1721, 1);
    addEntry(theArray, 51, 248, 187, 128, 217, 254, 98, 255, 18505, 1);
    addEntry(theArray, 241, 240, 149, 252, 7, 255, 160, 252, 10146, 1);
    addEntry(theArray, 215, 255, 153, 240, 204, 224, 39, 128, 14202, 0);
    addEntry(theArray, 52, 252, 145, 252, 24, 252, 202, 254, 24411, 1);
    addEntry(theArray, 143, 240, 62, 255, 36, 248, 43, 192, 3930, 1);
    addEntry(theArray, 176, 252, 238, 254, 6, 128, 91, 252, 14301, 1);
    addEntry(theArray, 31, 248, 104, 254, 50, 224, 198, 240, 26717, 1);
    addEntry(theArray, 114, 254, 26, 254, 66, 254, 160, 192, 5686, 1);
    addEntry(theArray, 252, 248, 178, 252, 209, 255, 58, 192, 11932, 1);
    addEntry(theArray, 137, 254, 199, 255, 195, 128, 88, 248, 13466, 1);
    addEntry(theArray, 187, 128, 102, 224, 183, 192, 75, 252, 9343, 1);
    addEntry(theArray, 27, 252, 141, 252, 121, 252, 130, 254, 32665, 1);
    addEntry(theArray, 237, 252, 126, 240, 94, 224, 87, 224, 21174, 1);
    addEntry(theArray, 117, 192, 167, 128, 74, 128, 70, 255, 20409, 1);
    addEntry(theArray, 203, 255, 33, 254, 105, 252, 204, 255, 19700, 1);
    addEntry(theArray, 70, 255, 31, 240, 13, 255, 164, 128, 18628, 1);
    addEntry(theArray, 44, 254, 189, 192, 225, 128, 163, 240, 27334, 1);
    addEntry(theArray, 25, 128, 113, 128, 164, 240, 74, 255, 22701, 1);
    addEntry(theArray, 255, 252, 85, 224, 73, 252, 255, 224, 23128, 1);
    addEntry(theArray, 171, 128, 141, 224, 117, 255, 41, 255, 9133, 0);
    addEntry(theArray, 157, 192, 230, 128, 245, 240, 137, 224, 27633, 1);
    addEntry(theArray, 196, 255, 98, 255, 134, 248, 150, 255, 26922, 0);
    addEntry(theArray, 72, 192, 59, 224, 34, 192, 253, 248, 3313, 1);
    addEntry(theArray, 200, 255, 80, 224, 234, 255, 213, 128, 2790, 1);
    addEntry(theArray, 235, 240, 57, 255, 52, 248, 101, 224, 17894, 1);
    addEntry(theArray, 252, 252, 41, 252, 172, 248, 52, 255, 28441, 1);
    addEntry(theArray, 230, 255, 221, 254, 191, 224, 233, 240, 8531, 1);
    addEntry(theArray, 41, 252, 245, 255, 153, 224, 63, 252, 14398, 1);
    addEntry(theArray, 56, 248, 95, 248, 190, 255, 80, 224, 23844, 0);
    addEntry(theArray, 201, 224, 30, 254, 30, 224, 25, 240, 13772, 1);
    addEntry(theArray, 219, 255, 217, 254, 1, 254, 154, 128, 11910, 1);
    addEntry(theArray, 108, 248, 66, 128, 36, 252, 22, 255, 8767, 1);
    addEntry(theArray, 184, 240, 79, 128, 206, 192, 1, 248, 9660, 1);
    addEntry(theArray, 170, 252, 226, 255, 161, 252, 6, 248, 2306, 1);
    addEntry(theArray, 254, 240, 171, 240, 213, 252, 18, 248, 31611, 0);
    addEntry(theArray, 228, 248, 145, 224, 228, 252, 159, 128, 143, 1);
    addEntry(theArray, 78, 224, 140, 248, 66, 128, 28, 240, 15496, 0);
    addEntry(theArray, 190, 240, 40, 255, 32, 192, 72, 240, 11752, 0);
    addEntry(theArray, 19, 224, 224, 192, 56, 255, 208, 224, 26281, 1);
    addEntry(theArray, 146, 224, 80, 224, 20, 128, 53, 254, 8543, 0);
    addEntry(theArray, 86, 224, 18, 224, 220, 128, 44, 255, 8139, 1);
    addEntry(theArray, 234, 224, 186, 254, 138, 240, 63, 128, 27641, 1);
    addEntry(theArray, 218, 248, 148, 255, 20, 255, 78, 252, 27143, 0);
    addEntry(theArray, 171, 252, 238, 255, 47, 255, 2, 128, 2141, 1);
    addEntry(theArray, 133, 224, 101, 224, 37, 128, 74, 254, 2038, 1);
    addEntry(theArray, 45, 254, 58, 224, 17, 240, 133, 192, 28845, 1);
    addEntry(theArray, 65, 248, 170, 192, 87, 224, 154, 255, 7130, 1);
    addEntry(theArray, 1, 240, 35, 224, 237, 224, 178, 252, 19835, 0);
    addEntry(theArray, 184, 248, 109, 252, 180, 255, 198, 254, 17217, 1);
    addEntry(theArray, 25, 224, 246, 192, 72, 240, 120, 128, 22777, 1);
    addEntry(theArray, 124, 240, 185, 255, 192, 252, 48, 248, 15200, 1);
    addEntry(theArray, 1, 224, 248, 240, 18, 252, 245, 248, 16602, 0);
    addEntry(theArray, 95, 128, 67, 248, 223, 240, 240, 254, 30907, 1);
    addEntry(theArray, 114, 255, 162, 240, 150, 248, 175, 128, 25069, 1);
    addEntry(theArray, 133, 252, 223, 255, 112, 240, 197, 254, 8964, 1);
    addEntry(theArray, 84, 255, 226, 240, 29, 255, 165, 192, 25630, 1);
    addEntry(theArray, 31, 192, 4, 255, 67, 224, 217, 240, 15707, 0);
    addEntry(theArray, 160, 255, 149, 254, 97, 192, 179, 254, 32038, 1);
    addEntry(theArray, 148, 248, 190, 252, 0, 252, 161, 224, 4549, 1);
    addEntry(theArray, 112, 248, 232, 240, 27, 254, 23, 252, 2640, 1);
    addEntry(theArray, 232, 248, 50, 248, 117, 255, 215, 252, 11620, 0);
    addEntry(theArray, 202, 254, 213, 224, 231, 255, 223, 248, 13559, 0);
    addEntry(theArray, 49, 128, 191, 254, 99, 254, 148, 255, 24152, 0);
    addEntry(theArray, 71, 252, 162, 128, 31, 248, 152, 252, 2257, 1);
    addEntry(theArray, 206, 254, 135, 255, 208, 255, 121, 254, 10669, 1);
    addEntry(theArray, 15, 240, 171, 254, 192, 192, 22, 224, 24712, 1);
    addEntry(theArray, 193, 128, 166, 240, 30, 255, 63, 252, 24356, 1);
    addEntry(theArray, 120, 252, 131, 128, 69, 255, 190, 240, 21459, 1);
    addEntry(theArray, 63, 128, 196, 192, 132, 248, 34, 248, 466, 1);
    addEntry(theArray, 95, 128, 230, 128, 239, 224, 89, 252, 23246, 1);
    addEntry(theArray, 64, 255, 84, 254, 64, 254, 79, 254, 3186, 1);
    addEntry(theArray, 158, 248, 36, 252, 54, 240, 116, 240, 4129, 0);
    addEntry(theArray, 206, 224, 19, 248, 127, 224, 221, 255, 6869, 1);
    addEntry(theArray, 71, 192, 158, 224, 178, 224, 170, 252, 10459, 1);
    addEntry(theArray, 186, 255, 194, 192, 209, 128, 251, 254, 20989, 0);
    addEntry(theArray, 241, 128, 149, 240, 86, 224, 168, 252, 18955, 0);
    addEntry(theArray, 227, 240, 223, 254, 168, 128, 148, 252, 5015, 1);
    addEntry(theArray, 98, 248, 129, 248, 218, 128, 173, 252, 16589, 0);
    addEntry(theArray, 186, 254, 160, 248, 208, 240, 226, 254, 18833, 1);
    addEntry(theArray, 217, 240, 147, 224, 125, 252, 173, 252, 8015, 1);
    addEntry(theArray, 105, 224, 115, 248, 219, 255, 167, 255, 16501, 1);
    addEntry(theArray, 120, 255, 237, 255, 105, 255, 142, 192, 29193, 0);
    addEntry(theArray, 239, 252, 19, 248, 36, 255, 240, 255, 16622, 1);
    addEntry(theArray, 109, 254, 186, 192, 237, 192, 174, 240, 2607, 0);
    addEntry(theArray, 72, 128, 99, 252, 167, 128, 227, 252, 1812, 1);
    addEntry(theArray, 120, 192, 31, 128, 167, 224, 144, 255, 21204, 0);
    addEntry(theArray, 44, 128, 50, 128, 89, 192, 176, 192, 19615, 1); 

//	OmnTArray_dump((void ***)theArray);
//	OmnTC(OmnExpected<int>(25983), OmnActual<int>((int)theArray[22][29][86][132])) 
//		<< "Index: 22, 29, 86, 132" << endtc;
//	cout << "Value = " << (int)theArray[212][114][186][241] << endl;
	return true;
}


bool
OmnTArrayTester::specialTest8()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

    addEntry(theArray, 183, 128, 112, 240, 209, 254, 208, 254, 18604, 1);
    addEntry(theArray, 142, 192, 173, 254, 109, 192, 126, 128, 26599, 1);
    addEntry(theArray, 121, 192, 67, 248, 58, 192, 107, 255, 8445, 1);
    addEntry(theArray, 105, 192, 87, 240, 94, 248, 49, 192, 32557, 1);
    addEntry(theArray, 220, 255, 204, 192, 121, 254, 194, 255, 19260, 1);
    addEntry(theArray, 50, 255, 214, 254, 93, 192, 85, 128, 29623, 1);
    addEntry(theArray, 215, 255, 19, 192, 166, 255, 227, 254, 10302, 1);
    addEntry(theArray, 62, 248, 92, 192, 34, 240, 147, 252, 4089, 1);
    addEntry(theArray, 150, 128, 83, 240, 205, 192, 159, 254, 18768, 0);
    addEntry(theArray, 48, 192, 100, 224, 149, 248, 181, 248, 21539, 1);
    addEntry(theArray, 129, 224, 16, 224, 55, 248, 129, 240, 23240, 1);
    addEntry(theArray, 25, 255, 140, 224, 93, 248, 137, 255, 7670, 1);
    addEntry(theArray, 234, 192, 240, 192, 249, 248, 105, 255, 26623, 1);
    addEntry(theArray, 69, 192, 93, 192, 5, 128, 244, 248, 20918, 1);
    addEntry(theArray, 61, 248, 243, 128, 152, 224, 180, 248, 7200, 1);
    addEntry(theArray, 188, 224, 164, 254, 117, 255, 119, 254, 5935, 1);
    addEntry(theArray, 62, 128, 230, 240, 2, 128, 99, 255, 11410, 1);
    addEntry(theArray, 83, 255, 126, 128, 110, 252, 99, 240, 20996, 1);
    addEntry(theArray, 204, 255, 153, 192, 176, 128, 139, 255, 25851, 1);
    addEntry(theArray, 222, 252, 17, 254, 105, 255, 123, 252, 18396, 1);
    addEntry(theArray, 244, 240, 41, 248, 255, 254, 1, 252, 27755, 0);
    addEntry(theArray, 41, 192, 40, 224, 185, 252, 199, 224, 12980, 0);
    addEntry(theArray, 208, 255, 181, 255, 108, 252, 106, 128, 27467, 1);
    addEntry(theArray, 196, 255, 112, 252, 83, 224, 138, 252, 2485, 1);
    addEntry(theArray, 73, 224, 72, 128, 149, 254, 254, 128, 13707, 1);
    addEntry(theArray, 36, 192, 236, 240, 134, 224, 250, 128, 2497, 1);
    addEntry(theArray, 124, 252, 44, 252, 130, 254, 91, 128, 791, 1);
    addEntry(theArray, 175, 254, 139, 240, 57, 224, 130, 192, 18405, 0);
    addEntry(theArray, 38, 252, 224, 255, 200, 255, 84, 240, 17674, 1);
    addEntry(theArray, 254, 254, 194, 128, 83, 240, 226, 254, 30639, 1);
    addEntry(theArray, 114, 255, 229, 252, 21, 192, 131, 240, 15248, 1);
    addEntry(theArray, 245, 224, 189, 254, 130, 248, 15, 192, 2951, 1);
    addEntry(theArray, 53, 240, 21, 224, 46, 255, 48, 254, 12397, 1);
    addEntry(theArray, 231, 248, 99, 224, 207, 128, 241, 252, 97, 0);
    addEntry(theArray, 196, 255, 84, 224, 174, 128, 112, 254, 11822, 1);
    addEntry(theArray, 3, 192, 141, 252, 145, 254, 173, 248, 16668, 1);
    addEntry(theArray, 181, 224, 87, 254, 12, 240, 15, 255, 5891, 1);
    addEntry(theArray, 39, 255, 213, 240, 30, 240, 80, 252, 8820, 0);
    addEntry(theArray, 39, 255, 36, 128, 193, 240, 173, 254, 6955, 1);
    addEntry(theArray, 40, 240, 48, 255, 107, 248, 45, 128, 31727, 1);
    addEntry(theArray, 29, 254, 244, 252, 177, 255, 55, 240, 5340, 1);
    addEntry(theArray, 32, 224, 47, 240, 54, 248, 2, 248, 18678, 1);
    addEntry(theArray, 74, 252, 82, 224, 231, 224, 209, 252, 12555, 1);
    addEntry(theArray, 127, 240, 115, 248, 42, 248, 90, 255, 8802, 1);
    addEntry(theArray, 141, 224, 172, 248, 27, 192, 73, 128, 7618, 1);
    addEntry(theArray, 52, 128, 62, 240, 148, 224, 72, 240, 15275, 0);
    addEntry(theArray, 228, 224, 106, 252, 164, 255, 94, 224, 20124, 1);
    addEntry(theArray, 70, 254, 88, 248, 179, 128, 184, 255, 23581, 0);
    addEntry(theArray, 228, 192, 28, 224, 225, 192, 80, 255, 8178, 1);
    addEntry(theArray, 200, 248, 230, 240, 228, 248, 183, 248, 9968, 1);
    addEntry(theArray, 124, 255, 61, 255, 200, 255, 78, 254, 22314, 1);
    addEntry(theArray, 224, 248, 99, 254, 218, 254, 145, 254, 8366, 1);
    addEntry(theArray, 176, 128, 189, 240, 11, 255, 180, 224, 2433, 1);
    addEntry(theArray, 118, 240, 173, 254, 66, 192, 150, 252, 17498, 1);
    addEntry(theArray, 248, 192, 75, 255, 131, 192, 180, 252, 10113, 0);
    addEntry(theArray, 117, 128, 205, 224, 97, 128, 237, 192, 11598, 1);
    addEntry(theArray, 145, 254, 109, 252, 251, 128, 102, 254, 24941, 1);
    addEntry(theArray, 138, 240, 46, 192, 120, 252, 89, 248, 27480, 1);
    addEntry(theArray, 206, 252, 196, 192, 98, 240, 110, 255, 5951, 1);
    addEntry(theArray, 138, 240, 40, 240, 23, 252, 44, 192, 32399, 0);
    addEntry(theArray, 253, 252, 97, 192, 56, 128, 142, 254, 2706, 1);
    addEntry(theArray, 104, 252, 17, 252, 48, 240, 220, 254, 19124, 0);
    addEntry(theArray, 46, 128, 250, 252, 245, 240, 41, 192, 5034, 1);
    addEntry(theArray, 123, 255, 246, 240, 67, 248, 122, 192, 14091, 1);
    addEntry(theArray, 3, 248, 70, 255, 50, 240, 71, 255, 20688, 1);
    addEntry(theArray, 58, 254, 47, 252, 120, 224, 175, 224, 18794, 1);
    addEntry(theArray, 215, 255, 101, 252, 69, 252, 165, 252, 46, 1);
    addEntry(theArray, 228, 224, 131, 224, 87, 248, 207, 240, 17536, 0);
    addEntry(theArray, 183, 248, 241, 240, 182, 128, 58, 252, 20141, 0);
    addEntry(theArray, 23, 248, 86, 248, 59, 192, 237, 248, 24893, 1);
    addEntry(theArray, 237, 128, 103, 240, 244, 128, 53, 255, 24005, 1);
    addEntry(theArray, 198, 192, 37, 128, 143, 248, 92, 192, 6013, 0);
    addEntry(theArray, 167, 252, 214, 192, 4, 224, 255, 255, 20971, 1);
    addEntry(theArray, 221, 254, 45, 252, 63, 248, 253, 224, 201, 1);
    addEntry(theArray, 104, 254, 255, 254, 80, 240, 253, 252, 14199, 1);

	OmnTArray_dump((void ***)theArray);
	return true;
}


bool
OmnTArrayTester::specialTest9()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

	addEntry(theArray, 197, 224, 56, 248, 71, 254, 174, 192, 23637, 1);
	addEntry(theArray, 218, 255, 48, 192, 22, 248, 161, 192, 10431, 1);

	OmnTArray_dump((void ***)theArray);
	OmnTC(OmnExpected<int>(10431), OmnActual<int>((int)theArray[218][0][16][128])) 
		<< "Index: 218, 0, 16, 128" << endtc;
	return true;
}


bool
OmnTArrayTester::specialTest10()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

    addEntry(theArray, 85, 248, 90, 192, 205, 254, 186, 128, 6097, 1);
    addEntry(theArray, 65, 248, 151, 224, 35, 240, 58, 128, 3643, 0);
    addEntry(theArray, 67, 248, 176, 128, 25, 248, 104, 254, 26844, 0);
    addEntry(theArray, 236, 240, 27, 255, 231, 192, 118, 224, 5971, 0);
    addEntry(theArray, 66, 224, 204, 248, 3, 255, 192, 252, 28480, 0);
    addEntry(theArray, 176, 224, 59, 254, 33, 248, 253, 254, 17827, 1);
    addEntry(theArray, 250, 192, 33, 224, 73, 192, 0, 128, 11169, 1);
    addEntry(theArray, 30, 192, 33, 128, 65, 252, 26, 192, 28573, 1);
    addEntry(theArray, 157, 224, 33, 192, 254, 254, 24, 255, 11835, 1);
    addEntry(theArray, 120, 192, 35, 252, 68, 192, 209, 248, 28511, 1);
    addEntry(theArray, 249, 192, 175, 224, 249, 255, 39, 255, 6197, 1);
    addEntry(theArray, 52, 224, 235, 192, 228, 192, 88, 248, 17889, 1);
    addEntry(theArray, 94, 252, 96, 248, 74, 254, 163, 252, 4355, 1);
    addEntry(theArray, 253, 252, 220, 252, 214, 248, 143, 255, 14767, 1);
    addEntry(theArray, 65, 252, 190, 255, 16, 254, 62, 128, 2602, 1);
    addEntry(theArray, 154, 254, 101, 192, 62, 254, 83, 240, 31857, 1);
    addEntry(theArray, 193, 192, 195, 128, 184, 128, 213, 192, 26906, 1);
    addEntry(theArray, 159, 254, 203, 252, 39, 192, 117, 252, 6596, 1);
    addEntry(theArray, 235, 128, 123, 224, 177, 128, 41, 255, 7135, 1);
    addEntry(theArray, 149, 254, 12, 192, 236, 224, 189, 224, 2439, 1);
    addEntry(theArray, 122, 254, 182, 224, 86, 252, 82, 224, 13983, 0);
    addEntry(theArray, 104, 255, 4, 252, 30, 240, 241, 192, 22295, 0);
    addEntry(theArray, 214, 255, 61, 192, 133, 224, 150, 252, 29898, 1);
    addEntry(theArray, 113, 224, 157, 224, 38, 224, 124, 240, 10158, 1);
    addEntry(theArray, 155, 224, 207, 252, 93, 192, 182, 248, 27352, 1);
    addEntry(theArray, 233, 240, 23, 252, 139, 255, 84, 240, 2174, 0);
    addEntry(theArray, 126, 252, 175, 252, 24, 192, 98, 224, 20977, 1);
    addEntry(theArray, 124, 255, 227, 248, 87, 240, 93, 248, 23317, 1);
    addEntry(theArray, 174, 252, 223, 240, 184, 192, 81, 254, 9952, 1);
    addEntry(theArray, 13, 128, 33, 248, 138, 224, 10, 192, 25504, 1);
    addEntry(theArray, 3, 255, 54, 192, 175, 252, 65, 240, 8669, 1);
    addEntry(theArray, 74, 248, 157, 252, 197, 192, 154, 255, 2856, 1);
    addEntry(theArray, 17, 224, 95, 254, 80, 254, 101, 128, 30036, 0);
    addEntry(theArray, 174, 255, 212, 192, 91, 248, 33, 192, 8632, 1);
    addEntry(theArray, 136, 240, 201, 248, 24, 252, 103, 240, 16211, 1);
    addEntry(theArray, 158, 240, 255, 192, 131, 192, 175, 248, 13326, 1);
    addEntry(theArray, 56, 224, 238, 254, 94, 254, 57, 128, 4011, 1);
    addEntry(theArray, 101, 224, 228, 252, 16, 248, 176, 248, 11680, 0);
    addEntry(theArray, 225, 192, 53, 248, 105, 224, 176, 248, 21470, 1);
    addEntry(theArray, 53, 255, 110, 248, 51, 248, 162, 192, 29393, 1);
    addEntry(theArray, 156, 248, 32, 224, 228, 254, 0, 255, 7937, 1);
    addEntry(theArray, 123, 252, 96, 248, 107, 240, 225, 252, 29958, 1);
    addEntry(theArray, 167, 224, 14, 254, 212, 254, 145, 248, 29344, 1);
    addEntry(theArray, 105, 255, 33, 224, 134, 252, 2, 254, 25781, 1);
    addEntry(theArray, 226, 128, 111, 192, 159, 254, 238, 252, 1968, 1);
    addEntry(theArray, 217, 224, 20, 240, 83, 255, 119, 240, 1403, 0);
    addEntry(theArray, 35, 128, 125, 255, 197, 192, 7, 248, 8492, 1);
    addEntry(theArray, 70, 240, 52, 248, 138, 224, 24, 240, 30041, 1);
    addEntry(theArray, 26, 254, 178, 128, 48, 224, 124, 192, 9629, 1);
    addEntry(theArray, 246, 240, 51, 128, 251, 224, 158, 254, 12267, 1);
    addEntry(theArray, 50, 128, 3, 128, 175, 252, 35, 255, 19777, 1);
    addEntry(theArray, 53, 254, 129, 252, 26, 252, 235, 248, 27202, 1);
    addEntry(theArray, 42, 240, 9, 252, 156, 248, 166, 252, 959, 1);
    addEntry(theArray, 182, 254, 44, 192, 32, 240, 249, 128, 20750, 1);
    addEntry(theArray, 143, 224, 148, 248, 193, 255, 162, 128, 5965, 1);
    addEntry(theArray, 2, 248, 212, 128, 133, 192, 65, 248, 19032, 1);
    addEntry(theArray, 120, 252, 190, 252, 226, 254, 220, 254, 427, 0);
    addEntry(theArray, 205, 192, 72, 254, 17, 248, 1, 128, 11905, 0);
    addEntry(theArray, 252, 128, 148, 192, 76, 240, 161, 255, 24526, 0);
    addEntry(theArray, 36, 224, 141, 252, 67, 252, 113, 255, 31171, 1);
    addEntry(theArray, 182, 192, 189, 255, 20, 252, 159, 224, 3603, 1);
    addEntry(theArray, 229, 248, 138, 254, 61, 224, 2, 254, 32179, 1);
    addEntry(theArray, 185, 248, 16, 192, 228, 248, 208, 224, 24570, 0);
    addEntry(theArray, 117, 128, 211, 255, 238, 252, 244, 254, 25133, 0);
    addEntry(theArray, 105, 192, 221, 254, 133, 252, 216, 128, 12670, 1);
    addEntry(theArray, 87, 192, 165, 252, 121, 192, 56, 252, 26573, 1);
    addEntry(theArray, 2, 252, 179, 248, 246, 248, 246, 252, 9451, 1);
    addEntry(theArray, 57, 224, 108, 252, 194, 192, 237, 240, 31404, 1);
    addEntry(theArray, 95, 254, 135, 128, 88, 128, 39, 255, 18836, 1);
    addEntry(theArray, 65, 254, 151, 224, 48, 252, 117, 254, 29758, 1);
    addEntry(theArray, 213, 240, 54, 255, 120, 255, 182, 224, 16884, 1);
    addEntry(theArray, 58, 252, 223, 255, 3, 128, 233, 224, 4830, 0);
    addEntry(theArray, 212, 254, 6, 248, 98, 128, 180, 128, 16795, 1);
    addEntry(theArray, 223, 224, 139, 192, 91, 255, 192, 224, 24503, 0);
    addEntry(theArray, 62, 252, 166, 254, 77, 252, 241, 248, 4740, 1);
    addEntry(theArray, 159, 252, 162, 224, 234, 252, 252, 240, 47, 1);
    addEntry(theArray, 102, 192, 59, 254, 181, 224, 156, 224, 61, 0);
    addEntry(theArray, 133, 255, 154, 128, 140, 128, 37, 254, 16016, 1);
    addEntry(theArray, 226, 254, 29, 240, 205, 192, 231, 128, 19582, 1);
    addEntry(theArray, 15, 254, 49, 254, 144, 248, 169, 255, 16946, 1);
    addEntry(theArray, 38, 128, 106, 240, 27, 224, 209, 252, 15631, 1);
    addEntry(theArray, 175, 248, 78, 252, 112, 248, 95, 248, 27036, 1);
    addEntry(theArray, 180, 255, 201, 192, 244, 240, 115, 252, 286, 1);
    addEntry(theArray, 237, 224, 43, 240, 204, 252, 11, 252, 32396, 1);
    addEntry(theArray, 201, 224, 25, 192, 85, 192, 104, 240, 1654, 1);
    addEntry(theArray, 221, 240, 0, 254, 251, 192, 82, 224, 29341, 1);
    addEntry(theArray, 68, 252, 195, 248, 205, 254, 200, 252, 27254, 1);
    addEntry(theArray, 43, 240, 161, 252, 202, 240, 134, 192, 360, 1);
    addEntry(theArray, 247, 248, 90, 255, 198, 240, 128, 255, 29333, 1);
    addEntry(theArray, 183, 240, 5, 240, 11, 255, 221, 224, 24809, 0);
    addEntry(theArray, 57, 192, 32, 252, 155, 255, 186, 192, 15826, 1);
    addEntry(theArray, 18, 128, 248, 255, 155, 224, 74, 240, 29219, 1);
    addEntry(theArray, 252, 240, 168, 254, 159, 224, 248, 224, 13346, 0);
    addEntry(theArray, 109, 254, 135, 128, 252, 192, 24, 255, 5867, 1);

	OmnTArray_dump((void ***)theArray);
	//mnTC(OmnExpected<int>(10431), OmnActual<int>((int)theArray[218][0][16][128])) 
	//	<< "Index: 218, 0, 16, 128" << endtc;
	return true;
}


bool
OmnTArrayTester::specialTest11()
{
	void **array = 0;
	int rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	void * ****theArray = (void * ****)array;

	addEntry(theArray, 58, 255, 243, 240, 101, 240, 142, 128, 31242, 1);
	OmnTArray_dump((void ***)theArray);
	addEntry(theArray, 32, 224, 203, 192, 109, 254, 125, 128, 7074, 0);// Entry : 220
	OmnTArray_dump((void ***)theArray);
	addEntry(theArray, 35, 224, 220, 128, 73, 240, 65, 240, 24261, 1);

	return true;
}


bool
OmnTArrayTester::dim13Test()
{
//	struct OmnEntryDef1 entryList[eMaxEntry];
	int rslt;

	OmnBeginTest << "TARRAY Random Tester";
	mTcNameRoot = "Random13";

	void * *************array = 0;
	rslt = OmnTArray_init((void ***)&array, 13, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	randomCheck13(array);
//	randomAddDelete13();
	randomTester13();
	return true;
}


bool
OmnTArrayTester::randomCheck13(void * *************array)
{
	for (int i=0; i<10000; i++)
	{
		unsigned char i01 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i02 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i03 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i04 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i05 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i06 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i07 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i08 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i09 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i10 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i11 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i12 = (unsigned char)OmnRandom::nextInt(0, 255);
		unsigned char i13 = (unsigned char)OmnRandom::nextInt(0, 255);

//		cout << "Check: [" 
//			<< (int)i01 << ", " << (int)i02 << ", " << (int)i03 << ", " << (int)i04 << ", "
//			<< (int)i05 << ", " << (int)i06 << ", " << (int)i07 << ", " << (int)i08 << ", "
//			<< (int)i09 << ", " << (int)i10 << ", " << (int)i11 << ", " << (int)i12 << ", "
//			<< (int)i13 << "]" << endl;
		int v = (int)array[i01][i02][i03][i04][i05][i06][i07][i08][i09][i10][i11][i12][i13];
		v = 1;
	}

	return true;
}


bool
OmnTArrayTester::randomAddDelete13()
{
	struct OmnEntryDef13 entryList[eMaxEntry];
	int entries = 0;
	int rslt;

	OmnBeginTest << "TARRAY Random Tester";
	mTcNameRoot = "Random4";

	void **array = 0;
	rslt = OmnTArray_init(&array, 4, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	while (entries < eMaxEntry)
	{
		// 
		// Create the next entry
		//
		for (int i=0; i<13; i++)
		{
			entryList[entries].mIndex[i] = OmnRandom::nextInt(0, 255);
			entryList[entries].mMask[i] = sgMasks[sgRandom.nextInt(0, 7)];
			entryList[entries].mValue = OmnRandom::nextInt();
			entryList[entries].mIsInsert = (OmnRandom::nextInt() & 0x03) != 0;
		}

		// 
		// Insert it. 
		//
		cout << "	addEntry(theArray, "
			<< (int)entryList[entries].mIndex[0] << ", " << (int)entryList[entries].mMask[0] << ", "
			<< (int)entryList[entries].mIndex[1] << ", " << (int)entryList[entries].mMask[1] << ", "
			<< (int)entryList[entries].mIndex[2] << ", " << (int)entryList[entries].mMask[2] << ", "
			<< (int)entryList[entries].mIndex[3] << ", " << (int)entryList[entries].mMask[3] << ", "
			<< (int)entryList[entries].mIndex[4] << ", " << (int)entryList[entries].mMask[4] << ", "
			<< (int)entryList[entries].mIndex[5] << ", " << (int)entryList[entries].mMask[5] << ", "
			<< (int)entryList[entries].mIndex[6] << ", " << (int)entryList[entries].mMask[6] << ", "
			<< (int)entryList[entries].mIndex[7] << ", " << (int)entryList[entries].mMask[7] << ", "
			<< (int)entryList[entries].mIndex[8] << ", " << (int)entryList[entries].mMask[8] << ", "
			<< (int)entryList[entries].mIndex[9] << ", " << (int)entryList[entries].mMask[9] << ", "
			<< (int)entryList[entries].mIndex[10] << ", " << (int)entryList[entries].mMask[10] << ", "
			<< (int)entryList[entries].mIndex[11] << ", " << (int)entryList[entries].mMask[11] << ", "
			<< (int)entryList[entries].mIndex[12] << ", " << (int)entryList[entries].mMask[12] << ", "
			<< entryList[entries].mValue << ", "
			<< entryList[entries].mIsInsert << ");";

		if (entryList[entries].mIsInsert)
		{
			rslt = OmnTArray_insert1((void***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask, 
				(void*)entryList[entries].mValue, 0);
		}
		else
		{
			rslt = OmnTArray_remove1((void ***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask);
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
			<< (int)entryList[i].mIndex[0] << ", " << (int)entryList[i].mMask[0] << ", "
			<< (int)entryList[i].mIndex[1] << ", " << (int)entryList[i].mMask[1] << ", "
			<< (int)entryList[i].mIndex[2] << ", " << (int)entryList[i].mMask[2] << ", "
			<< (int)entryList[i].mIndex[3] << ", " << (int)entryList[i].mMask[3] << ", "
			<< (int)entryList[i].mIndex[4] << ", " << (int)entryList[i].mMask[4] << ", "
			<< (int)entryList[i].mIndex[5] << ", " << (int)entryList[i].mMask[5] << ", "
			<< (int)entryList[i].mIndex[6] << ", " << (int)entryList[i].mMask[6] << ", "
			<< (int)entryList[i].mIndex[7] << ", " << (int)entryList[i].mMask[7] << ", "
			<< (int)entryList[i].mIndex[8] << ", " << (int)entryList[i].mMask[8] << ", "
			<< (int)entryList[i].mIndex[9] << ", " << (int)entryList[i].mMask[9] << ", "
			<< (int)entryList[i].mIndex[10] << ", " << (int)entryList[i].mMask[10] << ", "
			<< (int)entryList[i].mIndex[11] << ", " << (int)entryList[i].mMask[11] << ", "
			<< (int)entryList[i].mIndex[12] << ", " << (int)entryList[i].mMask[12] << ");" << endl; 

			rslt = OmnTArray_remove1((void ***)array, entryList[i].mIndex, entryList[i].mMask);
			OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
		}
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)array[OMN_TARRAY_ENTRIES])) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>((int)array[OMN_TARRAY_ENTRYHEAD])) << endtc;

	OmnTArray_dump((void ***)array);

	return true;
}


int
OmnTArrayTester::addEntry(void * ****array, 
		 unsigned char idx1,  unsigned char mask1, 
		 unsigned char idx2,  unsigned char mask2, 
		 unsigned char idx3,  unsigned char mask3, 
		 unsigned char idx4,  unsigned char mask4, 
		 unsigned char idx5,  unsigned char mask5, 
		 unsigned char idx6,  unsigned char mask6, 
		 unsigned char idx7,  unsigned char mask7, 
		 unsigned char idx8,  unsigned char mask8, 
		 unsigned char idx9,  unsigned char mask9, 
		 unsigned char idx10, unsigned char mask10, 
		 unsigned char idx11, unsigned char mask11, 
		 unsigned char idx12, unsigned char mask12, 
		 unsigned char idx13, unsigned char mask13, 
		 int value, bool isInsert)
{
	unsigned char min1 = (unsigned char)(idx1 & mask1); 
	unsigned char min2 = (unsigned char)(idx2 & mask2); 
	unsigned char min3 = (unsigned char)(idx3 & mask3); 
	unsigned char min4 = (unsigned char)(idx4 & mask4); 
	unsigned char min5 = (unsigned char)(idx5 & mask5); 
	unsigned char min6 = (unsigned char)(idx6 & mask6); 
	unsigned char min7 = (unsigned char)(idx7 & mask7); 
	unsigned char min8 = (unsigned char)(idx8 & mask8); 
	unsigned char min9 = (unsigned char)(idx8 & mask9); 
	unsigned char min10 = (unsigned char)(idx10 & mask10); 
	unsigned char min11 = (unsigned char)(idx11 & mask11); 
	unsigned char min12 = (unsigned char)(idx12 & mask12); 
	unsigned char min13 = (unsigned char)(idx13 & mask13); 

	unsigned char max1 = (unsigned char)(idx1 | (~mask1));
	unsigned char max2 = (unsigned char)(idx2 | (~mask2));
	unsigned char max3 = (unsigned char)(idx3 | (~mask3));
	unsigned char max4 = (unsigned char)(idx4 | (~mask4));	
	unsigned char max5 = (unsigned char)(idx5 | (~mask5));
	unsigned char max6 = (unsigned char)(idx6 | (~mask6));
	unsigned char max7 = (unsigned char)(idx7 | (~mask7));
	unsigned char max8 = (unsigned char)(idx8 | (~mask8));
	unsigned char max9 = (unsigned char)(idx9 | (~mask9));
	unsigned char max10 = (unsigned char)(idx10 | (~mask10));
	unsigned char max11 = (unsigned char)(idx11 | (~mask11));
	unsigned char max12 = (unsigned char)(idx12 | (~mask12));	
	unsigned char max13 = (unsigned char)(idx13 | (~mask13));
	
	cout << "To add entry: "
		<< "[" << (unsigned int)min1  << "-" << (unsigned int)max1 << "]"
		<< "[" << (unsigned int)min2  << "-" << (unsigned int)max2 << "]"
		<< "[" << (unsigned int)min3  << "-" << (unsigned int)max3 << "]"
		<< "[" << (unsigned int)min4  << "-" << (unsigned int)max4 << "]"
		<< "[" << (unsigned int)min5  << "-" << (unsigned int)max5 << "]"
		<< "[" << (unsigned int)min6  << "-" << (unsigned int)max6 << "]"
		<< "[" << (unsigned int)min7  << "-" << (unsigned int)max7 << "]"
		<< "[" << (unsigned int)min8  << "-" << (unsigned int)max8 << "]"
		<< "[" << (unsigned int)min9  << "-" << (unsigned int)max9 << "]"
		<< "[" << (unsigned int)min10 << "-" << (unsigned int)max10 << "]"
		<< "[" << (unsigned int)min11 << "-" << (unsigned int)max11 << "]"
		<< "[" << (unsigned int)min12 << "-" << (unsigned int)max12 << "]"
		<< "[" << (unsigned int)min13 << "-" << (unsigned int)max13 << "]"
		<< ", value = " << value << ", is insert: " << isInsert << endl;

	unsigned char index[13] = {idx1, idx2, idx3, idx4, idx5, idx6, idx7, idx8, idx9, 
		idx10, idx11, idx12, idx13};
	unsigned char mask[13]  = {mask1, mask2, mask3, mask4, mask5, mask6, mask7, mask8,
		mask9, mask10, mask11, mask12, mask13};
	int rslt;
	if (isInsert)
	{
		rslt = OmnTArray_insert1((void***)array, index, mask, (void*)value, 0);
	}
	else
	{
		rslt = OmnTArray_remove1((void ***)array, index, mask);
	}

	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;
	OmnTC(OmnExpected<int>(0), OmnActual<int>(OmnTArray_checkArray((void ***)array))) <<endtc;
	return rslt;
}


bool
OmnTArrayTester::randomTester13()
{
	struct OmnEntryDef13 entryList[eMaxEntry];
	int entries = 0;
	int rslt;

	OmnBeginTest << "TARRAY Random Tester13";
	mTcNameRoot = "Random13";

	void **array = 0;
	rslt = OmnTArray_init(&array, 13, (void*)10);
	OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

	while (entries < eMaxEntry)
	{
		// 
		// Create the next entry
		//
		for (int i=0; i<13; i++)
		{
			entryList[entries].mIndex[i]	= sgRandom.nextInt(0, 255);
			entryList[entries].mMask[i]		= sgMasks[sgRandom.nextInt(0, 7)];
			entryList[entries].mValue		= sgRandom.nextInt();
			entryList[entries].mIsInsert	= (sgRandom.nextInt() & 0x03) != 0;
		}

		// 
		// Insert it. 
		//
		cout << "    addEntry(theArray, "
			<< (int)entryList[entries].mIndex[0] << ", " << (int)entryList[entries].mMask[0] << ", "
			<< (int)entryList[entries].mIndex[1] << ", " << (int)entryList[entries].mMask[1] << ", "
			<< (int)entryList[entries].mIndex[2] << ", " << (int)entryList[entries].mMask[2] << ", "
			<< (int)entryList[entries].mIndex[3] << ", " << (int)entryList[entries].mMask[3] << ", "
			<< (int)entryList[entries].mIndex[4] << ", " << (int)entryList[entries].mMask[4] << ", "
			<< (int)entryList[entries].mIndex[5] << ", " << (int)entryList[entries].mMask[5] << ", "
			<< (int)entryList[entries].mIndex[6] << ", " << (int)entryList[entries].mMask[6] << ", "
			<< (int)entryList[entries].mIndex[7] << ", " << (int)entryList[entries].mMask[7] << ", "
			<< (int)entryList[entries].mIndex[8] << ", " << (int)entryList[entries].mMask[8] << ", "
			<< (int)entryList[entries].mIndex[9] << ", " << (int)entryList[entries].mMask[9] << ", "
			<< (int)entryList[entries].mIndex[10] << ", " << (int)entryList[entries].mMask[10] << ", "
			<< (int)entryList[entries].mIndex[11] << ", " << (int)entryList[entries].mMask[11] << ", "
			<< (int)entryList[entries].mIndex[12] << ", " << (int)entryList[entries].mMask[12] << ", "
			<< entryList[entries].mValue << ", "
			<< entryList[entries].mIsInsert << ");" << endl;

		if (entryList[entries].mIsInsert)
		{
			rslt = OmnTArray_insert1((void***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask, 
				(void*)entryList[entries].mValue, 0);
		}
		else
		{
			rslt = OmnTArray_remove1((void ***)array, 
				entryList[entries].mIndex, 
				entryList[entries].mMask);
		}

		OmnTC(OmnExpected<int>(0), OmnActual<int>(rslt)) << endtc;

		entries++;
		verify13(array, entryList, entries);
	}

	return true;
}


bool
OmnTArrayTester::verify13(void **arr, struct OmnEntryDef13 *entryList, const int entries)
{
	unsigned char index[13];
	void * *************array = (void * *************)arr;
	static int numHit = 0;

	for (int i=0; i<eTriesPerVerification; i++)
	{
		for (int j=0; j<13; j++)
		{
			index[j] = (unsigned char)sgRandom.nextInt(0, 255); 
		}

		check(array, 
			index[0], index[1], index[2], index[3], 
			index[4], index[5], index[6], index[7], 
			index[8], index[9], index[10], index[11], index[12],
			entryList, entries, &numHit);
	}

	// 
	// Find the right value
	//
	unsigned char min1  = (entryList[entries-1].mIndex[0]  &   entryList[entries-1].mMask[0]);
	unsigned char max1  = (entryList[entries-1].mIndex[0]  | (~entryList[entries-1].mMask[0]));
	unsigned char min2  = (entryList[entries-1].mIndex[1]  &   entryList[entries-1].mMask[1]);
	unsigned char max2  = (entryList[entries-1].mIndex[1]  | (~entryList[entries-1].mMask[1]));
	unsigned char min3  = (entryList[entries-1].mIndex[2]  &   entryList[entries-1].mMask[2]);
	unsigned char max3  = (entryList[entries-1].mIndex[2]  | (~entryList[entries-1].mMask[2]));
	unsigned char min4  = (entryList[entries-1].mIndex[3]  &   entryList[entries-1].mMask[3]);
	unsigned char max4  = (entryList[entries-1].mIndex[3]  | (~entryList[entries-1].mMask[3]));
	unsigned char min5  = (entryList[entries-1].mIndex[4]  &   entryList[entries-1].mMask[4]);
	unsigned char max5  = (entryList[entries-1].mIndex[4]  | (~entryList[entries-1].mMask[4]));
	unsigned char min6  = (entryList[entries-1].mIndex[5]  &   entryList[entries-1].mMask[5]);
	unsigned char max6  = (entryList[entries-1].mIndex[5]  | (~entryList[entries-1].mMask[5]));
	unsigned char min7  = (entryList[entries-1].mIndex[6]  &   entryList[entries-1].mMask[6]);
	unsigned char max7  = (entryList[entries-1].mIndex[6]  | (~entryList[entries-1].mMask[6]));
	unsigned char min8  = (entryList[entries-1].mIndex[7]  &   entryList[entries-1].mMask[7]);
	unsigned char max8  = (entryList[entries-1].mIndex[7]  | (~entryList[entries-1].mMask[7]));
	unsigned char min9  = (entryList[entries-1].mIndex[8]  &   entryList[entries-1].mMask[8]);
	unsigned char max9  = (entryList[entries-1].mIndex[8]  | (~entryList[entries-1].mMask[8]));
	unsigned char min10 = (entryList[entries-1].mIndex[9]  &   entryList[entries-1].mMask[9]);
	unsigned char max10 = (entryList[entries-1].mIndex[9]  | (~entryList[entries-1].mMask[9]));
	unsigned char min11 = (entryList[entries-1].mIndex[10] &   entryList[entries-1].mMask[10]);
	unsigned char max11 = (entryList[entries-1].mIndex[10] | (~entryList[entries-1].mMask[10]));
	unsigned char min12 = (entryList[entries-1].mIndex[11] &   entryList[entries-1].mMask[11]);
	unsigned char max12 = (entryList[entries-1].mIndex[11] | (~entryList[entries-1].mMask[11]));
	unsigned char min13 = (entryList[entries-1].mIndex[12] &   entryList[entries-1].mMask[12]);
	unsigned char max13 = (entryList[entries-1].mIndex[12] | (~entryList[entries-1].mMask[12]));

	int tries[13][4] = 
	{
		{min1-1,  min1, max1, max1+1},
		{min2-1,  min2, max2, max2+1},
		{min3-1,  min3, max3, max3+1},
		{min4-1,  min4, max4, max4+1},
		{min5-1,  min5, max5, max5+1},
		{min6-1,  min6, max6, max6+1},
		{min7-1,  min7, max7, max7+1},
		{min8-1,  min8, max8, max8+1},
		{min9-1,  min9, max9, max9+1},
		{min10-1, min10, max10, max10+1},
		{min11-1, min11, max11, max11+1},
		{min12-1, min12, max12, max12+1},
		{min13-1, min13, max13, max13+1}
	};

	for (int m1=0; m1<4; m1++)
	{
		cout << "m1 = " << m1 << endl;
		for (int m2=0; m2<4; m2++)
		{
			cout << "m2 = " << m2 << endl;
			for (int m3=0; m3<4; m3++)
			{
				cout << "m3 = " << m3 << endl;
				for (int m4=0; m4<4; m4++)
				{
					cout << "m4 = " << m4 << endl;
					for (int m5=0; m5<4; m5++)
					{
						cout << "m5 = " << m5 << endl;
						for (int m6=0; m6<4; m6++)
						{
							cout << "m6 = " << m6 << endl;
							for (int m7=0; m7<4; m7++)
							{			
								for (int m8=0; m8<4; m8++)
								{
									for (int m9=0; m9<4; m9++)
									{
										for (int m10=0; m10<4; m10++)
										{
											for (int m11=0; m11<4; m11++)
											{
												for (int m12=0; m12<4; m12++)
												{
													for (int m13=0; m13<4; m13++)
													{
														check(array, 
															tries[0][m1], 
															tries[1][m2],
															tries[2][m3],
															tries[3][m4],
															tries[4][m1], 
															tries[5][m2],
															tries[6][m3],
															tries[7][m4],
															tries[8][m1], 
															tries[9][m2],
															tries[10][m3],
															tries[11][m4],
															tries[12][m4],
															entryList, entries, &numHit);
													}
												}
											}
										}
									}
								}
							}
						}
					}
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
OmnTArrayTester::check(void * *************array, 
					   const unsigned char i1, 
					   const unsigned char i2, 
					   const unsigned char i3, 
					   const unsigned char i4, 
					   const unsigned char i5, 
					   const unsigned char i6, 
					   const unsigned char i7, 
					   const unsigned char i8, 
					   const unsigned char i9, 
					   const unsigned char i10, 
					   const unsigned char i11, 
					   const unsigned char i12, 
					   const unsigned char i13, 
					   struct OmnEntryDef13 *entryList, 
					   const int entries, 
					   int *hit)
{
	unsigned char index[13];
	index[0] = i1;
	index[1] = i2;
	index[2] = i3;
	index[3] = i4;
	index[4] = i5;
	index[5] = i6;
	index[6] = i7;
	index[7] = i8;
	index[8] = i9;
	index[9] = i10;
	index[10] = i11;
	index[11] = i12;
	index[12] = i13;
	bool found = false;

	for (int k=entries-1; k>=0; k--)
	{
		int expected;
		unsigned char min, max;

		// 
		// Check whether it matches the entry
		//
		int m;
		for (m=0; m<13; m++)
		{
			min = (entryList[k].mIndex[m] & entryList[k].mMask[m]);
			max = (entryList[k].mIndex[m] | (~entryList[k].mMask[m]));
			if (index[m] < min || index[m] > max)
			{
				break;
			}
		}

		// 
		// A match is found
		//
		if (m >= 13)
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
			int actual = (int)array[i1][i2][i3][i4][i5][i6][i7][i8][i9][i10][i11][i12][i13];
			if (actual != expected)
			{
				OmnTrace << "Not the same: " 
					<< "Actual: " << actual << ", Expected: " << expected
					<< "index: " 
					<< i1 << ", " << i2 << ", " << i3 << ", " << i4 
					<< i5 << ", " << i6 << ", " << i7 << ", " << i8
					<< i9 << ", " << i10 << ", " << i11 << ", " << i12 << ", " << i13 
					<< endl;

				/*
				// 
				// Dump all the affected entries
				//
				for (int p=0; p<entries; p++)
				{
					if ((entryList[p].mIndex[0] & entryList[p].mMask[0]) <= i1 &&
						i1 <= (entryList[p].mIndex[0] | (~entryList[p].mMask[0])) &&
						(entryList[p].mIndex[1] & entryList[p].mMask[1]) <= i2 &&
						i2 <= (entryList[p].mIndex[1] | (~entryList[p].mMask[1])) &&
						(entryList[p].mIndex[2] & entryList[p].mMask[2]) <= i3 &&
						i3 <= (entryList[p].mIndex[2] | (~entryList[p].mMask[2])) &&
						(entryList[p].mIndex[3] & entryList[p].mMask[3]) <= i4 &&
						i4 <= (entryList[p].mIndex[3] | (~entryList[p].mMask[3])))
					{
						OmnTrace << "    addEntry(theArray, "
							<< (int)entryList[p].mIndex[0] << ", " << (int)entryList[p].mMask[0] << ", "
							<< (int)entryList[p].mIndex[1] << ", " << (int)entryList[p].mMask[1] << ", "
							<< (int)entryList[p].mIndex[2] << ", " << (int)entryList[p].mMask[2] << ", "
							<< (int)entryList[p].mIndex[3] << ", " << (int)entryList[p].mMask[3] << ", "
							<< entryList[p].mValue << ", "
							<< entryList[p].mIsInsert << ");" << endl;
					}
				}
				*/

				exit(0);
			}

			OmnTC(OmnExpected<int>(expected), OmnActual<int>(actual))
				<< i1 << ", " << i2 << ", " << i3 << ", " << i4 
				<< i5 << ", " << i6 << ", " << i7 << ", " << i8
				<< i9 << ", " << i10 << ", " << i11 << ", " << i12 << ", " << i13 << endtc;

			break;
		}
	}

	// 
	// Didn't find the match. 
	//
	if (!found)
	{
		int actual = (int)array[i1][i2][i3][i4][i5][i6][i7][i8][i9][i10][i11][i12][i13];
		if (actual != sgDefaultValue)
		{
			OmnTrace << "not the same" << endl;
		}

		OmnTC(OmnExpected<int>(sgDefaultValue), OmnActual<int>(actual))
				<< i1 << ", " << i2 << ", " << i3 << ", " << i4 
				<< i5 << ", " << i6 << ", " << i7 << ", " << i8
				<< i9 << ", " << i10 << ", " << i11 << ", " << i12 << ", " << i13 << endtc;
	}

	return true;
}

#endif

