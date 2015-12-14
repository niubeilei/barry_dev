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
// Modification History:
// 2013/02/08 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "QueryTorturer/Exe/TestSuite.h"

#include "QueryTorturer/SmartQueryTester.h"
#include "QueryTorturer/SmartStatisticsTester.h"


//#include "Random/RandomBuffArray.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosQueryTorturerTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("StatisticsTorturer", "StatisticsTorturer Test Suite");

	// suite->addTestPkg(OmnNew AosBitmapExecutorTester());
	// suite->addTestPkg(OmnNew AosBitmapCacheTester());
	// suite->addTestPkg(OmnNew AosIndexMgrTester());
	suite->addTestPkg(OmnNew AosSmartStatisticsTester());

	return suite;
}
