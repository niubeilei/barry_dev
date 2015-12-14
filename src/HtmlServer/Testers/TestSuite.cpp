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
// 2011/01/15	Created by Chen Ding 
////////////////////////////////////////////////////////////////////////////
#include "HtmlServer/Testers/TestSuite.h"

#include "HtmlServer/Testers/HtmlServerTester.h"
#include "IILMgr/IILStr.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosHtmlServerTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("HtmlServer", "HtmlServer Test Suite");

	suite->addTestPkg(OmnNew AosHtmlServerTester());

	return suite;
}
