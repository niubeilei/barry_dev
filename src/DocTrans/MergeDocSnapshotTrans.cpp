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
#include "DocTrans/MergeDocSnapshotTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "XmlUtil/XmlTag.h"

AosMergeDocSnapshotTrans::AosMergeDocSnapshotTrans(const bool regflag)
:
AosDocTrans(AosTransType::eMergeDocSnapshot, regflag)
{
}


AosMergeDocSnapshotTrans::AosMergeDocSnapshotTrans(
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 target_snap_id,
		const u64 merge_snap_id,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eMergeDocSnapshot, virtual_id, true, need_save, need_resp, 0),
mVirtualId(virtual_id),
mDocType(doc_type),
mTargetSnapId(target_snap_id),
mMergeSnapId(merge_snap_id)
{
}


AosMergeDocSnapshotTrans::~AosMergeDocSnapshotTrans()
{
}


bool
AosMergeDocSnapshotTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mVirtualId = buff->getInt(-1);
	aos_assert_r(mVirtualId >= 0, false);
	mDocType = (AosDocType::E)buff->getU32(0);
	
	mTargetSnapId = buff->getU64(0);
	aos_assert_r(mTargetSnapId >=0, false);

	mMergeSnapId = buff->getU64(0);
	aos_assert_r(mMergeSnapId >=0, false);

	setDistId(mVirtualId);
	return true;
}


bool
AosMergeDocSnapshotTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mVirtualId);
	buff->setU32(mDocType);
	buff->setU64(mTargetSnapId);
	buff->setU64(mMergeSnapId);
	return true;
}


AosTransPtr
AosMergeDocSnapshotTrans::clone()
{
	return OmnNew AosMergeDocSnapshotTrans(false);
}


bool
AosMergeDocSnapshotTrans::proc()
{
	bool rslt = AosDocSvrObj::getDocSvr()->mergeSnapshot(
			mVirtualId, mDocType, mTargetSnapId, mMergeSnapId, getTransId(), mRdata);	
	AosBuffPtr resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	sendResp(resp_buff);
	return true;
}


bool
AosMergeDocSnapshotTrans::procGetResp()
{
	return proc();
}

