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
// 12/26/2012 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_TaskUtil_LogicMapTask_h
#define AOS_TaskUtil_LogicMapTask_h

#include "Thread/Mutex.h"
#include "Rundata/Rundata.h"
#include "TaskUtil/LogicTask.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "TaskUtil/Ptrs.h"

using namespace std;

class AosLogicMapTask : public AosLogicTask
{
public:
	AosLogicMapTask(const u64 &job_docid);
	~AosLogicMapTask();
	virtual bool			config(
								const AosJobObjPtr &job,
								const AosXmlTagPtr &conf,
								const AosRundataPtr &rdata); 
	virtual bool			taskFinished(
								const u64 &task_docid, 
								const AosRundataPtr &rdata);
	virtual bool			createMapTasks(
								const AosRundataPtr &rdata,
								const AosJobObjPtr &job);
	virtual bool			taskFinished(
								const AosJobObjPtr &job,
								const AosTaskRunnerInfoPtr &run_info,
								const AosRundataPtr &rdata);
	virtual bool			reStartLogicTask(
								const AosJobObjPtr &job, 
								const AosRundataPtr &rdata);
private:
	bool					create(
								const AosJobObjPtr &job,
								const AosRundataPtr &rdata);
	bool					setDataSetConfig(
								int &physical_id, 
								const AosXmlTagPtr &input_dataset_tag,
								const AosRundataPtr &rdata);
	//AosXmlTagPtr			createTaskDoc(
	//							const AosJobObjPtr &job,
	//							const u32 physical_id,
	//							const OmnString &task_id,
	//							const OmnString &logic_id,
	//							const u32 max_num_thrd,
	//							const OmnString &input_dataset_str,
	//							const OmnString &output_dataset_str,
	//							const AosXmlTagPtr &action_tag,
	//							const AosRundataPtr &rdata);
	bool					updateTaskOutPutPriv(
								const u64 &task_docid,
								const AosXmlTagPtr &outputs,
								const AosRundataPtr &rdata);
	bool					taskInfoFinished(
								const u64 &task_docid,
								const AosRundataPtr &rdata);
};

#endif

