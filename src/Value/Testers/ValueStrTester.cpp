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
// 2013/12/01	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Value/Testers/ValueStrTester.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Rundata/Rundata.h"
#include "Rundata/RundataParm.h"
#include "SEInterfaces/BitmapObj.h"
#include "SEInterfaces/ActionObj.h"
#include "SEInterfaces/ActionType.h"
#include "Tester/TestMgr.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadPool.h"
#include "Value/ValueDefs.h"

static u64 sgCounts[10];
static u64 sgCount = 0;
static OmnMutex sgLock;

AosValueStrTester::AosValueStrTester()
:
OmnThrdShellProc("value_tester"),
mNumThreads(5),
mInstId(-1)
{
	for (int i=0; i<mNumThreads; i++) sgCounts[i] = 0;
}


AosValueStrTester::AosValueStrTester(const int inst_id)
:
OmnThrdShellProc("value_tester"),
mNumThreads(5),
mInstId(inst_id)
{
	for (int i=0; i<mNumThreads; i++) sgCounts[i] = 0;
}


bool 
AosValueStrTester::start()
{
	init();
	basicTest();
	return true;
}


bool
AosValueStrTester::init()
{
	config();
	return true;
}


bool
AosValueStrTester::config()
{
	AosXmlTagPtr conf = OmnApp::getAppConfig();
	aos_assert_r(conf, false);
	AosXmlTagPtr tag = conf->getFirstChild("value_tester");
	if (!tag) return true;

	mTries = tag->getAttrInt("tries", 100);
	mNumThreads = tag->getAttrInt("num_threads", 1);

	OmnScreen << "Num Threads: " << mNumThreads << endl;
	OmnScreen << "Num Tries: " << mTries<< endl;
	
	return true;
}


bool 
AosValueStrTester::basicTest()
{
	vector<OmnThrdShellProcPtr> runners;
	for (int i=0; i<mNumThreads; i++)
	{
		OmnThrdShellProcPtr runner = OmnNew AosValueStrTester(i);
		runners.push_back(runner);
	}
	OmnThreadPool::runProcAsync(runners);

	u64 tt1 = OmnGetTimestamp();
	while (1)
	{
		OmnSleep(1);
		u64 total = sgCount;
		OmnString ss;
		ss << total;
		ss.convertToScientific();
		u64 tt2 = OmnGetTimestamp();
		OmnString ss2;
		ss2 << total*1000000/(tt2-tt1);
		ss2.convertToScientific();
		OmnScreen << "Total: " << ss << ". Rate: " << ss2 << endl;
	}
	return true;
}


bool
AosValueStrTester::run()
{
	int nn = 0;
	for (u64 i=0; i<1000000000; i++)
	{
		if (nn++ == 1000000)
		{
			sgLock.lock();
			sgCount += nn;
			sgLock.unlock();
			nn = 0;
			// OmnScreen << "Inst: " << mInstId << ". Counts: " << i << endl;
		}

		AosValueRslt rslt;
		rslt.setStrValue("123456789");
		OmnString vv = rslt.getOmnStr("");
		aos_assert_r(vv == "123456789", false);
	}
	return true;
}


bool
AosValueStrTester::procFinished()
{
	return true;
}

