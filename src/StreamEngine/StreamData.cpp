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
#include "StreamEngine/StreamData.h"

#include <time.h>
#include "API/AosApi.h"
#include "SEInterfaces/DataBlobObj.h"
#include "SEInterfaces/DataProcObj.h"
#include "SEInterfaces/SysInfo.h"
#include "Debug/Debug.h"
#include "StreamEngine/StreamDataProc.h"
#include "StreamEngine/StreamSyncer.h"
#include "Job/Job.h"
#include "JQLStatement/JqlStmtInsertItem.h"
#include "JQLStatement/JqlStmtUpdateItem.h"
#include "Porting/Sleep.h"
#include "StreamEngine/SendStreamDataTrans.h"
#include <string> 
using namespace std;

static map<OmnString, AosStreamDataPtr> sgDataMap;
static OmnMutex	sgLock;
AosStreamData::AosStreamData(
			const AosRundataPtr &rdata,
			const OmnString &dataId,
			const OmnString &serviceId)
:
mStartTime(0),
mTotalProcs(0),
mServiceDocid(0),
mCurRDDId(0),
mInterval(eDefaultInterval),
mMaxCacheSize(eMaxCacheSize),
mRecvDataLen(0),
mDataId(dataId),
mName(""),
mDataProcName(""),
mServiceId(serviceId),
mDataProc(0),
mPolicy(OmnNew AosStreamPolicy(AosStreamPolicy::eShuffle)),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin)),
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	if (mServiceId != "" && !mServiceDocid)
	{
		OmnString objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, mServiceId);
		AosXmlTagPtr doc = AosGetDocByObjid(objid, mRundata);
		aos_assert(doc);

		u64 interval = doc->getAttrStr("zky_interval").toU64();
		i64 volume = doc->getAttrStr("zky_volume").toInt64();
		//aos_assert_r(interval >= 0 && volume > 0, false);

		//set interval and volume
		setInterval(interval);
		setMaxCacheSize(volume);

		mServiceDocid = doc->getAttrU64(AOSTAG_DOCID, 0);
		aos_assert(mServiceDocid);

		OmnString objname = doc->getAttrStr("zky_objname");
		aos_assert(objname != "");

		objid = AosObjid::getObjidByJQLDocName(JQLTypes::eServiceDoc, objname);
		doc = AosGetDocByObjid(objid, mRundata);
		aos_assert(doc);

		AosXmlTagPtr json_node = doc->getFirstChild("jsonstr");
		aos_assert(json_node);
		OmnString dp_json = json_node->getNodeText();

		JSONValue json;
		JSONReader reader;
		bool rslt = reader.parse(dp_json, json);
		aos_assert(rslt);
		JSONValue dataflow = json["dataflow"];

		findDataFlow(dataflow, mDataId);
	}
	OmnThreadedObjPtr thisPtr(this, false);
	mThread = OmnNew OmnThread(thisPtr, "StreamDataThrd", 0, true, true, __FILE__, __LINE__);
	mThread->start();
}

AosStreamData::~AosStreamData()
{
}

bool
AosStreamData::findDataFlow(const JSONValue &dataflow, const OmnString &fname)
{
	OmnString input_name;
	OmnString output_name;
	int idx = fname.indexOf(".", 0);
	if (idx > 0)
	{
		//xxxxx.input
		output_name = fname.substr(0, idx - 1);
		Value::Members dataMembers = dataflow.getMemberNames();
		for (u32 i=0; i<dataMembers.size(); i++)
		{
			OmnString mname = dataMembers[i];
			idx = mname.indexOf(".", 0);
			if (idx > 0)
			{
				input_name = mname.substr(0, idx - 1);
				if (output_name == input_name)
				{
					for (u32 j=0; j<dataflow[mname].size(); j++)
					{
						mTotalProcs++;
						findDataFlow(dataflow, dataflow[mname][j].asString());
					}
				}
			}
		}
	}
	else
	{
		//dataname
		for (u32 i=0; i<dataflow[fname].size(); i++)
		{
			mTotalProcs++;
			findDataFlow(dataflow, dataflow[fname][i].asString());
		}
	}
	return true;
}

u64
AosStreamData::getCrtRDDId()
{
	int crt_rddid = 0;
	mLock->lock();
	mCurRDDId++;
	crt_rddid = mCurRDDId;
	mLock->unlock();
	return crt_rddid;
}

bool
AosStreamData::feedStreamData(AosRundata *rdata,AosDataRecordObjPtr &rcd)
{
	AosBuffDataPtr tmp = NULL;
	bool flag = false;
	if(!mRecord_set)
	{
		mRecord_set = AosRecordsetObj::createStreamRecordsetStatic(rdata, rcd);
	}

	flag = mRecord_set->appendRecord(rdata,rcd,tmp);
	if (mRecord_set->size() == eMaxRecordset)
	{
		flag = feedStreamData(rdata, mRecord_set);
		mRecord_set = NULL;
	}
	return flag;
}

bool
AosStreamData::finishFeedStreamData(AosRundata *rdata)
{
	if(!mRecord_set)
	{
		return true;
	}
	bool flag = feedStreamData(rdata, mRecord_set);
	mRecord_set = NULL;
	return flag;
}

bool
AosStreamData::feedStreamData(const AosRundataPtr &rdata, const AosRecordsetObjPtr &rs)
{
	if (mStartTime == 0)
		mStartTime = OmnGetSecond();

	AosRDDPtr rdd = OmnNew AosRDD(mDataId, "", mDataProcName, mServiceId, getCrtRDDId(), rs, rdata);

	aos_assert_r(rdd, false);
	rdd->setTotalNum(mTotalProcs);
	rdd->setIsStreamRecordset(rs->getIsStreamRecordset());

	//get the stream syncer if any
	while (!AosStreamSyncer::holdByStream(mName, rdd->getRDDId()))
		OmnMsSleep(100);

	i64 data_len = rdd->getDataLen();
	OmnScreen << "*******jozhi deubg, dsName: " << mDataId << " , feed rdd id: " << rdd->getRDDId() << endl;
	bool timeout = false;
	mLock->lock();	
	mWaitRDDs.push_back(rdd);
	mRecvDataLen += data_len;
	while(mRecvDataLen > mMaxCacheSize)
	{
		mCondVar->timedWait(mLock, timeout, 1);
	}
//OmnScreen << "**********add recv data length: " << mRecvDataLen << endl;
	mLock->unlock();
	return true;
}

/*
bool
AosStreamData::dataProcessed(
		const AosRundataPtr &rdata, 
		const u64 &rddId)
{
	map<u64, vector<AosRDDPtr> >::iterator itr;
	vector<AosRDDPtr>::iterator rddItr;
	AosRDDPtr rdd;
	mLock->lock();
	itr = mRDDMap.find(rddId);
	if (itr != mRDDMap.end())
	{
		rddItr = (itr->second).begin();
		while (rddItr != (itr->second).end())
		{
			rdd = *rddItr;
			if (rdd->isInProcess())
			{
				rdd->processed(rdata);
				rddItr = (itr->second).erase(rddItr);
			}
			else
			{
				rddItr++;
			}
		}
//OmnScreen << "debug memory leak delete, rddId: " << rddId << " mRDDMap.size : " << mRDDMap.size() << endl;
		mRDDMap.erase(itr);
	}

	mLock->unlock();
	return true;
}

//
//this is for external stream data
//
bool
AosStreamData::dataCommitted(const u64 &rddId)
{
	map<u64, vector<AosRDDPtr> >::iterator itr;

	mLock->lock();
	
	itr = mRDDMap.find(rddId);
	if (itr != mRDDMap.end())
	{
		(itr->second).clear();
	}

	//release the syncers
	AosStreamSyncer::releaseByStream(mName, rddId);

	mLock->unlock();
	return true;
}
*/

AosStreamDataPtr
AosStreamData::getStreamData(AosRundata *rdata, const OmnString &dataset_name, const OmnString &serviceId)
{
	AosStreamDataPtr stream_data;
	sgLock.lock();
	map<OmnString, AosStreamDataPtr>::iterator itr;
	itr = sgDataMap.find(dataset_name);
	if (itr != sgDataMap.end())
	{
		stream_data = itr->second;
	}
	else
	{
		stream_data = OmnNew AosStreamData(rdata, dataset_name, serviceId);
		sgDataMap[dataset_name] = stream_data;
	}
	sgLock.unlock();
	return stream_data;
}

bool
AosStreamData::sendRDD(const AosRDDPtr &rdd)
{
	OmnScreen << "**********************send rdd id: " << rdd->getRDDId() << endl;
	int phy_id = -1;
	u32 logic_pid = 0;
	mPolicy->getTargetProcess(phy_id, logic_pid, rdd->getRDDId(), mServiceDocid);
	rdd->setStartTime();
	AosTransPtr trans = OmnNew AosSendStreamDataTrans(rdd, phy_id, logic_pid, false, false);
	AosBuffPtr resp_buff;
	bool timeout = false;
	bool rslt = AosSendTrans(mRundata, trans, timeout, resp_buff);
	aos_assert_r(rslt && resp_buff && resp_buff->getU8(0), false);
	return true;
}


bool    
AosStreamData::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread)
{
	bool timeout = false;
	while (state == OmnThrdStatus::eActive)
	{
		AosRDDPtr rdd;
		mLock->lock();
		if (mWaitRDDs.empty())
		{
			mCondVar->timedWait(mLock, timeout, 1);
			mLock->unlock();
			continue;
		}
		else
		{
			rdd = mWaitRDDs.front();
			mWaitRDDs.pop_front();
			bool rslt = sendRDD(rdd);
			aos_assert_rl(rslt, mLock, false);
			mRecvDataLen -= rdd->getDataLen();
			mStartRDDs[rdd->getRDDId()] = rdd;
//OmnScreen << "**********remove recv data length: " << mRecvDataLen << endl;
			mCondVar->timedWait(mLock, timeout, mInterval);
		}
		mLock->unlock();
	}
	return true;
}

bool    
AosStreamData::signal(const int threadLogicId)
{
	return true;
}


bool	
AosStreamData::setInterval(const u64 &val)
{
	mLock->lock();
	mInterval = val;
	mLock->unlock();
	return true;
}

bool
AosStreamData::setMaxCacheSize(const i64 &max_cache_size)
{
	mLock->lock();
	mMaxCacheSize = max_cache_size;
	mLock->unlock();
	return true;
}

bool
AosStreamData::updateRDDStatus(
		const AosTaskStatus::E status, 
		const OmnString &dp_name, 
		const u64 &rddid, 
		const int remain, 
		const AosRundataPtr &rdata)
{
OmnScreen << "****dataid: " << mDataId 
	<< " , rddid: " << rddid 
	<< " , dataproc name: " << dp_name 
	<< " , status: " << AosTaskStatus::toStr(status)
	<< " , total procs: " << mTotalProcs 
	<< " , remain: " << remain << endl;
	mLock->lock();
	AosRDDPtr rdd = mStartRDDs[rddid];
	mLock->unlock();
	aos_assert_r(rdd, false);
	switch(status)
	{
	case AosTaskStatus::eStart:
		rdd->start();
		break;
	case AosTaskStatus::eFinish:
		rdd->finish(remain);
		break;
	default:
		OmnAlarm << "invalid status" << enderr;
		break;
	}
	if (rdd->isFinished())
	{
		rdd->setFinishTime();
		mLock->lock();
		mStartRDDs.erase(rddid);
		u64 start = rdd->getStartTime();
		u64 finish = rdd->getFinishTime();
OmnScreen << "==============[ " << mDataId << " RDD Finished, " << rddid << " , take time: " <<  finish - start << " ]================"<< endl;
		if (mStartRDDs.empty() && mWaitRDDs.empty())
		{
OmnScreen << "Dataset: " << mDataId << " RDD is Finished spend " << OmnGetSecond() - mStartTime << "s" << endl;
			map<u64, AosRDDPtr> tmp_s;
			mStartRDDs = tmp_s;
			deque<AosRDDPtr> tmp_q;
			mWaitRDDs = tmp_q;
		}
		mLock->unlock();
	}
	return true;
}
