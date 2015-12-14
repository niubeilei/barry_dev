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
// 07/27/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TaskUtil/TaskInfo.h"

#include "SEInterfaces/TaskObj.h"
#include "SEInterfaces/TaskDataObj.h"


AosTaskInfo::AosTaskInfo()
:
mTaskDocid(0),
mTaskType(AosTaskType::eInvalid),
mStatus(AosTaskStatus::eStop),
mOptimalPhyId(-1),
mProgress(0),
mTaskServerId(-1),
mTaskStartStampTime(0),
mLogicId(""),
mPriority(0),
mUpdateEndTime(0),
mIsNeedSnapShot(false)
{
}


AosTaskInfo::~AosTaskInfo()
{
}


AosTaskInfoPtr
AosTaskInfo::create(const AosTaskObjPtr &task)
{
	aos_assert_r(task, 0);
	
	AosTaskInfoPtr info = OmnNew AosTaskInfo();

	info->mTaskDocid = task->getTaskDocid();
	info->mTaskType = task->getTaskType();
	info->mTaskId = task->getTaskId();
	info->mOptimalPhyId = task->getPhysicalId();
	info->mStatus = task->getStatus();
	if (info->mStatus == AosTaskStatus::eStart)
	{
		AosXmlTagPtr task_doc = task->getTaskDoc(task->getRundata());
		aos_assert_r(task_doc, 0);
		info->mTaskServerId = task_doc->getAttrInt(AOSTAG_TASKSERVERID, -1);
	}
	else
	{
		info->mTaskServerId = -1; 
	}
	info->mLogicId = task->getLogicId();
	info->mUpdateEndTime = 0;
	info->mIsNeedSnapShot = false;

	return info;
}


AosTaskInfoPtr
AosTaskInfo::create(const AosXmlTagPtr &task_doc)
{
	// Ketty 2013/12/26
	aos_assert_r(task_doc, 0);
	
	AosTaskInfoPtr info = OmnNew AosTaskInfo();

	info->mTaskDocid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	info->mTaskType = AosTaskType::toEnum(task_doc->getAttrStr(AOSTAG_TASK_TYPE));
	aos_assert_r(AosTaskType::isValid(info->mTaskType), 0);

	info->mTaskId = task_doc->getAttrStr(AOSTAG_TASKID);;
	info->mOptimalPhyId = task_doc->getAttrInt(AOSTAG_PHYSICALID, -1);
	info->mStatus = AosTaskStatus::toEnum(task_doc->getAttrStr(AOSTAG_STATUS));
	if (info->mStatus == AosTaskStatus::eStart)
	{
		info->mTaskServerId = task_doc->getAttrInt(AOSTAG_TASKSERVERID, -1);
	}
	else
	{
		info->mTaskServerId = -1; 
	}
	info->mLogicId = task_doc->getAttrStr(AOSTAG_LOGICID);
	info->mUpdateEndTime = 0;
	info->mIsNeedSnapShot = task_doc->getAttrBool("need_snapshot", false);

	return info;
}


