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
// 2015-05-30 Created by Phil
////////////////////////////////////////////////////////////////////////////
#include "JimoRaft/Tester/JimoRaftTestSuite.h"
#include "JimoRaft/Tester/RaftTester.h"
#include "DfmUtil/DfmDoc.h"
#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosJimoRaftTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("JimoRaftTestSuite", "JimoRaft Test Suite");

	// Now add all the testers
	suite->addTestPkg(OmnNew AosRaftTester());
	return suite;
}
