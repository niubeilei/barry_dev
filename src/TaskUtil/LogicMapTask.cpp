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
// 12/26/2014 Created by Jozhi
////////////////////////////////////////////////////////////////////////////
#include "TaskUtil/LogicMapTask.h"

#include "API/AosApi.h"
#include "DataCollector/DataCollectorMgr.h"
#include "Dataset/DatasetSplit.h"
#include "Job/Job.h"
#include "SEInterfaces/TaskDataType.h"
#include "TaskUtil/MapTaskCreator.h"
#include "TaskUtil/MapTaskCreator.h"
#include "SEInterfaces/NetFileCltObj.h"
#include "UtilTime/TimeUtil.h"

AosLogicMapTask::AosLogicMapTask(const u64 &job_docid)
:
AosLogicTask(AosLogicTaskType::eMap, job_docid)
{
}


AosLogicMapTask::~AosLogicMapTask()
{
}


bool
AosLogicMapTask::config(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &config,
		const AosRundataPtr &rdata)

{
	aos_assert_r(config, false);
	mLogicId = config->getAttrStr(AOSTAG_NAME);
	mPhyId = config->getAttrInt(AOSTAG_PHYSICALID, -1);

	aos_assert_r(mLogicId != "", false);
	OmnString name = config->getAttrStr("zky_task_name");
	if (name != "")
	{
		mLogicTaskSdoc = AosGetDocByKey(rdata, "", "zky_task_name", name);
	}
	else
	{
		mLogicTaskSdoc = config->clone(AosMemoryCheckerArgsBegin);
	}
	aos_assert_r(mLogicTaskSdoc, false);
	AosXmlTagPtr output_datasets_tag = mLogicTaskSdoc->getFirstChild("output_datasets");
	aos_assert_r(output_datasets_tag, false);
	AosXmlTagPtr dataset_tag = output_datasets_tag->getFirstChild(true);

	AosDataCollectorMgrPtr datacolmgr = job->getDataColMgr();
	aos_assert_r(datacolmgr, false);
	if (dataset_tag)
	{

		aos_assert_r(job, false);
		while(dataset_tag)
		{
			OmnString dataset_type = dataset_tag->getAttrStr(AOSTAG_ZKY_TYPE); 
			if (dataset_type == "datacol")
			{
				OmnString data_col_id = "";
				AosXmlTagPtr data_col_tag = dataset_tag->getFirstChild(true);
				aos_assert_r(data_col_tag, false);
				bool rslt = datacolmgr->createDataCollector(mJobDocid, job, data_col_tag, data_col_id, rdata);
				aos_assert_r(data_col_id != "" && rslt, false);
				mDataColIds.insert(data_col_id);
			}
			dataset_tag = output_datasets_tag->getNextChild();
		}
	}
	AosXmlTagPtr input_datasets = mLogicTaskSdoc->getFirstChild("input_datasets");
	aos_assert_r(input_datasets, false);
	AosXmlTagPtr dataset = input_datasets->getFirstChild("dataset");
	while(dataset)
	{
		OmnString trigger_type = dataset->getAttrStr("trigger_type", "");
		OmnString trigger_id = dataset->getAttrStr("zky_name", "");
		if (trigger_id != "" && trigger_type == "datacol")
		{
			mTriggerIds.push_back(make_pair(trigger_id, false));
		}
		dataset = input_datasets->getNextChild("dataset");
	}
	return true;
}


bool
AosLogicMapTask::createMapTasks(
		const AosRundataPtr &rdata,
		const AosJobObjPtr &job)
{
	AosLogicTaskObjPtr thisptr(this, false);
	aos_assert_r(mLogicTaskSdoc, false);
	return create(job, rdata);
}


bool
AosLogicMapTask::create(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	aos_assert_r(mLogicTaskSdoc, false);
	AosXmlTagPtr action_tag = mLogicTaskSdoc->getFirstChild("data_engine");
	aos_assert_r(action_tag, false);
	int max_num_thrd = mLogicTaskSdoc->getAttrInt(AOSTAG_MAXNUMTHREAD, 1);

	AosXmlTagPtr input_datasets_tag = mLogicTaskSdoc->getFirstChild("input_datasets");
	aos_assert_r(input_datasets_tag, false);

	//Jozhi 2015-04-27 implement dataset splitter
	AosXmlTagPtr dataset_splitter = input_datasets_tag->getFirstChild("dataset_splitter");
	AosXmlTagPtr dataset_splitter_clone;
	if (dataset_splitter)
	{
		dataset_splitter_clone = dataset_splitter->clone(AosMemoryCheckerArgsBegin);
	}

	//Barry 2015/02/14
	OmnString collectorid, type;
	AosLogicTaskObjPtr logic_task;
	AosXmlTagPtr task_doc, dataset_doc, new_dataset_doc;

	AosDataCollectorMgrPtr datacolmgr = job->getDataColMgr();
	aos_assert_r(datacolmgr, false);


	AosXmlTagPtr new_input_datasets_tag = input_datasets_tag->clone(AosMemoryCheckerArgsBegin);
	bool rslt = new_input_datasets_tag->removeAllChildren();
	aos_assert_r(rslt, false);

	AosXmlTagPtr input_dataset = input_datasets_tag->getFirstChild("dataset");
	while(input_dataset)
	{
		type = input_dataset->getAttrStr("zky_type", "");
		if (type == "datacol")
		{
			collectorid = input_dataset->getAttrStr(AOSTAG_NAME, ""); 
			logic_task = datacolmgr->getLogicTaskLocked(collectorid, rdata);
			aos_assert_r(logic_task, false);

			dataset_doc = logic_task->getTaskDoc(rdata);
			aos_assert_r(dataset_doc, false);

			new_dataset_doc = dataset_doc->clone(AosMemoryCheckerArgsBegin);
		}
		else
		{
			new_dataset_doc = input_dataset;
		}
		new_input_datasets_tag->addNode(new_dataset_doc);
		input_dataset = input_datasets_tag->getNextChild("dataset");
	}
	mLogicTaskSdoc->removeNode("input_datasets", false, true);

	rslt = mLogicTaskSdoc->addNode(new_input_datasets_tag);
	aos_assert_r(rslt, false);

	input_datasets_tag = mLogicTaskSdoc->getFirstChild("input_datasets");
	aos_assert_r(input_datasets_tag, false);

	if (dataset_splitter_clone)
	{
		input_datasets_tag->addNode(dataset_splitter_clone);
	}

	vector<AosXmlTagPtr> input_datasets;
	rslt = AosDatasetSplit::staticSplit(rdata, input_datasets_tag, input_datasets, job->getJobDocid());
	aos_assert_r(rslt && !input_datasets.empty(), false);

	AosXmlTagPtr output_datasets_tag = mLogicTaskSdoc->getFirstChild("output_datasets");
	aos_assert_r(output_datasets_tag, false);
	
	OmnString task_id;
	OmnString input_dataset_str;
	OmnString output_dataset_str = output_datasets_tag->toString();
	int physical_id = -1;
	int shuffle_id = -1;
	for (u32 i=0; i<input_datasets.size(); i++)
	{
		rslt = setDataSetConfig(physical_id, input_datasets[i], rdata);
		aos_assert_r(physical_id >= 0, false);
		shuffle_id = (input_datasets[i]->xpathQuery("dataset/datascanner/dataconnector/@zky_shuffle_id", "-1")).toInt();
		input_dataset_str = input_datasets[i]->toString();
		aos_assert_r(input_dataset_str != "", false);
		task_id = mLogicId;
		task_id << "_" << AOSTASKNAME_MAPTASK << "_" << i;
		//for log
		showCreateLogicTask(task_id);
OmnScreen << "@@@@andy ::" <<"Task id is " << task_id << ", " << "physical_id is " << physical_id << endl;
		task_doc = createTaskDoc(job, physical_id, task_id, max_num_thrd, 
				shuffle_id, input_dataset_str, output_dataset_str, action_tag, rdata); 

		AosXmlTagPtr tag = task_doc->getFirstChild("input_datasets");	
		aos_assert_r(tag, false);
		tag = tag->getFirstChild("dataset");
		OmnString trigger_type = tag->getAttrStr("trigger_type");
		if (trigger_type == "datacol")
		{
			tag = tag->getFirstChild("datascanner");
			aos_assert_r(tag, false);
			tag = tag->getFirstChild("dataconnector");
			aos_assert_r(tag, false);
			AosXmlTagPtr files = tag->getFirstChild("files");
			if (files)
			{
				aos_assert_r(files, false);
				list<AosXmlTagPtr> file_list;
				AosXmlTagPtr file = files->getFirstChild();
				while(file)
				{
					file_list.push_back(file->clone(AosMemoryCheckerArgsBegin));
					file = files->getNextChild();
				}
				u64 task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
				rslt = findInputFrom(task_docid, file_list, rdata);
				aos_assert_r(rslt, false);
			}
			else
			{
				OmnScreen << "jozhi this may be an error, you can continue" << endl;
			}
		}
		rslt = addTask(rdata, task_doc); 
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosLogicMapTask::setDataSetConfig(
		int &physical_id, 
		const AosXmlTagPtr &input_dataset_tag,
		const AosRundataPtr &rdata)
{
	aos_assert_r(input_dataset_tag, false);

	AosXmlTagPtr dataset_tag = input_dataset_tag->getFirstChild("dataset");
	aos_assert_r(dataset_tag, false);                                                 

	dataset_tag = AosDatasetObj::getDatasetConfig(rdata, dataset_tag);
	aos_assert_r(dataset_tag, false);

	AosXmlTagPtr data_scanner_tag = dataset_tag->getFirstChild("datascanner");
	if (!data_scanner_tag)
	{
		physical_id = rand() % AosGetNumPhysicals();
		return physical_id;
	}

	AosXmlTagPtr data_conn_tag = data_scanner_tag->getFirstChild("dataconnector");
	aos_assert_r(data_conn_tag, false);

	if (mPhyId < 0)
	{
		physical_id = data_conn_tag->getAttrInt(AOSTAG_PHYSICALID, false); 
		if (physical_id < 0)
			physical_id = rand() % AosGetNumPhysicals();
	}
	else
	{
		physical_id = mPhyId;
	}
	return true;
}

	
bool						
AosLogicMapTask::taskFinished(
			const u64 &task_docid, 
			const AosRundataPtr &rdata)
{
	return taskInfoFinished(task_docid, rdata);
}


bool			
AosLogicMapTask::taskFinished(
			const AosJobObjPtr &job,
			const AosTaskRunnerInfoPtr &run_info,
			const AosRundataPtr &rdata)
{
	bool rslt = false;
	u64 task_docid = run_info->getTaskDocid();
	if (!job->isValid(task_docid, run_info->getTaskStartSTime(), rdata)) return true; 
	OmnString output_str = run_info->getTaskOutPuts();
	AosXmlTagPtr outputs, output;
	if (output_str != "")
	{
		outputs = AosXmlParser::parse(output_str AosMemoryCheckerArgs);
		if (outputs)
		{
			rslt = updateTaskOutPutPriv(task_docid, outputs, rdata);
			aos_assert_r(rslt, false);
		}
	}
	OmnString snapshots_str= run_info->getTaskSnapShots();
	AosXmlTagPtr snapshots_tag;
	if (snapshots_str != "")
	{
		snapshots_tag = AosXmlParser::parse(snapshots_str AosMemoryCheckerArgs);
		if (snapshots_tag)
		{
			rslt = updateJobSnapShotPriv(task_docid, snapshots_tag, rdata);
			aos_assert_r(rslt, false);
		}
	}
	rslt = taskInfoFinished(task_docid, rdata);
	aos_assert_r(rslt, false);

	//for log
	showLogicTaskFinish(task_docid, rdata);

	rslt = job->updateJobProgress(rdata);
	aos_assert_r(rslt, false);

	rslt = job->scheduleNextTask(run_info, rdata);
	aos_assert_r(rslt, false);
	//finished bug to fix
	
	if (isFinished())
	{
		//each logic task only call this one time
OmnScreen << "logic_task finished, logic_task_id: " << mLogicId << endl;
		AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
		std::set<OmnString>::iterator  itr;
		for (itr = mDataColIds.begin(); itr != mDataColIds.end(); itr++)
		{
OmnScreen << "logic_task finished, call data col finish col_id: " << *itr << endl;
			bool rslt = colMgr->finishDataCollector(*itr, rdata);
			aos_assert_r(rslt, false);
		}
		//for log
		showLogicTaskTotalFinish();
		job->checkJobFinishing(rdata);
	}
	return true;
}


bool
AosLogicMapTask::taskInfoFinished(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	task_doc->setAttr(AOSTAG_STATUS, AosTaskStatus::toStr(AosTaskStatus::eFinish));
	task_doc->setAttr(AOSTAG_END_TIME, OmnGetTime(AosLocale::eChina));
	task_doc->setAttr(AOSTAG_PROGRESS, "100");                        
	//Jozhi 2015-04-24 calculate take time
	OmnString starttime_str = task_doc->getAttrStr("zky_starttime");
	OmnString finishtime_str = task_doc->getAttrStr("zky_endtime");
	i64 start_time = AosTimeUtil::str2EpochTime(starttime_str);
	i64 finish_time = AosTimeUtil::str2EpochTime(finishtime_str);
	i64 take_time = finish_time - start_time;
	task_doc->setAttr("zky_take_time", take_time);

	bool rslt = AosModifyDoc(task_doc, rdata);
	aos_assert_rl(rslt, mLock, false);
	AosTaskInfoPtr info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_rl(info, mLock, false);
	info->setStatus(AosTaskStatus::eFinish);
	info->setProgress(100);
	mFinishedNum++;
	mStartTasks.erase(task_docid);
	mLock->unlock();
	if (!isFinished())
	{
		return updateLogicTaskProgress(rdata);
	}
	mLock->lock();
	mProgress = 100;
	mStatus = AosTaskStatus::eFinish;
	mLock->unlock();
	return true;
}


bool
AosLogicMapTask::updateTaskOutPutPriv(
		const u64 &task_docid,
		const AosXmlTagPtr &outputs,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_docid != 0, false);
	aos_assert_r(outputs, false);
	OmnTagFuncInfo << "task_docid is: " << task_docid << endl;
	mLock->lock();
	AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
	aos_assert_rl(task_doc, mLock, false);
	AosXmlTagPtr tmp_tag = outputs;
	task_doc->addNode(tmp_tag);
	rdata->setOk();
	AosModifyDoc(task_doc, rdata);
	AosXmlTagPtr xml = task_doc->getFirstChild("output_data__n");
	aos_assert_rl(xml, mLock, false);
	//save storage file size
	AosXmlTagPtr entry = xml->getFirstChild(true);
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	aos_assert_r(job, false);
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
	aos_assert_rl(colMgr, mLock, false);
	while(entry)
	{
		AosTaskDataType::E type = AosTaskDataType::toEnum(entry->getAttrStr(AOSTAG_TYPE));
		if (type == AosTaskDataType::eOutPutDoc || type == AosTaskDataType::eOutPutIIL)
		{
			u64 file_id = entry->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			aos_assert_rl(file_id != 0, mLock, false);
			int physical_id = entry->getAttrInt(AOSTAG_PHYSICALID, -1);
			aos_assert_rl(physical_id != -1, mLock, false);
			int64_t file_len = 0;
			AosDiskStat disk_stat;
			bool rslt = AosNetFileCltObj::getFileLengthStatic(
				file_id, physical_id, file_len, disk_stat, rdata.getPtr());

			if (!rslt)
			{
				mLock->unlock();
				return false;
			}
			if (file_len == 0)
			{
				OmnAlarm << "file is empty, may be an error" << enderr;
			}
			entry->setAttr(AOSTAG_LENGTH, file_len);
			OmnString datacol_id = entry->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
			rslt = colMgr->addOutput(datacol_id, entry, rdata);
			aos_assert_rl(rslt, mLock, false);
			entry = outputs->getNextChild();
		}
		entry = xml->getNextChild();
	}
	mLock->unlock();
	return true;
}

bool
AosLogicMapTask::reStartLogicTask(
		const AosJobObjPtr &job, 
		const AosRundataPtr &rdata)
{
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
	aos_assert_rl(colMgr, mLock, false);
	set<OmnString>::iterator itr = mDataColIds.begin();
	while(itr != mDataColIds.end())
	{
		bool rslt = colMgr->reStartDataCol(job, *itr, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}

