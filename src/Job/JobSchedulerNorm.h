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
// 05/15/2012 Created by Ketty 
// 06/02/2012 Added member data 'mNumSlots'.
////////////////////////////////////////////////////////////////////////////
#if 0
#ifndef AOS_Job_JobSchedulerNorm_h
#define AOS_Job_JobSchedulerNorm_h

#include "Job/JobSchedulerId.h"
#include "Job/JobScheduler.h"
#include "Rundata/Ptrs.h"
#include "Thread/Mutex.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"

#include <vector>
#include <list>
using namespace std;


class AosJobSchedulerNorm : public AosJobScheduler 
{
private:
	enum
	{
		eMaxSlots = 50
	};

	OmnMutexPtr				mLock;
	int						mNumPhysicals;
	int						mNumSlots;
	vector<int>				mNumActiveMapTasks;
	vector<int>				mNumActiveReduceTasks;

public:
	AosJobSchedulerNorm(const bool flag);
	~AosJobSchedulerNorm();

	virtual bool startTasks(
				list<AosTaskInfoPtr> &task_list,
				const AosRundataPtr &rdata);

	virtual bool nextTask(
				list<AosTaskInfoPtr> &task_list,
				const AosTaskInfoPtr &finished_task,
				const AosRundataPtr &rdata);

	virtual bool rerunTask(
				const AosTaskObjPtr &task, 
				const AosRundataPtr &rdata);
	
	virtual AosJobSchedulerPtr create(
				const AosXmlTagPtr &sdoc, 
				const AosRundataPtr &rdata) const;

	bool	config(
				const AosXmlTagPtr &sdoc, 
				const AosRundataPtr &rdata);

private:
	bool	startTasksPriv(
				list<AosTaskInfoPtr> &task_list,
				const AosRundataPtr &rdata);

	bool	nextTaskPriv(
				list<AosTaskInfoPtr> &task_list,
				const AosTaskInfoPtr &finished_task,
				const AosRundataPtr &rdata);
};
#endif
#endif

