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
// 01/06/2013 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_AppIdGens_AppIdGenU64_h
#define AOS_AppIdGens_AppIdGenU64_h

#include "AppIdGens/Ptrs.h"
#include "Rundata/Ptrs.h"
#include "Thread/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "Thread/ThreadPool.h"
#include "Thread/ThrdShellProc.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "XmlUtil/Ptrs.h"
#include <vector>
using namespace std;


class AosAppIdGenU64Thrd : public OmnThrdShellProc
{
	OmnDefineRCObject;
	AosAppIdGenU64Ptr   	mIDGen;
	AosRundataPtr  		mRundata;
public:
	AosAppIdGenU64Thrd(
			const AosAppIdGenU64Ptr   idgen,
			const AosRundataPtr &rdata);
	~AosAppIdGenU64Thrd();

	virtual bool    run();
	virtual bool    procFinished(){return true;}
	virtual bool    waitUntilFinished(){return true;}
};


class AosAppIdGenU64 : virtual public OmnRCObject,
						public OmnThreadedObj
{
	OmnDefineRCObject;

	enum
	{
		eDftBlockSize = 100,
		eDftInitValue = 1000
	};

protected:
	OmnMutexPtr		mLock;
	OmnString		mObjid;
	vector<u64>		mCrtId;
	vector<u64>		mNextId;
	u64				mNoRegionCrtId;
	u64				mNoRegionNextId;
	u64				mBlockSize;
	u64				mInitValue;
	u64				mMaxId;
	u64				mPrefix;
	OmnString		mIILName;
	int				mNumRegions;
	OmnThreadPtr    mThread;
	AosRundataPtr	mRundata;
	bool				mThreadRunning;
public:
	AosAppIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid);
	AosAppIdGenU64(
		const AosRundataPtr &rdata,
		const OmnString &objid, 
		const u64 &block_size,
		const u64 &init_value,
		const u64 &max_id,
		const u64 &prefix);
	~AosAppIdGenU64();

	bool	start(const AosRundataPtr &rdata);
	u64		getNextId(const AosRundataPtr &rdata, const u32 &region_id);
	u64		getNextId(const AosRundataPtr &rdata);
	// u64		getCrtId() const {return mCrtId;}

	virtual OmnString getIILName() const;


	virtual bool    proc();

	static OmnThreadPoolPtr	smThreadPool;

	// ThreadedObj interface
	virtual bool    threadFunc(OmnThrdStatus::E &state, const OmnThreadPtr &thread);
	virtual bool    signal(const int threadLogicId);
};

#endif
