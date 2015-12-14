////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tester.cpp
// Description:
//   
//
// Modification History:
// 11/17/2006 	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "SystemCli/PppoeTester/TorturerPppoeSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "PppoeTorturer.h"


OmnTestSuitePtr		
AosPppoeSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("aosUtilTestSuite", "aosUtil Library Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosPppoeTorturer());

	return suite;
}

