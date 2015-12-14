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
// 08/21/2011	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "GICs/Testers/TestSuite.h"

#include "GICs/Testers/GicHtmlTreeSimpleTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosGicsTestSuite::getSuite(OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("GICs", "GICs Library Test Suite");

	if (type == "" || type == "*")
	{
		suite->addTestPkg(OmnNew AosGicHtmlTreeSimpleTester());
		return suite;
	}

	if (strcmp(type, "gichtmltreesimple") == 0)
	{
		suite->addTestPkg(OmnNew AosGicHtmlTreeSimpleTester());
	}
	
	return suite;
}
