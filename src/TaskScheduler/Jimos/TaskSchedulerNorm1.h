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
#ifndef AOS_TaskScheduler_Jimos_TaskSchedulerNorm_h
#define AOS_TaskScheduler_Jimos_TaskSchedulerNorm_h

#include "TaskScheduler/TaskScheduler.h"
#include "Rundata/Ptrs.h"
#include "SEInterfaces/TaskType.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"


class AosTaskSchedulerNorm : public AosTaskScheduler 
{
	OmnDefineRCObject;
private:
	enum
	{
		eMaxSlots = 50
	};

	int						mNumSlots;
	vector<int>				mNumActiveMapTasks;
	vector<int>				mNumActiveReduceTasks;

public:
	AosTaskSchedulerNorm();
	AosTaskSchedulerNorm(const AosRundataPtr &rdata, const AosXmlTagPtr &doc);
	~AosTaskSchedulerNorm();

	virtual AosJimoPtr cloneJimo(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &worker_doc,
				const AosXmlTagPtr &jimo_doc)  const;

	virtual bool nextTask(
				const AosRundataPtr &rdata,
				const AosTaskRunnerInfoPtr &run_info);

	virtual bool config(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &sdoc); 

	virtual bool returnNumActive(
				const AosRundataPtr &rdata,
				const AosTaskInfoPtr &info);

	virtual bool isTaskSvrBusy(const int physical_id);
private:
	bool	startTasksPriv(
				const int &physical_id,
				const AosRundataPtr &rdata);
};
#endif
#endif

