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
// 2014/11/28 Created by White
////////////////////////////////////////////////////////////////////////////
#include "RawSE/Tester/TestSuite.h"
#include "RawSE/Tester/RawSETester.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosRawSETestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("RawSETestSuite", "RawSE Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosRawSETester());

	return suite;
}