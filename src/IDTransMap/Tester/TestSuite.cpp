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
// 2009/10/09	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "IDTransMap/Tester/TestSuite.h"

#include "IDTransMap/Tester/TransBucketTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSearchEngineTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("UtilTestSuite", "Util Library Test Suite");

	// Now add all the testers
	//suite->addTestPkg(OmnNew AosXmlWordParserTester());
	suite->addTestPkg(OmnNew AosTransBucketTester());
	return suite;
}
