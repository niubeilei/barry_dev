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
// 2013/05/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Omn_Random_Tester_RandomBuffArrayTester_h
#define Omn_Random_Tester_RandomBuffArrayTester_h

#include "Debug/Debug.h"
#include "Tester/TestPkg.h"


class AosRandomBuffArrayTester : public OmnTestPkg
{
private:

public:
    AosRandomBuffArrayTester();
    ~AosRandomBuffArrayTester() {}

    virtual bool            start();

private:
	bool	testQuery1();
	bool	testQuery2();
};
#endif
