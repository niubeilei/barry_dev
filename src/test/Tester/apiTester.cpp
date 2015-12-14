//////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// This file is automatically generated by the TorturerGen facility. 
//
// Modification History:
// 3/13/2007 Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#include "test/Tester/apiTester.h"

#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"


AosapiTesterTester::AosapiTesterTester()
{
    mTries = 10000;
    mName = "AosapiTesterTester";
}


AosapiTesterTester::~AosapiTesterTester()
{
}


bool
AosapiTesterTester::start()
{
    basicTest();
    return true;
}


bool
AosapiTesterTester::basicTest()
{
    return true;
}

