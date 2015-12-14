////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 2014/08/23 Copied from IILCacheEngine by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IILCache_IILCacheEngine_h
#define AOS_IILCache_IILCacheEngine_h

#include "SEInterfaces/IILCacheEngineObj.h"
#include "Util1/Time.h"


class AosIILCacheEngine : public AosIILCacheEngineObj 
{
	OmnDefineRCObject;

	enum
	{
		eMaxThreads = 50,
	};

	AosIILCacheEngineCallerPtr	mCaller;
	IDMap					mIDMap;
	list<IDMapItr>			mQueue;
	OmnMutexPtr				mLock;
	bool					mIsStopping;

	u32						mSingleSize;
	u32						mQueueSize;
	u32						mProcQueueSize;
	u32						mMaxTransNum;
	u32						mBatchSize;
	bool					mShowLog;
	
	u32						mTotalTransNum;

public:
	AosIILCacheEngine(
			const AosIILCacheEngineCallerPtr &obj,
			const AosXmlTagPtr &xml);
	~AosIILCacheEngine();
	
	virtual bool addTrans(
					const u64 &id,
					const u32 siteid,
					const AosIILTransPtr &trans,
					const AosRundataPtr &rdata);

	virtual bool procAllTrans(const AosRundataPtr &rdata);
	virtual bool clearIDTransVector(const AosIDTransVectorPtr &p);
	virtual bool checkNeedSave(const u32 &num_trans);
	virtual bool needProcQueue();
	virtual bool needProcTrans();
	virtual bool procTrans(const AosRundataPtr &rdata);
	virtual bool procQueue(const AosRundataPtr &rdata);
	virtual bool procQueues(const AosRundataPtr &rdata);
	virtual bool isFree();
	virtual bool postProc(const AosRundataPtr &rdata);
	virtual void setIsStopping(){mIsStopping = true;}
	virtual void cleanCache();

	virtual bool getVectorById(
					const u64 &id,
					const u32 siteid,
					AosIDTransVectorPtr &p);

	virtual bool cleanBkpVirtual(
					const AosRundataPtr &rdata,
					const u32 virtual_id);

	virtual bool cleanCache(
					const u32 virtual_id, 
					const AosRundataPtr &rdata);
};	
#endif

 
