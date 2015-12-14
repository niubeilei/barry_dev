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
// 02/26/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/TransMgr/Tester/SobjTransMgrTester.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Tester/TestPkg.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"



bool 
SobjTransMgrTester::start()
{
	OmnScreen << "Start SobjTransMgr Tester ...";
	return basicTest();
}


bool
SobjTransMgrTester::basicTest()
{
	// OmnCreateTc << (k1 == "test&buff*123pk") << endtc;
	return true;
}

