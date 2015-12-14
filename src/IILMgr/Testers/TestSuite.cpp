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
#include "IILMgr/Testers/TestSuite.h"

#include "IILMgr/Testers/IILTester.h"
#include "IILMgr/Testers/IILTester2.h"
#include "IILMgr/Testers/IILTester3.h"
#include "IILMgr/Testers/JimoTableTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"


OmnTestSuitePtr		
AosIILTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("AosJimoTableTester", "");

	//suite->addTestPkg(OmnNew AosIILTester3());
	suite->addTestPkg(OmnNew AosJimoTableTester());
	return suite;
}

