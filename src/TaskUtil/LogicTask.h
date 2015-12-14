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
#ifndef AOS_TaskUtil_LogicTask_h
#define AOS_TaskUtil_LogicTask_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JobObj.h"
#include "TaskUtil/TaskInfo.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskUtil/TaskDriver.h"
#include "Thread/Mutex.h"
#include "TaskUtil/ReduceTaskCreator.h"
#include "SEInterfaces/LogicTaskObj.h"
#include "TaskUtil/LogicTaskType.h"

#include <set>
#include <map>

using namespace std;


class AosLogicTask : public AosLogicTaskObj
{
public:
	enum
	{
		eMaxFilePreReduceTask = 1000,
		eMaxFileLenPreReduceTask = 50000000000LL,
		eMaxTaskFailNum = 20
	};

protected:
	AosLogicTaskType::E					mType;
	OmnMutexPtr							mLock;
	OmnString							mLogicId;
	int									mPhyId;
	u64									mJobDocid;
	int64_t								mStartedNum;
	int64_t								mFinishedNum;
	//used by old version
	int64_t								mMapTaskNum;
	int									mProgress;
	map<u64, AosTaskInfoPtr>			mTaskInfo;
	map<u64, int>						mStartTasks;
	AosTaskStatus::E					mStatus;	
	AosXmlTagPtr						mLogicTaskSdoc;
	vector<pair<OmnString, bool> >		mTriggerIds;
	set<OmnString>						mDataColIds;
	
public:
	AosLogicTask(const AosLogicTaskType::E type, const u64 &job_docid);
	~AosLogicTask();

	virtual bool						removeStartTask(
											const AosTaskInfoPtr &info);
	virtual bool						clearTaskInfo(
											const AosTaskInfoPtr &info);
	virtual bool 						insertActiveTask(
											const AosXmlTagPtr &task_doc);
	virtual bool						merge(
											const AosLogicTaskObjPtr &datacol);
	virtual map<u64, AosTaskInfoPtr>& 	getAllTaskInfo();
	virtual set<OmnString>  			getDataColIds();
	virtual AosTaskInfoPtr 				getTaskInfo(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	virtual AosLogicTaskType::E			getType();
	virtual int							getPhyId(){return mPhyId;}
	virtual AosTaskStatus::E			getStatus();
	virtual OmnString					getLogicId() const;
	virtual u64							getJobDocId() const;
	virtual int							getProgress() const;
	virtual AosXmlTagPtr				getTaskDoc(const AosRundataPtr &rdata);
	virtual void 						setStatus(const AosTaskStatus::E &sts);

	virtual void						setTaskInfoStatus(
											const AosTaskInfoPtr &info,
											const AosTaskStatus::E sts);

	virtual bool						isFinished() const;
	virtual bool 						isInited() const;
	virtual bool						config(
											const AosXmlTagPtr &config, 
											const AosRundataPtr &rdata);
	virtual bool						config(
											const AosJobObjPtr &job,
											const AosXmlTagPtr &config, 
											const AosRundataPtr &rdata);
	virtual bool						stopAllTaskProcess(const AosRundataPtr &rdata);
	virtual bool						updateTaskProgress(
											const AosJobObjPtr &job,
											const u64 &task_docid,
											const int percent,
											const AosRundataPtr &rdata);
	virtual bool						findFailedTaskBySvrId(
											const AosJobObjPtr &job,
											const int death_svr_id,
											const AosRundataPtr &rdata);
	virtual bool						checkStartTask(
											const AosJobObjPtr &job,
											const AosRundataPtr &rdata);
	virtual bool						createTask(
											const AosJobObjPtr &obj,
											const AosXmlTagPtr &sdoc, 
											const AosTaskDataObjPtr &task_data,
											const AosRundataPtr &rdata);
	virtual bool						createTask(
											const AosJobObjPtr &obj,
											const AosXmlTagPtr &task_doc,
											const AosJobMgrObj::OprId oprid,
											const AosRundataPtr &rdata);
	virtual bool 						updateTaskStatus(
											const AosJobObjPtr &job,
											const AosTaskRunnerInfoPtr &run_info,
											const AosRundataPtr &rdata);
	virtual bool						taskFinished(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	virtual bool						taskInfoFinished(
											const u64 &task_docid,
											const AosRundataPtr &rdata);
	virtual bool						taskStarted(
											const u64 &task_docid,
											const u64 &timestamp,
											const int task_serverid,
											const AosRundataPtr &rdata);
	virtual bool						taskInfoStarted(
											const u64 &task_docid,
											const u64 &start_time,
											const int task_serverid,
											const AosRundataPtr &rdata);
	virtual void 						addStartNum();
	virtual bool						addTask(
											const AosRundataPtr &rdata,
											const AosXmlTagPtr &task_doc);
	virtual bool						addTaskLocked(
											const AosRundataPtr &rdata,
											const AosXmlTagPtr &task_doc);
	virtual bool 						addOutput(
											const AosXmlTagPtr &output,
											const AosRundataPtr &rdata);
	virtual bool						addTriggedTaskid(
											const OmnString &task_id,
											const AosRundataPtr &rdata);
	virtual bool						getOutputFiles(
											vector<AosXmlTagPtr> &files,
											const AosRundataPtr &rdata);
	virtual bool						callBack(
											const AosXmlTagPtr &output,
											const AosRundataPtr &rdata);
	virtual bool						finishDataCollector(const AosRundataPtr &rdata);
	virtual void						setParentDataColId(const OmnString &name, const int &shuff_id);
	virtual OmnString 					getDataColId();
	virtual int							getShuffleType();

	virtual void						clear();
	virtual bool						reset(const AosRundataPtr &rdata);
	virtual bool						setDataColTags(
											const AosXmlTagPtr &conf,
											const AosRundataPtr &rdata);
	virtual AosXmlTagPtr				getDataColTags() const;
	virtual bool						createMergeFileTask(
											const int level,
											const AosXmlTagPtr &datacol_tag, 
											const int physical_id, 
											list <AosXmlTagPtr> &info_list, 
											u64 &task_docid, 
											const AosRundataPtr &rdata);
	virtual bool						createMergeFinishTask(
											const AosXmlTagPtr &datacol_tag, 
											const int physical_id, 
											const AosXmlTagPtr &info, 
											const AosRundataPtr &rdata);
	virtual bool						 createImportDocTaskPriv(
											const AosXmlTagPtr &conf,
											int phy_id,
											std::list<AosXmlTagPtr> &outputs,
											const OmnString& action_type,
											const AosRundataPtr &rdata);
	virtual bool						 createOutputDatasetDocs(
											const AosRundataPtr& rdata,
											const AosJobObjPtr& job);
	virtual bool						 createMapTasks(
											const AosRundataPtr& rdata,
											const AosJobObjPtr& job);

	virtual bool						 createReduceTasks(
											const AosRundataPtr& rdata,
											const AosJobObjPtr& job);

	virtual bool						 taskFinished(
											const AosJobObjPtr& job,
											const AosTaskRunnerInfoPtr& info,
											const AosRundataPtr& rdata);

	virtual bool						 taskFailed(
											const AosJobObjPtr& job,
											const AosTaskRunnerInfoPtr& run_info,
											const AosRundataPtr& rdata);

	static AosLogicTaskObjPtr			createLogicTask(
											const AosJobObjPtr &job,
											const AosXmlTagPtr &config,
											const AosRundataPtr &rdata);

	virtual vector<OmnString> getTriggerIds() 
	{	
		vector<OmnString> v;
		for (size_t i=0; i<mTriggerIds.size(); i++)
		{
			v.push_back(mTriggerIds[i].first);
		}
		return v;
	}

	virtual bool checkTrigger(const OmnString &triggerid)  
	{
		bool needtrigge = true;
		for (size_t i=0; i<mTriggerIds.size(); i++)
		{
			if (mTriggerIds[i].second)
			{
				continue;
			}
			else
			{
				if (mTriggerIds[i].first == triggerid)
				{
					mTriggerIds[i].second = true;
				}
				else 
				{
					needtrigge = false;
				}
			}
		}
		return needtrigge;
	}

	virtual bool cleanDataCol(
					const u64 &task_docid,
					const AosRundataPtr &rdata);
	virtual bool 	eraseActiveTask(const AosXmlTagPtr &task_doc);
	virtual bool	reStartLogicTask(
						const AosJobObjPtr &job, 
						const AosRundataPtr &rdata);
	virtual bool 	reStartDataCol(
						const AosJobObjPtr &job,
						const AosRundataPtr &rdata);
	virtual AosCompareFunPtr getComp() { return 0;}
protected:
	bool				addLog(
							const OmnString &log_str,
							const AosRundataPtr &rdata);
	AosTaskInfoPtr 		getTaskInfoLocked(
							const u64 &task_docid,
							const AosRundataPtr &rdata);
	bool				updateLogicTaskProgress(const AosRundataPtr &rdata);
	void				print();
	AosTaskObjPtr 		createTaskPriv(
							const AosJobObjPtr &job,
							const AosXmlTagPtr &sdoc,
							const AosRundataPtr &rdata);
	AosXmlTagPtr		createTaskDoc(
							const AosJobObjPtr &job,
							const int physical_id,
							const OmnString &task_id,
							const u32 max_num_thrd,
							const int shuffle_id,
							const OmnString &input_dataset_str,
							const OmnString &output_dataset_str,
							const AosXmlTagPtr &action_tag,
							const AosRundataPtr &rdata);
	AosXmlTagPtr		createTaskDoc(
							const AosJobObjPtr &job,
							const int physical_id,
							const u32 max_num_thrd,
							const OmnString &sdoc,
							const AosRundataPtr &rdata);
	bool 				findInputFrom(
							const u64 &to_task_docid,
							list<AosXmlTagPtr> &output_list,
							const AosRundataPtr &rdata);
	bool				addMemberTaskPriv(
							const AosJobObjPtr &job,
							const AosTaskObjPtr &task,
							const AosRundataPtr &rdata);
	bool				addMemberTaskPriv(
							const AosJobObjPtr &job,
							const AosTaskObjPtr &task,
							AosXmlTagPtr &taskdoc,
							AosTaskInfoPtr &info,
							const AosRundataPtr &rdata);
	bool		 		updateJobSnapShotPriv(
							const u64 &task_docid,
							const AosXmlTagPtr &snapshots,
							const AosRundataPtr &rdata);
	bool				sanityCheck(
							const u64 &task_docid, 
							const int task_serverid,
							const AosRundataPtr &rdata);
	bool 				recoverLogicTaskMemberVar(
							const AosJobObjPtr &job,
							const AosXmlTagPtr &task_doc,
							const AosTaskInfoPtr &info,
							const AosJobMgrObj::OprId oprid,
							const AosRundataPtr &rdata);
	bool				matchOutPut(
							map<u64, AosXmlTagPtr> &from_task_docs, 
							const u64 &to_task_docid,
							const AosXmlTagPtr &input,
							const AosRundataPtr &rdata);

	//for log
	virtual void		showLogicTaskStart(const OmnString &task_id);
	virtual void 		showCreateLogicTask(const OmnString &task_id);
	virtual void 		showLogicTaskTotalFinish();
	virtual void 		showLogicTaskFinish(
							const u64 task_docid,
							const AosRundataPtr &rdata);

};

#endif

