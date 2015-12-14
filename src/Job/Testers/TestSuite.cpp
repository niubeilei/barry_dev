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
// 03/22/2012 Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Job/Testers/TestSuite.h"

#include "Job/Ptrs.h"
#include "Job/Testers/JobTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Util/AppendFile.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosJobTestSuite::getSuite(const OmnString &tester_name)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("Job", "Job Library Test Suite");

	if (tester_name == "jobtest")
	{
		suite->addTestPkg(OmnNew AosJobTester());
	}
	
	if(tester_name == "help" || tester_name == "--help")
	{
		cout << "Run: jobtester.exe -tries number-of-tries "
			<< "-tester tester-name (optional)" << endl;
		exit(0);
		return 0;
	}
	return suite;

	// suite->addTestPkg(OmnNew AosJobTester());
}

