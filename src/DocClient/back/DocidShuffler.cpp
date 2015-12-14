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
// 09/20/2012 Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocClient/DocidShuffler.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/Ptrs.h"
#include "SEInterfaces/DocClientCaller.h"
#include "SEInterfaces/BitmapMgrObj.h"
#include "SEInterfaces/QueryContextObj.h"
#include "Thread/Mutex.h"
#include "Thread/ThreadPool.h"
#include "XmlUtil/XmlDoc.h"


static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("docid_shuffler_mgr", __FILE__, __LINE__);


AosDocidShuffler::AosDocidShuffler(
		const OmnString &scanner_id, 
		const AosRundataPtr &rdata)
:
mScannerId(scanner_id),
//mNumPhysicals(AosGetNumPhysicals()),
mLock(OmnNew OmnMutex())
{
	vector<u32> total_vids = AosGetTotalCubeIds(); 
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		mTotalNumDataMsgs.push_back(0);
	}
}


AosDocidShuffler::~AosDocidShuffler()
{
}


bool
AosDocidShuffler::sendStart(
		const u32 &queue_size,
		const OmnString &read_policy,
		const u64 &block_size,
		const OmnString &batch_type,
		const AosXmlTagPtr &field_names,
		const AosRundataPtr &rdata)
{
	//Linda 2013/09/22
	OmnNotImplementedYet;
	return true;
	//bool rslt = false;
	//for (int serverid = 0; serverid < mNumPhysicals; serverid++)
	//{
	//	rslt = AosDocClient::getSelf()->batchSendStart(
	//			serverid, mScannerId, queue_size, read_policy, 
	//			block_size, batch_type, field_names, rdata);
	//	aos_assert_r(rslt, false);
	//}
	//return true;
}


bool
AosDocidShuffler::sendFinished(const AosRundataPtr &rdata)
{
	//Linda 2013/09/22
	OmnNotImplementedYet;
	return true;
	//bool rslt = false;
	//for (int serverid = 0; serverid < mNumPhysicals; serverid++)
	//{
	//	rslt = AosDocClient::getSelf()->batchSendFinished(
	//			serverid, mScannerId, mTotalNumDataMsgs[serverid], rdata);
	//	aos_assert_r(rslt, false);
	//}
	//return true;
}


bool
AosDocidShuffler::shuffle(
		const AosBitmapObjPtr &bitmap,
		const int numthrds,
		const AosDocClientCallerPtr &caller,
		const AosRundataPtr &rdata)
{
	//Linda 2013/09/22
	OmnNotImplementedYet;
	return true;
	//int num_thrds = eDftNumThreads;
	//if (numthrds > 0) num_thrds = numthrds; 
	//
	//bitmap->reset();
	//AosDocidShufflerPtr thisptr(this, false);
	//vector<OmnThrdShellProcPtr> runners;
	//int64_t start_idx = 0;
	//int64_t psize  = 0;
	//for (int i = 0; i < num_thrds; i++)
	//{
	//	OmnThrdShellProcPtr runner = OmnNew ShuffleRunner(
	//			thisptr, bitmap, mScannerId, start_idx, psize, caller, 
	//			mNumPhysicals, rdata);
	//	runners.push_back(runner);
	//	start_idx += psize;
	//}
	//
	//aos_assert_r(sgThreadPool, false);
	//return sgThreadPool->procSync(runners);
}


void
AosDocidShuffler::updateTotalMsgs(const int serverid, const int vv)
{
	aos_assert(serverid >= 0 && (u32)serverid < mTotalNumDataMsgs.size());
	mLock->lock();
	mTotalNumDataMsgs[serverid] += vv;
	mLock->unlock();
}


bool
AosDocidShuffler::ShuffleRunner::run()
{
	//u64 docid; 
	//int serverid;
	//while ((docid = mBitmap->nextDocid()))
	//{
	//	serverid = AosGetPhysicalId(AosXmlDoc::getOwnDocid(docid));
	//	mBitmaps[serverid]->appendDocid(docid);
	//}
	//
	//bool rslt = false;
	//for (serverid = 0; serverid < mNumPhysicals; serverid++)
	//{
	//	if (mBitmaps[serverid]->isEmpty()) continue;
	//	if (mCaller)
	//	{
	//		rslt = AosDocClient::getSelf()->batchSendDocids(
	//				mScannerId, serverid, mBitmaps[serverid], mCaller, mRundata);
	//	}
	//	else
	//	{
	//		rslt = AosDocClient::getSelf()->batchSendDocids(
	//				mScannerId, serverid, mBitmaps[serverid], mRundata);
	//	}
	//	mShuffler->updateTotalMsgs(serverid, 1);
	//	aos_assert_r(rslt, false);
	//}
	u64 docid; 
	int vid;
	while ((docid = mBitmap->nextDocid()))
	{
		vid = AosGetCubeId(AosXmlDoc::getOwnDocid(docid));
		mBitmaps[vid]->appendDocid(docid);
	}
	
	bool rslt = false;
	vector<u32> total_vids = AosGetTotalCubeIds();  
	for (u32 i = 0; i<total_vids.size(); i++)
	{
		u32 vid = total_vids[i];
		if (mBitmaps[vid]->isEmpty()) continue;
		if (mCaller)
		{
			rslt = AosDocClientObj::getDocClient()->batchSendDocids(
					mScannerId, vid, mBitmaps[vid], mCaller, mRundata);
		}
		else
		{
			rslt = AosDocClientObj::getDocClient()->batchSendDocids(
					mScannerId, vid, mBitmaps[vid], mRundata);
		}
		mShuffler->updateTotalMsgs(vid, 1);
		aos_assert_r(rslt, false);
	}
	return true;
}


AosDocidShuffler::ShuffleRunner::ShuffleRunner(
		const AosDocidShufflerPtr &shuffle,
		const AosBitmapObjPtr &bitmap,
		const OmnString &scanner_id,
		const int64_t &start_idx,
		const int64_t &psize,
		const AosDocClientCallerPtr &caller,
		const int num_physicals,
		const AosRundataPtr &rdata)
:
OmnThrdShellProc("docidShuffler"),
mShuffler(shuffle),
mBitmap(bitmap),
mScannerId(scanner_id),
mStartIdx(start_idx),
mPsize(psize),
mCaller(caller),
mNumPhysicals(num_physicals),
mRundata(rdata->clone(AosMemoryCheckerArgsBegin))
{
	for (int i = 0; i< mNumPhysicals; i++) 
	{                          
		mBitmaps.push_back(AosBitmapMgrObj::getBitmapStatic());
	}
}


AosDocidShuffler::ShuffleRunner::~ShuffleRunner()
{

}

bool
AosDocidShuffler::ShuffleRunner::procFinished()
{
	return true;
}
