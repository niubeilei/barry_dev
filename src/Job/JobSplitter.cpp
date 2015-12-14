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
// 04/30/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "Job/JobSplitter.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Job/JobSplitterAuto.h"
#include "Job/JobSplitterDir.h"
#include "Job/JobSplitterDocIds.h"
#include "Job/JobSplitterFile.h"
#include "Job/JobSplitterDoc.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


static OmnMutex				sgLock;
static AosJobSplitterObjPtr	sgJobSplitters[AosJobSplitterId::eMax];
static OmnMutex				sgInitLock;
static bool					sgInited = false;


AosJobSplitter::AosJobSplitter(
		const OmnString &name, 
		const AosJobSplitterId::E type, 
		const bool regflag)
:
mName(name),
mType(type)
{
	if (regflag)
	{
		registerJobSplitter(this, name);
	}
}


AosJobSplitter::~AosJobSplitter()
{
}


bool 
AosJobSplitter::init()
{
	if (sgInited) return true;
	
	sgInitLock.lock();
	sgInited = true;
	sgInitLock.unlock();
	
	//static AosJobSplitterAuto			lsSpliterAuto(true);
	static AosJobSplitterDir			lsSpliterDir(true);
	static AosJobSplitterDocIds			lsSpliterDocIds(true);
	static AosJobSplitterFile			lsSpliterFile(true);
	//static AosJobSplitterDoc			lsSpliterDoc(true);

	return true;
}


bool
AosJobSplitter::registerJobSplitter(AosJobSplitter *job, const OmnString &name)
{
	AosJobSplitterId::E type = job->mType;
	if (!AosJobSplitterId::isValid(type))
	{
		OmnAlarm << "Invalid job id: " << type << enderr;
		return false;
	}

	sgLock.lock();
	if (sgJobSplitters[type])
	{
		sgLock.unlock();
		OmnAlarm << "Job already registered: " << name << ":" << type << enderr;
		return false;
	}

	sgJobSplitters[type] = job;
	bool rslt = AosJobSplitterId::addName(name, type);
	sgLock.unlock();
	if (!rslt)
	{
		OmnAlarm << "Failed add job name" << enderr;
	}
	return rslt;
}


AosJobSplitterObjPtr
AosJobSplitter::createJobSplitter(
		const AosXmlTagPtr &sdoc,
		map<OmnString, OmnString> &job_env,
		const AosRundataPtr &rdata)
{
	if (!sgInited) init();

	aos_assert_r(sdoc, 0);
	AosJobSplitterId::E id = AosJobSplitterId::toEnum(sdoc->getAttrStr(AOSTAG_JOBSPLITTER_ID));
	if (!AosJobSplitterId::isValid(id))
	{
		AosSetErrorU(rdata, "invalid_job_split_id:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	AosJobSplitterObjPtr splitter = sgJobSplitters[id];
	if (!splitter)
	{
		AosSetErrorU(rdata, "job_splitter_not_defined:") << sdoc->toString();
		OmnAlarm << rdata->getErrmsg() << enderr;
		return 0;
	}

	return splitter->create(sdoc, job_env, rdata);
}


bool
AosJobSplitter::splitTasks(
		const AosXmlTagPtr &def,
		const AosTaskDataObjPtr &task_data,
		vector<AosTaskDataObjPtr> &tasks,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
	return false;
}


void
AosJobSplitter::clear()
{
}

#endif
