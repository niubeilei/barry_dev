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
// mCache
// Trans are appended to mCachedData. A hashmap is created that maps
// 		Map[id] = start_position
// It chains the trans that belong to the same IIL into a linked list. 
//
// Modification History:
// 2014/08/23 Copied from IILCacheEngine by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILCacheEngine/IILCacheEngine.h"

#include "API/AosApi.h"
#include "TransUtil/IILTrans.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Siteid.h"
#include "Thread/Mutex.h"
#include "TransBasic/Trans.h"
#include "Util/OmnNew.h"


static u32 sgDftBatchSize = 30;
static u32 sgDftSingleSize = 1000;
static u32 sgDftQueueSize = 20000;
static u32 sgDftMaxTransNum = 100000;

static u32 sgProcQueueSize = 10000;
static u32 sgProcTransSize = 50000;

static u32 sgTotalReceived = 0;
static u32 sgLastAddTransTime = 0;
static bool sgStartPostProc = false;
static bool sgSanityCheck = true;


AosIILCacheEngine::AosIILCacheEngine(
		const AosIILCacheEngineCallerPtr &caller,
		const AosXmlTagPtr &xml)
:
mCaller(caller),
mLock(OmnNew OmnMutex()),
mIsStopping(false),
mTotalTransNum(0)
{
	aos_assert(caller || xml);

	mMaxTransNum = xml->getAttrU32(AOSCONFIG_IDMAP_MAX_TRANS, sgDftMaxTransNum);
	mSingleSize = xml->getAttrU32(AOSCONFIG_IDMAP_SINGLE_SIZE, sgDftSingleSize);
	mQueueSize = xml->getAttrU32(AOSCONFIG_IDMAP_QUEUE_SIZE, sgDftQueueSize);
	mBatchSize = xml->getAttrU32(AOSCONFIG_IDMAP_BATCH_SIZE, sgDftBatchSize);
	mShowLog = xml->getAttrBool(AOSCONFIG_SHOWLOG, false);
	mBatchSize = 0;	// Ken Lee, 2013/06/07

	sgStartPostProc = xml->getAttrBool(AOSCONFIG_IDMAP_POSTPROC, false);
	sgProcQueueSize = xml->getAttrU32(AOSCONFIG_IDMAP_PROCQUEUESIZE, 10000);
	sgProcTransSize = xml->getAttrU32(AOSCONFIG_IDMAP_PROCTRANSSIZE, 50000);

	OmnScreen << "ID Trans Map created: \n" 
		<< "    Max Single Queue Length: " << mSingleSize << endl
		<< "    Max Trans:               " << mMaxTransNum << endl
		<< "    Max Buckets:             " << mQueueSize << endl
		<< "    Batch Size:              " << mBatchSize << endl;
}


AosIILCacheEngine::~AosIILCacheEngine()
{
}


bool
AosIILCacheEngine::addTrans(
		const u64 &id,
		const u32 siteid,
		AosIILTrans *trans,
		AosRundata *rdata)
{
	// An IIL trans is received. This function converts the trans into a
	// compact form and caches it in its cache.
	
	sgLastAddTransTime = OmnGetSecond();

	if (mIsStopping)
	{
		OmnAlarm << "is Stopping! should never add trans!" << enderr;
		return false;
	}
	aos_assert_r(mCaller, false);
	aos_assert_r(siteid > 0, false);

	// Save the trans.
	i64 pos = saveTrans(rdata, trans); 
	aos_assert_rr(pos >= 0, rdata, false);

	mLock->lock();
	sgTotalReceived++;
	if ((sgTotalReceived % 50000) == 0)
	{
		OmnScreen << "Total IILTrans: " << sgTotalReceived << ":" << mIDMap.size() 
			<< ":" << mTotalTransNum << endl;
	}

	u64 combinedId = AosSiteid::combineSiteid(id, siteid);
	IDMapItr itr = mIDMap.find(combinedId);
	if (itr == mIDMap.end())
	{
		// Did not find the queue. Need to create one.
		i64 pos = saveTrans(rdata, trans, -1); 
		mIDMap[id] = pos;
	}
	else
	{
		i64 tail = saveTrans(rdata, trans, itr->second.tail);
		itr->second.append(tail);
	}

	mTotalTransNum++;
	
	// Check whether it needs to process the transaction. It needs to 
	// process transactions if and only if the transaction requires response
	if (!mCaller->checkNeedProc(trans, rdata))
	{
		mLock->unlock();
		return true;
	}
	
	// It needs to process the transactions. It removes the entries from the
	// cache, and resets it.
	CachedInfo cached = itr->second;
	itr->second.reset();
		
	return mCaller->procTrans(rdata, cached, this);
}


bool	
AosIILCacheEngine::isFree()
{
	if (!sgStartPostProc)
	{
		return false;
	}

	u32 time = OmnGetSecond();
	if (time > sgLastAddTransTime && time - sgLastAddTransTime > 10)
	{
		return true;	
	}

	return false;
}


bool
AosIILCacheEngine::postProc(const AosRundataPtr &rdata)
{
	int sgCount = 20;
	while(!mQueue.empty())
	{
		if (sgCount-- <= 0)
		{
			return true;
		}
		procQueue(rdata);
	}
	
	return true;
}


bool
AosIILCacheEngine::clearIDTransVector(const AosIDTransVectorPtr &p)
{
	aos_assert_r(p, false);

	u32 len = p->size();
	p->clear();
	
	mLock->lock();
	if (mTotalTransNum >= len)
	{
		mTotalTransNum -= len;
	}
	else
	{
		OmnAlarm << "totalTransNum error, totalTransNum:" << mTotalTransNum << ", len:" << len << enderr;;
		mTotalTransNum = 0;;
	}
	mLock->unlock();
	return true;
}


bool
AosIILCacheEngine::getVectorById(const u64 &id, const u32 siteid, AosIDTransVectorPtr &p)
{
	aos_assert_r(siteid>0, false);
	u64 combinedId = AosSiteid::combineSiteid(id, siteid);
	
	mLock->lock();
	IDMapItr itr = mIDMap.find(combinedId);
	if (itr != mIDMap.end())
	{
		p = itr->second;	
		AosIDTransVectorPtr p2 = OmnNew AosIDTransVector();
		itr->second = p2;
		mLock->unlock();
		return true;
	}
		
	p = 0;
	mLock->unlock();
	return false;
}


bool
AosIILCacheEngine::procAllTrans(const AosRundataPtr &rdata)
{
	while(!mQueue.empty())
	{
		procQueue(rdata);
	}
	return true;
}


bool
AosIILCacheEngine::checkNeedSave(const u32 &num_trans)
{
	return (num_trans >= mSingleSize) ? true : false;
}


bool
AosIILCacheEngine::needProcQueue()
{
	mLock->lock();
	u32 size = mQueue.size();
	mLock->unlock();
	return (size >= mQueueSize) ? true : false;
}


bool
AosIILCacheEngine::needProcTrans()
{
	mLock->lock();
	u32 size = mTotalTransNum;
	mLock->unlock();
	return (size >= mMaxTransNum) ? true : false;
}


bool
AosIILCacheEngine::procTrans(const AosRundataPtr &rdata)
{
	mLock->lock();
	u32 size = mTotalTransNum;
	mLock->unlock();
	while(size >= mMaxTransNum - sgProcTransSize)
	{
		procQueue(rdata);
		mLock->lock();
		size = mTotalTransNum;
		mLock->unlock();
	}
	return true;
}


bool
AosIILCacheEngine::procQueues(const AosRundataPtr &rdata)
{
	for (u32 i=0; i<sgProcQueueSize; i++)
	{
		procQueue(rdata);
	}
	return true;
}


bool
AosIILCacheEngine::procQueue(const AosRundataPtr &rdata)
{
	// There is a queue for the hash table. When a new entry is added to the
	// hash table, it is appended to the queue. This means that the one in 
	// the front of the queue is the oldest. We will remove the oldest one.
	mLock->lock();
	if (mQueue.empty())
	{
		mLock->unlock();
		return true;
	}
	
	IDMapItr pop_itr = mQueue.front();
	mQueue.pop_front();
	u64 combinedId = pop_itr->first;
	AosIDTransVectorPtr p = pop_itr->second;
	mIDMap.erase(combinedId);
	mLock->unlock();
	
	u32 siteid;
	u64 id = AosSiteid::separateSiteid(combinedId, siteid);

	u32 len = p->size();
	bool rslt = true;
	if (mIsStopping || len > mBatchSize)
	{
		AosIDTransVectorPtr p2 = OmnNew AosIDTransVector();

		mLock->lock();
		mTotalTransNum += p2->size();
		mLock->unlock();

		p2->insert(p2->end(), p->begin(), p->end());
		rdata->setSiteid(siteid);	// Ketty 2012/03/19
		if (p2->size() > 0)
		{
			rslt = mCaller->procTrans(id, siteid, p2, rdata);
			clearIDTransVector(p2);
		}
	}
	else
	{
		if (p->size() > 0)
		{
			//rslt = AosTransBktMgr::getSelf()->addTrans(id, siteid, p, rdata);
			clearIDTransVector(p);
		}
	}
	if (!rslt) OmnAlarm << "procPopItr error:" << pop_itr->first << enderr;
	return rslt;
}


bool
AosIILCacheEngine::cleanBkpVirtual(
		const AosRundataPtr &rdata,
		const u32 virtual_id)
{
	//OmnScreen << "IILCacheEngine clean Bkp Virtuals start!" << endl;

	mLock->lock();
	
	vector<IDMapItr> map_itrs;
	list<IDMapItr>::iterator q_itr = mQueue.begin();
	while(q_itr != mQueue.end())
	{
		IDMapItr m_itr = *q_itr;
		aos_assert_rl(m_itr != mIDMap.end(), mLock, false);

		u64 combinedId = m_itr->first;
		u32 loc_iilid = (u32)combinedId;
		u32 c_vid = loc_iilid % AosGetNumCubes();
		if (c_vid != virtual_id)
		{
			q_itr++;
			continue;
		}

		map_itrs.push_back(m_itr);  
		list<IDMapItr>::iterator tmp_qitr = q_itr;  
		q_itr++;
		mQueue.erase(tmp_qitr);
	}

	for (u32 i=0; i<map_itrs.size(); i++)
	{
		mIDMap.erase(map_itrs[i]);
	}
	mLock->unlock();
	
	OmnScreen << "IILCacheEngine clean Bkp Virtuals end!" << endl;
	return true;
}


void
AosIILCacheEngine::cleanCache()
{
	mLock->lock();
	mMaxTransNum = 0;
	mSingleSize = 0;
	mQueueSize = 0;
	mBatchSize = 0;
	mLock->unlock();
}


bool
AosIILCacheEngine::cleanCache(
		const u32 virtual_id, 
		const AosRundataPtr &rdata)
{
	mLock->lock();

	list<IDMapItr>::iterator itr = mQueue.begin();
	while(itr != mQueue.end())
	{
		IDMapItr m_itr = *itr;

		u64 combinedId = m_itr->first;
		u32 siteid;
		u64 id = AosSiteid::separateSiteid(combinedId, siteid);
		u32 vid = id % AosGetNumCubes();
		if (vid != virtual_id)
		{
			itr++;
			continue;
		}

		list<IDMapItr>::iterator q_itr = itr;
		itr++;
		mQueue.erase(q_itr);

		AosIDTransVectorPtr p = m_itr->second;
		aos_assert_rl(p, mLock, false);

		mTotalTransNum -= p->size();
		for (u32 i=0; i<p->size(); i++)
		{
			AosIILTransPtr iil_trans = p->at(i);
			aos_assert_rl(iil_trans, mLock, false);

			AosTransPtr trans = iil_trans.getPtr();
			AosFinishTrans(p->at(i));
		}
		mIDMap.erase(m_itr);
	}

	mLock->unlock();
	return true;
}

