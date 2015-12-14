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
#include "IILTrans/MergeSnapShotTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransMergeSnapShot::AosIILTransMergeSnapShot(const bool flag)
:
AosIILTrans(AosTransType::eMergeSnapShot, flag AosMemoryCheckerArgs)
{
}


AosIILTransMergeSnapShot::AosIILTransMergeSnapShot(
		const u32 &virtual_id,
		const u64 &target_snap_id,
		const u64 &merge_snap_id,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eMergeSnapShot, virtual_id, 
	true, 0, need_save, need_resp AosMemoryCheckerArgs),
mVirtualId(virtual_id),
mTargetSnapId(target_snap_id),
mMergeSnapId(merge_snap_id)
{
}


AosTransPtr 
AosIILTransMergeSnapShot::clone()
{
	return OmnNew AosIILTransMergeSnapShot(false);
}


bool
AosIILTransMergeSnapShot::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mVirtualId = buff->getU32(0); 
	mTargetSnapId = buff->getU64(0);
	mMergeSnapId = buff->getU64(0);
	return true;
}


bool
AosIILTransMergeSnapShot::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mVirtualId);
	buff->setU64(mTargetSnapId);
	buff->setU64(mMergeSnapId);
	return true;
}


bool
AosIILTransMergeSnapShot::proc()
{
	bool rslt = AosIILMgrObj::getIILMgr()->mergeSnapshot(mVirtualId, mTargetSnapId, mMergeSnapId, getTransId(), mRdata);
	AosBuffPtr resp_buff = OmnNew AosBuff(15 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}

AosIILType 
AosIILTransMergeSnapShot::getIILType() const
{
	OmnNotImplementedYet;
	return eAosIILType_Invalid; 
}


int
AosIILTransMergeSnapShot::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}
