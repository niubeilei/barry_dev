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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/BatchFixedTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "StorageEngine/SengineDocInfo.h"
#include "TransUtil/TaskTransChecker.h"


AosBatchFixedTrans::AosBatchFixedTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchFixed, regflag)
{
}


AosBatchFixedTrans::AosBatchFixedTrans(
		const AosSengineDocInfoPtr &resp_caller,
		const int vid,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_size,
		const AosBuffPtr &docid_buff,
		const AosBuffPtr &buff,
		const u64 &snap_id,
		const u64 &task_docid)
:
AosDocTrans(AosTransType::eBatchFixed, vid, true, false, true, snap_id),
mVid(vid),
mRespCaller(resp_caller),
mSizeId(sizeid),
mNumDocs(num_docs),
mRecordSize(record_size),
mDocids(docid_buff),
mRawData(buff),
mTaskDocid(task_docid)
{
}


AosBatchFixedTrans::~AosBatchFixedTrans()
{
}


bool
AosBatchFixedTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mVid = buff->getInt(0); 
	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid, false);
	mSizeId = buff->getU32(0);
	mNumDocs = buff->getU64(0);
	mRecordSize = buff->getInt(-1);

	u32 docid_len = buff->getU32(0);
	mDocids = buff->getBuff(docid_len, true AosMemoryCheckerArgs);

	u32 raw_len = buff->getU32(0);
	mRawData = buff->getBuff(raw_len, true AosMemoryCheckerArgs);
	
	aos_assert_r(mNumDocs > 0 && mRecordSize > 0 
		&& mSizeId > 0 && mDocids && mRawData, false);

	setDistId(mVid);
	return true;
}


bool
AosBatchFixedTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mVid);
	buff->setU64(mTaskDocid);
	buff->setU32(mSizeId);
	buff->setU64(mNumDocs);
	buff->setInt(mRecordSize);

	buff->setU32(mDocids->dataLen());
	buff->setBuff(mDocids);
	
	buff->setU32(mRawData->dataLen());
	buff->setBuff(mRawData);
	
	return true;
}


AosTransPtr
AosBatchFixedTrans::clone()
{
	return OmnNew AosBatchFixedTrans(false);
}


bool
AosBatchFixedTrans::proc()
{
	aos_assert_r(mNumDocs > 0 && mDocids->dataLen() / sizeof(u64) == (u32)mNumDocs, false);

	//Linda, 2014/03/12
	//AosStorageEngineObjPtr engine = AosStorageEngineObj::getStorageEngine(AosDocType::eGroupedDoc);
	//aos_assert_r(engine, false);
	//
	////AosAsyncReqTransPtr thisPtr(this, false);
	//AosTransPtr thisPtr(this, false);
	//AosSengineSaveDocReqObjPtr req = OmnNew AosSengineSaveFixedDocReq(
	//	thisPtr, mVid, mSizeId, mNumDocs, mRecordSize, mDocids, mRawData);
	//
	//bool rslt = engine->addReq(req, mRdata);
	//aos_assert_r(rslt, false);

	OmnScreen << "Test--------------:trans_id:" << getTransId().toString()
		<< "; mTaskDocid:" << mTaskDocid
		<< "; mVirtualId:" << mVid
		<< "; mSnapshotId:" << mSnapshotId
		<< endl;
	bool rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, mVid, mSnapshotId); 
	if (!rslt)
	{
		AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
		sendResp(resp_buff);

		//Discard
		OmnScreen << "AosBatchFixedTrans Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
		return false;
	}
	
	AosTransPtr thisPtr(this, false);
	rslt = AosDocSvrObj::getDocSvr()->batchSaveGroupedDoc(
			thisPtr, mVid, mSizeId, mNumDocs, mRecordSize, 
			mDocids, mRawData, mSnapshotId, mRdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBatchFixedTrans::respCallBack()
{
	AosBuffPtr resp = getResp();
	aos_assert_r(resp && resp->dataLen() > 0, false);

	bool svr_death = isSvrDeath();
	if (!svr_death)
	{
		bool rslt = resp->getU8(0);
		if (!rslt) svr_death = true;
	}

	// will call DocInfo :: callback().
	mRespCaller->callback(svr_death);
	return false;
}

