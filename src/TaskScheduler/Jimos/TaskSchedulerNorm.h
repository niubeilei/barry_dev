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
	vector<int>				mNumActiveTasks;

public:
	AosTaskSchedulerNorm();
	AosTaskSchedulerNorm(const OmnString &version);
	~AosTaskSchedulerNorm();

	virtual AosJimoPtr cloneJimo()  const;
	
	virtual bool nextTask(
				const AosRundataPtr &rdata,
				const AosJobObjPtr &job,
				const AosTaskRunnerInfoPtr &run_info);
/*
	virtual bool nextTask(
				const AosRundataPtr &rdata,
				const AosTaskRunnerInfoPtr &run_info);
*/
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

