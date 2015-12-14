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
// 08/09/2013 	Created by Young pan 
////////////////////////////////////////////////////////////////////////////
#include "Snapshot/Testers/TestSuite.h"

#include "Snapshot/Testers/SnapShotTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosSnapShotTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SnapShotTester", "");
	suite->addTestPkg(OmnNew AosSnapShotTester());
	return suite;
}
