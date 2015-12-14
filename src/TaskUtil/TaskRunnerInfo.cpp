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
#include "TaskUtil/TaskRunnerInfo.h"

#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"


AosTaskRunnerInfo::AosTaskRunnerInfo()
{
}

AosTaskRunnerInfo::AosTaskRunnerInfo(
		const u64 &job_docid,
		const int task_run_svr_id,
		const bool idle)
:
mJobDocid(job_docid),
mLogicId(""),
mTaskDocid(0),
mTaskType(""),
mStatus(AosTaskStatus::eStop),
mTaskRunServerId(task_run_svr_id),
mTaskStartStampTime(0),
mProgress(0),
mIdle(idle),
mErrorType(AosTaskErrorType::eInvalid),
mErrmsg(""),
mOutPuts(""),
mProcNum("")
{
}

AosTaskRunnerInfo::AosTaskRunnerInfo(
		const u64 &job_docid,
		const OmnString &logic_id,
		const OmnString &task_type,
		const u64 &task_docid,
		const int task_run_svr_id,
		const AosTaskStatus::E sts,
		const int progress,
		const u64 &timestamp,
		const AosTaskErrorType::E &error_type,
		const OmnString &errmsg)
:
mJobDocid(job_docid),
mLogicId(logic_id),
mTaskDocid(task_docid),
mTaskType(task_type),
mStatus(sts),
mTaskRunServerId(task_run_svr_id),
mTaskStartStampTime(timestamp),
mProgress(progress),
mIdle(false),
mErrorType(error_type),
mErrmsg(errmsg),
mOutPuts(""),
mProcNum("")
{
}

AosTaskRunnerInfo::~AosTaskRunnerInfo()
{
}

bool
AosTaskRunnerInfo::serializeFrom(const AosBuffPtr &buff)
{
	mJobDocid = buff->getU64(0);
	mLogicId = buff->getOmnStr("");
	mTaskDocid = buff->getU64(0);
	mTaskType = buff->getOmnStr("");
	mStatus = (AosTaskStatus::E)buff->getU8(0);
	mTaskRunServerId = buff->getInt(-1);
	mTaskStartStampTime = buff->getU64(0);
	mProgress = buff->getInt(-1);
	mIdle = buff->getU8(0);
	mErrorType = (AosTaskErrorType::E)buff->getU8(0);
	mErrmsg = buff->getOmnStr("");
	mOutPuts = buff->getOmnStr("");
	mProcNum = buff->getOmnStr("");
	mSnapShots = buff->getOmnStr("");
	return true;
}


bool
AosTaskRunnerInfo::serializeTo(const AosBuffPtr &buff)
{
	buff->setU64(mJobDocid);
	buff->setOmnStr(mLogicId);
	buff->setU64(mTaskDocid);
	buff->setOmnStr(mTaskType);
	buff->setU8(mStatus);
	buff->setInt(mTaskRunServerId);
	buff->setU64(mTaskStartStampTime);
	buff->setInt(mProgress);
	buff->setU8(mIdle);
	buff->setU8(mErrorType);
	buff->setOmnStr(mErrmsg);
	buff->setOmnStr(mOutPuts);
	buff->setOmnStr(mProcNum);
	buff->setOmnStr(mSnapShots);
	return true;
}


OmnString
AosTaskRunnerInfo::toString()
{
	OmnString str = "Update Task Status:";
	str << mJobDocid << ":" << mLogicId << ":"
		<< mTaskDocid << ":" << mTaskDocid << ":"
		<< AosTaskStatus::toStr(mStatus);
	return str;
}

bool
AosTaskRunnerInfo::setTaskOutPuts(const vector<AosXmlTagPtr> &task_outputs)
{
	if (task_outputs.size() == 0)
	{
		return true;
	}
	OmnString str = "<output_data__n>";
	for (u32 i=0; i<task_outputs.size(); i++)
	{
		str << task_outputs[i]->toString();
	}
	str << "</output_data__n>";
	mOutPuts = str;
OmnScreen << "=====================================setTaskOutPuts" << mOutPuts << endl;
	return true;
}

bool
AosTaskRunnerInfo::setTaskProcNum(const AosXmlTagPtr &xml)
{
	if (xml)
	{
		mProcNum = xml->toString();
OmnScreen << "=====================================setTaskProcNum" << mProcNum << endl;
	}
	return true;
}

bool
AosTaskRunnerInfo::setTaskSnapShots(const AosXmlTagPtr &snapshots) 
{

	if (snapshots)
	{
		mSnapShots = snapshots->toString(); 
OmnScreen << "========================setTaskSnapShots: " << mSnapShots << endl;
	}
	return true; 
}

OmnString
AosTaskRunnerInfo::getTaskSnapShots() 
{ 
OmnScreen << "=========================getTaskSnapShots: " << mSnapShots << endl;
	return mSnapShots; 
}

OmnString	
AosTaskRunnerInfo::getTaskOutPuts() 
{ 
OmnScreen << "=========================getTaskOutputs: " << mOutPuts << endl;
	return mOutPuts; 
}
