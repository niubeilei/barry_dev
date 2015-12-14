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
#ifndef AOS_SEInterfaces_LogicTaskObj_h
#define AOS_SEInterfaces_LogicTaskObj_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/JobObj.h"
#include "TaskUtil/TaskInfo.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskUtil/LogicTaskType.h"
#include "Util/CompareFun.h"

#include <map>
using namespace std;

class AosLogicTaskObj : virtual public OmnRCObject
{
	OmnDefineRCObject;

public:
	virtual u64							getJobDocId() const = 0;
	virtual OmnString					getLogicId() const = 0;
	virtual AosXmlTagPtr				getDataColTags() const = 0;
	virtual int							getProgress() const = 0;
	virtual map<u64, AosTaskInfoPtr>& 	getAllTaskInfo() = 0;
	virtual AosLogicTaskType::E			getType() = 0 ;
	virtual int							getPhyId() = 0;
	virtual AosTaskStatus::E			getStatus() = 0 ;
	virtual AosTaskInfoPtr 				getTaskInfo(
											const u64 &task_docid, 
											const AosRundataPtr &rdata) = 0;
	virtual AosXmlTagPtr				getTaskDoc(const AosRundataPtr &rdata) = 0;
	//virtual AosTaskInfoPtr 				getTaskInfoLocked(
	//										const u64 &task_docid,
	//										const AosRundataPtr &rdata) = 0;
	virtual void						clear() = 0;
	//virtual void						print() = 0;
	//virtual void						sendDataToReduce() = 0;
	virtual void 						setStatus(const AosTaskStatus::E &sts) = 0;
	virtual void						setTaskInfoStatus(
											const AosTaskInfoPtr &info, 
											const AosTaskStatus::E sts) = 0;
	virtual bool						isFinished() const = 0;
	virtual bool 						isInited() const = 0; 
	virtual bool 						reset(const AosRundataPtr &rdata) = 0;
	virtual bool						stopAllTaskProcess(const AosRundataPtr &rdata) = 0;
	virtual bool						config(
											const AosXmlTagPtr &conf,
											const AosRundataPtr &rdata) = 0; 
	virtual bool						config(
											const AosJobObjPtr &job,
											const AosXmlTagPtr &conf,
											const AosRundataPtr &rdata) = 0; 
	virtual bool						setDataColTags(
											const AosXmlTagPtr &xml, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						updateTaskProgress(
											const AosJobObjPtr &job, 
											const u64 &task_docid, 
											const int percent, 
											const AosRundataPtr &rdata) = 0;
	//virtual bool						integrateResources(
	//										const AosJobObjPtr &job, 
	//										const AosRundataPtr &rdata) = 0;
	virtual bool						createMergeFileTask(
											const int level,
											const AosXmlTagPtr &datacol_tag, 
											const int physical_id, 
											list <AosXmlTagPtr> &info_list, 
											u64 &task_docid, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						createMergeFinishTask(
											const AosXmlTagPtr &datacol_tag, 
											const int physical_id, 
											const AosXmlTagPtr &info, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						createImportDocTaskPriv(
											const AosXmlTagPtr &dataproc_tag, 
											const int physical_id, 
											list<AosXmlTagPtr> &doc_list, 
											const OmnString &action_type, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						findFailedTaskBySvrId(
											const AosJobObjPtr &job, 
											const int death_svr_id, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						checkStartTask(
											const AosJobObjPtr &job, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						createTask(
											const AosJobObjPtr &obj, 
											const AosXmlTagPtr &sdoc, 
											const AosTaskDataObjPtr &task_data, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						createTask(
											const AosJobObjPtr &obj, 
											const AosXmlTagPtr &task_doc, 
											const AosJobMgrObj::OprId oprid, 
											const AosRundataPtr &rdata) = 0;
	virtual bool 						updateTaskStatus(
											const AosJobObjPtr &job, 
											const AosTaskRunnerInfoPtr &run_info, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						taskFinished(
											const u64 &task_docid, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						taskInfoFinished(
											const u64 &task_docid,
											const AosRundataPtr &rdata) = 0;
	virtual bool						taskStarted(
											const u64 &task_docid, 
											const u64 &timestamp, 
											const int task_serverid, 
											const AosRundataPtr &rdata) = 0;
	virtual bool						taskInfoStarted(
											const u64 &task_docid,
											const u64 &start_time,
											const int task_serverid,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						createOutputDatasetDocs(
											const AosRundataPtr &rdata,
											const AosJobObjPtr &job) = 0;
	virtual bool						createMapTasks(
											const AosRundataPtr &rdata,
											const AosJobObjPtr &job) = 0;
	virtual bool						createReduceTasks(
											const AosRundataPtr &rdata,
											const AosJobObjPtr &job) = 0;
	virtual bool						addTask(
											const AosRundataPtr &rdata,
											const AosXmlTagPtr &task_doc) = 0;
	virtual bool						addTaskLocked(
											const AosRundataPtr &rdata,
											const AosXmlTagPtr &task_doc) = 0;
	virtual bool                        taskFinished(              
											const AosJobObjPtr &job,             
						        			const AosTaskRunnerInfoPtr &run_info,
								        	const AosRundataPtr &rdata) = 0;
	virtual bool                        taskFailed(              
											const AosJobObjPtr &job,             
						        			const AosTaskRunnerInfoPtr &run_info,
								        	const AosRundataPtr &rdata) = 0;
	virtual vector<OmnString> 			getTriggerIds() = 0;
	virtual bool 						checkTrigger(const OmnString &id) = 0;

	// for data collector 
	virtual bool 						addOutput(
											const AosXmlTagPtr &output,
											const AosRundataPtr &rdata) = 0;
	virtual bool						addTriggedTaskid(
											const OmnString &task_id,
											const AosRundataPtr &rdata) = 0;
	virtual bool						getOutputFiles(
											vector<AosXmlTagPtr> &files,
											const AosRundataPtr &rdata) = 0;
	virtual bool						callBack(
											const AosXmlTagPtr &output,
											const AosRundataPtr &rdata) = 0;
	virtual bool						finishDataCollector(const AosRundataPtr &rdata) = 0;
	virtual void						setParentDataColId(const OmnString &name, const int &shuffle_id) = 0;
	virtual OmnString 					getDataColId() = 0;
	virtual int							getShuffleType() = 0;
	virtual void 						addStartNum() = 0;

	virtual bool						removeStartTask(const AosTaskInfoPtr &info) = 0;
	virtual bool						clearTaskInfo(
											const AosTaskInfoPtr &info) = 0;
	//Jozhi 2015-03-09
	virtual bool 						cleanDataCol(
											const u64 &task_docid,
											const AosRundataPtr &rdata) = 0;
	virtual bool 						eraseActiveTask(const AosXmlTagPtr &task_doc) = 0;
	virtual bool 						insertActiveTask(const AosXmlTagPtr &task_doc) = 0;
	virtual bool						merge(const AosLogicTaskObjPtr &logic_task) = 0;
	virtual set<OmnString>  getDataColIds() = 0;
	virtual bool						reStartLogicTask(
											const AosJobObjPtr &job, 
											const AosRundataPtr &rdata) = 0;
	virtual bool 						reStartDataCol(
											const AosJobObjPtr &job,
											const AosRundataPtr &rdata) = 0;
	virtual AosCompareFunPtr getComp() = 0;
};

#endif

