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
// 06/15/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocLock/Testers/TestSuite.h"

#include "XmlUtil/XmlTag.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "DocLock/Testers/DocLockTester.h"


OmnTestSuitePtr		
AosDocLockTestSuite::getSuite(const AosXmlTagPtr &testers)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("DocLock", "Test Suite");

	if (!testers)
	{
		// Get all the testers
		suite->addTestPkg(OmnNew AosDocLockTester());
		return suite;
	}

	AosXmlTagPtr tester = testers->getFirstChild();
	while (tester)
	{
		OmnString name = tester->getNodeText();
		if (name == "doclock")
		{
			suite->addTestPkg(OmnNew AosDocLockTester());
		}
		else
		{
			OmnAlarm << "Unrecognized tester: " << name << enderr;
		}
		tester = testers->getNextChild();
	}

	return suite;
}
