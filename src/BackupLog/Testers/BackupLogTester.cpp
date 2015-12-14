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
// 2013/05/17 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BackupLog/Testers/IILTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "BackupLog/Ptrs.h"

AosIILTester::AosIILTester()
{
}


bool 
AosIILTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	mRundata->setSiteid(AosGetDftSiteId());
	OmnScreen << "    Start Tester ..." << endl;

	config();
	init();
	return basicTest();
}


bool
AosIILTester::init()
{
	return true;
}


bool
AosIILTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("bitmap_testers");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", eDftTries);
	return true;
}


bool 
AosIILTester::basicTest()
{
	return true;
}


