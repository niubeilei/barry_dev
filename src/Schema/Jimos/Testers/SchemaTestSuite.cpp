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
// 2013/10/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Schema/Jimos/Testers/SchemaTestSuite.h"

#include "Schema/Jimos/Testers/SchemaUniLenTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosSchemaTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("Schema", "Schema Test Suite");

	suite->addTestPkg(OmnNew AosSchemaUniLenTester());

	return suite;
}
