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
// 09/25/2012	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocidShufflerMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "DocClient/DocidShuffler.h"
#include "DocServer/DocSvr.h"
#include "IILUtil/IILId.h"
#include "SEUtil/DocFileMgr.h"
#include "SEUtilServer/SeIdGenMgr.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocClientCaller.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Thread/Ptrs.h"


OmnSingletonImpl(AosDocidShufflerMgrSingleton,
                 AosDocidShufflerMgr,
                 AosDocidShufflerMgrSelf,
                "AosDocidShufflerMgr");


AosDocidShufflerMgr::AosDocidShufflerMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosDocidShufflerMgr::~AosDocidShufflerMgr()
{
}


bool
AosDocidShufflerMgr::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosDocidShufflerMgr::stop()
{
    return true;
}


bool
AosDocidShufflerMgr::start()
{
	return true;
}


bool
AosDocidShufflerMgr::sendStart(
		const OmnString &scanner_id,
		const u64 &block_size,
		const AosXmlTagPtr &field_names,
		const AosRundataPtr &rdata)
{
	return sendStart(scanner_id, 2, "allclientready", block_size, "read", field_names, rdata);
}


bool
AosDocidShufflerMgr::sendStart(
		const OmnString &scanner_id,
		const u64 &block_size,
		const AosRundataPtr &rdata)
{
	return sendStart(scanner_id, 2, "allclientready", block_size, "read", 0, rdata);
}

bool
AosDocidShufflerMgr::sendStart(
		const OmnString &scanner_id, 
		const u32 &queue_size,
		const OmnString &read_policy,
		const u64 &block_size,
		const OmnString &batch_type,
		const AosXmlTagPtr &field_names,
		const AosRundataPtr &rdata)
{
	mLock->lock();
	AosDocidShufflerPtr ptr;
	map<OmnString, AosDocidShufflerPtr>::iterator itr = mScanner.find(scanner_id);
	if (itr == mScanner.end())
	{
		ptr = OmnNew AosDocidShuffler(scanner_id, rdata);
		mScanner.insert(make_pair(scanner_id,  ptr));
	}
	aos_assert_rl(ptr, mLock, false);
	ptr->sendStart(queue_size, read_policy, block_size, batch_type, field_names, rdata);
	mLock->unlock();
	return true;
}


bool
AosDocidShufflerMgr::sendFinished(
		const OmnString &scanner_id, 
		const AosRundataPtr &rdata)
{
	mLock->lock();
	aos_assert_rl(mScanner.count(scanner_id) != 0, mLock, false);
	AosDocidShufflerPtr ptr = mScanner[scanner_id];
	aos_assert_rl(ptr, mLock, false);

	bool rslt = ptr->sendFinished(rdata);
	mScanner.erase(scanner_id);
	mLock->unlock();
	return rslt;
}


bool
AosDocidShufflerMgr::shuffle(
		const OmnString &scanner_id,
		const AosBitmapObjPtr &bitmap,
		const int num_thrds,
		const AosDocClientCallerPtr &caller,
		const AosRundataPtr &rdata)
{
	aos_assert_r(scanner_id != "", false);
	aos_assert_r(bitmap && !bitmap->isEmpty(), false);

	mLock->lock();
	bitmap->reset();
	aos_assert_rl(mScanner.count(scanner_id) != 0, mLock, false);
	AosDocidShufflerPtr ptr = mScanner[scanner_id];
	aos_assert_rl(ptr, mLock, false);
	bool rslt = ptr->shuffle(bitmap, num_thrds, caller, rdata);
	mLock->unlock();
	return rslt;
}

bool
AosDocidShufflerMgr::shuffle(
		const OmnString &scanner_id,
		const AosBitmapObjPtr &bitmap,
		const int num_thrds,
		const AosRundataPtr &rdata)
{
	return shuffle(scanner_id, bitmap, num_thrds, 0, rdata);
}

