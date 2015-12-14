////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// 1. This class maintaines the fixed numbers of buckets to cache iiltrans 
// that is a sorted  array of AosIILEntry. 
// 2. Every bucket has boundary of low and hight, the entries will append
// to the matched bucket by the range. 
// 3. The bucket can automaticly ajust range.  The mechanism of mergering 
// and spliting bucket by the aging and total entries  of the bucket. 
// If the bucket is too old. it will be merged to it's left bucket or the
// right bucket, and the bucket will free. If the bucket has full, 
// the entries will be proccesed, and the bucket will free. when the entrie 
// has not in all the buckets range .it will get the  free buckets 
// to split the range.
// 
// Modification History:
// 12/29/2014 Created by Andy zhang 
////////////////////////////////////////////////////////////////////////////
#include "DataCollector/DataCollector.h"

#include "DataCollector/DataCollectorSort.h"
#include "DataCollector/DataCollectorGroup.h"
#include "DataCollector/DataCollectorNorm.h"
#include "DataCollector/DataCollectorBuff.h"
#include "Job/Job.h"
#include "JQLStatement/JqlStmtSchema.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/JobMgrObj.h"
#include "TaskUtil/TaskRunnerInfo.h"
#include "Thread/ThreadPool.h"
#include "Thread/Sem.h"
#include "UtilTime/TimeUtil.h"


AosDataCollector::AosDataCollector(const u64 &job_docid)
:
AosLogicTask(AosLogicTaskType::eReduce, job_docid),
mType(AosDataCollector::eInvalid),
mStartNum(1),
mPhyNum(0),
mGroupNum(1),
mFinished(false),
//mShuffleId(0),
mShuffleId(-1),
mIsInited(false)
{
}


AosDataCollector::~AosDataCollector()
{
}


bool
AosDataCollector::config(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet
	return false;
}


bool 
AosDataCollector::addOutput(
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet
	return false;
}


bool
AosDataCollector::getOutputFiles(
		vector<AosXmlTagPtr> &files,
		const AosRundataPtr &rdata)
{
	files.insert(files.end(), mLastOutputFiles.begin(), mLastOutputFiles.end());
	return true;
}


bool 
AosDataCollector::addTriggedTaskid(
		const OmnString &task_id, 
		const AosRundataPtr &rdata)
{
	OmnScreen << "datacollector id : " << mDataColId << " TriggedTaskid is " << task_id << endl;  
	mTaskIds.push_back(task_id);
	return true;
}

bool
AosDataCollector::callBack(
		const AosXmlTagPtr &output,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet
	return false;
}

bool 
AosDataCollector::finishDataCollector(const AosRundataPtr &rdata)
{
	OmnNotImplementedYet
	return false;
}


bool 
AosDataCollector::isFinished() const
{
	return mFinished;
}


bool 
AosDataCollector::isInited() const 
{
	return mIsInited;
}


AosLogicTaskObjPtr
AosDataCollector::createDataCol(
		const u64 &job_docid,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	AosLogicTaskObjPtr task;
	OmnString type = conf->getAttrStr("zky_type", "");
	OmnString shuffle_type = conf->getAttrStr(AOSTAG_SHUFFLE_TYPE, "");
	if (shuffle_type != "" || type == "group")
	{
		task = OmnNew AosDataCollectorGroup(job_docid);
	}
	else if (type == "iil" || type == "sorted_file")
	{
		task = OmnNew AosDataCollectorSort(job_docid);
	}
	else if (type == "file")
	{
		task = OmnNew AosDataCollectorNorm(job_docid);
	}
	else if (type == "buff")
	{
		task = OmnNew AosDataCollectorBuff(job_docid);
	}
	aos_assert_r(task, NULL);
	return task;
}


AosXmlTagPtr	
AosDataCollector::getTaskDoc(const AosRundataPtr &rdata)
{
	if (mDataColTag)
	{
		AosXmlTagPtr tag = mDataColTag->clone(AosMemoryCheckerArgsBegin);
		aos_assert_r(tag, NULL);

		OmnString name = tag->getAttrStr(AOSTAG_NAME, "");
		OmnString schema = tag->getAttrStr("schema");
		OmnString spliter;
		/*
		if (mType == eGroup)
		else
			spliter = "dataspliter_datacolbysize_jimodoc_v0";
			*/

		spliter = "dataspliter_datacolbygroup_jimodoc_v0";
		tag = tag->getFirstChild("asm");
		aos_assert_r(tag, NULL);

		tag = tag->getFirstChild("datarecord");
		aos_assert_r(tag, NULL);

		tag = tag->getFirstChild("datarecord");

		OmnString str;
		if (schema == "")
		{
			str <<"<dataset jimo_objid=\"dataset_bydatascanner_jimodoc_v0\" trigger_type=\"datacol\" zky_name=\"" << name <<"\">"
				<< "<datascanner jimo_objid=\"datascanner_parallel_jimodoc_v0\" zky_name=\"" << name << "\">"
				<< "<dataconnector jimo_objid=\"dataconnector_datacol_jimodoc_v0\" zky_datacolid=\"" << name << "\">"
				<< "<split group_num=\"" << mGroupNum << "\" jimo_objid=\"" << spliter << "\" />"
				<< "</dataconnector>"
				<< "</datascanner>";
			AosDataRecordType::E type = AosDataRecordType::toEnum(mDataColTag->getAttrStr("record_type", AOSRECORDTYPE_FIXBIN));
			if (type == AosDataRecordType::eBuff)
			{
				str << "<dataschema jimo_objid=\"dataschema_recordbylen_jimodoc_v0\" zky_dataschema_type=\"static\" zky_name=\"" << name << "\">";
			}
			else if (type == AosDataRecordType::eFixedBinary)
			{
				str << "<dataschema jimo_objid=\"dataschema_record_jimodoc_v0\" zky_dataschema_type=\"static\" zky_name=\"" << name << "\">";
			}
			str  << tag->toString() << "</dataschema>";
		}
		else
		{
			str << "<dataset jimo_objid=\"dataset_byreaddoc_jimodoc_v0\" trigger_type=\"datacol\" zky_name=\"" << name <<"\">"
			 	<< "<datascanner jimo_objid=\"datascanner_parallel_jimodoc_v0\" zky_name=\"" << name << "\">"
				<< "<dataconnector jimo_objid=\"dataconnector_datacol_jimodoc_v0\" zky_datacolid=\"" << name << "\">"
				<< "<split group_num=\"" << mGroupNum << "\" jimo_objid=\"" << spliter << "\" />"
				<< "</dataconnector>"
				<< "</datascanner>";

			aos_assert_r(schema != "", 0);
			OmnString objid = AosJqlStatement::getObjid(
					rdata, JQLTypes::eSchemaDoc, schema);
			AosXmlTagPtr schema_doc = AosJqlStatement::getDocByObjid(rdata, objid);                                     
			aos_assert_r(schema_doc, 0);
			AosXmlTagPtr datarecord = schema_doc->getFirstChild("datarecord");
			aos_assert_r(datarecord, 0);
			AosXmlTagPtr clone_rcd = datarecord->clone(AosMemoryCheckerArgsBegin);
			aos_assert_r(clone_rcd, 0);
			clone_rcd->removeMetadata();
			clone_rcd->setAttr("zky_name", name);
			str << "<dataset jimo_objid=\"dataset_bydatascanner_jimodoc_v0\" zky_name=\"" << name << "\">"
				<< "<datascanner jimo_objid=\"datascanner_cube_jimodoc_v0\" zky_name=\"" << name << "\">"
			    << "<dataconnector doc_type=\"common_doc\" jimo_objid=\"dataconnector_readdoc_bysort_jimodoc_v0\"/>"
				<< "</datascanner>"
				<< "<dataschema jimo_objid=\"dataschema_record_bylenid_jimodoc_v0\" zky_dataschema_type=\"static\" zky_name=\"" << name << "\">"
				<< clone_rcd->toString()
				<< "</dataschema>"
				<< "</dataset>";
		}
		str << "</dataset>";

		AosXmlTagPtr new_tag = AosStr2Xml(rdata.getPtr(), str AosMemoryCheckerArgs);
		return new_tag;
	}
	return NULL;
}


bool			
AosDataCollector::taskFinished(
			const AosJobObjPtr &job,
			const AosTaskRunnerInfoPtr &run_info,
			const AosRundataPtr &rdata)
{
	// job and task
	// 'task' has finished. It checks whether it can kick off the next
	// task, if any.
	
	bool rslt = false;
	u64 file_id = 0 ;
	int physical_id = -1;
	int64_t file_len = -1;
	u64 task_docid = run_info->getTaskDocid();
	if (!job->isValid(task_docid, run_info->getTaskStartSTime(), rdata)) return true; 
	OmnString output_str = run_info->getTaskOutPuts();
	AosXmlTagPtr outputs, output;
	if (output_str != "")
	{
		outputs = AosXmlParser::parse(output_str AosMemoryCheckerArgs);

		// save output to task doc
		mLock->lock();
		AosXmlTagPtr task_doc = AosGetDocByDocid(task_docid, rdata);
		aos_assert_rl(task_doc, mLock, false);
		AosXmlTagPtr tmp_tag = outputs;
		task_doc->addNode(tmp_tag);
		rdata->setOk();
		AosModifyDoc(task_doc, rdata);
		mLock->unlock();

		AosXmlTagPtr xml = task_doc->getFirstChild("output_data__n");
		aos_assert_rl(xml, mLock, false);
		output = xml->getFirstChild(true);
		aos_assert_r(output, false);
		while (output)
		{
			// get outfile length
			file_id = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
			aos_assert_rl(file_id != 0, mLock, false);
			physical_id = output->getAttrInt(AOSTAG_PHYSICALID, -1);
			aos_assert_rl(physical_id != -1, mLock, false);
			AosDiskStat disk_stat;
			rslt = AosNetFileCltObj::getFileLengthStatic(
				file_id, physical_id, file_len, disk_stat, rdata.getPtr());
			if (!rslt)
			{
				mLock->unlock();
				return false;
			}
			if (file_len == 0)
			{
				OmnAlarm << "this is empty, may be an error" << enderr;
			}
			output->setAttr(AOSTAG_LENGTH, file_len);
			
			// datacollector callBack
			rslt = callBack(output, rdata);
			aos_assert_r(rslt, false);
			output = xml->getNextChild();
		}
	}

	// update task status
	rslt = taskInfoFinished(task_docid, rdata);
	aos_assert_r(rslt, false);

	//for log
	showLogicTaskFinish(task_docid, rdata);

	rslt = updateLogicTaskProgress(rdata);
	aos_assert_r(rslt, false);

	rslt = job->updateJobProgress(rdata);
	aos_assert_r(rslt, false);

	rslt = job->scheduleNextTask(run_info, rdata);
	aos_assert_r(rslt, false);
	if (isFinished())
	{
		showLogicTaskTotalFinish();
		job->checkJobFinishing(rdata);
	}
	return true;
}


bool
AosDataCollector::taskInfoFinished(
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
AosDataCollector::cleanOutPut(
		const int level,
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	OmnScreen << "=======ready to clean OutPut ,level: " << level << " , task_docid: " << task_docid << endl;
	list<AosXmlTagPtr>::iterator itr;
	list<AosXmlTagPtr>::iterator tmp_itr;
	u64 docid = 0;
	if ((u32)level >= mFileList.size())
	{
		return true;
	}
	list<AosXmlTagPtr> info_list = mFileList[level];
	itr = info_list.begin();
	while(itr != info_list.end())
	{
		docid = (*itr)->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
		aos_assert_r(docid != 0, false);
		if (docid == task_docid)
		{
			OmnScreen << "=======real to clean OutPut ,level: " << level << " , task_docid: " << task_docid << endl;
			tmp_itr = itr;
			itr++;
			info_list.erase(tmp_itr);
		}
		else
		{
			itr++;
		}
	}
	return true;
}


bool						
AosDataCollector::merge(const AosLogicTaskObjPtr &logic_task)
{
	AosDataCollector* from = dynamic_cast<AosDataCollector*>(logic_task.getPtr());
	aos_assert_r(from, false);
	mTaskIds = from->mTaskIds;
	mTaskInfo = from->mTaskInfo;
	mStartTasks = from->mStartTasks;
	mStartedNum = from->mStartedNum;
	mFinishedNum = from->mFinishedNum;
	mTriggerIds = from->mTriggerIds;
	mStartNum = from->mStartNum;
	return true;
}


AosCompareFunPtr 
AosDataCollector::getComp()
{
	AosXmlTagPtr asm_tag = mDataColTag->getFirstChild("asm");
	aos_assert_r(asm_tag, 0);
	AosXmlTagPtr cmp_tag = asm_tag->getFirstChild("CompareFun");
	aos_assert_r(cmp_tag, 0);
	AosCompareFunPtr comp = AosCompareFun::getCompareFunc(cmp_tag);
	aos_assert_r(comp, 0);
	return comp;
}
