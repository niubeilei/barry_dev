////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SPtrTester.h
// Description:
//   
//
// Modification History:
// 11/21/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_SPtrTester_h
#define Omn_TestUtil_SPtrTester_h

#include "Tester/TestPkg.h"


class AosSPtrTester : public OmnTestPkg
{
private:

public:
	AosSPtrTester() {mName = "SPtrTester";}
	~AosSPtrTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

