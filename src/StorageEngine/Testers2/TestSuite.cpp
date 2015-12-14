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
// 2013/03/24 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/Testers2/TestSuite.h"

#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "StorageEngine/Testers2/FixedLengthTesterMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosStorageEngineTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("StorageEngine", "StorageEngine Test Suite");

	// IILStrTester
	if (type == "fixed_length")
	{
		suite->addTestPkg(OmnNew AosFixedLengthTesterMgr());
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
		suite->addTestPkg(OmnNew AosFixedLengthTesterMgr());
	}
	return suite;
}
