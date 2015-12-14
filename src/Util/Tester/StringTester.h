////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: StringTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_StringTester_h
#define Omn_TestUtil_StringTester_h

#include "Tester/TestPkg.h"


class OmnStringTester : public OmnTestPkg
{
private:

public:
	OmnStringTester() {mName = "OmnSIVTester";}
	~OmnStringTester() {}

	virtual bool		start();

private:
	bool	testDefaultConstructor();
	bool	testConstructorB();
	bool	testTrimLastWord();
	bool	miscTest();
	bool	testRemoveLine();
	bool	testGetLine();
	bool	testGetLine(const u32 tries);
	bool	testReplace(const u32 tries);
	bool	testParsingValues(const u32);
	bool	testFindSubString();
};


#endif

