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
// 2013/02/08	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BitmapEngine/Testers/BitmapCacheTester.h"

#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Tester/TestMgr.h"


AosBitmapCacheTester::AosBitmapCacheTester()
{
}


bool 
AosBitmapCacheTester::start()
{
	mRundata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);
	cout << "    Start Tester ..." << endl;
	config();
	basicTest();
	return true;
}


bool
AosBitmapCacheTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("iilmgr_big_testers");
	if (!tag) return true;

	return true;
}


bool 
AosBitmapCacheTester::basicTest()
{
	// BitmapCache caches bitmaps. This torturer does the following:
	// 1. Randomly generate a bitmap
	// 2. Add the bitmap to the cache
	// 3. Get a bitmap
	// 4. 
	return true;
}


