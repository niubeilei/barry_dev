////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: Tester.cpp
// Description:
//   
//
// Modification History:
// 11/17/2006 	Created by Chen Ding
//
////////////////////////////////////////////////////////////////////////////
#include "QoS/Tester1/QosTorturerSuite.h"

#include "Tester/TestSuite.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Util/OmnNew.h"
#include "QoS/Tester1/QosTorturer.h"


OmnTestSuitePtr		
AosQosTorturerSuite::getSuite()
{
	OmnTestSuitePtr suite = OmnNew 
		OmnTestSuite("QosTorturerSuite", "Qos Torturer Suite");

	// 
	// Now add all the testers
	//
	suite->addTestPkg(OmnNew QosTorturer());

	return suite;
}

