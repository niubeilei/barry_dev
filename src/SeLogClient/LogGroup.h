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
// 10/31/2013	Create by Young
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeLogClient_LogGroup_h
#define AOS_SeLogClient_LogGroup_h

#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Rundata/Rundata.h"
#include "Rundata/Ptrs.h"
#include "SeLogClient/Ptrs.h"
#include "SEInterfaces/SeLogClientObj.h"
#include "SEInterfaces/Ptrs.h"
#include "Thread/ThreadedObj.h"
#include "TransClient/Ptrs.h"
#include "TransUtil/RoundRobin.h"
#include "SeLogUtil/LogOpr.h"

class AosLogGroup : virtual public OmnRCObject
{
	OmnDefineRCObject;

private:
	u32 			mEntryNums;
	u32				mCrtEntryNums;
	u64 			mGroupId;
	u64 			mFirstLogid;
	AosBuffPtr		mContentsBuff;
	u64 			mCreatorTime;
	OmnMutexPtr     mLock;
	OmnMutex *    	mLockRaw;

		
public:
	AosLogGroup(const AosRundataPtr &rdata, const u32 &entry_nums);
	~AosLogGroup();
	
	bool 		init(const AosRundataPtr &rdata);
	bool 		isFull() { return mCrtEntryNums >= mEntryNums; }
	AosBuffPtr 	getBuff(const AosRundataPtr &rdata);
	u64 		getDocid() { return mGroupId; }
	u64   		getCreatorTime() { return mCreatorTime; }
	u64 		getGroupSize() { return mContentsBuff->dataLen(); }
	u64  		putLogIntoBuff(
					const u64 &createTime,
					const OmnString &log);

private:
	u64		getFirstLogid(const u64 groupid);
};
#endif

