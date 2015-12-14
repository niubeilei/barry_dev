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
#include "DocTrans/BatchDeleteGroupDocTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "StorageEngine/SengineDocInfo.h"
#include "TransUtil/TaskTransChecker.h"


AosBatchDeleteGroupDocTrans::AosBatchDeleteGroupDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchDeleteGroupDoc, regflag)
{
}


AosBatchDeleteGroupDocTrans::AosBatchDeleteGroupDocTrans(
		const AosSengineDocInfoPtr &resp_caller,
		const AosDataRecordType::E type,
		const int vid,
		const u32 sizeid,
		const u64 &num_docs,
		const int record_size,
		const u64 &data_len,
		const AosBuffPtr &buff,
		const u64 &snap_id,
		const u64 &task_docid)
:
AosDocTrans(AosTransType::eBatchDeleteGroupDoc, vid, true, false, true, snap_id),
mVid(vid),
mRespCaller(resp_caller),
mSizeId(sizeid),
mNumDocs(num_docs),
mRecordSize(record_size),
mDataLen(data_len),
mBuff(buff),
mTaskDocid(task_docid),
mRecordType(type)
{
}


AosBatchDeleteGroupDocTrans::~AosBatchDeleteGroupDocTrans()
{
}


bool
AosBatchDeleteGroupDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mRecordType = AosDataRecordType::toEnum(buff->getOmnStr(""));
	mVid = buff->getInt(0); 
	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid, false);
	mSizeId = buff->getU32(0);
	mNumDocs = buff->getU64(0);
	mRecordSize = buff->getInt(-1);
	mDataLen = buff->getU64(0);

	u32 len = buff->getU32(0);
	mBuff = buff->getBuff(len, true AosMemoryCheckerArgs);

	aos_assert_r(mNumDocs > 0 && mRecordSize > 0
		&& mSizeId > 0 && mDataLen > 0
		&& mBuff, false);

	setDistId(mVid);
	return true;
}


bool
AosBatchDeleteGroupDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(AosDataRecordType::toStr(mRecordType));
	buff->setInt(mVid);
	buff->setU64(mTaskDocid);
	buff->setU32(mSizeId);
	buff->setU64(mNumDocs);
	buff->setInt(mRecordSize);
	buff->setU64(mDataLen);

	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);

	return true;
}


AosTransPtr
AosBatchDeleteGroupDocTrans::clone()
{
	return OmnNew AosBatchDeleteGroupDocTrans(false);
}


bool
AosBatchDeleteGroupDocTrans::proc()
{
	aos_assert_r(mNumDocs > 0, false);
	bool rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, mVid, mSnapshotId); 
	if (!rslt)
	{
		AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		resp_buff->setU8(false);
		sendResp(resp_buff);
		//Discard
		OmnScreen << "AosBatchDeleteGroupDocTrans Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
		return false;
	}
	
	AosTransPtr thisPtr(this, false);
	if (mRecordType == AosDataRecordType::eCSV)
	{
		rslt = AosDocSvrObj::getDocSvr()->batchDeleteCSVDoc(
				thisPtr, mVid, mSizeId, mNumDocs, mRecordSize, mDataLen,
				mBuff, mSnapshotId, mRdata);
	}
	else if (mRecordType == AosDataRecordType::eFixedBinary)
	{
		OmnNotImplementedYet;
	}
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBatchDeleteGroupDocTrans::respCallBack()
{
	//Jozhi 2014-07-22 if cube death return svr death
	// will call DocInfo :: callback().
	//mRespCaller->callback(false);
	//return false;
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

