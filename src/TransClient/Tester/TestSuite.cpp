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
// 2011/06/03	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransClient/Tester/TestSuite.h"

#include "TransClient/Tester/TransClientTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosTransClientTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("TransClientTestSuite", ".....");

	// Now add all the testers
	//suite->addTestPkg(OmnNew AosXmlWordParserTester());
	suite->addTestPkg(OmnNew AosTransClientTester());
	return suite;
}
