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
// 03/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/StrBatchAddMergeTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/BitmapEngineObj.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrBatchAddMerge::AosIILTransStrBatchAddMerge(const bool flag)
:
AosIILTrans(AosTransType::eStrBatchAddMerge, flag AosMemoryCheckerArgs),
mTrueDelete(false)
{
}


AosIILTransStrBatchAddMerge::AosIILTransStrBatchAddMerge(
		const OmnString &iilname,
		const int len,
		const u64 &executor_id,
		const bool true_delete,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrBatchAddMerge, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mLen(len),
mExecutorID(executor_id),
mTrueDelete(true_delete)
{
}


AosTransPtr 
AosIILTransStrBatchAddMerge::clone()
{
	return OmnNew AosIILTransStrBatchAddMerge(false);
}


bool
AosIILTransStrBatchAddMerge::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mLen = buff->getInt(-1);
	mExecutorID = buff->getU64(0);
	mTrueDelete = buff->getU8(0);
	return true;
}


bool
AosIILTransStrBatchAddMerge::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mLen);
	buff->setU64(mExecutorID);
	buff->setU8(mTrueDelete);
	return true;
}


bool
AosIILTransStrBatchAddMerge::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_BigStr, false);

//	AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutorByID(mExecutorID, rdata);
	AosIILExecutorObjPtr executor = AosBitmapEngineObj::getBitmapEngine()->getIILExecutor(rdata);
	if(!mExecutorID)
	{
		executor->disable();
	}
	if(mExecutorID == eStreamingExecutorId)
	{
		executor->disable();
		executor->setExecutorID(mExecutorID);
	}

	OmnScreen << "Batch Add Merge Start, iilname:" << mIILName
		<< ", len:" << mLen << endl;

	u64 start_time = OmnGetTimestamp();
	bool rslt = AosIILMgrObj::getIILMgr()->StrBatchAddMergeSafe(
		mIILName, mLen, executor, mTrueDelete, rdata);
	u64 cost = OmnGetTimestamp() - start_time;

#ifdef AOS_SUPPORT_BITMAP_QUERY
	executor->finish(rdata, 0);
#endif
	OmnScreen << "Batch Add Merge Finish, iilname:" << mIILName
		<< ", time cost:" << AosTimestampToHumanRead(cost) << endl;

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	return true;
}


AosIILType 
AosIILTransStrBatchAddMerge::getIILType() const
{
	return eAosIILType_BigStr;
}


int
AosIILTransStrBatchAddMerge::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

