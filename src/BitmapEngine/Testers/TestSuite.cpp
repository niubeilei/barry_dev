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
#include "BitmapEngine/Testers/TestSuite.h"

#include "BitmapEngine/BitmapExecutor.h"
#include "BitmapEngine/Testers/BitmapCacheTester.h"
#include "BitmapEngine/Testers/BitmapExecutorTester.h"
#include "BitmapEngine/Testers/BitmapTest.h"
#include "BitmapEngine/Testers/IILTester.h"


#include "BitmapTreeMgr/BitmapTree.h"
#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "Random/RandomBuffArray.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosBitmapEngineTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("BitmapEngine", "BitmapEngine Test Suite");

	// IILStrTester
	if (type == "executor_tester")
	{
//		suite->addTestPkg(OmnNew AosBitmapExecutorTester());
	}
	if (type == "iil_tester")
	{
		suite->addTestPkg(OmnNew AosIILTester());
	}	
	else if (type == "cache_tester")
	{
		suite->addTestPkg(OmnNew AosBitmapCacheTester());
	}
	else if(type == "help" || type == "--help")
	{
		cout << "param can be :\n"
			 << "executor_test \n"
			 << "cache_test \n"
			 << "all         \n";
		exit(0);
	}
	else
	{
//		suite->addTestPkg(OmnNew AosBitmapExecutorTester());
//		suite->addTestPkg(OmnNew AosBitmapCacheTester());
	}
	return suite;
}
