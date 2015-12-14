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
// 10/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILMerger/Tester/TestSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "IILMerger/Tester/IILMergerTester.h"
#include "IILMerger/Tester/IILMergerTester2.h"
#include "Util/OmnNew.h"


OmnTestSuitePtr		
AosIILTransTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("TestSuite", "IILTrans Library Test Suite");

	//suite->addTestPkg(OmnNew AosIILMergerTester2());
	suite->addTestPkg(OmnNew AosIILMergerTester());

	return suite;
}
