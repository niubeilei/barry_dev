////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpTestSuite.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProc/http_proc/Tester/HttpTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "AppProc/http_proc/Tester/ForwardTableTester.h"
#include "AppProc/http_proc/Tester/HTTPTester.h"
#include "AppProc/http_proc/Tester/HttpProcTester.h"

OmnTestSuitePtr		
HttpTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("HttpTestSuite", "HTTP Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew HTTPTester());
	suite->addTestPkg(OmnNew HttpProcTester());
	//suite->addTestPkg(OmnNew AosForwardTableTester());

	return suite;
}

