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
// 10/25/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IDTransMap/TransBktMgr.h"
/*
#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "IDTransMap/IDTransMap.h"
#include "IDTransMap/TransBucket.h"
#include "SEUtil/Siteid.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/Mutex.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"


static OmnString sgFilename = "transbkt";

OmnSingletonImpl(AosTransBktMgrSingleton,
                 AosTransBktMgr,
                 AosTransBktMgrSelf,
                "AosTransBktMgr");

AosTransBktMgr::AosTransBktMgr()
:
mLock(OmnNew OmnMutex()),
mNumBuckets(0),
mMaxBuckets(eDftMaxBuckets),
mNumIILsPerBucket(eDftNumIILsPerBucket)
{
	memset(mCounts, 0, sizeof(mCounts));
}


AosTransBktMgr::~AosTransBktMgr()
{
}


bool
AosTransBktMgr::threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread){
	return true;
}


bool
AosTransBktMgr::signal(const int threadLogicId)
{
	return true;
}


bool
AosTransBktMgr::checkThread(OmnString &err, const int thrdLogicId) const
{
	return true;
}


bool
AosTransBktMgr::start()
{
	return true;
}

	
bool
AosTransBktMgr::stop()
{
    return true;
}


bool
AosTransBktMgr::start(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosTransBktMgr::config(const AosXmlTagPtr &config)
{
	aos_assert_r(config, false);
	
	AosXmlTagPtr conf = config->getFirstChild("transbktmgr");
	mFname << sgFilename;
	if (conf)
	{
		OmnString dirname = conf->getAttrStr("dirname");
		if (dirname != "") 
		{
			mFname = dirname;
			mFname << "/";
		}
		else
		{
			mFname = "";
		}

		OmnString ff = conf->getAttrStr("filename");
		if (ff == "") ff = sgFilename;
		mFname << ff;
		mNumIILsPerBucket = conf->getAttrInt("iils_per_bucket", eDftNumIILsPerBucket);
		if (mNumIILsPerBucket <= 0)
		{
			OmnAlarm << "Invalid number of iils per bucket: " << mNumIILsPerBucket << enderr;
			mNumIILsPerBucket = eDftNumIILsPerBucket;
		}

		mMaxBuckets = conf->getAttrInt("max_buckets", eDftMaxBuckets);
		if (mMaxBuckets <= 0)
		{
			OmnAlarm << "Invalid max buckets: " << mMaxBuckets << enderr;
			mMaxBuckets = eDftMaxBuckets;
		}
	}

	AosTransBucket::config(eDftNumIILsPerBucket, mMaxBuckets, 0xfff0, 0x400); //64k, 1k
	return true;
}


bool 
AosTransBktMgr::loadBucket(const u64 &start_iilid, const u32 siteid, const AosRundataPtr &rdata)
{
	// 1. Find the one that is used the least.
	aos_assert_rr(mBucketHead, rdata, false);
	AosTransBucketPtr bucket = mBucketHead->mPrev;
	aos_assert_rr(bucket, rdata, false);
	aos_assert_rr(bucket != mBucketHead, rdata, false);

	mTransMap.erase(AosSiteid::combineSiteid(bucket->getID(), siteid));

	OmnFilePtr ff = getBucketFile(siteid);
	aos_assert_r(ff, NULL);
	bucket->readFromFile(start_iilid, ff ,rdata);
	moveBucket(bucket);
	return true;
}


AosTransBucketPtr 
AosTransBktMgr::getBucket(const u64 &iilid, const u32 siteid, const AosRundataPtr &rdata)
{
	aos_assert_rr(siteid > 0, rdata, 0);

	u64 start_iilid = 0;
	bool rslt = getStartIILID(iilid, start_iilid);
	aos_assert_rr(rslt, rdata, 0);
	mLock->lock();
	u64 key = AosSiteid::combineSiteid(start_iilid, siteid);
	TransMapItr_t itr = mTransMap.find(key);
	AosTransBucketPtr bucket;
	if (itr == mTransMap.end())
	{
		// Did not find it. Check whether need to create it.
		if (mNumBuckets > mMaxBuckets)
		{
			// Need to pop one.
			loadBucket(start_iilid, siteid, rdata);
			mLock->unlock();
			return NULL;
		}
		else
		{
			OmnFilePtr ff = getBucketFile(siteid);
			aos_assert_r(ff, NULL);
			bucket = OmnNew AosTransBucket(start_iilid, ff, rdata);
			mTransMap[key] = bucket;
			addBucket(bucket);
		}
	}
	else
	{
		bucket = itr->second;
	}
	u64 count = bucket->addCount();
	if ((count % eUpdateSize) == 0)
	{
		moveBucket(bucket);
	}
	mLock->unlock();
	return bucket;
}


OmnFilePtr
AosTransBktMgr::getBucketFile(const u32 siteid)
{
	map<u32, OmnFilePtr>::iterator itr = mFiles.find(siteid);
	if (itr == mFiles.end())
	{
		OmnString fname = mFname;
		fname << "_" << siteid;
		
		// Ketty 2013/05/16
		OmnString full_fname = OmnApp::getAppBaseDir();
		full_fname << fname;
		//OmnFilePtr ff = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		OmnFilePtr ff = OmnNew OmnFile(full_fname, OmnFile::eCreate AosMemoryCheckerArgs);
		aos_assert_r(ff && ff->isGood(), NULL);
		mFiles.insert(make_pair(siteid, ff));
		return ff;
	}
	else
	{
		return itr->second;
	}
}


bool
AosTransBktMgr::addBucket(const AosTransBucketPtr &bucket)
{
	if (!mBucketHead)
	{
		mBucketHead = bucket;
		bucket->mPrev = bucket;
		bucket->mNext = bucket;
		return true;
	}

	bucket->mNext = mBucketHead;
	bucket->mPrev = mBucketHead->mPrev;
	mBucketHead->mPrev->mNext = bucket;
	mBucketHead->mPrev = bucket;
	mBucketHead = bucket;
	return true;
}

		
bool
AosTransBktMgr::moveBucket(const AosTransBucketPtr &bucket)
{
	// this funciton moves 'bucket' to the head
	if (mBucketHead == bucket) return true;

	bucket->mPrev->mNext = bucket->mNext;		// remove 'bucket' from the list
	bucket->mNext->mPrev = bucket->mPrev;		// remove 'bucket' from the list

	bucket->mPrev = mBucketHead->mPrev;			// bucket prev points to head prev
	mBucketHead->mPrev->mNext = bucket;			// head prev's next points to bucket

	bucket->mNext = mBucketHead;				// bucket next points to head's next
	mBucketHead->mPrev = bucket;				// bucket prev points to bucket

	mBucketHead = bucket;
	return true;
}


bool 
AosTransBktMgr::addTrans(  
		const u64 &iilid,
		const u32 siteid,
		const AosIDTransVectorPtr &transes,
		const AosRundataPtr &rdata)
{
	AosTransBucketPtr bucket = getBucket(iilid, siteid, rdata);
	aos_assert_rr(bucket, rdata, false);
	return bucket->addTrans(iilid, transes, rdata);
}


bool 
AosTransBktMgr::procTrans(
		const u64 &iilid,
		const u32 siteid,
		const AosIDTransVectorPtr &transes,
		const AosRundataPtr &rdata)
{
	AosTransBucketPtr bucket = getBucket(iilid, siteid, rdata);
	aos_assert_rr(bucket, rdata, false);
	return bucket->procTrans(iilid, transes, rdata);
}


bool 
AosTransBktMgr::procAllTrans(const AosRundataPtr &rdata)
{
	map<u32, OmnFilePtr>::iterator itr;
	for (itr = mFiles.begin(); itr != mFiles.end(); ++itr)
	{
		for (int i=0; i<mMaxBuckets*mNumIILsPerBucket; i+= mNumIILsPerBucket)
		{
			//OmnScreen << "Proc Bucket: start id " << i << endl;
			AosTransBucketPtr bucket = getBucket(i, itr->first, rdata);
			aos_assert_r(bucket, false);
			bucket->procAllTrans(itr->first, rdata);
		}
	}
	return true;
}

bool 
AosTransBktMgr::procOneTrans(bool &trans_processed,const AosRundataPtr &rdata)
{
	trans_processed = false;
	map<u32, OmnFilePtr>::iterator itr;
	for (itr = mFiles.begin(); itr != mFiles.end(); ++itr)
	{
		for (int i=0; i<mMaxBuckets*mNumIILsPerBucket; i+= mNumIILsPerBucket)
		{
			//OmnScreen << "Proc Bucket: start id " << i << endl;
			AosTransBucketPtr bucket = getBucket(i, itr->first, rdata);
			aos_assert_r(bucket, false);
			bucket->procOneTrans(trans_processed, itr->first, rdata);
			if(trans_processed)return true;
		}
	}
	return true;	
}

bool 
AosTransBktMgr::procTransWithCheck(const AosIDTransMapObjPtr &idTransMap, bool &trans_processed,const AosRundataPtr &rdata)
{
	trans_processed = false;
	map<u32, OmnFilePtr>::iterator itr;
	for (itr = mFiles.begin(); itr != mFiles.end(); ++itr)
	{
		for (int i=0; i<mMaxBuckets*mNumIILsPerBucket; i+= mNumIILsPerBucket)
		{
			//OmnScreen << "Proc Bucket: start id " << i << endl;
			AosTransBucketPtr bucket = getBucket(i, itr->first, rdata);
			aos_assert_r(bucket, false);
			bucket->procOneTrans(trans_processed, itr->first, rdata);
			if(!trans_processed)return true;
			if(idTransMap->needProcQueue() || 
			   (!idTransMap->isFree()))
			{
				return true;
			}
		}
	}
	return true;	
}
*/
