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
// 2013/03/21 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "DataPicker/Testers/TestSuite.h"


#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosDataPickerTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("DataPicker", "DataPicker Test Suite");

	// IILStrTester
	if (type == "basic_tester")
	{
		suite->addTestPkg(OmnNew AosBasicTester());
	}
	else if(type == "help" || type == "--help")
	{
		cout << "param can be :\n"
			 << "xxx_test \n"
			 << "all         \n";
		exit(0);
	}
	else
	{
		suite->addTestPkg(OmnNew AosBasicTester());
	}
	return suite;
}
