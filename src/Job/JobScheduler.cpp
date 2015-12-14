////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 05/16/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobScheduler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Job/JobSchedulerId.h"
#include "Job/JobSchedulerNorm.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


static OmnMutex				sgLock;
static AosJobSchedulerPtr	sgJobSchedulers[AosJobSchedulerId::eMax];
static bool					sgInited = false;


AosJobScheduler::AosJobScheduler(
		const AosJobSchedulerId::E type, 
		const OmnString &name, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		registerJobScheduler(this, name);
	}
}


AosJobScheduler::~AosJobScheduler()
{
}


bool 
AosJobScheduler::init()
{
	if (sgInited) return true;
	static AosJobSchedulerNorm	lsJobSchedulerNorm(true);
	sgInited = true;
	return true;
}


bool
AosJobScheduler::registerJobScheduler(
		AosJobScheduler *scheduler,
		const OmnString &name)
{
	AosJobSchedulerId::E type = scheduler->mType;
	if (!AosJobSchedulerId::isValid(type))
	{
		OmnAlarm << "Invalid scheduler id: " << type << enderr;
		return false;
	}

	sgLock.lock();
	if (sgJobSchedulers[type])
	{
		sgLock.unlock();
		OmnAlarm << "Job already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgJobSchedulers[type] = scheduler;
	bool rslt = AosJobSchedulerId::addName(name, type);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add scheduler name" << enderr;
	}
	return rslt;
}


AosJobSchedulerPtr
AosJobScheduler::createJobScheduler(
		const AosXmlTagPtr &sdoc,
		const AosRundataPtr &rdata)
{
	if (!sgInited) init();

	aos_assert_rr(sdoc, rdata, 0);
	OmnString idstr = sdoc->getAttrStr(AOSTAG_JOBSCHEDULER_ID);
	if (idstr == "")
	{
		AosSetErrorU(rdata, "missing_jobscheduler_id:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosJobSchedulerId::E id = AosJobSchedulerId::toEnum(idstr);
	if (!AosJobSchedulerId::isValid(id))
	{
		AosSetErrorU(rdata, "invalid_jobscheduler_id:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosJobSchedulerPtr splitter = sgJobSchedulers[id];
	if (!splitter)
	{
		AosSetErrorU(rdata, "job_scheduler_not_defined:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return splitter->create(sdoc, rdata);
}
#endif
