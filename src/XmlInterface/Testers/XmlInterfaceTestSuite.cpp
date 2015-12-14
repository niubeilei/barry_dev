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
// 03/18/2009	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "XmlInterface/Testers/XmlInterfaceTestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "XmlInterface/Testers/ReqProcTester.h"


OmnTestSuitePtr		
AosXmlInterfaceTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("XmlInterfaceTester", 
			"XmlInterface Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosXmlInterfaceReqProcTester());
	return suite;
}
