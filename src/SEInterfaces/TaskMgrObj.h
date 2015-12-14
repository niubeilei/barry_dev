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
#ifndef Aos_SEInterfaces_TaskMgrObj_h
#define Aos_SEInterfaces_TaskMgrObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "XmlUtil/Ptrs.h"
#include "XmlUtil/XmlTag.h"
#include "TaskUtil/TaskDriver.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskMgr/TaskErrorType.h"

class AosTaskMgrObj : virtual public OmnRCObject
{
	static AosTaskMgrObjPtr	smTaskMgr;
public:
	virtual bool addTask(
			const int crt_jobsvrid,
			const u64 &task_docid,
			const AosRundataPtr &rdata) = 0;

	virtual bool addTaskLocal(
			const u64 &job_docid,
			const OmnString &logic_id,
			const OmnString &task_type,
			const int crt_jobsvrid,
			const u64 &task_docid,
			const u64 &start_stamp,
			const AosRundataPtr &rdata) = 0;

	virtual bool getTaskRunnerInfos(
			const u64 &job_docid,
			const u32 num_slots,
			const int crt_job_svrid,
			vector<AosTaskRunnerInfoPtr> &run_infos,
			const AosRundataPtr &rdata) = 0;

	virtual bool updateTaskStatus(
			const AosTaskStatus::E &status,
			const AosTaskErrorType::E &error_type,
			const OmnString &error_msg,
			const u32 logic_pid,
			const u64 &task_docid, 
			const AosRundataPtr &rdata) = 0;

	virtual bool startTaskProcCb(const u32 logic_pid, const int proc_pid, const int svr_id) = 0;
	virtual bool stopTaskProcCb(const u32 logic_pid, const int svr_id) = 0;

	virtual bool stopTask(const u64 &task_docid, const AosRundataPtr &rdata) = 0;
	virtual bool updateTaskProgress(
			const u64 &task_doc,
			const int precent,
			const AosRundataPtr &rdata) = 0;

	virtual bool updateTaskOutPut(
			const u64 &task_docid,
			const AosXmlTagPtr &xml,
			const AosRundataPtr &rdata) = 0;

	virtual bool updateTaskSnapShots(
			const u64 &task_docid,
			const AosXmlTagPtr &snapshots_tag,
			const AosRundataPtr &rdata) = 0;

	virtual bool updateTaskProcNum(
			const u64 &task_docid,
			const AosXmlTagPtr &xml,
			const AosRundataPtr &rdata) = 0;

	virtual bool getDataColOutput(
			const OmnString &datacol_id,
			const u64 &job_id,
			const u64 &task_docid,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual bool createDataCol(
			const OmnString &datacol_id,
			const u64 &job_id,
			const u64 &task_docid,
			const OmnString &config,
			AosBuffPtr &buff,
			const AosRundataPtr &rdata) = 0;

	virtual AosBuffPtr getRunTimeValue(
			const u64 &job_docid,
			const u64 &task_docid,
			const OmnString &key,
			const AosRundataPtr &rdata) = 0;

	virtual AosBuffPtr setRunTimeValue(
				const u64 &job_docid,
				const u64 &task_docid,
				const OmnString &key,
				const AosValueRslt &value,
				const AosRundataPtr &rdata) = 0;
public:
	static void setTaskMgr(const AosTaskMgrObjPtr &d) {smTaskMgr = d;}
	static AosTaskMgrObjPtr getTaskMgr() {return smTaskMgr;}

	static bool addTaskStatic(
			const int crt_jobsvrid,
			const u64 &task_docid,
			const AosRundataPtr &rdata);

	static bool addTaskLocalStatic(
			const u64 &job_docid,
			const OmnString &logic_id,
			const OmnString &task_type,
			const int crt_jobsvrid, 
			const u64 &task_docid,
			const u64 &start_stamp,
			const AosRundataPtr &rdata);

	static bool updateTaskStatusStatic(
			const AosTaskStatus::E &status,
			const AosTaskErrorType::E &error_type,
			const OmnString &error_msg,
			const u32 logic_pid, 
			const u64 &task_docid, 
			const AosRundataPtr &rdata);

	static bool updateTaskOutPutStatic(
			const u64 &task_docid,
			const AosXmlTagPtr &xml,
			const AosRundataPtr &rdata);

	static bool updateTaskSnapShotsStatic(
			const u64 &task_docid,
			const AosXmlTagPtr &snapshots_tag,
			const AosRundataPtr &rdata);

	static bool startTaskProcCbStatic(const u32 logic_pid, const int proc_pid, const int svr_id);
	static bool stopTaskProcCbStatic(const u32 logic_pid, const int svr_id);
	static bool updateTaskProgressStatic(
			const u64 &task_doc,
			const int precent,
			const AosRundataPtr &rdata);

	static bool updateTaskProcNumStatic(
			const u64 &task_docid,
			const AosXmlTagPtr &xml,
			const AosRundataPtr &rdata);

	static bool getTaskRunnerInfosStatic(
			const u64 &job_docid,
			const u32 num_slots,
			const int crt_job_svrid,
			vector<AosTaskRunnerInfoPtr> &run_infos,
			const AosRundataPtr &rdata);

};

#endif
