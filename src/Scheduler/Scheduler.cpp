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
#include "Scheduler/Scheduler.h"

#include "Scheduler/Schedule.h"
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "Util/Buff.h"
#include "SEInterfaces/Ptrs.h"
#include "Porting/Sleep.h"

AosScheduler::AosScheduler()
:
mLock(OmnNew OmnMutex())
{
}


AosScheduler::~AosScheduler()
{
}

bool 
AosScheduler::run(const AosRundataPtr &rdata) 
{
	return true;
}

bool 
AosScheduler::run(const AosRundataPtr &rdata, const AosXmlTagPtr &xml) 
{
	return true;
}

