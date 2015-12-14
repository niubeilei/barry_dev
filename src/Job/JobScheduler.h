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
#ifndef AOS_Job_JobScheduler_h
#define AOS_Job_JobScheduler_h

#include "Job/JobSchedulerId.h"
#include "Job/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "TaskUtil/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <list>

using namespace std;


class AosJobScheduler : virtual public OmnRCObject
{
	OmnDefineRCObject;

protected:
	AosJobSchedulerId::E	mType;

public:
	AosJobScheduler(
			const AosJobSchedulerId::E type,
			const OmnString &name,
			const bool flag);
	~AosJobScheduler();

	virtual bool startTasks(
				list<AosTaskInfoPtr> &task_list,
				const AosRundataPtr &rdata) = 0;

	virtual bool nextTask(
				list<AosTaskInfoPtr> &task_list,
				const AosTaskInfoPtr &finished_task,
				const AosRundataPtr &rdata) = 0;

	virtual bool rerunTask(
				const AosTaskObjPtr &task, 
				const AosRundataPtr &rdata) = 0;

	virtual AosJobSchedulerPtr create(
				const AosXmlTagPtr &sdoc, 
				const AosRundataPtr &rdata) const = 0;

	static AosJobSchedulerPtr createJobScheduler(
				const AosXmlTagPtr &sdoc, 
				const AosRundataPtr &rdata);

	static bool init();

private:
	bool	registerJobScheduler(
				AosJobScheduler *scheduler,
				const OmnString &name);
};
#endif
#endif

