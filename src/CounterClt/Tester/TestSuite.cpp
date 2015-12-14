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
#include "CounterClt/Tester/TestSuite.h"

#include "CounterClt/Tester/CounterTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosCounterCltTestSuite::getSuite(const AosXmlTagPtr &testers)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("CounterClt", "Test Suite");
	if (!testers)
	{
		// Get all the testers
		suite->addTestPkg(OmnNew AosCounterTester());
		return suite;
	}

	// 'testers' should be in the form:
	// 	<testers>
	// 		<tester>name</tester>
	// 		...
	// 	</testers>
	AosXmlTagPtr tester = testers->getFirstChild();
	while (tester)
	{
		OmnString name = tester->getNodeText();
		if (name == "counter")
		{
			suite->addTestPkg(OmnNew AosCounterTester());
		}
		else
		{
			OmnAlarm << "Unrecognized tester: " << name << enderr;
		}
		tester = testers->getNextChild();
	}

	return suite;
}
