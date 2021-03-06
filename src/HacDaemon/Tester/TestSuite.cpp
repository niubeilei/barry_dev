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
// 4/21/2007 Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
// 4/21/2007 Created by TorturerGen Facility
//////////////////////////////////////////////////////////////////////////
#include "HacDaemon/Tester/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "HacDaemon/Tester/HacTester.h"

//
// Default constructor.
// Code automatically generated by TorturerGen
//
AosHacTorturerTestSuite::AosHacTorturerTestSuite()
{
}


//
// Destructor.
// Code automatically generated by TorturerGen
//
AosHacTorturerTestSuite::~AosHacTorturerTestSuite()
{
}


//
// Code automatically generated by TorturerGen
//
OmnTestSuitePtr
AosHacTorturerTestSuite::getSuite()
{
    OmnTestSuitePtr suite = OmnNew OmnTestSuite("HacTorturerTestSuite", "");
    // 
    // Add all the testers
    // 
    suite->addTestPkg(OmnNew AosHacTester());

    return suite;
}

