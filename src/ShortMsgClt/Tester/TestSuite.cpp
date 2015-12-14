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
#include "ShortMsgClt/Tester/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "ShortMsgClt/Tester/ShortMsgCltTester.h"
#include "XmlUtil/XmlTag.h"


OmnTestSuitePtr		
AosShortMsgCltTestSuite::getSuite(const AosXmlTagPtr &testers)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("ShortMsgClt", "Test Suite");

	if (!testers)
	{
		// Get all the testers
		suite->addTestPkg(OmnNew AosShortMsgCltTester());
		return suite;
	}

	AosXmlTagPtr tester = testers->getFirstChild(AOSTAG_TESTER);
	if (tester)
	{
		suite->addTestPkg(OmnNew AosShortMsgCltTester());
	}

	return suite;
}
