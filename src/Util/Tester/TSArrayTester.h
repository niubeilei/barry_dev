////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TSArrayTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_TSArrayTester_h
#define Omn_TestUtil_TSArrayTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


struct OmnEntryDef1
{
	unsigned char	mIndex[4];
	bool			mIsInsert;
	int				mValue;
};


class OmnTSArrayTester : public OmnTestPkg
{
private:
	enum
	{
		eMaxEntry = 5000, 
		eDimension = 4, 
		eTriesPerVerification = 5000
	};

public:
	OmnTSArrayTester()
	{
		mName = "OmnTSArrayTester";
	}
	~OmnTSArrayTester() {}

	virtual bool		start();

private:
	bool testInitialization();

	bool check(void * ****array, 
			   const unsigned char i1, 
			   const unsigned char i2, 
			   const unsigned char i3, 
			   const unsigned char i4, 
			   struct OmnEntryDef1 *entryList, 
			   const int entries, 
			   int *hit);
	bool verify(void **arr, struct OmnEntryDef1 *entryList, const int entries);
	bool randomTester();
	bool randomAddDelete();
	int	 addEntry(void * ****array, 
			unsigned char idx1, 
			unsigned char idx2, 
			unsigned char idx3, 
			unsigned char idx4, 
			int value, 
			bool isInsert);
	bool	specialTest1();
	bool	specialTest2();
};




#endif

