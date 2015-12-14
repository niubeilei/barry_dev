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
#include "Scheduler/ScheduleDummy.h"

AosScheduleDummy::AosScheduleDummy()
{
}


AosScheduleDummy::AosScheduleDummy(
		const AosRundataPtr &rdata, 
		const AosXmlTagPtr &schedule_def)
:
AosSchedule(rdata, schedule_def)
{
}


AosScheduleDummy::~AosScheduleDummy()
{
}


bool 
AosScheduleDummy::scheduleFinished()
{
	return true;
}


bool 
AosScheduleDummy::readyToRun(AosRundataPtr &rdata)
{
	return true;
}


bool 
AosScheduleDummy::runnableStarted()
{
	return true;
}


bool 
AosScheduleDummy::setRunnableFinished()
{
	return true;
}
