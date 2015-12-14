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
#include "IILTrans/HitBatchAddTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "SEInterfaces/IILExecutorObj.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransHitBatchAdd::AosIILTransHitBatchAdd(const bool flag)
:
AosIILTrans(AosTransType::eHitBatchAdd, flag AosMemoryCheckerArgs)
{
}


AosIILTransHitBatchAdd::AosIILTransHitBatchAdd(
		const OmnString &iilname,
		const vector<u64> &docids,
		const u64 &snap_id,
		const u64 &task_docid,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eHitBatchAdd, iilname,
	false, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mDocIds(docids),
mTaskDocid(task_docid)
{
}


AosTransPtr 
AosIILTransHitBatchAdd::clone()
{
	return OmnNew AosIILTransHitBatchAdd(false);
}


bool
AosIILTransHitBatchAdd::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	u32 size = buff->getU32(0);

	mDocIds.clear();
	for(u32 i=0; i<size; i++)
	{
		u64 docid = buff->getU64(0);
		mDocIds.push_back(docid);
	}

	mTaskDocid= buff->getU64(0);
	if (mSnapshotId != 0)
	{
		aos_assert_r(mTaskDocid, false);
	}
	return true;
}


bool
AosIILTransHitBatchAdd::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	u32 size = mDocIds.size();
	buff->setU32(size);

	for(u32 i=0; i<size; i++)
	{
		buff->setU64(mDocIds[i]);
	}

	buff->setU64(mTaskDocid);
	
	return true;
}


bool
AosIILTransHitBatchAdd::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	aos_assert_r(iilobj->getIILType() == eAosIILType_Hit, false);

	//Linda, 2014/03/13
	if (mSnapshotId != 0)
	{
		int vid = 0;
		u64 localid = 0;
		//bool rslt = AosIILMgr::getSelf()->parseIILID(mIILID, localid, vid, rdata);
		bool rslt = AosIILMgrObj::getIILMgr()->parseIILID(mIILID, localid, vid, rdata);
		aos_assert_r(rslt, false);

		rslt = AosTaskTransChecker::getSelf()->checkTrans(mTaskDocid, vid, mSnapshotId); 
		if (!rslt)
		{
			//Discard
			OmnScreen << "AosIILTransU64BatchAdd Discard taskdocid:" << mTaskDocid << ";snap_id:" << mSnapshotId << ";"<< endl;
			return false;
		}
	}


	//AosIILHit *hitiil = (AosIILHit*)(iilobj.getPtr());
	bool rslt = true;
	for(u32 i=0; i<mDocIds.size(); i++)
	{
		rslt = iilobj->addDocSafe(mDocIds[i], rdata);
		if (!rslt) break;
	}

	//hitiil->saveToFileSafe(rdata);
	iilobj->saveToFileSafe(rdata);

	resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	return true;
}


AosIILType 
AosIILTransHitBatchAdd::getIILType() const
{
	return eAosIILType_Hit;
}


int
AosIILTransHitBatchAdd::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

