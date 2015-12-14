////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2013/10/25 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_Scheduler_ScheduleDummy_h
#define Aos_Scheduler_ScheduleDummy_h

#include "Scheduler/Schedule.h"

class AosScheduleDummy : public AosSchedule
{
	OmnDefineRCObject;

public:
	AosScheduleDummy();
	AosScheduleDummy(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def);
	~AosScheduleDummy();

	// Schedule Interface
	virtual bool scheduleFinished();
	virtual bool readyToRun(AosRundataPtr &rdata);
	virtual bool runnableStarted();
	virtual bool setRunnableFinished();
};
#endif

