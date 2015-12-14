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
// 2013/02/28	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Datalet/Testers/DataletBuffTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Datalet/DataletBuff.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"


AosDataletBuffTester::AosDataletBuffTester()
{
}


bool 
AosDataletBuffTester::start()
{
	config();
	init();
	basicTest();
	return true;
}


bool
AosDataletBuffTester::init()
{
	return true;
}


bool
AosDataletBuffTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("value_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", 100);
	
	return true;
}


bool 
AosDataletBuffTester::basicTest()
{
	AosDataletBuff datalet;

	datalet.setU64("test", 100);
	u64 vv = datalet.getU64("test", 0);
	aos_assert_r(vv == 100, false);
	return true;
}


