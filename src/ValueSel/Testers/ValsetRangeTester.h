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
#ifndef Aos_Valueset_Testers_ValsetTester_h
#define Aos_Valueset_Testers_ValsetTester_h

#include "Util/Opr.h"
#include "Valueset/Ptrs.h"
#include "Tester/TestPkg.h"


class AosValuesetRangeTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 100
	};

public:
	AosValuesetRangeTester();
	~AosValuesetRangeTester() {}

	virtual bool		start();

private:
	bool basicTest();
	bool testOneRange();
	bool testNormalRange();
	bool testNormalEqual();
	bool testNormalIncorrect();
	bool testNormalString();
	bool testNormalU64();
	bool testNormalInt64();
	bool testEqualU64();
	bool testEqualInt64();
	bool testEqualString();
	bool testIncorrectU64();
	bool testIncorrectInt64();
	bool testIncorrectString();
	bool testIncorrectXml();
};


#endif

