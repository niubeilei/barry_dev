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
#include "StorageEngine/SengineDocInfoFixed.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "DocServer/DocSvr.h"
#include "DocTrans/BatchFixedTrans.h"
#include "DocTrans/BatchDeleteGroupDocTrans.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"


AosSengineDocInfoFixed::AosSengineDocInfoFixed(
		const AosGroupDocOpr::E opr,
		const int vid, 
		const u64 &snap_id,
		const u64 &task_docid)
:
mLock(OmnNew OmnMutex()),
mRawData(0),
mDocids(0),
mDocSize(-1),
mNumDocs(0),
mMaxDocs(0),
mVid(vid),
mCrtSizeid(0),
mFirstDocOffset(eDftFirstDocOffset),
mFirstDocid(0),
mMaxDocidIdx(0),
mSem(OmnNew OmnSem(0)),
mTotalReqs(0),
mTempPost(0),
mSnapId(snap_id),
mTaskDocid(task_docid),
mSvrDeath(false),
mOpr(opr)
{
	//OmnScreen << "create docinfo:" << this<< endl;
}


AosSengineDocInfoFixed::~AosSengineDocInfoFixed()
{
	//OmnScreen << "delete docinfo:" << this<< endl;
	OmnDelete [] mDocids;
	OmnDelete [] mRawData; 
	mDocids = 0;
	mRawData = 0;
}
		

bool
AosSengineDocInfoFixed::createMemory(
		const u64 &buff_len,
		const int doc_size)
{
	aos_assert_r(buff_len > 0, false);
	aos_assert_r(doc_size > 0, false);
	
	mLock->lock();
	mDocSize = doc_size;
	mMaxDocs = buff_len / doc_size;
	
	if(mRawData) OmnDelete [] mRawData;
	mRawData = OmnNew char[buff_len + 10]; 
	memset(mRawData, 0, buff_len + 10);
	
	if(mDocids) OmnDelete [] mDocids;
	mDocids = OmnNew u64[mMaxDocs];
	memset(mDocids, 0, sizeof(u64) * mMaxDocs);
	
	mLock->unlock();
	return true;
}

bool
AosSengineDocInfoFixed::deleteDoc(
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	aos_assert_r(docid, false);
	u64 sizeid = AosGetSizeIdByDocid(docid);
	aos_assert_r(sizeid, false);

	static int lsNumVirtuals = AosGetNumCubes();
	u64 local_docid = AosGetLocalIdByDocid(docid) / lsNumVirtuals;

	mLock->lock();
	if (mNumDocs > 0 && (mCrtSizeid != sizeid || local_docid < mFirstDocid
			|| local_docid >= (mFirstDocid + mMaxDocs)))
	{
		bool rslt = sendRequestPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	if (mNumDocs == 0)
	{
		mFirstDocid = (local_docid < mFirstDocOffset) ? 0 : local_docid - mFirstDocOffset;
	}
	u64 docid_idx = local_docid - mFirstDocid;
	mCrtSizeid = sizeid;

	mNumDocs++;
	if (mMaxDocidIdx < docid_idx) mMaxDocidIdx = docid_idx;
	aos_assert_r(docid_idx < mMaxDocs, false);
	mDocids[docid_idx] = local_docid;
	mLock->unlock();
	return true;
}


bool 
AosSengineDocInfoFixed::addDoc(
		const char *doc, 
		const int len, 
		const u64 &docid,
		const AosRundataPtr &rdata)
{
	// Docs are buff-ed based on virtual_id. Docs belonging to the same
	// virtual ID are saved in the same buff.
	aos_assert_r(len == mDocSize, false);
	aos_assert_r(docid, false);
	u64 sizeid = AosGetSizeIdByDocid(docid);
	aos_assert_r(sizeid, false);

	static int lsNumVirtuals = AosGetNumCubes();
	u64 local_docid = AosGetLocalIdByDocid(docid) / lsNumVirtuals;

	mLock->lock();
	if (mNumDocs > 0 && (mCrtSizeid != sizeid || local_docid < mFirstDocid
			|| local_docid >= (mFirstDocid + mMaxDocs)))
	{
		// Need to send the transaction to DocServer.
		bool rslt = sendRequestPriv(rdata);
		aos_assert_rl(rslt, mLock, false);
	}
	if (mNumDocs == 0)
	{
		// This is the first doc to add. 
		mFirstDocid = (local_docid < mFirstDocOffset) ? 0 : local_docid - mFirstDocOffset;
	}
	u64 docid_idx = local_docid - mFirstDocid;
	mCrtSizeid = sizeid;

	mNumDocs++;
	if (mMaxDocidIdx < docid_idx) mMaxDocidIdx = docid_idx;
	aos_assert_r(docid_idx < mMaxDocs, false);
	mDocids[docid_idx] = local_docid;
	memcpy(&mRawData[docid_idx * mDocSize], doc, mDocSize);

	mLock->unlock();
	return true;
}


bool
AosSengineDocInfoFixed::sendRequestPriv(const AosRundataPtr &rdata) 
{
	aos_assert_r(mDocSize > 0, false);
	aos_assert_rr(mNumDocs > 0, rdata, false);
	AosSengineDocInfoPtr thisptr(this, false);
	u32 docid_len = (mMaxDocidIdx + 1) * sizeof(u64);
	AosBuffPtr docid_buff = OmnNew AosBuff(docid_len AosMemoryCheckerArgs);
	docid_buff->setBuff((char *)mDocids, docid_len);
	aos_assert_r(docid_buff->dataLen() / sizeof(u64) == mMaxDocidIdx + 1, false);
	switch(mOpr)
	{
	case AosGroupDocOpr::eBatchInsert:
	{
		// It sends the buff 'vid' to the backend server. Its format is:
		// 	number of docs			u32
		// 	docids[...]				an array of u64
		// 	offsets[...]			an array of u32
		// 	buff
		// Need to save the batch docs.
		// Chen Ding, 2015/05/25
		// AosBuffPtr buff = OmnNew AosBuff(mRawData, (mMaxDocidIdx + 1) * mDocSize,
		// 		(mMaxDocidIdx + 1) * mDocSize AosMemoryCheckerArgs);
		AosBuffPtr buff = OmnNew AosBuff(mRawData, (mMaxDocidIdx + 1) * mDocSize,
				(mMaxDocidIdx + 1) * mDocSize, true AosMemoryCheckerArgs);
		AosTransPtr trans =  OmnNew AosBatchFixedTrans(
				thisptr, mVid, mCrtSizeid, mMaxDocidIdx + 1, 
				mDocSize, docid_buff, buff, mSnapId, mTaskDocid);
		AosSendTransAsyncResp(rdata, trans);
		break;
	}
	case AosGroupDocOpr::eBatchDelete:
	{
		AosTransPtr trans =  OmnNew AosBatchDeleteGroupDocTrans(thisptr, AosDataRecordType::eFixedBinary, 
				mVid, mCrtSizeid, mNumDocs, mDocSize, docid_buff->dataLen(), docid_buff, mSnapId, mTaskDocid);
		AosSendTransAsyncResp(rdata, trans);
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
	mTotalReqs++;
	mNumDocs = 0;
	memset(mDocids, 0, sizeof(u64) * mMaxDocs);
	mFirstDocid = 0;
	mMaxDocidIdx = 0;
	return true;
}


bool
AosSengineDocInfoFixed::sendRequestPublic(const AosRundataPtr &rdata)
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


void
AosSengineDocInfoFixed::callback(const bool svr_death)
{
	if (svr_death) 
	{
		mSvrDeath = svr_death;
	}

	mTempPost++;
	mSem->post();
}

