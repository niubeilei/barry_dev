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
#if 0
#ifndef AOS_TaskUtil_LogicTaskVer1_h
#define AOS_TaskUtil_LogicTaskVer1_h

#include "Rundata/Ptrs.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/JobObj.h"
#include "TaskUtil/DataColCtlr.h"
#include "TaskUtil/Ptrs.h"
#include "TaskUtil/TaskInfo.h"
#include "TaskUtil/TaskStatus.h"
#include "TaskUtil/TaskStartType.h"
#include "UtilData/ModuleId.h"
#include "TaskUtil/TaskDriver.h"
#include "Thread/Mutex.h"
#include "TaskUtil/ReduceTaskCreator.h"
#include "TaskUtil/LogicTask.h"

#include <set>
#include <map>

using namespace std;

class AosLogicTaskVer1 : public AosLogicTask
{
public:
	struct ColInfo
	{
		int 				mFinishNum;
		int 				mOutputNum;
		list<AosXmlTagPtr>	mInfoList;

		bool				mIsMergeFile;
		int					mMergeTaskNum;
		int					mLevel;

		ColInfo()
		:
		mFinishNum(0),
		mOutputNum(0),
		mIsMergeFile(false),
		mMergeTaskNum(0),
		mLevel(0)
		{
		}

		~ColInfo(){}
	};


	enum
	{
		eMaxFilePreReduceTask = 100,
		eMaxFileLenPreReduceTask = 50000000000LL,
		eMaxTaskFailNum = 20
	};

private:
	int64_t								mRunNum; //how many runs in a service
	int64_t								mFinishedMapTaskNum;
	int									mCalcuationSize;
	AosXmlTagPtr						mDataColTags;
	AosXmlTagPtr						mReduceTaskTag;
	map<OmnString, AosDataColCtlrPtr>	mDataColCtlr;
	map<OmnString, AosReduceTaskCreator::ReduceConf>			mReduceTaskTypeMap;
	map<OmnString, ColInfo>				mInfoMap;
public:
	AosLogicTaskVer1(const u64 &job_docid);
	~AosLogicTaskVer1();
	virtual bool		config(
							const AosXmlTagPtr &conf,
							const AosRundataPtr &rdata);
	virtual void		clear();
	virtual bool 		reset(const AosRundataPtr &rdata);
	virtual bool		setDataColTags(
							const AosXmlTagPtr &xml,
							const AosRundataPtr &rdata);
	virtual bool		createMergeFileTask(
							const AosXmlTagPtr &datacol_tag,		
							const int physical_id,
							list <AosXmlTagPtr> &info_list,
							u64 &task_docid,
							const AosRundataPtr &rdata);
	virtual bool		createMergeFinishTask(
							const AosXmlTagPtr &datacol_tag,		
							const int physical_id,
							const AosXmlTagPtr &info,
							const AosRundataPtr &rdata);
	virtual bool		createImportDocTaskPriv(
							const AosXmlTagPtr &dataproc_tag,
							const int physical_id,
							list<AosXmlTagPtr> &doc_list,
							const OmnString &action_type,
							const AosRundataPtr &rdata);
	virtual bool		taskFinished(
							const u64 &task_docid,
							const AosRundataPtr &rdata);
	virtual bool 		createOutputDatasetDocs(
							const AosRundataPtr &rdata,
							const AosJobObjPtr &job);
	virtual bool		createMapTasks(
							const AosRundataPtr &rdata,
							const AosJobObjPtr &job);
	virtual bool		createReduceTasks(
							const AosRundataPtr &rdata,
							const AosJobObjPtr &job);
	virtual AosXmlTagPtr	getDataColTags() const {return mDataColTags;}
	virtual bool		taskFinished(
							const AosJobObjPtr &job,
							const AosTaskRunnerInfoPtr &run_info,
							const AosRundataPtr &rdata);
	virtual bool		taskInfoFinished(
							const u64 &task_docid,
							const AosRundataPtr &rdata);
	static bool checkConfigStatic(
							const AosRundataPtr &rdata,
							const AosXmlTagPtr &logic_task_conf);
private:
	bool 	configDataCols(const AosRundataPtr &rdata);

	bool 	initDataColCtlr(
				const AosRundataPtr &rdata,	
				const OmnString data_colid,
				const AosXmlTagPtr &data_proc_conf,
				AosXmlTagPtr &data_col_conf);
	
	bool 	getDataColsBySvrShuffle(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &orig_dataproc_conf,
				const OmnString &orig_datacol_id);
	
	bool 	getDataColsByCubeShuffle(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &orig_dataproc_conf,
				const OmnString &orig_datacol_id);
	
	bool 	getDataColsByCubeAsm(
				const AosRundataPtr &rdata,
				const AosXmlTagPtr &orig_dataproc_conf,
				const OmnString &orig_datacol_id);
	
	bool	setReduceMap(const AosRundataPtr &rdata);

	bool		updateTaskOutPutPriv(
					const u64 &task_docid,
					const AosXmlTagPtr &outputs,
					const AosRundataPtr &rdata);

	bool		addOutPut(
					const AosXmlTagPtr &xml, 
					const AosRundataPtr &rdata);

	bool		logTask(
					const OmnString &entry_key,
					const AosTaskInfoPtr &info,
					const AosJobObjPtr &job,
					const AosRundataPtr &rdata);

	bool		checkReduceTaskData(
					const u64 &task_docid,
					const AosRundataPtr &rdata);

	bool 		checkMapTaskData(
					const u64 &task_docid,
					const AosRundataPtr &rdata);

	bool		cleanParentTask(
					const AosJobObjPtr &job,
					const set<u64> &parent_task_docids,
					const AosRundataPtr &rdata);

	AosTaskObjPtr createTaskPriv(
					const AosJobObjPtr &job,
					const AosXmlTagPtr &sdoc,
					const AosRundataPtr &rdata);
	bool	createMapTask(
			const AosRundataPtr &rdata,
			const AosJobObjPtr &job,
			const AosXmlTagPtr &logic_task_conf);

	OmnString getDataColConfStr(const AosXmlTagPtr data_col_conf);
	
	bool findInputFrom(
			const u64 &to_task_docid,
			const AosXmlTagPtr &info,
			const AosRundataPtr &rdata);
	
	bool	createGroupDataCol(
			map<OmnString, AosDataColCtlrPtr> &data_cols,
			const OmnString &grp_type,
			const vector<AosXmlTagPtr> &groupCols,
			const AosXmlTagPtr &pxml,
			const AosRundataPtr &rdata);
	
	bool	readyToCreate(
			const AosXmlTagPtr &datacol_tag,
			const AosXmlTagPtr &info,
			list<AosXmlTagPtr> &info_list);

	bool recoverLogicTaskMemberVar(
			const AosJobObjPtr &job,
			const AosXmlTagPtr &task_doc,
			const AosTaskInfoPtr &info,
			const AosJobMgrObj::OprId oprid,
			const AosRundataPtr &rdata);

	bool initSnapShots(
			const AosJobObjPtr &job,
			const AosXmlTagPtr &task_doc,
			const AosTaskInfoPtr &info,
			const AosRundataPtr &rdata); 

	bool tryRollBackSnapShot(
			const AosJobObjPtr &job,
			const AosXmlTagPtr &task_doc,
			const AosRundataPtr &rdata);

	bool procParentDataCol(
			const AosXmlTagPtr &datacol_tag,
			const OmnString &group_type,
			const AosXmlTagPtr &info,
			list<AosXmlTagPtr> &info_list);
	
	bool procChildDataCol(
			const AosXmlTagPtr &datacol_tag,
			const OmnString &group_type,
			const AosXmlTagPtr &info,
			list<AosXmlTagPtr> &info_list);
	bool dataColFinished(
			const u64 &task_docid,
			const AosRundataPtr &rdata);

};

#endif
#endif
