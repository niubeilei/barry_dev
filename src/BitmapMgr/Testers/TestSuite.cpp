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
#include "BitmapMgr/Testers/TestSuite.h"

#include "BitmapMgr/Testers/BitmapMgrTester.h"
#include "BitmapMgr/Testers/BitmapTester.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"

AosBitmapMgrTestSuite::AosBitmapMgrTestSuite()
{

}

AosBitmapMgrTestSuite::~AosBitmapMgrTestSuite()
{

}

OmnTestSuitePtr		
AosBitmapMgrTestSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("BitmapMgrTestSuite", "");
	// suite->addTestPkg(OmnNew AosBitmapMgrTester());
	suite->addTestPkg(OmnNew AosBitmapTester());
	return suite;
}
