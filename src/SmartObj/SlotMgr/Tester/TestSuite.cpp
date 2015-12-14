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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/SlotMgr/Tester/TestSuite.h"

#include "Rundata/Rundata.h"
#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "SmartObj/SlotMgr/SobjSlotMgr.h"
#include "SmartObj/SlotMgr/Tester/SlotMgrTester.h"


OmnTestSuitePtr		
AosSlotMgrTestSuite::getSuite(const AosXmlTagPtr &config)
{
	OmnTestSuitePtr suite = OmnNew OmnTestSuite("SlotMgrTestSuite", 
			"SlotMgr Library Test Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew AosSlotMgrTester());
	return suite;
}
