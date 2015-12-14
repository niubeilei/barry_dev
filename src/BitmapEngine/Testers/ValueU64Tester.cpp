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
// 2013/02/15	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Value/Testers/ValueU64Tester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"


AosValueU64Tester::AosValueU64Tester()
{
}


bool 
AosValueU64Tester::start()
{
	config();
	init();
	basicTest();
	return true;
}


bool
AosValueU64Tester::init()
{
	return true;
}


bool
AosValueU64Tester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("value_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", 100);
	
	return true;
}


bool 
AosValueU64Tester::basicTest()
{
	return true;
}

#endif
