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
// 10/30/2013	Created by Young
////////////////////////////////////////////////////////////////////////////
#include "DataSplitter/Jimos/Tester/TestSuite.h"
#include "DataSplitter/Jimos/Tester/DataSplitterDirTester.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"


OmnTestSuitePtr		
AosDataSplitterJimosTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("AosDataSplitterJimoTester", "");

	suite->addTestPkg(OmnNew AosDataSplitterDirTester());

	return suite;
}
