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
// 2013/10/31 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Scheduler/SchedulerDummy.h"


AosSchedulerDummy::AosSchedulerDummy()
{
}


AosSchedulerDummy::~AosSchedulerDummy()
{
}

bool	
AosSchedulerDummy::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	return true;
}

bool	
AosSchedulerDummy::signal(const int threadLogicId)
{
	return true;
}

u64 AosSchedulerDummy::addSchedule( const AosRundataPtr &rdata, 
				 const AosSchedulePtr &schedule)
{
	return 0;
}

bool AosSchedulerDummy::removeSchedule(	const AosRundataPtr &rdata, 
						const u64 schedule_id)
{
	return true;
}

bool	AosSchedulerDummy::start()
{
	return true;
}
