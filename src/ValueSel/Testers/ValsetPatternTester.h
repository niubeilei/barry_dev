////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//   
//
// Modification History:
// 05/14/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Valueset_Testers_ValsetPatternTester_h
#define Aos_Valueset_Testers_ValsetPatternTester_h

#include "Util/Opr.h"
#include "Valueset/Ptrs.h"
#include "Tester/TestPkg.h"


class AosValuesetPatternTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 100
	};

public:
	AosValuesetPatternTester();
	~AosValuesetPatternTester() {}

	virtual bool		start();

private:
	bool basicTest();
	bool testOnePattern();
	bool testNormalPattern();
	bool testNormalPattern2();
	bool testNormalString();
	bool testNormalU64();
	bool testNormalInt64();
	bool test2NormalString();
	bool test2NormalU64();
	bool test2NormalInt64();
};


#endif

