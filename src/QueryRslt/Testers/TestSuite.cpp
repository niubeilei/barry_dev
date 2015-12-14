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
// 09/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "QueryRslt/Testers/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "QueryRslt/Testers/BitmapTester.h"
#include "QueryRslt/Testers/BitmapTester.h"
#include "QueryRslt/Testers/BitmapTesterNew.h"
#include "QueryRslt/Testers/BitmapSectionNew.h"
#include "Util/OmnNew.h"

AosQueryRsltTestSuite::AosQueryRsltTestSuite()
{

}

AosQueryRsltTestSuite::~AosQueryRsltTestSuite()
{

}

OmnTestSuitePtr		
AosQueryRsltTestSuite::getSuite(const OmnString &type)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("BitmapTestSuite", "");
	//suite->addTestPkg(OmnNew AosBitmapSectionNew());
	suite->addTestPkg(OmnNew AosBitmapTesterNew());
	return suite;
}
