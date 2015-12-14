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
// 2013/03/28 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Datalet/Testers/TestSuite.h"

#include "Datalet/Testers/DataletBuffTester.h"

#include "IILMgr/IILStr.h"
#include "IILMgr/IILU64.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosDataletTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("Value", "Value Test Suite");

	// IILStrTester
	if (type == "datalet_buff_tester")
	{
		suite->addTestPkg(OmnNew AosDataletBuffTester());
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
		suite->addTestPkg(OmnNew AosDataletBuffTester());
	}
	return suite;
}
