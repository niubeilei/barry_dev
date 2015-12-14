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
// 2014/10/19 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Torturers/XmlDocTorturer/TestSuite.h"

#include "Rundata/Rundata.h"
#include "Tester/TestPkg.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosXmlDocTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UtilTestSuite", "Util Library Test Suite");

	// suite->addTestPkg(OmnNew AosVarUnintTester());

	return suite;
}
