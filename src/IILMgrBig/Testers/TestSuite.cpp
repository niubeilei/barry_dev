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
// 2013/02/07 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "IILMgrBig/Testers/TestSuite.h"

#include "IILMgrBig/Testers/IILStrTester.h"
#include "IILMgrBig/Testers/ConcurrentIILTester.h"

#include "IILMgrBig/IILStr.h"
#include "IILMgrBig/IILU64.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosIILMgrBigTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("IILMgrBig", "IILMgrBig Test Suite");

	// IILStrTester
	if (type == "iilstr")
	{
		suite->addTestPkg(OmnNew AosIILStrTester());
	}
	
	if (type == "concurrent_iil")
	{
		suite->addTestPkg(OmnNew AosConcurrentIILTester());
	}
	
	if (type == "all")
	{
		suite->addTestPkg(OmnNew AosIILStrTester());
		suite->addTestPkg(OmnNew AosConcurrentIILTester());
	}

	if(type == "help" || type == "--help")
	{
		cout << "param can be :\n"
			 << "strtest     \n"
			 << "all         \n";
		exit(0);
	}
	return suite;
}
