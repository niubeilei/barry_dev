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

#include "aosUtil/Tester/Tester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "aosUtil/Tester/MutexTester.h"
#include "aosUtil/Tester/RwlockTester.h"
#include "aosUtil/Tester/SlabTester.h"
#include "aosUtil/Tester/MemoryTester.h"
#include "aosUtil/Tester/AlarmTester.h"
#include "aosUtil/Tester/RandomTester.h"
#include "aosUtil/Tester/TracerTester.h"
#include "aosUtil/Tester/HashBinTester.h"
#include "aosUtil/Tester/StrUtilTester.h"
#include "aosUtil/Tester/SerializerTester.h"
#include "aosUtil/Tester/MemoryTester.h"
#include "aosUtil/Tester/CharPTreeTester.h"
#include "aosUtil/Tester/TimerPackTester.h"
#include "aosUtil/Tester/TimerTester.h"


OmnTestSuitePtr		
OmnAosUtilTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("aosUtilTestSuite", "aosUtil Library Test Suite");

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
	suite->addTestPkg(OmnNew AosTimerPackTester());
//	suite->addTestPkg(OmnNew AosTimerTester());

	return suite;
}

