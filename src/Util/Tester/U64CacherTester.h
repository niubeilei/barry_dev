////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: U64CacherTester.h
// Description:
//   
//
// Modification History:
// 01/01/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_U64CacherTester_h
#define Omn_TestUtil_U64CacherTester_h

#include "Tester/TestPkg.h"


class AosU64CacherTester : public OmnTestPkg
{
private:

public:
	AosU64CacherTester() {mName = "U64CacherTester";}
	~AosU64CacherTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

