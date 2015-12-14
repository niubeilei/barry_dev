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
// 2013/05/17 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "BackupLog/Testers/TestSuite.h"

#include "BackupLog/Testers/BackupLogTester.h"



OmnTestSuitePtr		
AosBitmapEngineTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("BitmapEngine", "BitmapEngine Test Suite");

	// IILStrTester
	if (type == "backup_log_tester")
	{
		suite->addTestPkg(OmnNew AosBackupLogTester());
		return suite;
	}

	if (type == "all")
	{
		suite->addTestPkg(OmnNew AosBackupLogTester());
		return suite;
	}

	cout << "param can be :\n"
			 << "executor_test \n"
			 << "cache_test \n"
			 << "all         \n";
		exit(0);
}
