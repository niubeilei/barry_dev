
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
#include "SeLogClient/LogGroup.h"

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
#include "SEInterfaces/DocidMgrObj.h"
#include "XmlUtil/XmlDoc.h"

AosLogGroup::AosLogGroup(const AosRundataPtr &rdata, const u32 &entry_nums)
:
mEntryNums(entry_nums),
mCrtEntryNums(0),
mGroupId(0),
mLock(OmnNew OmnMutex()),
mLockRaw(mLock.getPtr())
{
	init(rdata);
}


AosLogGroup::~AosLogGroup()
{
}


bool 
AosLogGroup::init(const AosRundataPtr &rdata)
{
	mLockRaw->lock();
	mCrtEntryNums = 0;
	mGroupId = AosDocidMgrObj::getDocidMgr()->nextLogGroupDocid(rdata);
	mFirstLogid = getFirstLogid(mGroupId);
	aos_assert_rl(mGroupId > 0 && mFirstLogid > 0, mLockRaw, false);
	mContentsBuff = OmnNew AosBuff(100 AosMemoryCheckerArgs);
	mLockRaw->unlock();
	return true;
}


AosBuffPtr 	
AosLogGroup::getBuff(const AosRundataPtr &rdata)
{
	AosBuffPtr buff = mContentsBuff->clone(AosMemoryCheckerArgsBegin); 
	aos_assert_r(buff, NULL);
	init(rdata);

	return buff;
}


u64 
AosLogGroup::putLogIntoBuff(
		const u64& logCreateTime,
		const OmnString &log)
{
	mLockRaw->lock();
	u64 crtLogid = mFirstLogid + mCrtEntryNums;
	mCrtEntryNums++;

	aos_assert_rl(mCrtEntryNums > 0 && mCrtEntryNums <= mEntryNums, mLockRaw, 0);
	u32 metaLen = sizeof(u64) + sizeof(int) + log.length();

	mContentsBuff->setU32(metaLen);
	mContentsBuff->setU64(logCreateTime);
	mContentsBuff->setOmnStr(log);

	if (1 == mCrtEntryNums) mCreatorTime = logCreateTime;
	mLockRaw->unlock();
	
	return crtLogid;
}


u64
AosLogGroup::getFirstLogid(const u64 groupid)
{
	AosDocType::E type = AosDocType::getDocType(groupid);
	//u64 tmp = (groupid & 0x00FFFFFFFFFFFFFF) * mEntryNums;
	u64 logid = AosXmlDoc::getOwnDocid(groupid);
	logid = logid * mEntryNums;
	AosDocType::setDocidType(type, logid);
	return logid;
}
