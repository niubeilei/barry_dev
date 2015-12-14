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
// 2012/04/04 Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/SengineDocInfoCSV.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "DocTrans/BatchCSVTrans.h"
#include "DocTrans/BatchDeleteCSVTrans.h"
#include "DocTrans/BatchDeleteGroupDocTrans.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"


AosSengineDocInfoCSV::AosSengineDocInfoCSV(
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


AosSengineDocInfoCSV::~AosSengineDocInfoCSV()
{
}
		

bool
AosSengineDocInfoCSV::createMemory(
		const u64 &buff_len,
		const int doc_size)
{
	aos_assert_r(buff_len > 0, false);
	//aos_assert_r(doc_size > 0, false);
	
	mLock->lock();
	mDocSize = doc_size;
	mMaxBuffLen = buff_len;
	mBuff = OmnNew AosBuff(buff_len + 200 AosMemoryCheckerArgs);
	mOffsetMap.clear();

	mLock->unlock();
	return true;
}

bool 
AosSengineDocInfoCSV::addDoc(
		const char *doc, 
		const int len, 
		const u64 &docid,
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
	mBuff->setInt(len);
	mBuff->setBuff(doc, len);
	mCrtSizeid = sizeid;
	mNumDocs++;

	mLock->unlock();
	return true;
}


bool
AosSengineDocInfoCSV::sendRequestPriv(const AosRundataPtr &rdata) 
{
	if (mNumDocs <= 0)
	{
		return true;
	}
	AosSengineDocInfoPtr thisptr(this, false);
	int64_t data_len = mBuff->dataLen();
	int64_t expect_size = data_len + mNumDocs * sizeof(u64) * 2 + 20;
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
		AosTransPtr trans =  OmnNew AosBatchCSVTrans(thisptr, mVid, mCrtSizeid, 
					mNumDocs, mDocSize, data_len, mBuff, mSnapId, mTaskDocid);
		AosSendTransAsyncResp(rdata, trans);
		mTotalReqs++;
		mOffsetMap.clear();
		break;
	}
	case AosGroupDocOpr::eBatchDelete:
	{
		aos_assert_r(mDocSize > 0, false);
		AosTransPtr trans =  OmnNew AosBatchDeleteGroupDocTrans(thisptr, AosDataRecordType::eCSV, 
				mVid, mCrtSizeid, mNumDocs, mDocSize, data_len, mBuff, mSnapId, mTaskDocid);
		AosSendTransAsyncResp(rdata, trans);
		mTotalReqs++;
		break;
	}
	case AosGroupDocOpr::eUpdate:
	{
		OmnNotImplementedYet;
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
AosSengineDocInfoCSV::sendRequestPublic(const AosRundataPtr &rdata)
{
	// Call this function after finishing the assembling. It 
	// sends all the buffs. 
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


bool
AosSengineDocInfoCSV::deleteDoc(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, false);
	u64 sizeid = AosGetSizeIdByDocid(docid);
	aos_assert_r(sizeid, false);

	static int lsNumVirtuals = AosGetNumCubes();
	u64 local_docid = AosGetLocalIdByDocid(docid) / lsNumVirtuals;

	mLock->lock();
	if (mCrtSizeid != sizeid || mBuff->dataLen() > (int64_t)mMaxBuffLen)
	{
		bool rslt = sendRequestPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}

	mBuff->setU64(local_docid);
	mCrtSizeid = sizeid;
	mNumDocs++;
	mLock->unlock();
	return true;
}

bool
AosSengineDocInfoCSV::updateDoc(
		const char* doc,
		const int len,
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	OmnNotImplementedYet;
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
	mBuff->setInt(len);
	mBuff->setBuff(doc, len);
	mCrtSizeid = sizeid;
	mNumDocs++;
	mLock->unlock();
	return true;
}

void
AosSengineDocInfoCSV::callback(const bool svr_death)
{
	if (svr_death)
	{
		mSvrDeath = svr_death;
	}
	mTempPost++;
	mSem->post();
}

