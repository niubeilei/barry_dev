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
#if 0
#include "DocServer/DocInfo2.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "API/AosApiG.h"
#include "DocClient/DocClient.h"
#include "DocServer/ProcNames.h"
#include "DocServer/DocSvr.h"
#include "SEInterfaces/StorageEngineObj.h"
#include "Util/Buff.h"
#include "Util/Ptrs.h"


AosDocInfo2::AosDocInfo2(const u32 vid)
:
mLock(OmnNew OmnMutex()),
mRawData(0),
mDocids(0),
mDocSize(-1),
mNumDocs(0),
mMaxDocs(0),
mVid(vid),
mLastSizeid(0),
mFirstDocOffset(eDftFirstDocOffset),
mFirstDocid(0),
mMaxDocidIdx(0)
{
	//OmnScreen << "create docinfo:" << this<< endl;
}


AosDocInfo2::~AosDocInfo2()
{
	//OmnScreen << "delete docinfo:" << this<< endl;
	OmnDelete [] mDocids;
	OmnDelete [] mRawData; 
	mDocids = 0;
	mRawData = 0;
}
		

bool
AosDocInfo2::createMemory(
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
AosDocInfo2::addDoc(
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
	if (mNumDocs > 0 && (mLastSizeid != sizeid || local_docid < mFirstDocid
			|| local_docid >= (mFirstDocid + mMaxDocs)))
	{
		// Need to send the transaction to DocServer.
		bool rslt = sendRequestPriv(rdata, NULL);
		aos_assert_rl(rslt, mLock, false);
	}
	if (mNumDocs == 0)
	{
		// This is the first doc to add. 
		mFirstDocid = (local_docid < mFirstDocOffset) ? 0 : local_docid - mFirstDocOffset;
	}
	u64 docid_idx = local_docid - mFirstDocid;
	mLastSizeid = sizeid;

	mNumDocs++;
	if (mMaxDocidIdx < docid_idx) mMaxDocidIdx = docid_idx;
	aos_assert_r(docid_idx < mMaxDocs, false);
	mDocids[docid_idx] = local_docid;
	memcpy(&mRawData[docid_idx * mDocSize], doc, mDocSize);

	mLock->unlock();
	return true;
}


bool
AosDocInfo2::sendRequestPriv(
		const AosRundataPtr &rdata, 
		const AosTaskObjPtr &task) 
{
	// It sends the buff 'vid' to the backend server. Its format is:
	// 	number of docs			u32
	// 	docids[...]				an array of u64
	// 	offsets[...]			an array of u32
	// 	buff
	aos_assert_rr(mNumDocs > 0, rdata, false);
		
	AosDocFileMgrObjPtr doc_mgr = AosDocSvr::getSelf()->getFixedDocFileMgr(mVid, true, rdata);
	if (!doc_mgr)
	{
		AosSetErrorU(rdata, "no_file_mgr") << ": " << mVid;
		OmnAlarm << rdata->getErrmsg() << enderr;
		return false;
	}

	// Need to save the batch docs.
	AosBuffPtr buff = OmnNew AosBuff(mRawData, (mMaxDocidIdx + 1) * mDocSize,
		(mMaxDocidIdx + 1) * mDocSize AosMemoryCheckerArgs);

	AosDocType::E type  = AosDocType::eFixedLength;
	AosStorageEngineObjPtr engine;
	engine = AosStorageEngineObj::getStorageEngine(type);
	aos_assert_r(engine, false);

	bool rslt = engine->saveBatchFixedDocs(mLastSizeid, mMaxDocidIdx + 1, mDocids, buff, mDocSize, doc_mgr, 0, rdata);
	aos_assert_r(rslt, false);

	mNumDocs = 0;
	memset(mDocids, 0, sizeof(u64) * mMaxDocs);
	mFirstDocid = 0;
	mMaxDocidIdx = 0;
	return true;
}


bool
AosDocInfo2::sendRequestPublic(
		const AosRundataPtr &rdata, 
		const AosTaskObjPtr &task)
{
	// Call this function after finishing the assembling. It 
	// sends all the buffs. 
	mLock->lock();
	if (mNumDocs > 0)
	{
		bool rslt = sendRequestPriv(rdata, task);
		aos_assert_rl(rslt, mLock, false);
	}
	mLock->unlock();
	return true;
}
#endif
