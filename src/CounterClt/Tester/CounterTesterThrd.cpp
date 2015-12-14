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
// 2011/06/15	Created by Lynch 
////////////////////////////////////////////////////////////////////////////
#include "CounterClt/Tester/CounterTesterThrd.h"

#include "alarm_c/alarm.h"
#include "Debug/Debug.h"
#include "SEUtil/DocTags.h"
#include "SEUtil/XmlTag.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Random/RandomUtil.h"
#include "Util/OmnNew.h"
#include "Util1/Time.h"
#include "CounterUtil/CounterName.h"
#include "CounterClt/Tester/CounterTester.h"
#include "CounterClt/Tester/Ptrs.h"
#include "CounterClt/CounterClt.h"


AosCounterTesterThrd::AosCounterTesterThrd(
		const AosCounterTesterPtr &tester,
		const int thrdid)
:
mThreadId(thrdid),
mTester(tester)
{
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "Thrd", mThreadId, true, true, __FILE__, __LINE__);
	mThread->start();
}


bool
AosCounterTesterThrd::threadFunc(
		OmnThrdStatus::E &state,
        const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
    {
		mTester->basicTest(mThreadId);
		//mTester->threadFinished(mThreadId);
        return true;
    }
    return false;
}

