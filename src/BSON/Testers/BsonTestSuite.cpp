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
// 2014/08/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BSON/Testers/BsonTestSuite.h"

#include "BSON/Testers/BsonTester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "DataRecord/Ptrs.h"


OmnTestSuitePtr		
AosBsonTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("UtilTestSuite", "Util Library Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosBsonTester());

	return suite;
}
