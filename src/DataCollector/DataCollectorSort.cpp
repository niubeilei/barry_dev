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
#include "DataCollector/DataCollectorSort.h"

#include "DataAssembler/DataAssembler.h"
#include "DataCollector/DataCollectorMgr.h"
#include "Dataset/DatasetSplit.h"

#include "Job/Job.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"

#include "SEUtil/IILName.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/JobMgrObj.h"

#include "Thread/ThreadPool.h"
#include "Thread/Sem.h"


AosDataCollectorSort::AosDataCollectorSort(const u64& job_id)
:
AosDataCollector(job_id),
//mLock(OmnNew OmnMutex()),
mIsNeedFinish(false),
mIsMergeAll(false)
{
}


AosDataCollectorSort::~AosDataCollectorSort()
{
}


bool
AosDataCollectorSort::config(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	//not implement
	//bool rslt = AosDataAssembler::checkConfigStatic(conf, rdata.getPtrNoLock());
	//aos_assert_r(rslt, false);

	mType = eSort;

	mPhyNum = AosGetNumPhysicals();
	aos_assert_r(mPhyNum >= 0, false);
	mDataColId = conf->getAttrStr(AOSTAG_NAME, "");
	aos_assert_r(mDataColId != "", false);

	mLogicId = mDataColId;

	AosXmlTagPtr iilAsmConfig = conf->getFirstChild("asm");
	aos_assert_r(iilAsmConfig, 0);
	mDataColTag = conf->clone(AosMemoryCheckerArgsBegin);
	mIsInited = true;
	return true;
}


bool
AosDataCollectorSort::addOutput(
		const AosXmlTagPtr &output,
		const AosRundataPtr &rdata)
{
	if (mShuffleId > 0)
	{
OmnScreen << " " << output->toString()
		  << ", " << " shuffle id is " << mShuffleId << endl;
	}
	aos_assert_r(!mIsNeedFinish, false);
	int level = output->getAttrInt("zky_level", 0);
	return addOutputPriv(rdata, output, level);
}


bool
AosDataCollectorSort::addOutputPriv(
		const AosRundataPtr &rdata,
		const AosXmlTagPtr &output,
		const int &level)
{
	aos_assert_r(output, false);
	aos_assert_r(output->getAttrInt("zky_level", -1) == level, false);

	bool rslt = false;
	// check data is not null.
	u64 fileid = output->getAttrU64(AOSTAG_STORAGE_FILEID, 0);
	aos_assert_r(fileid, false);

	AosXmlTagPtr info = output->clone(AosMemoryCheckerArgsBegin);
	int physical_id = output->getAttrInt(AOSTAG_PHYSICALID, -1);
	aos_assert_r(physical_id >= 0, false);

	mLock->lock();
	if ((int64_t)mFileList.size() <= level) mFileList.resize(level+1);
	mFileList[level].push_back(info);
	//if ((int64_t)mFilesMgr.size() <= level+1) mFilesMgr.resize(level+1);
	//mFilesMgr[level].add(physical_id);

	if (mFileList[level].size() >= AosLogicTask::eMaxFilePreReduceTask)
	{
		rslt = mergeFiles(level, rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}


bool
AosDataCollectorSort::mergeFiles(
		const int &level,
		const AosRundataPtr &rdata)
{
	aos_assert_r(level >= 0 && level < (int64_t)mFileList.size(), false);
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	aos_assert_r(job, false);

	//bool rslt = false;
	u64 task_docid = 0;
	int size = mFileList[level].size();
	AosXmlTagPtr info;
	list<AosXmlTagPtr> info_list;

	if (mIsMergeAll)
	{
		if (size == 1)
		{
			info = mFileList[level].front();
			mFileList[level].pop_front();

			int next_level = getNextNotEmptyLevel(level);
			if (next_level < 0 && checkActiveTaskIsEmpty())
			{
				mFinished = true;

				aos_assert_r(checkActiveTaskIsEmpty() && mIsMergeAll, false);
				info->setAttr(AOSTAG_SHUFFLE_ID, mShuffleId);
				mLastOutputFiles.push_back(info);
				return dataCollectorFinished(rdata);
			}
			else if (!checkActiveTaskIsEmpty() && next_level < 0)
			{
				next_level = getNextActiveTaskLevel(level);
				next_level = next_level + 1;
				if ((int64_t)mFileList.size() <= next_level) mFileList.resize(next_level+1);

				info->setAttr("zky_level", next_level);
				mFileList[next_level].push_back(info);
				return true;
			}
			//mFilesMgr[level].clear();

			mFileList[next_level].push_back(info);
			info->setAttr("zky_level", next_level);
			return mergeFiles(next_level, rdata);
		}

		if (size == 0)
		{
			int next_level = getNextNotEmptyLevel(level);
			if(next_level < level)
			{
				return true;
			}
			return mergeFiles(next_level, rdata);
		}
		aos_assert_r(size <= AosLogicTask::eMaxFilePreReduceTask, false);
		info_list = mFileList[level];
		mFileList[level].clear();
	}
	else
	{
		aos_assert_r(size >= AosLogicTask::eMaxFilePreReduceTask, false);
		for (int i=0; i<AosLogicTask::eMaxFilePreReduceTask; i++)
		{
			info_list.push_back(mFileList[level].front());
			mFileList[level].pop_front();
		}
	}

	AosXmlTagPtr tag = mDataColTag->clone(AosMemoryCheckerArgsBegin);
	aos_assert_r(tag, false);

	// 1.create task doc
	// 2.add task doc to mTaskInfo
OmnScreen << "Create Merge File Task. level is " << level << endl;
	createMergeFileTask(level, tag, info_list, task_docid, rdata);
	if (!task_docid)
	{
		return true;
	}
	//aos_assert_r(task_docid > 5000, false);

	if ((int64_t)mActiveTaskDocids.size() <= level) mActiveTaskDocids.resize(level+1);
	mActiveTaskDocids[level].insert(task_docid);
	return true;
}


bool
AosDataCollectorSort::callBack(
		const AosXmlTagPtr &output,
		const AosRundataPtr &rdata)
{
	// test
	int level = output->getAttrInt("zky_level", -1);
	aos_assert_r(level >= 0, false);
	level--;
OmnScreen << "Merge File Task call back. level is " << level << endl;

	u64 task_docid = output->getAttrU64("zky_from_task_docid", 0);
	aos_assert_r(task_docid > 0, false);

	mLock->lock();

	//aos_assert_r(mCallBackTaskDocids.find(task_docid) == mCallBackTaskDocids.end(), false);

	if (mCallBackTaskDocids.find(task_docid) != mCallBackTaskDocids.end())
	{
		OmnScreen << "jozhi this task may be restart task" << endl;
	}
	mCallBackTaskDocids.insert(task_docid);

	aos_assert_r(level < (int64_t)mActiveTaskDocids.size(), false);
	set<u64>::iterator itr;
	itr = mActiveTaskDocids[level].find(task_docid);
	aos_assert_r(itr != mActiveTaskDocids[level].end(), false);
	mActiveTaskDocids[level].erase(itr);

	mLock->unlock();

	bool last_merge = output->getAttrBool(AOSTAG_LAST_MERGE, false);
	if (last_merge)
	{
		output->setAttr(AOSTAG_SHUFFLE_ID, mShuffleId);
		AosXmlTagPtr info = output->clone(AosMemoryCheckerArgsBegin);
		mLastMergeFiles.push_back(info);
		if (mIsNeedFinish && checkActiveTask(level))
		{
			AosXmlTagPtr file = AosXmlParser::parse("<file/>" AosMemoryCheckerArgs);
			aos_assert_r(file, false);
			for (u32 i=0; i<mLastMergeFiles.size(); i++)
			{
				AosXmlTagPtr block_tag = mLastMergeFiles[i]->clone(AosMemoryCheckerArgsBegin);
				file->addNode(block_tag);
			}
			file->setAttr(AOSTAG_SHUFFLE_ID, mShuffleId);
			mLastOutputFiles.push_back(file);
			mFinished = true;
			return dataCollectorFinished(rdata);
		}
		return true;
	}

	if (mIsNeedFinish && checkActiveTask(level))
	{
		if (!mIsMergeAll)
		{
			addOutputPriv(rdata, output, level+1);
			mIsMergeAll = true;
			return mergeFiles(0, rdata);
		}
		else if (level + 1 <= (int64_t)mFileList.size())
		{
			addOutputPriv(rdata, output, level+1);
			if (!mFileList[level].empty())
				return mergeFiles(level, rdata);
			return mergeFiles(level+1, rdata);
		}
		else
		{
			aos_assert_r(checkActiveTaskIsEmpty() && mIsMergeAll, false);
			output->setAttr(AOSTAG_SHUFFLE_ID, mShuffleId);
			mLastOutputFiles.push_back(output);
			return dataCollectorFinished(rdata);
		}
	}
	return addOutputPriv(rdata, output, level+1);
}


bool
AosDataCollectorSort::finishDataCollector(const AosRundataPtr &rdata)
{
	aos_assert_r(mStartNum > 0, false);
	mStartNum--;
	if (mStartNum > 0)
	{
		return true;
	}

	mIsNeedFinish = true;
	if (mFileList.empty())
	{
		//OmnAlarm << "============================================ No Output." << enderr;
		OmnScreen << "DataCollector name is " << mDataColId << "====No Output." << endl;
		mFinished = true;
		return dataCollectorFinished(rdata);
	}

	if (1 == mFileList.size() && 1 == mFileList[0].size() && checkActiveTaskIsEmpty())
	{
		AosXmlTagPtr output = mFileList[0].back();
		output->setAttr(AOSTAG_SHUFFLE_ID, mShuffleId);
		mLastOutputFiles.push_back(output);
		mFileList[0].pop_back();
		mFinished = true;
		return dataCollectorFinished(rdata);
	}

	if (checkActiveTaskIsEmpty())
	{
		mIsMergeAll = true;
		return mergeFiles(0, rdata);
	}
	return true;
}


bool
AosDataCollectorSort::dataCollectorFinished(const AosRundataPtr &rdata)
{
	OmnScreen << "DataCollectorSort Finished." <<  mDataColId << endl;

	bool rslt = false;
	if (mParentDataColId != "")
	{
		//for log
		showDataCollectorFinish();
		mLock->lock();
		mFinished = true;
		mLock->unlock();
		AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
		aos_assert_r(job, false);

		AosDataCollectorMgrPtr colMgr = job->getDataColMgr();
		aos_assert_r(colMgr, false);
		return colMgr->callBackLocked(mParentDataColId, NULL, rdata);
	}
	//for log
	showDataCollectorTotalFinish();

	mLock->lock();
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	if (!job)
	{
		OmnScreen << "jozhi job restart, may be not started : " << mJobDocid << endl;
		mLock->unlock();
		return true;
	}
	//aos_assert_r(job, false);
	aos_assert_r(mTaskIds.size() > 0, false);
	rslt = job->datacollectorFinished(rdata, mLogicId, mTaskIds);
	aos_assert_rl(rslt, mLock, false);

	mLock->unlock();
	return true;
}

bool
AosDataCollectorSort::checkLastMerge(const int &level)
{
	return mIsNeedFinish && (getNextNotEmptyLevel(level) < 0) && checkActiveTaskIsEmpty();
}


bool
AosDataCollectorSort::createTaskDocPriv(
		u64 &task_docid,
		list<AosXmlTagPtr> &info_list,
		const int level,
		const AosJobObjPtr &job,
		const AosXmlTagPtr &datacol,
		const AosRundataPtr &rdata,
		const int block_idx,
		const bool is_last_merge)
{
    int shuffle_id = -1;

	if (checkLastMerge(level) && !mTaskIds.empty())
    {
		AosLogicTaskObjPtr logic_task = job->getLogicTaskLocked(mTaskIds[0], rdata);
		shuffle_id = logic_task->getPhyId();
		if (shuffle_id == -1)
		{
			shuffle_id = mShuffleId;
		}
    }
	if (!mTaskIds.empty())
	{
OmnScreen << "@@@@andy ::" <<" Task id is " << mTaskIds[0] << ", " << "physical_id is " << shuffle_id << ", Is last megrge " << checkLastMerge(level) <<  endl;
	}
	OmnString docstr;
	docstr << "<sdoc "
		   << AOSTAG_TYPE << "=\"" << AOSTASKNAME_REDUCETASK << "\" "
		   << AOSTAG_PHYSICALID << "=\"" << shuffle_id << "\">"
		   << "<dataproc "
		   << AOSTAG_TYPE << "=\"mergefile\" "
		   << AOSTAG_LAST_MERGE << "=\"" << (is_last_merge?"true":"false") << "\" "
		   << AOSTAG_BLOCK_IDX << "=\"" << block_idx << "\" "
		   << AOSTAG_LEVEL << "=\"" << level << "\">"
		   << datacol->toString()
		   << "<files>";
	list<AosXmlTagPtr>::iterator itr = info_list.begin();
	while(itr != info_list.end())
	{
		docstr << (*itr)->toString();
		itr++;
	}
	docstr << "</files></dataproc></sdoc>";

	AosXmlTagPtr task_doc = createTaskDoc(job, -1, 1, docstr, rdata);
	aos_assert_r(task_doc, false);

	bool rslt = addTaskLocked(rdata, task_doc);
	aos_assert_r(rslt, false);

	task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(task_docid != 0, false);

	rslt = findInputFrom(task_docid, info_list, rdata);
	aos_assert_r(rslt, false);

	AosTaskInfoPtr task_info = getTaskInfoLocked(task_docid, rdata);
	aos_assert_r(task_info, false);

	//for log
	OmnString task_id = task_info->getTaskId();
	showCreateLogicTask(task_id);

	task_info->setStatus(AosTaskStatus::eWait);
	rslt = job->addTaskInfo(task_info, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDataCollectorSort::createMergeFileTask(
		const int level,
		const AosXmlTagPtr &datacol_tag,
		list<AosXmlTagPtr> &info_list,
		u64 &task_docid,
		const AosRundataPtr &rdata)
{
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);
	aos_assert_r(job, false);

	if (checkLastMerge(level))
	{
		AosXmlTagPtr first_output = *(info_list.begin());
		int64_t length = first_output->getAttrInt64(AOSTAG_LENGTH, -1);
		aos_assert_r(length > 0, false);
		u32 num_slots = job->getNumSlots();
		aos_assert_r(num_slots > 0, false);
		u32 total_task_process = num_slots * AosGetNumPhysicals();
		if (length > AosDatasetSplit::eFileLength && total_task_process > 1)
		{
			OmnScreen << "debug do the last merge task========================" << endl;
			AosCompareFunPtr comp = getComp();
			aos_assert_r(comp, false);
			vector<list<AosXmlTagPtr> > block_files;
			u64 start_time = OmnGetSecond();
			bool rslt = AosDatasetSplit::splitMergeFile(block_files, datacol_tag, comp, total_task_process, info_list, rdata);
			aos_assert_r(rslt, false);
			OmnScreen << "do last merge task take time: " << OmnGetSecond() - start_time << endl;
			if ((int64_t)mActiveTaskDocids.size() <= level) mActiveTaskDocids.resize(level+1);
			for (u32 i=0; i<block_files.size(); i++)
			{
				rslt = createTaskDocPriv(task_docid, block_files[i], level, job, datacol_tag, rdata, i, true);
				aos_assert_r(rslt, false);
				mActiveTaskDocids[level].insert(task_docid);
			}
			task_docid = 0;
			return true;
		}
	}

	bool rslt = createTaskDocPriv(task_docid, info_list, level, job, datacol_tag, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDataCollectorSort::checkActiveTaskIsEmpty()
{
	int i = mActiveTaskDocids.size() - 1;
	while (i >= 0)
	{
		if ( !(mActiveTaskDocids[i].empty()))
			return false;
		i--;
	}
	return true;
}


int
AosDataCollectorSort::getNextActiveTaskLevel(const int &level)
{
	int i = level;
	while (i >= 0)
	{
		if ( !(mActiveTaskDocids[i].empty()))
			return i;
		++i;
	}
	return -1;
}


bool
AosDataCollectorSort::checkActiveTask(const int &level)
{
	int i = level;
	while (i >= 0)
	{
		if ( !(mActiveTaskDocids[i].empty()))
			return false;
		i--;
	}
	return true;
}


int
AosDataCollectorSort::getNextNotEmptyLevel(const int &level)
{
	aos_assert_r((int64_t)mFileList.size() > level, false);

	for (u32 i = level + 1; i < mFileList.size(); i++)
	{
		if (!mFileList[i].empty())
		{
			return i;
		}
	}
	return -1;
}


bool
AosDataCollectorSort::insertActiveTask(const AosXmlTagPtr &task_doc)
{
	AosXmlTagPtr sdoc = task_doc->getFirstChild("sdoc");
	aos_assert_r(sdoc, false);
	AosXmlTagPtr dataproc = sdoc->getFirstChild("dataproc");
	aos_assert_r(dataproc, false);
	int level = dataproc->getAttrInt("zky_level", -1);
	aos_assert_r(level != -1, false);
	u64 task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(task_docid, false);
	mLock->lock();
	if ((int64_t)mActiveTaskDocids.size() <= level)
	{
		mActiveTaskDocids.resize(level+1);
	}
	mActiveTaskDocids[level].insert(task_docid);
	mLock->unlock();
	return true;
}

bool
AosDataCollectorSort::eraseActiveTask(const AosXmlTagPtr &task_doc)
{
	AosXmlTagPtr sdoc = task_doc->getFirstChild("sdoc");
	aos_assert_r(sdoc, false);
	AosXmlTagPtr dataproc = sdoc->getFirstChild("dataproc");
	aos_assert_r(dataproc, false);
	int level = dataproc->getAttrInt("zky_level", -1);
	aos_assert_r(level != -1, false);
	u64 task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(task_docid, false);
	if (mActiveTaskDocids[level].find(task_docid) != mActiveTaskDocids[level].end())
	{
		mActiveTaskDocids[level].erase(task_docid);
	}
	return true;
}


/*
bool
AosDataCollectorSort::eraseActiveTask(const u64 &task_docid)
{
	for (u32 i=0; i<mActiveTaskDocids.size(); i++)
	{
		if (mActiveTaskDocids[i].find(task_docid) != mActiveTaskDocids[i].end())
		{
			mActiveTaskDocids[i].erase(task_docid);
		}
	}
	return true;
}
*/

bool
AosDataCollectorSort::cleanDataCol(
		const u64 &task_docid,
		const AosRundataPtr &rdata)
{
	u64 docid = 0;
	list<AosXmlTagPtr>::iterator tmp_itr;
	for (u32 i=0; i<mFileList.size(); i++)
	{
		list<AosXmlTagPtr>::iterator f_itr = mFileList[i].begin();
		while(f_itr != mFileList[i].end())
		{
			docid = (*f_itr)->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
			aos_assert_r(docid != 0, false);
			if (docid == task_docid)
			{
				OmnScreen << "=======real to clean OutPut ,level: " << i << " , task_docid: " << task_docid << endl;
				tmp_itr = f_itr;
				f_itr++;
				mFileList[i].erase(tmp_itr);
			}
			else
			{
				f_itr++;
			}
		}
	}
	if (mLastOutputFiles.size() == 0)
	{
		return true;
	}
	vector<AosXmlTagPtr>::iterator last_itr = mLastOutputFiles.begin();
	while(last_itr != mLastOutputFiles.end())
	{
		docid = (*last_itr)->getAttrU64(AOSTAG_FROM_TASK_DOCID, 0);
		aos_assert_r(docid != 0, false);
		if (docid == task_docid)
		{
			OmnScreen << "=======real to clean OutPut in last output, task_docid: " << task_docid << endl;
			mLastOutputFiles.erase(last_itr);
			break;
		}
		last_itr++;
	}
	return true;
}

bool
AosDataCollectorSort::reStartLogicTask(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	mFinished = false;
	mIsNeedFinish = false;
	bool rslt = reStartDataCol(job, rdata);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosDataCollectorSort::reStartDataCol(
		const AosJobObjPtr &job,
		const AosRundataPtr &rdata)
{
	//this datacollector triggered logic task set status is stop
	for (u32 i=0; i<mTaskIds.size(); i++)
	{
		AosLogicTaskObjPtr logic_task = job->getLogicTaskLocked(mTaskIds[i], rdata);
		aos_assert_r(logic_task, false);
		logic_task->setStatus(AosTaskStatus::eStop);
	}
	return true;
}

//for log
void 
AosDataCollectorSort::showDataCollectorFinish()
{
	AosJobLog(mJobDocid) << "Act: finish PDCLT:" << mParentDataColId << " CDCLT:" << mDataColId << endlog;
}

void 
AosDataCollectorSort::showDataCollectorTotalFinish()
{
	AosJobLog(mJobDocid) << "Act:finish DCLT:" << mLogicId << endlog;
}

