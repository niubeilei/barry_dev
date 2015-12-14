////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: skbGeneratorTester.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef Aos_SkbGeneratorTester_skbGeneratorTester_h
#define Aos_SkbGeneratorTester_skbGeneratorTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosSkbGeneratorTester : public OmnTestPkg
{
private:

public:
	AosSkbGeneratorTester()
	{
		mName = "AosSkbGeneratorTester";
	}
	~AosSkbGeneratorTester() {}

	virtual bool		start();

private:
	bool testSkbGeneration();
};
#endif

