////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TArrayTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_TArrayTester_h
#define Omn_TestUtil_TArrayTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


struct OmnEntryDef
{
	unsigned char	mIndex[4];
	unsigned char	mMask[4];
	bool			mIsInsert;
	int				mValue;
};


struct OmnEntryDef13
{
	unsigned char	mIndex[13];
	unsigned char	mMask[13];
	bool			mIsInsert;
	int				mValue;
};


class OmnTArrayTester : public OmnTestPkg
{
private:
	enum
	{
		eMaxEntry = 5000, 
		eDimension = 4, 
		eTriesPerVerification = 5000
	};

public:
	OmnTArrayTester()
	{
		mName = "OmnTArrayTester";
	}
	~OmnTArrayTester() {}

	virtual bool		start();

private:
	bool testInitialization();
	bool testInsert();

	bool verify(void ** array, 
				const int index1, const int index2, 
				const int min1,   const int max1,
				const int min2,   const int max2,
				const int value);
	bool check(void * ****array, 
			   const unsigned char i1, 
			   const unsigned char i2, 
			   const unsigned char i3, 
			   const unsigned char i4, 
			   struct OmnEntryDef *entryList, 
			   const int entries, 
			   int *hit);
	bool verify(void **arr, struct OmnEntryDef *entryList, const int entries);
	bool randomTester();
	bool randomAddDelete();
	bool specialTest();
	bool specialTest1();
	bool specialTest2();
	bool specialTest3();
	bool specialTest4();
	bool specialTest5();
	bool specialTest6();
	bool specialTest7();
	bool specialTest8();
	bool specialTest9();
	bool specialTest10();
	bool specialTest11();
	int	 addEntry(void * ****array, 
			unsigned char idx1, 
			unsigned char idx2, 
			unsigned char idx3, 
			unsigned char idx4, 
			unsigned char mask1, 
			unsigned char mask2, 
			unsigned char mask3, 
			unsigned char mask4, 
			int value, 
			bool isInsert);
	int	addEntry(void * ****array, 
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
		 int value, bool isInsert);

	bool randomCheck13(void * *************array);
	bool randomAddDelete13();
	bool dim13Test();
	bool verify13(void **arr, struct OmnEntryDef13 *entryList, const int entries);
	bool randomTester13();
	bool check(void * *************array, 
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
			   int *hit);
};




#endif

