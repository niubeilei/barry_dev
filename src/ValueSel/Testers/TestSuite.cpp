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
// 05/14/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "Valueset/Testers/TestSuite.h"

#include "SEUtil/XmlTag.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "Valueset/Testers/ValsetRangeTester.h"
#include "Valueset/Testers/ValsetMulRangesTester.h"
#include "Valueset/Testers/ValsetEnumTester.h"
#include "Valueset/Testers/ValsetPatternTester.h"


OmnTestSuitePtr		
AosValuesetTestSuite::getSuite(const AosXmlTagPtr &testers)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Valueset", "Test Suite");

	if (!testers)
	{
		// Get all the testers
		suite->addTestPkg(OmnNew AosValuesetRangeTester());
		return suite;
	}

	AosXmlTagPtr tester = testers->getFirstChild();
	while (tester)
	{
		OmnString name = tester->getNodeText();
		cout<< "my start-------------"<<name<<endl;
		if (name == "valueset_range")
		{
			suite->addTestPkg(OmnNew AosValuesetRangeTester());
		}
		else if(name == "valueset_mulranges")
		{
			suite->addTestPkg(OmnNew AosValuesetMulRangesTester());
		}
		else if(name == "valueset_enum")
		{
			suite->addTestPkg(OmnNew AosValuesetEnumTester());
		}
		else if(name == "valueset_pattern")
		{
			suite->addTestPkg(OmnNew AosValuesetPatternTester());
		}
		else if(name == "valueset_all")
		{
			suite->addTestPkg(OmnNew AosValuesetRangeTester());
			suite->addTestPkg(OmnNew AosValuesetMulRangesTester());
			suite->addTestPkg(OmnNew AosValuesetEnumTester());
			suite->addTestPkg(OmnNew AosValuesetPatternTester());
		}
		else
		{
			OmnAlarm << "Unrecognized tester: " << name << enderr;
		}
		tester = testers->getNextChild();
	}

	return suite;
}
