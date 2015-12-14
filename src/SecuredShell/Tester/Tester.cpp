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
// 
////////////////////////////////////////////////////////////////////////////

#include "SecuredShell/Tester/Tester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "SecuredShell/Tester/SecuredShellTester.h"


OmnTestSuitePtr		
AosSecuredShellTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("SecuredShellTestSuite", "SecuredShell Library Test Suite");

	// 
	// Now add all the testers
	//
//	suite->addTestPkg(OmnNew AosMutexTester());
//	suite->addTestPkg(OmnNew AosRwlockTester());
//	suite->addTestPkg(OmnNew AosSlabTester());
//	suite->addTestPkg(OmnNew AosMemoryTester());
//	suite->addTestPkg(OmnNew AosAlarmTester());
//	suite->addTestPkg(OmnNew AosRandomTester());
//	suite->addTestPkg(OmnNew AosTracerTester());
//	suite->addTestPkg(OmnNew AosHashBinTester());
//	suite->addTestPkg(OmnNew AosStrUtilTester());
//	suite->addTestPkg(OmnNew AosSerializerTester());
//	suite->addTestPkg(OmnNew AosMemoryTester());
//	suite->addTestPkg(OmnNew OmnCharPTreeTester());
	suite->addTestPkg(OmnNew AosSecuredShellTester());
//	suite->addTestPkg(OmnNew AosTimerTester());

	return suite;
}

