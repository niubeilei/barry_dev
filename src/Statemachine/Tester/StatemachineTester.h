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
// 01/07/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_TestUtil_StatemachineTester_h
#define Omn_TestUtil_StatemachineTester_h

#include "Tester/TestPkg.h"


class OmnStatemachineTester : public OmnTestPkg
{
private:

public:
	OmnStatemachineTester() {mName = "Statemachine";}
	~OmnStatemachineTester() {}

	virtual bool		start();

private:
	bool	basicTest();
};


#endif

