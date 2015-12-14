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
#include "DataCollector/DataCollectorMgr.h"

#include "DataCollector/DataCollector.h"
#include "Job/Job.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/JobMgrObj.h"
#include "SEInterfaces/TaskDataType.h"
#include "TaskUtil/LogicTask.h"
#include "Thread/ThreadPool.h"
#include "Thread/Sem.h"


AosDataCollectorMgr::AosDataCollectorMgr()
:
mLock(OmnNew OmnMutex()),
mFinishedNum(0)
{
}

AosDataCollectorMgr::~AosDataCollectorMgr()
{
}


bool 
AosDataCollectorMgr::createDataCollector(
		const u64 &job_docid,
		const AosJobObjPtr &job,
		const AosXmlTagPtr &config, 
		OmnString &data_col_name,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	OmnString type = config->getAttrStr("zky_type", "");
	data_col_name =  config->getAttrStr(AOSTAG_NAME, ""); 
	mItr_t itr = mCollectors.find(data_col_name);
	if (itr != mCollectors.end())
	{
		if (mCollectors[data_col_name]->isInited())
		{
			mLock->unlock();
			return true;
		}
		if (type == "")
		{
			mCollectors[data_col_name]->addStartNum();
			mLock->unlock();
			return true;
		}
	}
	AosLogicTaskObjPtr datacol;
	if (type != "")
	{
		datacol = AosDataCollector::createDataCol(job_docid, config, rdata);
		aos_assert_r(datacol, false);
		//for log
		showDataCollectorStart(job_docid, data_col_name);

		bool rslt = datacol->config(job, config, rdata);
		aos_assert_r(rslt, false);

		AosLogicTaskObjPtr old_datacol = mCollectors[data_col_name];
		if (old_datacol)
		{
			datacol->merge(old_datacol);
		}
	}
	else
	{
		AosDataCollectorPtr datacoll = OmnNew AosDataCollector(job_docid);
		datacoll->setDataColId(data_col_name);
		datacol = datacoll;
	}
	aos_assert_r(datacol, false);
	mCollectors[data_col_name] = datacol;
	mLock->unlock();
	return true;
}

bool 
AosDataCollectorMgr::createDataCollectorLocked(
		const u64 &job_docid,
		const AosJobObjPtr &job,
		const AosXmlTagPtr &config, 
		OmnString &data_col_name,
		const AosRundataPtr &rdata)
{
	data_col_name =  config->getAttrStr(AOSTAG_NAME, ""); 
	mItr_t itr = mCollectors.find(data_col_name);
	if (itr != mCollectors.end())
	{
		if (mCollectors[data_col_name]->isInited())
		{
			return true;
		}
	}
	AosLogicTaskObjPtr datacol;
	datacol = AosDataCollector::createDataCol(job_docid, config, rdata);
	aos_assert_r(datacol, false);
	bool rslt = datacol->config(job, config, rdata);
	aos_assert_r(rslt, false);
	aos_assert_r(datacol, false);
	mCollectors[data_col_name] = datacol;
	return true;
}


bool 
AosDataCollectorMgr::addOutput(
		const OmnString &collectorid, 
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	mLock->unlock();
	itr->second->addOutput(output, rdata);
	return true;
}


bool 
AosDataCollectorMgr::callBack(
		const OmnString &collectorid, 
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	mLock->unlock();
	itr->second->callBack(output, rdata);
	return true;
}


bool 
AosDataCollectorMgr::callBackLocked(
		const OmnString &collectorid, 
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	itr->second->callBack(output, rdata);
	return true;
}


bool 
AosDataCollectorMgr::addTriggedTaskid(
		const OmnString &task_id,
		const OmnString &collectorid, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	mLock->unlock();
	itr->second->addTriggedTaskid(task_id, rdata);
	return true;
}

bool 
AosDataCollectorMgr::finishDataCollector(
		const OmnString &collectorid, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	mLock->unlock();
	itr->second->finishDataCollector(rdata);
	//mFinishedNum++;
	return true;
}


AosLogicTaskObjPtr
AosDataCollectorMgr::getLogicTask(
			const OmnString &collectorid, 
			const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, 0);
	AosLogicTaskObjPtr logic_task = itr->second;
	mLock->unlock();
	return logic_task;
}

AosLogicTaskObjPtr
AosDataCollectorMgr::getLogicTaskLocked(
			const OmnString &collectorid, 
			const AosRundataPtr &rdata)
{
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_r(itr != mCollectors.end(), 0);
	AosLogicTaskObjPtr logic_task = itr->second;
	return logic_task;
}


bool
AosDataCollectorMgr::getOutputFiles(
		const OmnString &collectorid,
		vector<AosXmlTagPtr> &files,
		const AosRundataPtr &rdata)
{
OmnScreen << "getDataCollectorOutput:: DataCollector id is " << collectorid << endl;
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	mLock->unlock();
	itr->second->getOutputFiles(files, rdata);
	return true;
}


bool
AosDataCollectorMgr::getOutputFilesLocked(
		const OmnString &collectorid,
		vector<AosXmlTagPtr> &files,
		const AosRundataPtr &rdata)
{
	mItr_t itr = mCollectors.find(collectorid);
	aos_assert_rl(itr != mCollectors.end(), mLock, false);
	itr->second->getOutputFiles(files, rdata);
	return true;
}

AosLogicTaskObjPtr 
AosDataCollectorMgr::getDataCol(
		const OmnString &collectorid, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.find(collectorid);
	if (itr == mCollectors.end())
	{
		mLock->unlock();
		return NULL;
	}
	mLock->unlock();
	return itr->second;
}

AosLogicTaskObjPtr
AosDataCollectorMgr::getDataColLocked(
		const OmnString &collectorid,
		const AosRundataPtr &rdata)
{
	mItr_t itr = mCollectors.find(collectorid);
	if (itr == mCollectors.end())
		return NULL;
	return itr->second;
}


bool
AosDataCollectorMgr::checkAllFinished(const AosRundataPtr &rdata)
{
	mLock->lock();
	mItr_t itr = mCollectors.begin();
	while (itr != mCollectors.end())
	{
		if (itr->second->isFinished())
		{
			itr++;
			continue;
		}
		mLock->unlock();
		return false;
	}
	mLock->unlock();
	return true;
}

bool 
AosDataCollectorMgr::cleanDataCol(
		const set<u64> &pdocids,
		const set<u64> &cdocids,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	AosXmlTagPtr task_doc;
	AosLogicTaskObjPtr logic_task;
	OmnString logic_id = "";
	set<u64>::iterator itr = pdocids.begin();
	while(itr != pdocids.end())
	{
		task_doc = AosGetDocByDocid(*itr, rdata);
		aos_assert_r(task_doc, false);
		rslt = eraseActiveTask(task_doc, rdata);
		aos_assert_r(rslt, false);
		rslt = cleanOutPut(task_doc, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}

	itr = cdocids.begin();
	while(itr != cdocids.end())
	{
		task_doc = AosGetDocByDocid(*itr, rdata);
		aos_assert_r(task_doc, false);
		rslt = eraseActiveTask(task_doc, rdata);
		aos_assert_r(rslt, false);
		rslt = cleanOutPut(task_doc, rdata);
		aos_assert_r(rslt, false);
		itr++;
	}
	return true;
}

bool
AosDataCollectorMgr::eraseActiveTask(
		const AosXmlTagPtr &task_doc, 
		const AosRundataPtr &rdata)
{
	OmnString logic_id = task_doc->getAttrStr(AOSTAG_LOGICID);
	aos_assert_r(logic_id != "", false);
	mLock->lock();
	map<OmnString, AosLogicTaskObjPtr>::iterator itr = mCollectors.find(logic_id);
	if (itr == mCollectors.end())
	{
		mLock->unlock();
		return true;
	}
	AosLogicTaskObjPtr logic_task = itr->second;
	mLock->unlock();
	if (logic_task)
	{
		bool rslt = logic_task->eraseActiveTask(task_doc);
		aos_assert_r(rslt, false);
	}
	return true;
}

bool
AosDataCollectorMgr::cleanOutPut(
		const AosXmlTagPtr &task_doc,
		const AosRundataPtr &rdata)
{
	aos_assert_r(task_doc, false);
	u64 task_docid = task_doc->getAttrU64(AOSTAG_DOCID, 0);
	aos_assert_r(task_docid != 0, false);
	AosXmlTagPtr output_tags = task_doc->getFirstChild("output_data__n");
	if (output_tags)
	{
		AosXmlTagPtr task_data = output_tags->getFirstChild();
		while(task_data)
		{
			AosTaskDataType::E type = AosTaskDataType::toEnum(task_data->getAttrStr(AOSTAG_TYPE));
			if (type == AosTaskDataType::eOutPutIIL)
			{
				OmnString datacol_id = task_data->getAttrStr(AOSTAG_DATACOLLECTOR_ID);
				aos_assert_r(datacol_id != "", false);
				map<OmnString, AosLogicTaskObjPtr>::iterator d_itr = mCollectors.find(datacol_id);
				aos_assert_r(d_itr != mCollectors.end(), false);
				AosLogicTaskObjPtr logic_task = d_itr->second;
				aos_assert_r(logic_task, false);
				logic_task->cleanDataCol(task_docid, rdata);
			}
			task_data = output_tags->getNextChild();
		}
	}
	return true;
}

bool
AosDataCollectorMgr::reStartDataCol(
		const AosJobObjPtr &job,
		const OmnString &datacol_id,
		const AosRundataPtr &rdata)
{
	map<OmnString, AosLogicTaskObjPtr>::iterator d_itr = mCollectors.find(datacol_id);
	aos_assert_r(d_itr != mCollectors.end(), false);
	AosLogicTaskObjPtr logic_task = d_itr->second;
	return logic_task->reStartLogicTask(job, rdata);
}


void
AosDataCollectorMgr::getAllDataCol( 
		map<OmnString, AosLogicTaskObjPtr> &collectors) 
{ 
	mLock->lock();
	collectors = mCollectors;
	mLock->unlock();
}

//for log
void
AosDataCollectorMgr::showDataCollectorStart(
		const u64 job_docid,
		const OmnString &logic_id)
{
	AosJobLog(job_docid) << "Act:start" << " DCLT:" << logic_id << endlog;
}

