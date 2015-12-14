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
//
// Modification History:
// 2013/10/29:	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Scheduler/Testers/SchedulerTester.h"

#include "Scheduler/Testers/Ptrs.h"
#include "Scheduler/Ptrs.h"
#include "Scheduler/Scheduler.h"
#include "Scheduler/Schedule.h"
#include "API/AosApi.h"
#include "Debug/Debug.h"
#include "Porting/MinMax.h"
#include "Porting/Sleep.h"
#include "Random/RandomUtil.h"
#include "Tester/Test.h"
#include "Tester/TestMgr.h"
#include "Thread/Thread.h"
#include "Thread/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include <map>

#include "API/AosApiS.h" 


AosSchedulerTester::AosSchedulerTester() 
{
}


bool AosSchedulerTester::start()
{
	// Test default constructor
	cout << "    Start AosScheduler Tester..." << endl;
	basic();
	return true;
}


bool 
AosSchedulerTester::basic()
{
	AosRundataPtr rdata = OmnNew AosRundata(AosMemoryCheckerArgsBegin);

	OmnString jimo_schedule_config = "<jimo zky_objid=\"jackie_schedule\" ";
	jimo_schedule_config << "zky_classname=\"AosScheduleTime\" "
		<< "zky_otype=\"zkyotp_jimo\" "
		<< "repeattimes=\"2\" "
		<< "repeat=\"hourly\" "
//		<< "repeat_seconds=\"10\" "
		<< "time=\"2013/11/01 12\" "
		<< "current_version=\"1.0\">"
		<< "<versions>"
		<< "<ver_0>libSchedulerJimos.so</ver_0>"
		<< "</versions>"
		<< "</jimo>";
	AosXmlTagPtr schedule_jimo_doc = AosStr2Xml(rdata, jimo_schedule_config AosMemoryCheckerArgs);
	aos_assert_r(schedule_jimo_doc, false);

	OmnString worker_schedule_config = "<schedule time=\"16:00\" ";
	worker_schedule_config << "repeat=\"hourly\"></schedule>";
	AosXmlTagPtr schedule_doc = AosStr2Xml(rdata, worker_schedule_config AosMemoryCheckerArgs);
	aos_assert_r(schedule_doc, false);

	AosJimoPtr schedule_jimo = AosCreateJimo(rdata, schedule_doc, schedule_jimo_doc);
	aos_assert_r(schedule_jimo, false);
//	aos_assert_r(AOS_JIMOTYPE_SCHEDULE == schedule_jimo->getJimoType(), false);
	AosSchedulePtr schedule = dynamic_cast<AosSchedule*>(schedule_jimo.getPtr());
//	schedule->run(rdata);

//	schedule_jimo->run(rdata);

	
	OmnString jimo_scheduler_config = "<jimo zky_objid=\"jackie_scheduler\" ";
	jimo_scheduler_config << "zky_classname=\"AosSchedulerOneSlot\" "
		<< "zky_otype=\"zkyotp_jimo\" "
		<< "current_version=\"1.0\">"
		<< "<versions>"
		<< "<ver_0>libSchedulerJimos.so</ver_0>"
		<< "</versions>"
		<< "</jimo>";
	AosXmlTagPtr scheduler_doc = AosStr2Xml(rdata, jimo_scheduler_config AosMemoryCheckerArgs);
	aos_assert_r(scheduler_doc, false);

	AosJimoPtr scheduler_jimo = AosCreateJimo(rdata, scheduler_doc);
	aos_assert_r(scheduler_jimo, false);

	AosSchedulerPtr scheduler = dynamic_cast<AosScheduler*>(scheduler_jimo.getPtr());

//	AosSchedulerPtr scheduler = OmnNew AosScheduler();

	OmnBeginTest << "Test";

	// Schedule(10, "seconds", 20, 10);
	// Schedule(5, "seconds", 10, 5);
	// Schedule(5, "seconds", 10, 5);
	//scheduler_jimo->addSchedule();

	u32 runtime = 3;
	AosScheduleTestRunnablePtr run1 = OmnNew AosScheduleTestRunnable(runtime);

	schedule->setRunner(run1);

	scheduler->addSchedule(rdata, schedule);

	scheduler->run(rdata);

	return true;
}


bool    
AosSchedulerTester::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
    while (state == OmnThrdStatus::eActive)
	{
		OmnSleep(1);
	}
	return true;
}


bool    
AosSchedulerTester::signal(const int threadLogicId)
{
	return true;
}


