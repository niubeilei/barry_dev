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
//
// Modification History:
// 	2013/10/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEUtil/ObjectCacher.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/CachedObject.h"
#include "XmlUtil/XmlTag.h"

static u64 sgHitCount = 0;
static u64 sgMissCount = 0;
static u64 sgReplaceCount = 0;
static u64 sgAgeOutCount = 0;
static OmnString sgLogs[2];
static int sgLogIdx = 0;
static int sgLogMaxLength = 300;

#define ObjectCacherLogAndCheck(opr, docid) \
	if (sgLogs[sgLogIdx].length() > sgLogMaxLength) \
	{ \
		sgLogIdx = 1 - sgLogIdx; \
		sgLogs[sgLogIdx] = ""; \
	} \
	sgLogs[sgLogIdx] << opr << ":" << __LINE__ << ":" << docid << ","

#define ObjectCacherLogOnly(opr, docid)\
	sgLogs[sgLogIdx] << opr << ":" << __LINE__ << ":" << docid << ","

AosObjectCacher::AosObjectCacher(const u64 max_cached_objs)
:
mLock(OmnNew OmnMutex()),
mHead(0),
mMaxCachedObjs(max_cached_objs)
{
}


AosObjectCacher::~AosObjectCacher()
{
}


bool 
AosObjectCacher::start()
{
	return true;
}


bool 
AosObjectCacher::stop()
{
	return true;
}


bool 
AosObjectCacher::config(const AosXmlTagPtr &conf)
{
	if (mMaxCachedObjs == 0)
	{
		mMaxCachedObjs = conf->getAttrInt("max_cached_objects", eMaxCachedObjs);
		if (mMaxCachedObjs <= 0 || mMaxCachedObjs > eMaxCachedObjs)
		{
			OmnAlarm << "Invalid configuration: " << conf->toString() << enderr;
			mMaxCachedObjs = eMaxCachedObjs;
		}
	}
	return true;
}


AosCachedObjectPtr 
AosObjectCacher::getObjByDocid(
		const AosRundataPtr &rdata,
		const u64 docid)
{
	// 1 Read doc from map
	//   if the doc exsit ,return doc ,put the doc to front of the link list 
	//   else retreive the doc from file, and put it to front of the link list
	//   if the link list if full, it will remove the end docs of the list 
	u32 siteid = rdata->getSiteid();
	aos_assert_r(siteid, NULL);
	aos_assert_r(docid, NULL);
	mLock->lock();
	CachedObjKey dockey(docid, siteid);
	mapitr_t itr = mObjMap.find(dockey);
	if (itr != mObjMap.end())
	{
		// This means that there are entries that match 'docid'. 
		AosCachedObjectPtr node = itr->second;
		if (mObjMap.size() != 1)
		{
			// Move the doc to the front of the list. This will 
			// ensure that the doc will not be removed from 
			// the cache in the near future.
			moveNodeLocked(node);
		}

		sgHitCount++;
		mLock->unlock();
		return node;
	}

	sgMissCount++;
	mLock->unlock();
	return 0;
}


bool
AosObjectCacher::addObj(
		const AosRundataPtr &rdata,
		const AosCachedObjectPtr &obj, 
		const u64 docid)
{
	// This function adds the object 'obj' into the class. 
	// If the object already exists, it replaces it.
	aos_assert_rr(obj, rdata, false);
	aos_assert_rr(docid > 0, rdata, false);
	u32 siteid = rdata->getSiteid();
	aos_assert_rr(siteid > 0, rdata, false);

	mLock->lock();
	ObjectCacherLogAndCheck("add", docid);

	CachedObjKey dockey(docid, siteid);
	mapitr_t itr = mObjMap.find(dockey);
	if (itr != mObjMap.end())
	{
		// This means that the object exists. 
		ObjectCacherLogOnly("exist", docid);
		sgHitCount++;
		AosCachedObjectPtr node = itr->second; 
		if (node != obj)
		{
			// This is a new object for the same obj. Need to replace it.
			AosCachedObjectPtr next = node->getNextCachedObjDNO();
			AosCachedObjectPtr prev = node->getPrevCachedObjDNO();
			aos_assert_rl(prev, mLock, false);
			aos_assert_rl(next, mLock, false);
			obj->setNextCachedObjDNO(next);
			obj->setPrevCachedObjDNO(prev);
			prev->setNextCachedObjDNO(obj);
			next->setPrevCachedObjDNO(obj);
			itr->second = obj;
			sgReplaceCount++;
			if (node == mHead) mHead = obj;
		}

		if (mObjMap.size() > 1)
		{
			// Move the obj to the front of the list. This will 
			// ensure that the obj will not be removed from 
			// the cache in the near future.
			moveNodeLocked(obj);
		}
		mLock->unlock();
		return true;
	}

	// The obj does not exist. Add it.
	sgMissCount++;
	if (mObjMap.size() >= mMaxCachedObjs)
	{
		// Need to age out one obj 
		AosCachedObjectPtr node = mHead->getPrevCachedObjDNO();
		ObjectCacherLogOnly("aged", docid);
		aos_assert_r(node, false);
		u32 node_siteid = node->getCachedObjSiteid();
		CachedObjKey key(node->getCachedObjDocid(), node_siteid); 
		mObjMap.erase(key);
		removeNodeLocked(node);
		node->resetCachedObjLinks();

		sgAgeOutCount++;
	}

	if (mObjMap.size() == 0) 
	{
		ObjectCacherLogOnly("first", docid);
		mObjMap[dockey] = obj;
		mHead = obj;
		obj->setPrevCachedObjDNO(obj);
		obj->setNextCachedObjDNO(obj);
		mLock->unlock();
		return true;
	}

	// Place 'obj' to the front of the obj chains
	ObjectCacherLogOnly("added", docid);
	mObjMap[dockey] = obj;
	aos_assert_rl(mHead, mLock, false);
	obj->setNextCachedObjDNO(mHead);
	obj->setPrevCachedObjDNO(mHead->getPrevCachedObjDNO());
	mHead->setPrevCachedObjDNO(obj);
	obj->getPrevCachedObjDNO()->setNextCachedObjDNO(obj);
	mHead = obj;
	mLock->unlock();
	return obj;
}


bool
AosObjectCacher::removeObj(
		const AosRundataPtr &rdata, 
		const u64 docid)
{
	mLock->lock();
	ObjectCacherLogAndCheck("del", docid);
	u32 size = mObjMap.size();
	if (size == 0)
	{
		// The map is empty. Do nothing
		ObjectCacherLogOnly("empty", docid);
		aos_assert_rl(!mHead, mLock, false);
		mLock->unlock();
		return true;
	}

	aos_assert_rl(mHead, mLock, false);
	u32 siteid = rdata->getSiteid();
	CachedObjKey dockey(docid, siteid);
	mapitr_t itr = mObjMap.find(dockey);
	if (itr == mObjMap.end())
	{
		// Not in the map. Do nothing.
		ObjectCacherLogOnly("notfd", docid);
		mLock->unlock();
		return true;
	}

	AosCachedObjectPtr node = itr->second;
	mObjMap.erase(itr);
	if (size == 1)
	{
		// There are only one doc. 
		// Chen Ding, 2013/05/31
		node->resetCachedObjLinks();

		ObjectCacherLogOnly("one", docid);
		aos_assert_rl(mHead == node, mLock, false);
		mHead = 0;
		aos_assert_rl(mObjMap.size() == 0, mLock, false);
		
		mLock->unlock();
		return true;
	}

	ObjectCacherLogOnly("removed", docid);
	bool rslt = removeNodeLocked(node);
	node->resetCachedObjLinks();
	aos_assert_rl(rslt, mLock, false);
	aos_assert_rl(mObjMap.size() > 0, mLock, false);
	aos_assert_rl(mHead, mLock, false);
	mLock->unlock();
	return true;
}


bool
AosObjectCacher::removeNodeLocked(const AosCachedObjectPtr &node)
{
	// It assumes mObjMap.size() > 1. 
	node->getNextCachedObjDNO()->setPrevCachedObjDNO(node->getPrevCachedObjDNO());
	node->getPrevCachedObjDNO()->setNextCachedObjDNO(node->getNextCachedObjDNO());
	if (node == mHead)
	{
		mHead = node->getNextCachedObjDNO();
	}
	return true;
}


bool
AosObjectCacher::moveNodeLocked(const AosCachedObjectPtr &node)
{
	if (mHead == node) return true;	

	node->getPrevCachedObjDNO()->setNextCachedObjDNO(node->getNextCachedObjDNO());
	node->getNextCachedObjDNO()->setPrevCachedObjDNO(node->getPrevCachedObjDNO());
	node->setPrevCachedObjDNO(mHead->getPrevCachedObjDNO());
	mHead->getPrevCachedObjDNO()->setNextCachedObjDNO(node);
	node->setNextCachedObjDNO(mHead);
	mHead->setPrevCachedObjDNO(node);
	mHead = node;
	return true;
}


bool
AosObjectCacher::sanityCheck(const bool lock)
{
	if (lock) mLock->lock();
	if (mObjMap.size() == 0)
	{
		if (lock) 
		{
			aos_assert_rl(!mHead, mLock, false);
		}
		else
		{
			aos_assert_r(!mHead, false);
		}
		if (lock) mLock->unlock();
		return true;
	}

	if (lock)
	{
		aos_assert_rl(mHead, mLock, false);
	}
	else
	{
		aos_assert_r(mHead, false);
	}
	AosCachedObjectPtr node = mHead;
	u32 num_objs = 0;
	do
	{
		u64 docid = node->getCachedObjDocid();
		u32 siteid = node->getCachedObjSiteid();
		CachedObjKey dockey(docid, siteid);
		mapitr_t itr = mObjMap.find(dockey);
		if (lock)
		{
			aos_assert_rl(docid > 0, mLock, false)
			aos_assert_rl(siteid > 0, mLock, false)
			aos_assert_rl(itr != mObjMap.end(), mLock, false);
		}
		else
		{
			aos_assert_r(docid > 0, false)
			aos_assert_r(siteid > 0, false)
			aos_assert_r(itr != mObjMap.end(), false);
		}

		node = node->getNextCachedObjDNO();
		num_objs++;
	} 
	while (num_objs < mObjMap.size() && node != mHead);

	if (lock) 
	{
		aos_assert_rl(node == mHead, mLock, false);
		aos_assert_rl(num_objs == mObjMap.size(), mLock, false);
		mLock->unlock();
	}
	else
	{
		aos_assert_r(node == mHead, false);
		aos_assert_r(num_objs == mObjMap.size(), false);
	}
	return true;
}


OmnString
AosObjectCacher::getLogs()
{
	OmnString logs = sgLogs[sgLogIdx];
	logs << "---" << sgLogs[1-sgLogIdx];
	return logs;
}


AosCachedObjectPtr	
AosObjectCacher::getLastObj()
{
	return mHead->getPrevCachedObjDNO();
}


AosCachedObjectPtr	
AosObjectCacher::getFirstObj() const 
{
	return mHead;
}
