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
#include "DocTrans/BatchCreateDocTrans.h"

#include "SEInterfaces/DocSvrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "XmlUtil/SeXmlParser.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/XmlDoc.h"

AosBatchCreateDocTrans::AosBatchCreateDocTrans(const bool regflag)
:
AosDocTrans(AosTransType::eBatchCreateDoc, regflag)
{
}


AosBatchCreateDocTrans::AosBatchCreateDocTrans(
		const u64 docid,
		const char *new_doc,
		const int data_len,   
		const u64 &snap_id,
		const u64 &task_docid,
		const bool need_save,
		const bool need_resp)
:
AosDocTrans(AosTransType::eBatchCreateDoc,
		docid, need_save, need_resp, snap_id),
mDocid(docid),
mDocBuff(0),
mDocLen(data_len),
mTaskDocid(task_docid)
{
	//This function is used by run task;
	mDocBuff = OmnNew AosBuff(mDocLen AosMemoryCheckerArgs);
	mDocBuff->setBuff(new_doc, mDocLen);
}


AosBatchCreateDocTrans::~AosBatchCreateDocTrans()
{
}


bool
AosBatchCreateDocTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mDocid = buff->getU64(0);
	mDocLen = buff->getU32(0);
	mDocBuff = buff->getBuff(mDocLen, true AosMemoryCheckerArgs);
	aos_assert_r(mDocid && mDocLen, false);
	mTaskDocid = buff->getU64(0);
	setDistId(mDocid);
	return true;
}


bool
AosBatchCreateDocTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mDocid);
	aos_assert_r(mDocBuff && mDocLen > 0, false);
	buff->setU32(mDocLen);
	buff->setBuff(mDocBuff);
	buff->setU64(mTaskDocid);
	return true;
}


AosTransPtr
AosBatchCreateDocTrans::clone()
{
	return OmnNew AosBatchCreateDocTrans(false);
}


bool
AosBatchCreateDocTrans::proc()
{
	aos_assert_r(mSnapshotId && mTaskDocid, false);
	int virtual_id = AosGetCubeId(AosXmlDoc::getOwnDocid(mDocid));
	bool rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, virtual_id, mSnapshotId); 
	if (!rslt)
	{
		//Discard
		OmnScreen << "AosBatchCreateDocTrans Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
		return false;
	}
	
	setFinishLater();

	aos_assert_r(mDocBuff && mDocLen > 0, false);
	rslt = AosDocSvrObj::getDocSvr()->createDoc(mRdata, mDocid, mDocBuff->data(), mDocLen, getTransId(), mSnapshotId);
	aos_assert_r(rslt, false);
	return true;
}
