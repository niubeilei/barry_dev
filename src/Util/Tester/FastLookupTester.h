////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FastLookupTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_FastLookupTester_h
#define Omn_TestUtil_FastLookupTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"

struct aosDnMatchEntry
{
	int		mStartIndex;
	int		mLength;
};

class OmnFastLookupTester : public OmnTestPkg
{
private:

public:
	OmnFastLookupTester()
	{
		mName = "OmnFastLookupTester";
	}
	~OmnFastLookupTester() {}

	virtual bool		start();

private:
	bool randomTester();
};
#endif

