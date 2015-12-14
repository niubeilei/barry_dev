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
#ifndef Aos_Valueset_Testers_ValsetEnumTester_h
#define Aos_Valueset_Testers_ValsetEnumTester_h

#include "Util/Opr.h"
#include "Valueset/Ptrs.h"
#include "Tester/TestPkg.h"


class AosValuesetEnumTester : public OmnTestPkg
{
private:
	enum
	{
		eNormalTries = 100
	};

public:
	AosValuesetEnumTester();
	~AosValuesetEnumTester() {}

	virtual bool		start();

private:
	bool basicTest();
	bool testOneEnum();
	bool testNormalEnum();
	bool testNormalEnum2();
	bool testNormalString();
	bool testNormalU64();
	bool testNormalInt64();
	bool test2NormalString();
	bool test2NormalU64();
	bool test2NormalInt64();
};


#endif

