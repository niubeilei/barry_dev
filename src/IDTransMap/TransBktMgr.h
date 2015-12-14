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
// 10/25/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IDTransMap_TransBktMgr_h
#define AOS_IDTransMap_TransBktMgr_h
/*
#include "IDTransMap/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadedObj.h"
#include "Util/Ptrs.h"
#include "Util/HashUtil.h"
#include <hash_map>
#include <map>

OmnDefineSingletonClass(AosTransBktMgrSingleton,
						AosTransBktMgr,
						AosTransBktMgrSelf,
						OmnSingletonObjId::eTransBktMgr,
						"TransBktMgr");

class AosTransBktMgr : virtual public OmnThreadedObj
{
	OmnDefineRCObject;

private:
	typedef hash_map<u64, AosTransBucketPtr, AosU64Hash1> TransMap_t;
	typedef hash_map<u64, AosTransBucketPtr, AosU64Hash1>::iterator TransMapItr_t;

	enum
	{
		eMaxBuckets = 10000,
		eDftMaxBuckets = 4000,
		eDftNumIILsPerBucket = 10000,
		eUpdateSize = 100
	};

	OmnMutexPtr			mLock;
	TransMap_t			mTransMap;
	u32					mCounts[eMaxBuckets];
	int					mNumBuckets;
	int					mMaxBuckets;
	int					mNumIILsPerBucket;
	AosTransBucketPtr	mBucketHead;
	OmnString 			mFname;
	map<u32, OmnFilePtr> mFiles;

public:
	AosTransBktMgr();
	~AosTransBktMgr();

    // OmnThreadedObj Interface
    virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
    virtual bool    signal(const int threadLogicId);
    virtual bool    checkThread(OmnString &err, const int thrdLogicId) const;
	
    // Singleton class interface
    static AosTransBktMgr *	getSelf();
    virtual bool      	start();
    virtual bool        stop();
	virtual bool        config(const AosXmlTagPtr &def);
	bool 	start(const AosXmlTagPtr &config);

	bool addTrans( 	const u64 &iilid,
					const u32 siteid,
					const AosIDTransVectorPtr &transes,
					const AosRundataPtr &rdata);
	bool procTrans( const u64 &iilid, 
					const u32 siteid,
					const AosIDTransVectorPtr &transes,
					const AosRundataPtr &rdata);
	u64  getNextBlock(OmnFilePtr &file);
	bool procAllTrans(const AosRundataPtr &rdata);
	bool procOneTrans(bool &trans_processed,const AosRundataPtr &rdata);
	bool procTransWithCheck(const AosIDTransMapObjPtr &idTransMap, bool &trans_processed,const AosRundataPtr &rdata);

private:
	AosTransBucketPtr getBucket(const u64 &start_iilid, const u32 siteid, const AosRundataPtr &rdata);
	OmnFilePtr 	getBucketFile(const u32 siteid);
	inline bool getStartIILID(const u64 &iilid, u64 &start_iilid) const
	{
		aos_assert_r(mNumIILsPerBucket > 0, false);
		start_iilid = iilid / mNumIILsPerBucket * mNumIILsPerBucket;
		return true;
	}
	bool loadBucket(const u64 &start_iilid, const u32 siteid, const AosRundataPtr &rdata);
	bool moveBucket(const AosTransBucketPtr &bucket);
	bool addBucket(const AosTransBucketPtr &bucket);
};
*/
#endif
