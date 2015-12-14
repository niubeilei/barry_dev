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
#ifndef Aos_Scheduler_Testers_SchedulerTester_h
#define Aos_Scheduler_Testers_SchedulerTester_h

#include "Tester/TestPkg.h"
#include "Thread/Thread.h"
#include "Thread/ThreadedObj.h"
#include "Util/RCObjImp.h"
#include "Util/RCObject.h"
#include "Util/Tester/Ptrs.h"
#include "Util/SmtIdxVList.h"

#include "Porting/Sleep.h"
#include "SEInterfaces/RunnableObj.h"

class AosScheduleTestRunnable : public AosRunnableObj
{
	OmnDefineRCObject;

private : 

	u32 	start_time;
	u32 	end_time;
	bool	status; 
	u32 	runtime;
	int 	num_runs;

	bool finished;

	virtual bool run(const AosRundataPtr &rdata)
	{
		start_time = OmnGetSecond(); 
		status = true;
		num_runs++;
		OmnSleep(runtime);
		end_time = OmnGetSecond(); 
		return true;
	}

public :
	AosScheduleTestRunnable(u32 &time)
	:
	status(false),
	runtime(time),
	num_runs(0)	
	{
	}

	~AosScheduleTestRunnable()
	{
	}

};


class OmnTestMgr;


class AosSchedulerTester : public OmnTestPkg, public OmnThreadedObj
{
private:

public:
	AosSchedulerTester();
	~AosSchedulerTester() {}

	virtual bool		start();
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);

private:
	bool basic();
};
#endif

