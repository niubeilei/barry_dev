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
#include "TaskScheduler/TaskSchedulerDummy.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/Ptrs.h"
#include "Util/Buff.h"


AosTaskSchedulerDummy::AosTaskSchedulerDummy()
{
}

AosTaskSchedulerDummy::AosTaskSchedulerDummy(const u32 version)
:
AosTaskScheduler(AOSTASKSCHEDULER_DUMMY, version)
{
}


AosTaskSchedulerDummy::~AosTaskSchedulerDummy()
{
}


bool
AosTaskSchedulerDummy::config(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &def) 
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosTaskSchedulerDummy::nextTask(
		const AosRundataPtr &rdata,
		const AosJobObjPtr &job,
		const AosTaskRunnerInfoPtr &info)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosTaskSchedulerDummy::returnNumActive(
		const AosRundataPtr &rdata,
		const int task_svr_id)
{
	OmnNotImplementedYet;
	return false;
}


bool 
AosTaskSchedulerDummy::recoverNumActive(
		const AosRundataPtr &rdata,
		const int task_svr_id)
{
	OmnNotImplementedYet;
	return false;
}

