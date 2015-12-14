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
#include "DocTrans/CreateDocSnapshotTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/DocSvrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "XmlUtil/XmlTag.h"

AosCreateDocSnapshotTrans::AosCreateDocSnapshotTrans(const bool regflag)
:
AosDocTrans(AosTransType::eCreateDocSnapshot, regflag)
{
}


AosCreateDocSnapshotTrans::AosCreateDocSnapshotTrans(
		const int virtual_id,
		const AosDocType::E doc_type,
		const u64 &task_docid,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eCreateDocSnapshot, virtual_id, true, need_save, need_resp, snap_id),
mVirtualId(virtual_id),
mDocType(doc_type),
mTaskDocid(task_docid)
{
}


AosCreateDocSnapshotTrans::~AosCreateDocSnapshotTrans()
{
}


bool
AosCreateDocSnapshotTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mVirtualId = buff->getInt(-1);
	aos_assert_r(mVirtualId >=0, false);


	mDocType = (AosDocType::E)buff->getU32(0);
	mTaskDocid = buff->getU64(0);
	aos_assert_r(mTaskDocid, false);

	setDistId(mVirtualId);
	return true;
}


bool
AosCreateDocSnapshotTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setInt(mVirtualId);
	buff->setU32(mDocType);
	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosCreateDocSnapshotTrans::clone()
{
	return OmnNew AosCreateDocSnapshotTrans(false);
}


bool
AosCreateDocSnapshotTrans::proc()
{
	u64 snap_id = AosDocSvrObj::getDocSvr()->createSnapshot(
			mVirtualId, mSnapshotId, mDocType, getTransId(), mRdata);	
	
	OmnScreen << "Test-------------- add snaoshpt :trans_id:" << getTransId().toString() 
		<< "; mTaskDocid:" << mTaskDocid
		<< "; mVirtualId:" << mVirtualId
		<< "; mSnapshotId:" << snap_id 
		<< endl;

	AosTaskTransChecker::getSelf()->addEntry(mTaskDocid, mVirtualId, snap_id); 

	AosBuffPtr resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU64(snap_id);
	sendResp(resp_buff);
	return true;
}


bool
AosCreateDocSnapshotTrans::procGetResp()
{
	return proc();
}

