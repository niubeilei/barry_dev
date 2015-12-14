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
//
// Modification History:
// 2015-05-07 Jozhi 
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/SengineDocInfoCommon.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "DocTrans/BatchInsertCommonDocTrans.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"

AosSengineDocInfoCommon::AosSengineDocInfoCommon(
		const AosGroupDocOpr::E opr,
		const int vid, 
		const u64 &snap_id,
		const u64 &task_docid)
:
mLock(OmnNew OmnMutex()),
mVid(vid),
mDocSize(-1),
mNumDocs(0),
mCrtSizeid(0),
mMaxBuffLen(0),
mSem(OmnNew OmnSem(0)),
mTotalReqs(0),
mTempPost(0),
mSnapId(snap_id),
mTaskDocid(task_docid),
mSvrDeath(false),
mOpr(opr)
{
}


AosSengineDocInfoCommon::~AosSengineDocInfoCommon()
{
}
		

bool
AosSengineDocInfoCommon::createMemory(
		const u64 &buff_len,
		const int doc_size)
{
	aos_assert_r(buff_len > 0, false);
	mLock->lock();
	mDocSize = doc_size;
	mMaxBuffLen = buff_len;
	mBuff = OmnNew AosBuff(buff_len + 200 AosMemoryCheckerArgs);
	mOffsetMap.clear();

	mLock->unlock();
	return true;
}

bool 
AosSengineDocInfoCommon::addDoc(
		const u64 &docid,
		const int doc_len,
		const u64 &schema_docid,
		const char *doc, 
		const AosRundataPtr &rdata)
{
	// Docs are buff-ed based on virtual_id. Docs belonging to the same
	// virtual ID are saved in the same buff.
	aos_assert_r(docid, false);
	u64 sizeid = AosGetSizeIdByDocid(docid);
	aos_assert_r(sizeid, false);

	static int lsNumVirtuals = AosGetNumCubes();
	u64 local_docid = AosGetLocalIdByDocid(docid) / lsNumVirtuals;

	mLock->lock();
	if (mCrtSizeid != sizeid || mBuff->dataLen() > (int64_t)mMaxBuffLen)
	{
		// Need to send the transaction to DocServer.
		bool rslt = sendRequestPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	mOffsetMap[local_docid] = mBuff->getCrtIdx();
	mBuff->setU64(local_docid);
	mBuff->setInt(doc_len);
	mBuff->setU64(schema_docid);
	mBuff->setBuff(doc, doc_len);
	mCrtSizeid = sizeid;
	mNumDocs++;

	mLock->unlock();
	return true;
}


bool
AosSengineDocInfoCommon::sendRequestPriv(const AosRundataPtr &rdata) 
{
	if (mNumDocs <= 0)
	{
		return true;
	}
	AosSengineDocInfoPtr thisptr(this, false);
	int64_t length = mBuff->dataLen();
	int64_t expect_size = length + mNumDocs * sizeof(u64) * 2 + 20;
	mBuff->resize(expect_size);
	switch(mOpr)
	{
	case AosGroupDocOpr::eBatchInsert:
	{
		aos_assert_r(mOffsetMap.size() == mNumDocs, false);
		map<u64, u64>::iterator itr = mOffsetMap.begin();
		while (itr != mOffsetMap.end())
		{
			mBuff->setU64(itr->first);
			mBuff->setU64(itr->second);
			itr++;
		}
		aos_assert_r(mDocSize > 0, false);
		AosTransPtr trans = OmnNew AosBatchInsertCommonDocTrans(thisptr, mVid, 
				mCrtSizeid, mNumDocs, mDocSize, length, mBuff, mSnapId, mTaskDocid);
		AosSendTransAsyncResp(rdata, trans);
		mTotalReqs++;
		mOffsetMap.clear();
		break;
	}
	default:
	break;
	}
	mNumDocs = 0;
	mBuff = OmnNew AosBuff(expect_size AosMemoryCheckerArgs);
	return true;
}


bool
AosSengineDocInfoCommon::sendRequestPublic(const AosRundataPtr &rdata)
{
	mLock->lock();
	if (mNumDocs > 0)
	{
		bool rslt = sendRequestPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();

	for (int i = 0; i < mTotalReqs; i++)
	{
		mSem->wait();                   
	}
	if (mSvrDeath) return false;
	return true;
}

void
AosSengineDocInfoCommon::callback(const bool svr_death)
{
	if (svr_death)
	{
		mSvrDeath = svr_death;
	}
	mTempPost++;
	mSem->post();
}
