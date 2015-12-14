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
// 2014/12/04 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_JimoCall_Tester_JimoCallTester_h
#define Aos_JimoCall_Tester_JimoCallTester_h

#include "Tester/TestPkg.h"


class AosJimoCallTester : public OmnTestPkg
{
private:

public:
	AosJimoCallTester();
	~AosJimoCallTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

