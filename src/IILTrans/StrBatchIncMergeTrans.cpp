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
#include "IILTrans/StrBatchIncMergeTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILMgrObj.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransStrBatchIncMerge::AosIILTransStrBatchIncMerge(const bool flag)
:
AosIILTrans(AosTransType::eStrBatchIncMerge, flag AosMemoryCheckerArgs),
mTrueDelete(false)
{
}


AosIILTransStrBatchIncMerge::AosIILTransStrBatchIncMerge(
		const OmnString &iilname,
		const int entry_len,
		const u64 init_value,
		const AosIILUtil::AosIILIncType inc_type,
		const bool true_delete,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eStrBatchIncMerge, iilname, 
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mLen(entry_len),
mInitdocid(init_value),
mIncType(inc_type),
mTrueDelete(true_delete)
{
}


AosTransPtr 
AosIILTransStrBatchIncMerge::clone()
{
	return OmnNew AosIILTransStrBatchIncMerge(false);
}


bool
AosIILTransStrBatchIncMerge::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mLen = buff->getInt(-1);
	mInitdocid = buff->getU64(0);
	mIncType = (AosIILUtil::AosIILIncType)buff->getU32(0);
	mTrueDelete = buff->getU8(0);
	return true;
}


bool
AosIILTransStrBatchIncMerge::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setInt(mLen);
	buff->setU64(mInitdocid);
	buff->setU32(mIncType);
	buff->setU8(mTrueDelete);
	return true;
}


bool
AosIILTransStrBatchIncMerge::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_BigStr, false);

	OmnScreen << "Batch Inc Merge Start, iilname:" << mIILName
		<< ", len:" << mLen << ", inctype:" << mIncType << endl;

	u64 start_time = OmnGetTimestamp();
	bool rslt = AosIILMgrObj::getIILMgr()->StrBatchIncMergeSafe(
		mIILName, mLen, mInitdocid, mIncType, mTrueDelete, rdata);
	u64 cost = OmnGetTimestamp() - start_time;

	OmnScreen << "Batch Inc Merge Finish, iilname:" << mIILName
		<< ", time cost:" << AosTimestampToHumanRead(cost) << endl;

	resp_buff = OmnNew AosBuff(200 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	return true;
}


AosIILType 
AosIILTransStrBatchIncMerge::getIILType() const
{
	return eAosIILType_BigStr;
}


int
AosIILTransStrBatchIncMerge::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

