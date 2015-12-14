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
// 05/18/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/TaskMgrObj.h"

AosTaskMgrObjPtr AosTaskMgrObj::smTaskMgr;

bool 
AosTaskMgrObj::addTaskStatic(
			const int crt_jobsvrid,
			const u64 &task_docid,
			const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->addTask(crt_jobsvrid, task_docid, rdata);
}

bool
AosTaskMgrObj::addTaskLocalStatic(
			const u64 &job_docid,
			const OmnString &logic_id,
			const OmnString &task_type,
			const int crt_jobsvrid, 
			const u64 &task_docid,
			const u64 &start_stamp,
			const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->addTaskLocal(job_docid, logic_id, task_type, crt_jobsvrid, task_docid, start_stamp, rdata);
}

bool
AosTaskMgrObj::updateTaskStatusStatic(
			const AosTaskStatus::E &status,
			const AosTaskErrorType::E &error_type,
			const OmnString &error_msg,
			const u32 logic_pid, 
			const u64 &task_docid, 
			const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->updateTaskStatus(status, error_type, error_msg, logic_pid, task_docid, rdata);
}

bool
AosTaskMgrObj::startTaskProcCbStatic(const u32 logic_pid, const int proc_pid, const int svr_id) 
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->startTaskProcCb(logic_pid, proc_pid, svr_id);
}

bool
AosTaskMgrObj::stopTaskProcCbStatic(const u32 logic_pid, const int svr_id) 
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->stopTaskProcCb(logic_pid, svr_id);
}

bool
AosTaskMgrObj::updateTaskProgressStatic(
		const u64 &task_docid,
		const int percent,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->updateTaskProgress(
					task_docid,
					percent,
					rdata);
}

bool 
AosTaskMgrObj::updateTaskOutPutStatic(
		const u64 &task_docid,
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->updateTaskOutPut(
						task_docid,
						xml,
						rdata);
}

bool
AosTaskMgrObj::updateTaskSnapShotsStatic(
		const u64 &task_docid,
		const AosXmlTagPtr &snapshots_tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->updateTaskSnapShots(
							task_docid,
							snapshots_tag,
							rdata);
}

bool
AosTaskMgrObj::updateTaskProcNumStatic(
		const u64 &task_docid,
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->updateTaskProcNum(
						task_docid,
						xml,
						rdata);
}

bool 
AosTaskMgrObj::getTaskRunnerInfosStatic(
			const u64 &job_docid,
			const u32 num_slots,
			const int crt_job_svrid,
			vector<AosTaskRunnerInfoPtr> &run_infos,
			const AosRundataPtr &rdata)
{
	aos_assert_r(smTaskMgr, false);
	return smTaskMgr->getTaskRunnerInfos(
							job_docid,
							num_slots,
							crt_job_svrid,
							run_infos,
							rdata);
}

