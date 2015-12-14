////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Array4Tester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_Array4Tester_h
#define Omn_TestUtil_Array4Tester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"



class OmnArray4Tester : public OmnTestPkg
{
private:

public:
	OmnArray4Tester()
	{
		mName = "OmnArray4Tester";
	}
	~OmnArray4Tester() {}

	virtual bool		start();

private:
	bool testInitialization();
	bool testInsert();

	bool verify(int **** array, 
				const int index1, const int index2, 
				const int min1,   const int max1,
				const int min2,   const int max2,
				const int value);
};




#endif

