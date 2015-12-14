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
#include "DataCollector/DataCollectorGroup.h"

#include "DataCollector/DataCollectorMgr.h"
#include "Job/Job.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/DataCacherObj.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "SEInterfaces/JobMgrObj.h"
#include "Thread/ThreadPool.h"
#include "Thread/Sem.h"
#include "Porting/Sleep.h"


AosDataCollectorGroup::AosDataCollectorGroup(const u64 &job_docid)
:
AosDataCollector(job_docid),
//mLock(OmnNew OmnMutex()),
mChildSize(0),
mFinishedNum(0)
{
}


AosDataCollectorGroup::~AosDataCollectorGroup()
{
}

bool
AosDataCollectorGroup::config(
		const AosJobObjPtr &job,
		const AosXmlTagPtr &conf,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	aos_assert_r(conf, false);

	mLock->lock();
	mDataColTag = conf;
	mDataColId = conf->getAttrStr(AOSTAG_NAME, "");
	mLogicId = mDataColId;
	mType = eGroup;

	aos_assert_r(job, false); 
	AosDataCollectorMgrPtr colMgr = job->getDataColMgr();

	int shuff_id = -1;;
	OmnString child_id, new_id;
	AosXmlTagPtr child_conf;
	AosLogicTaskObjPtr child, parent;
	OmnString child_type = conf->getAttrStr("zky_child_type", "iil");

	shuff_id = conf->getAttrInt(AOSTAG_SHUFFLE_ID, -1);
	if (shuff_id >= 0)
	{
		parent = colMgr->getDataColLocked(mDataColId, rdata);
		if (parent)
		{
			rslt = mergePre(parent);
			aos_assert_r(rslt, false);
		}

		child_id = mDataColId;
		child_id << "_" << shuff_id;
OmnScreen << "child datacollector id is " << child_id << endl;
		child_conf = conf->clone(AosMemoryCheckerArgsBegin);
		child_conf->removeAttr(AOSTAG_SHUFFLE_TYPE);
		child_conf->setAttr(AOSTAG_NAME, child_id);
		child_conf->setAttr(AOSTAG_ZKY_TYPE, child_type);
		rslt = colMgr->createDataCollectorLocked(mJobDocid, job, child_conf, new_id, rdata);
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(new_id == child_id, mLock, false);
		child = colMgr->getLogicTaskLocked(child_id, rdata);
		child->setParentDataColId(mDataColId, shuff_id);
		mChilds.push_back(child);
		mChildSize = mChilds.size();
		mLock->unlock();
		return true;
	}

	OmnString group_type = conf->getAttrStr(AOSTAG_SHUFFLE_TYPE, "");
	aos_assert_r(group_type != "", false);

	toEnum(group_type);

	//rslt = isValid(mGroupType);
	//aos_assert_rl(rslt, mLock, false);

	switch(mGroupType)
	{
	case eCubeShuffler:
		mChildSize = AosGetNumCubes();
		break;
	case ePhyShuffler: 
		mChildSize = AosGetNumPhysicals();
		break;
	default:
		mChildSize = group_type.toInt();
		if (mChildSize == 0 ) mChildSize  = 1;
		break;
	}

	mGroupNum = mChildSize;
	for (u32 i = 0; i < mChildSize; i++)
	{
		child_id = mDataColId;
		child_id << "_" << i;

OmnScreen << "child datacollector id is " << child_id << endl;

		child_conf = conf->clone(AosMemoryCheckerArgsBegin);
		child_conf->removeAttr(AOSTAG_SHUFFLE_TYPE);
		child_conf->setAttr(AOSTAG_NAME, child_id);
		child_conf->setAttr(AOSTAG_ZKY_TYPE, child_type);
		rslt = colMgr->createDataCollectorLocked(mJobDocid, job, child_conf, new_id, rdata);
		aos_assert_rl(rslt, mLock, false);
		aos_assert_rl(new_id == child_id, mLock, false);
		//for log
		showCreateDataCollector(new_id);
		child = colMgr->getLogicTaskLocked(child_id, rdata);
		child->setParentDataColId(mDataColId, i);
		mChilds.push_back(child);
OmnScreen << "DataCollectorGroup create child . index : " << i << " ." << endl;
	}

	mLock->unlock();
	return true;
}


bool 
AosDataCollectorGroup::addOutput(
		const AosXmlTagPtr &output, 
		const AosRundataPtr &rdata)
{
OmnNotImplementedYet;
return true;
/*
	mLock->unlock();
	bool rslt = false;
	int group_id = -1, idx = -1;
	switch(mGroupType)
	{
	case eCubeShuffler:
		group_id = output->getAttrInt(AOSTAG_CUBE_ID, -1);
		aos_assert_r(group_id != -1, false);
		break;
	case ePhyShuffler:
		group_id = output->getAttrInt(AOSTAG_PHYSICALID, -1);
		aos_assert_r(group_id != -1, false);
		break;
	default:
		OmnAlarm  << " undefined group type ." << enderr;
		return false;
	}
	idx = group_id % mChildSize;
	rslt = mChilds[idx]->addOutput(output, rdata);
	aos_assert_rl(rslt, mLock, false);
	return true;
	*/
}


bool
AosDataCollectorGroup::getOutputFiles(
		vector<AosXmlTagPtr> &files,
		const AosRundataPtr &rdata)
{
	bool rslt = false;
	for (u32 i = 0; i < mChilds.size(); i++)
	{
		rslt = mChilds[i]->getOutputFiles(files, rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool 
AosDataCollectorGroup::addTriggedTaskid(
		const OmnString &task_id, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	mTaskIds.push_back(task_id);
	mLock->unlock();
	return true;
}


bool
AosDataCollectorGroup::callBack(
		const AosXmlTagPtr &output,
		const AosRundataPtr &rdata)
{
OmnScreen << "DataCollectorGroup callBack." << endl;
	// chlid finished call 
	mLock->lock();
	bool rslt = false;
	mFinishedNum++;
	if (mFinishedNum == mChildSize)
	{
		mFinished = true;
		rslt = dataCollectorFinished(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}

bool 
AosDataCollectorGroup::finishDataCollector(const AosRundataPtr &rdata)
{
	aos_assert_r(mStartNum > 0, false);
	mStartNum--;
	if (mStartNum > 0)
	{
		return true;
	}

	bool rslt = false;
	for (u32 i = 0; i < mChilds.size(); i++)
	{
		rslt = mChilds[i]->finishDataCollector(rdata);
		aos_assert_r(rslt, false);
	}
	return true;
}


bool 
AosDataCollectorGroup::isFinished()
{
	if (mFinished) return true;

	for (u32 i = 0; i < mChilds.size(); i++)
	{
		if (!mChilds[i]->isFinished())
			return false;
	}
	mFinished = true;
	return mFinished;
}


bool
AosDataCollectorGroup::dataCollectorFinished(const AosRundataPtr &rdata)
{
OmnScreen << "DataCollectorGroup Finished." << mDataColId << endl;
	showDataCollectorFinish();
	bool rslt = false;

	mFinished = true;
	AosJobObjPtr job = AosJobMgrObj::getJobMgr()->getStartJob(mJobDocid, rdata);  
	aos_assert_r(job, false); 
	rslt = job->datacollectorFinished(rdata, mLogicId, mTaskIds);		
	aos_assert_r(rslt, false);

	return true;
}

void 
AosDataCollectorGroup::toEnum(const OmnString &group_type)
{
	OmnString type = group_type;
	type.toLower();

	if (type == "cube")
	{
		mGroupType = eCubeShuffler;
	}
	else if (type == "physical")
	{
		mGroupType = ePhyShuffler;
	}
	return ;
}

bool
AosDataCollectorGroup::mergePre(const AosLogicTaskObjPtr &data_col)
{
	AosDataCollectorGroup* from = dynamic_cast<AosDataCollectorGroup*>(data_col.getPtr()); 
	if (from)
	{
		mChilds = from->mChilds;
		mChildSize = from->mChildSize;
		mFinishedNum = from->mFinishedNum;
	}
	return true;
}

void 
AosDataCollectorGroup::showCreateDataCollector(
		const OmnString &col_id)
{
	AosJobLog(mJobDocid) << "Act:create PDCLT:" << mLogicId << " CDCLT:" << col_id << endlog;
}


void 
AosDataCollectorGroup::showDataCollectorFinish()
{
	AosJobLog(mJobDocid) << "Act:finish DCLT:" << mLogicId << endlog;
}

