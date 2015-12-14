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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Sorter/Testers/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Sorter/Testers/SorterTester.h"
#include "Sorter/Testers/BuffArrayTesterNew.h"
#include "Sorter/Testers/MergeFileSorterNew.h"
#include "Sorter/Testers/MergeVarFileSortTest.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosSorterTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TestSuite", "IILTrans Library Test Suite");

	//suite->addTestPkg(OmnNew AosBuffArrayTesterNew());
	//suite->addTestPkg(OmnNew AosMergeFileSorterNew());
	suite->addTestPkg(OmnNew AosMergeVarFileSortTest());
	return suite;
}
