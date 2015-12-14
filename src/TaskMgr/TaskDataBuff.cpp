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
// 05/14/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "TaskMgr/TaskDataBuff.h"

#include "API/AosApi.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/SysInfo.h"
#include "Debug/Debug.h"
#include "Job/Job.h"
#include "JQLStatement/JqlStmtInsertItem.h"
#include "JQLStatement/JqlStmtUpdateItem.h"


AosTaskDataBuff::AosTaskDataBuff(const bool flag)
:
AosTaskData(AosTaskDataType::eBuff, AOSTASKDATA_BUFF, flag)
{
	mPkts = 0;
	mBytes = 0;
	mDoc = 0;
}

AosTaskDataBuff::AosTaskDataBuff(
		const AosTaskObjPtr task)
:
AosTaskData(AosTaskDataType::eBuff, AOSTASKDATA_BUFF, false),
mTask(task)
{
	mPkts = 0;
	mBytes = 0;
	mDoc = 0;
	mActionName = "";
	mAsmName = "";
	mInOut = "";

	mLock = OmnNew OmnMutex();
	mFieldMap.clear();
}

AosTaskDataBuff::~AosTaskDataBuff()
{
}

bool
AosTaskDataBuff::config(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata)
{
	OmnTagFuncInfo << endl;

	bool rslt = AosTaskData::config(def, rdata);
	if (!rslt)
	{
		AosSetErrorU(rdata, "config_error:") << def->toString();
		return false;
	}

	mPkts = def->getAttrInt(AOSTAG_DATA_PKTS, -1);
	if (mPkts < 0)
	{
		AosSetErrorU(rdata, "invalid_total_pkts:") << def->toString();
		return false;
	}

	mPkts = def->getAttrInt(AOSTAG_DATA_BYTES, -1);
	if (mPkts < 0)
	{
		AosSetErrorU(rdata, "invalid_total_bytes:") << def->toString();
		return false;
	}

	return true;
}


bool
AosTaskDataBuff::serializeTo(
		AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	OmnString docstr;
	docstr << "<" << AOSTAG_TASKDATA_TAGNAME << " "
		<< AOSTAG_DATA_PKTS << "=\"" << mPkts << "\" " 
		<< AOSTAG_DATA_BYTES << "=\"" << mBytes << "\" ></" 
		<< AOSTAG_TASKDATA_TAGNAME << ">";	

	xml = AosXmlParser::parse(docstr AosMemoryCheckerArgs);
	aos_assert_r(xml, false);

	bool rslt = AosTaskData::serializeTo(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


bool 
AosTaskDataBuff::serializeFrom(
		const AosXmlTagPtr &xml,
		const AosRundataPtr &rdata)
{
	aos_assert_r(xml, false);

	mPkts = xml->getAttrInt(AOSTAG_DATA_PKTS, -1);
	mBytes = xml->getAttrInt(AOSTAG_DATA_BYTES, -1);
	bool rslt = AosTaskData::serializeFrom(xml, rdata);
	aos_assert_r(rslt, false);

	return true;
}


AosTaskDataObjPtr
AosTaskDataBuff::create() const
{
	OmnTagFuncInfo << endl;
	return OmnNew AosTaskDataBuff(false);
}


AosTaskDataObjPtr
AosTaskDataBuff::create(
		const AosXmlTagPtr &def, 
		const AosRundataPtr &rdata) const
{
	OmnTagFuncInfo << endl;
	AosTaskDataBuff * data = OmnNew AosTaskDataBuff(false);
	bool rslt = data->config(def, rdata);
	aos_assert_r(rslt, 0);
	return data;
}

bool
AosTaskDataBuff::isDataExist(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mBuffList.size() > 0)
	{
		mLock->unlock();
		return true;
	}

	mLock->unlock();
	return false;
}

void
AosTaskDataBuff::setDataId(OmnString dataId)
{
	mDataId = dataId;
}

//
//We will add some flow control policy later on. Currently just
//append the mBuff to the list of mBuff list
//
//We will also need to create a map to save different mbuff for
//different dataId
//
bool 
AosTaskDataBuff::addBuff(AosBuffPtr buff)
{
	mLock->lock();

	if (true)
	{
		mBuffList.push_back(buff);
		mPkts++;
		mBytes += buff->dataLen();
		mLock->unlock();

		//update data status info in sys job table
		updateJobDataStat(mTask->getRundata());
		return true;
	}

	mLock->unlock();
	return false;
}

/*
AosBuffPtr 	
AosTaskDataBuff::getNextBuff()
{
	AosBuffPtr buff;

	mLock->lock();
	if (mBuffList.size() > 0)
	{
		buff = mBuffList.front();
		mBuffList.pop_front();
		mLock->unlock();
		mBuff = buff;
		return buff;
	}

	mLock->unlock();
	mBuff = 0;
	return 0;
}
*/

AosBuffPtr 	
AosTaskDataBuff::getNextBuff(u64 iduId)
{
	AosIDU *idu;

	idu = findNextData(iduId);
	if (idu)
	{
		idu->inProcess(mTask->getRundata());
		return idu->getBuff();
	}
	
	return 0;
}

bool
AosTaskDataBuff::hasNextBuff(u64 iduId)
{
	AosIDU *idu = findNextData(iduId);
	if (idu)
		return true;

	return false;
}

AosIDU*
AosTaskDataBuff::findNextData(u64 iduId)
{
	vector<AosIDU*>* iduList;
	map<u64, vector<AosIDU*>*>::iterator itr;
	AosIDU *idu;

	mLock->lock();
	
	itr = mIDUMap.find(iduId);
	if (itr != mIDUMap.end())
	{
		iduList = itr->second;
		aos_assert_r(iduList, 0);
		
		for (u32 i = 0; i < iduList->size(); i++)
		{
			idu = (*iduList)[i];
			if (!idu->isInProcess() && !idu->isProcessed())
			{
				mLock->unlock();
				return idu;
			}
		}
	}

	mLock->unlock();
	return NULL;
}

bool
AosTaskDataBuff::updateCounter(int pkts, int bytes)
{
	mPkts += pkts;
	mBytes += bytes;

	bool rslt = updateJobDataStat(mTask->getRundata());
	return rslt;
}

bool
AosTaskDataBuff::sendTaskData(u64 iduId, int phyId, AosBuffPtr buff)
{
	//for debugging, use default parameters. Later on, the params
	//need to be passed from caller
	AosRundataPtr rdata = mTask->getRundata();
	vector<u64>* downStreamIds = AosJob::findDownStreamStatic(
			rdata, mTask->getJobDocid(), mDataId );
	AosIDU *idu;
	bool rslt;
	int segId;

	//if phyId = -1, use some policy to determine the downstream node
	//later on, we will have more policies
	if (phyId < 0)
		phyId = rand() % AosGetNumPhysicals();

	if (downStreamIds)
	{
		u64 docid;

		if (downStreamIds->size() > 0)
			segId = getNextSegId(iduId); 

		for (u32 i = 0; i < downStreamIds->size(); i++)
		{
			docid = (*downStreamIds)[i];

			//create a new IDU
			idu = new AosIDU();
			idu->setDataId(mDataId);
			idu->setIDUId(iduId);
			idu->setSegId(segId);
			idu->setSendPhyId(AosGetSelfServerId());
			idu->setSendTaskId(mTask->getTaskDocid());
			idu->setRecvPhyId(phyId);
			idu->setRecvTaskId(docid);
			idu->setBuff(buff);

			//send the idu
			rslt = idu->send(rdata);
			aos_assert_r(rslt, false);

			//update counter
			updateCounter(1, buff->dataLen());
		}
	}

	return true;
}

u64
AosTaskDataBuff::recvTaskData(AosBuffPtr confBuff, AosBuffPtr dataBuff)
{
	u64 iduId;
	AosIDU *idu;
	map<u64, vector<AosIDU*>*>::iterator itr;
	vector<AosIDU*> *iduList;
	AosRundataPtr rdata = mTask->getRundata();

	OmnTagFuncInfo << "data buff len is: " << dataBuff->dataLen() << endl;
	idu = new AosIDU();
	iduId = idu->recv(rdata, confBuff, dataBuff); 
	itr = mIDUMap.find(iduId);
	if (itr == mIDUMap.end())
	{
		//an IDU with new IDUid
		iduList = new vector<AosIDU*>();
		iduList->push_back(idu);
		mIDUMap[iduId] = iduList;
	}
	else
	{
		iduList = itr->second;
		iduList->push_back(idu);
	}

	OmnTagFuncInfo << "data buff len is: " << dataBuff->dataLen() << endl;
	updateCounter(1, dataBuff->dataLen());
	return iduId;
}

bool
AosTaskDataBuff::insertJobDataStat(const AosRundataPtr &rdata)
{
	OmnString str;

	//init fileldmap
	mFieldMap[JOBDATASTAT_SERVICE] = mTask->getServiceName(); 
	mFieldMap[JOBDATASTAT_JOBNAME] = "J1";
	
	str = "";
	str << mTask->getJobDocid();
	mFieldMap[JOBDATASTAT_JOBID] = str;

	str = "";
	str << mTask->getTaskDocid();
	mFieldMap[JOBDATASTAT_TASKID] = str;

	str = "";
	str << mTask->getPhysicalId();
	mFieldMap[JOBDATASTAT_PHYID] = str;

	mFieldMap[JOBDATASTAT_ACTNAME] = mActionName;
	mFieldMap[JOBDATASTAT_ASMNAME] = mAsmName;
	mFieldMap[JOBDATASTAT_DATAID] = mDataId;
	mFieldMap[JOBDATASTAT_INOUT] = mInOut;

	str = "";
	str << mPkts;
	mFieldMap[JOBDATASTAT_PKTS] = str;

	str = "";
	str << mBytes;
	mFieldMap[JOBDATASTAT_BYTES] = str;

	//insert an entry to sys job table
	mDoc = AosJqlStmtInsertItem::insertJobDataStat(rdata, &mFieldMap);
	aos_assert_r(mDoc, false);

	return true;
}

bool		
AosTaskDataBuff::updateJobDataStat(const AosRundataPtr &rdata)
{
	OmnString str;

	//update counters 
	str = "";
	str << mPkts;
	mFieldMap[JOBDATASTAT_PKTS] = str;

	str = "";
	str << mBytes;
	mFieldMap[JOBDATASTAT_BYTES] = str;

	//get current time
	//mFieldMap[JOBDATASTAT_TIME] = "10:01:02";

	return AosJqlStmtUpdateItem::updateData(rdata, mDoc, &mFieldMap);
}

int
AosTaskDataBuff::getNextSegId(u64 iduId)
{
	map<u64, int>::iterator itr;

	itr = mSegMap.find(iduId);
	if (itr != mSegMap.end())
		mSegMap[iduId]++;
	else
		mSegMap[iduId] = 1; //starting from 1

	return mSegMap[iduId];
}

bool
AosTaskDataBuff::dataProcessed(AosRundataPtr rdata, 
							   u64 iduId)
{
	vector<AosIDU*>* iduList;
	map<u64, vector<AosIDU*>*>::iterator itr;
	AosIDU *idu;

	mLock->lock();
	
	itr = mIDUMap.find(iduId);
	if (itr != mIDUMap.end())
	{
		iduList = itr->second;
		aos_assert_r(iduList, 0);
		for (u32 i = 0; i < iduList->size(); i++)
		{
			idu = (*iduList)[i];
			if (idu->isInProcess())
				idu->processed(rdata);
		}
	}

	mLock->unlock();
	return true;
}

#endif
