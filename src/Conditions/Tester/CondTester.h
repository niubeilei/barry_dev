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
// 01/28/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Conditions_Tester_ConditionTester_h
#define Omn_Conditions_Tester_ConditionTester_h

#include "Tester/TestPkg.h"


class AosCondTester : public OmnTestPkg
{
private:

public:
	AosCondTester() {mName = "CondTester";}
	~AosCondTester() {}

	virtual bool		start();

private:
	bool	basicTest(const u32 tries);
};


#endif

