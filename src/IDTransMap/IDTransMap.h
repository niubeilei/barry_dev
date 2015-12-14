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
// 	Created: 2011/08/19 by Ken Lee
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_IDTransMap_IDTransMap_h
#define AOS_IDTransMap_IDTransMap_h 

#include "SEInterfaces/IDTransMapObj.h"
#include "Util1/Time.h"

//#include <queue>
#include <list>

class AosIDTransMap : public AosIDTransMapObj 
{
	OmnDefineRCObject;

	enum
	{
		eMaxThreads = 50,
	};

	AosIDTransMapCallerPtr	mCaller;
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
	AosIDTransMap(
			const AosIDTransMapCallerPtr &obj,
			const AosXmlTagPtr &xml);
	~AosIDTransMap();
	
	virtual bool addTrans(
					const u64 &id,
					const u32 siteid,
					const AosIILTransPtr &trans,
					const AosRundataPtr &rdata);

	virtual bool procAllTrans(const AosRundataPtr &rdata);
	virtual bool clearIDTransVector(const AosIDTransVectorPtr &p);
	virtual bool getVectorById(
					const u64 &id,
					const u32 siteid,
					AosIDTransVectorPtr &p);
	virtual bool checkNeedSave(const u32 &num_trans);
	virtual bool needProcQueue();
	virtual bool needProcTrans();
	virtual bool procTrans(const AosRundataPtr &rdata);
	virtual bool procQueue(const AosRundataPtr &rdata);
	virtual bool procQueues(const AosRundataPtr &rdata);
	virtual bool isFree();
	virtual bool postProc(const AosRundataPtr &rdata);

	// Ketty 2013/01/30
	virtual void setIsStopping(){mIsStopping = true;}
	virtual bool cleanBkpVirtual(
					const AosRundataPtr &rdata,
					const u32 virtual_id);

	// Ken Lee, 2013/05/17
	virtual void cleanCache();

	// Ken Lee, 2013/06/06
	virtual bool cleanCache(
					const u32 virtual_id, 
					const AosRundataPtr &rdata);
};	
#endif

 
