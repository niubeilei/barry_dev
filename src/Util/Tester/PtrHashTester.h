////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: PtrHashTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Omn_TestUtil_PtrHashTester_h
#define Omn_TestUtil_PtrHashTester_h

#include "Tester/TestPkg.h"


class OmnPtrHashTester : public OmnTestPkg
{
private:

public:
	OmnPtrHashTester() {mName = "OmnPtrHashTester";}
	~OmnPtrHashTester() {}

	virtual bool		start();

private:
	bool testDefaultConstructor();
	bool testAddAndDelete();
};


#endif

