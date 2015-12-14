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
#include "DocTrans/BatchInsertDocTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "StorageEngine/SengineDocInfo.h"
#include "TransUtil/TaskTransChecker.h"


AosBatchInsertDocTrans::AosBatchInsertDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchInsertDoc, regflag),
mVid(-1),
mGroupId(0),
mSnapshotId(0),
mTaskDocid(0),
mCompressFlag(false)
{
}


AosBatchInsertDocTrans::AosBatchInsertDocTrans(
		const AosDataProcObjPtr &resp_caller,
		const int vid,
		const u64 group_id,
		const u64 &snap_id,
		const u64 &task_docid,
		const AosBuffPtr &buff)
:
AosDocTrans(AosTransType::eBatchInsertDoc, vid, true, false, true, snap_id),
mRespCaller(resp_caller),
mVid(vid),
mGroupId(group_id),
mSnapshotId(snap_id),
mTaskDocid(task_docid),
mBuff(buff),
mCompressFlag(false)
{
	if(buff->dataLen() > eDftCompressSize)
	{
		mCompressFlag = true;
		mBuff = AosBuff::compress(buff);
	}
}


AosBatchInsertDocTrans::~AosBatchInsertDocTrans()
{
}


bool
AosBatchInsertDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mVid = buff->getInt(0); 
	aos_assert_r(mVid >= 0, false);

	mGroupId = buff->getU64(0);
	aos_assert_r(mVid >= 0, false);

	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid != 0, false);

	mSnapshotId  = buff->getU64(0);
	u32 len = buff->getU32(0);
	mBuff = buff->getBuff(len, true AosMemoryCheckerArgs);
	aos_assert_r(mBuff, false);

	mCompressFlag = buff->getU8(0);

	setDistId(mVid);
	return true;
}


bool
AosBatchInsertDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mVid);
	buff->setU64(mGroupId);
	buff->setU64(mTaskDocid);
	buff->setU64(mSnapshotId);
	aos_assert_r(mBuff, false);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);
	buff->setU8(mCompressFlag);

	return true;
}


AosTransPtr
AosBatchInsertDocTrans::clone()
{
	return OmnNew AosBatchInsertDocTrans(false);
}


bool
AosBatchInsertDocTrans::proc()
{
	bool rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, mVid, mSnapshotId); 
	AosTransPtr thisPtr(this, false);
	if(mCompressFlag)
	{
		mBuff = AosBuff::unCompress(mBuff);
	}
	aos_assert_r(mBuff->dataLen() > 0, false);
	rslt = AosDocSvrObj::getDocSvr()->batchInsertDocNew(thisPtr, mBuff, mVid, mGroupId, mSnapshotId, mRdata);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosBatchInsertDocTrans::respCallBack()
{
	AosBuffPtr resp = getResp();                      
	aos_assert_r(resp && resp->dataLen() > 0, false); 
	                                                  
	bool svr_death = isSvrDeath();                    
	if (!svr_death)                                   
	{                                                 
		bool rslt = resp->getU8(0);                   
		if (!rslt) svr_death = true;                  
	}                                                 
	mRespCaller->callback(svr_death);                 
	return false;                                     
}

