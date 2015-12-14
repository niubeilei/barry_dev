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
// 	Created: 2011/08/19 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "IDTransMap/IDTransMap.h"

#include "API/AosApi.h"
//#include "IDTransMap/TransBktMgr.h"
#include "TransUtil/IILTrans.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Siteid.h"
#include "Thread/Mutex.h"
#include "TransBasic/Trans.h"
#include "Util/OmnNew.h"
#include "UtilData/JPID.h" 
#include "UtilData/JSID.h"


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


AosIDTransMap::AosIDTransMap(
		const AosIDTransMapCallerPtr &caller,
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
	
	//AosTransBktMgr::getSelf()->config(OmnApp::getAppConfig());
	//AosTransBktMgr::getSelf()->start();
}


AosIDTransMap::~AosIDTransMap()
{
}


bool
AosIDTransMap::addTrans(
		const u64 &id,
		const u32 siteid,
		const AosIILTransPtr &trans,
		const AosRundataPtr &rdata)
{
	sgLastAddTransTime = OmnGetSecond();
	// It received a transaction from a client. It saves the transaction into the queue. 
	if (mIsStopping)
	{
		OmnAlarm << "is Stopping! should never add trans!" << enderr;
		return false;
	}
	aos_assert_r(mCaller, false);
	aos_assert_r(siteid > 0, false);

	mLock->lock();
	sgTotalReceived++;
	if ((sgTotalReceived % 50000) == 0)
	{
		OmnScreen << "Total IILTrans: " << sgTotalReceived << ":" << mIDMap.size() << ":" << mTotalTransNum << endl;
	}

	u64 combinedId = AosSiteid::combineSiteid(id, siteid);
	IDMapItr itr = mIDMap.find(combinedId);
	if (itr == mIDMap.end())
	{
		// Did not find the queue. Need to create one.
		AosIDTransVectorPtr v = OmnNew AosIDTransVector();
		pair<IDMapItr, bool> m = mIDMap.insert(make_pair(combinedId, v));
		itr = m.first; 
		mQueue.push_back(itr);
	}
	AosIDTransVectorPtr p = itr->second;
	aos_assert_rl(p, mLock, false);

	if (!p->empty() && sgSanityCheck)
	{
		AosIILTransPtr first_tran = (*(p.getPtr()))[0];
		AosIILType type1 = first_tran->getIILType();
		AosIILType type2 = trans->getIILType();

		if (type1 != eAosIILType_Invalid && type2 != eAosIILType_Invalid)
		{
			aos_assert_rl(type1 == type2, mLock, false);
		}
	}

	if ( trans->getRundata().isNull() )
	{
		AosRundataPtr global_rdata = OmnApp::getRundata();
		global_rdata->setSiteid(100);
		global_rdata->setJPID(AOSJPID_SYSTEM);
		global_rdata->setJSID(AOSJSID_SYSTEM);
		trans->setRundata(global_rdata);
	}
	else 
	{
		trans->setRundata(rdata);
	}
	p->push_back(trans);
	mTotalTransNum++;
	
	// Check whether it needs to process the transaction. It needs to 
	// process transactions if and only if:
	// 	1. The queue is too long
	// 	2. The total outstanding transactions are too many
	// 	3. The transaction requires response
	u32 len = p->size();
	if (len < mSingleSize && mTotalTransNum <= mMaxTransNum && 
		!mCaller->checkNeedProc(trans, rdata))
	{
		mLock->unlock();
		return true;
	}
	
	// It needs to process the transactions. It removes the queue from the
	// hash table, and creates a new queue for it.
	AosIDTransVectorPtr new_p = OmnNew AosIDTransVector();
	itr->second = new_p;
		
	bool needProc = false;
	if (len >= mSingleSize || len >= mBatchSize || 
		mCaller->checkNeedProc(trans, rdata))
	{
		needProc = true;
	}
	mLock->unlock();
		
	bool rslt = true;
	if (needProc)
	{
		AosIDTransVectorPtr p2 = OmnNew AosIDTransVector();

		mLock->lock();
		mTotalTransNum += p2->size();
		mLock->unlock();

		p2->insert(p2->end(), p->begin(), p->end());
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
			OmnAlarm << "p size is not empty" << enderr;
			clearIDTransVector(p);
		}
	}

	return rslt;
}


bool	
AosIDTransMap::isFree()
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
AosIDTransMap::postProc(const AosRundataPtr &rdata)
{
	int sgCount = 20;
	while(!mQueue.empty())
	{
		if (sgCount-- <= 0)
		{
			return true;
		}
		rdata->setJPID(AOSJPID_SYSTEM);
		rdata->setJSID(AOSJSID_SYSTEM);
		procQueue(rdata);
	}
	
	// The Queue is empty
	//AosIDTransMapObjPtr thisPtr(this, false);
	//AosTransBktMgr::getSelf()->procTransWithCheck(thisPtr, trans_processed, rdata);
	return true;
}


bool
AosIDTransMap::clearIDTransVector(const AosIDTransVectorPtr &p)
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
AosIDTransMap::getVectorById(const u64 &id, const u32 siteid, AosIDTransVectorPtr &p)
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
AosIDTransMap::procAllTrans(const AosRundataPtr &rdata)
{
	// Ketty 2013/01/30
	//mIsStopping = true;
	while(!mQueue.empty())
	{
		procQueue(rdata);
	}
	//AosTransBktMgr::getSelf()->procAllTrans(rdata);
	return true;
}


bool
AosIDTransMap::checkNeedSave(const u32 &num_trans)
{
	return (num_trans >= mSingleSize) ? true : false;
}


bool
AosIDTransMap::needProcQueue()
{
	mLock->lock();
	u32 size = mQueue.size();
	mLock->unlock();
	return (size >= mQueueSize) ? true : false;
}


bool
AosIDTransMap::needProcTrans()
{
	mLock->lock();
	u32 size = mTotalTransNum;
	mLock->unlock();
	return (size >= mMaxTransNum) ? true : false;
}


bool
AosIDTransMap::procTrans(const AosRundataPtr &rdata)
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
AosIDTransMap::procQueues(const AosRundataPtr &rdata)
{
	for (u32 i=0; i<sgProcQueueSize; i++)
	{
		procQueue(rdata);
	}
	return true;
}


bool
AosIDTransMap::procQueue(const AosRundataPtr &rdata)
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
		//AosTransBktMgr::getSelf()->procTrans(id, siteid, p2, rdata);

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


// Ketty 2012/12/18
bool
AosIDTransMap::cleanBkpVirtual(
		const AosRundataPtr &rdata,
		const u32 virtual_id)
{
	//OmnScreen << "IDTransMap clean Bkp Virtuals start!" << endl;

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
	
	OmnScreen << "IDTransMap clean Bkp Virtuals end!" << endl;
	return true;
}


// Ken Lee, 2013/05/17
void
AosIDTransMap::cleanCache()
{
	mLock->lock();
	mMaxTransNum = 0;
	mSingleSize = 0;
	mQueueSize = 0;
	mBatchSize = 0;
	mLock->unlock();
}


bool
AosIDTransMap::cleanCache(
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

