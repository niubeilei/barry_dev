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
#include "IILTrans/CommitSnapShotTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransCommitSnapShot::AosIILTransCommitSnapShot(const bool flag)
:
AosIILTrans(AosTransType::eCommitSnapShot, flag AosMemoryCheckerArgs)
{
}


AosIILTransCommitSnapShot::AosIILTransCommitSnapShot(
		const u64 &taskdocid,
		const u32 &virtual_id,
		const u64 &snap_id,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eCommitSnapShot, virtual_id, 
		true, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mTaskDocid(taskdocid),
mVirtualId(virtual_id)
{
}


AosTransPtr 
AosIILTransCommitSnapShot::clone()
{
	return OmnNew AosIILTransCommitSnapShot(false);
}


bool
AosIILTransCommitSnapShot::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0); 
	aos_assert_r(mTaskDocid, false);
	mVirtualId = buff->getU32(0); 
	return true;
}


bool
AosIILTransCommitSnapShot::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mTaskDocid);
	buff->setU32(mVirtualId);
	return true;
}


bool
AosIILTransCommitSnapShot::proc()
{
	bool rslt = AosIILMgrObj::getIILMgr()->commitSnapshot(
			mVirtualId, mSnapshotId, getTransId(), mRdata);

	AosTaskTransChecker::getSelf()->removeEntry(mTaskDocid, mVirtualId, mSnapshotId); 
	return rslt;
}


AosIILType 
AosIILTransCommitSnapShot::getIILType() const
{
	return eAosIILType_Invalid;
}


int
AosIILTransCommitSnapShot::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}




/*
bool
AosIILTransCommitSnapShot::proc()
{
	bool rslt = AosIILMgrObj::getIILMgr()->commitSnapShot(mIILName, mRdata);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs); 
	resp_buff->setU8(rslt);

	if (isNeedResp())
	{
		if (resp_buff && rslt)
		{
			// Ketty 2013/07/23
			sendResp(resp_buff);
			//AosTransPtr thisptr(this, false);
			//AosSendResp(thisptr, resp_buff);
			return true;
		}

		if (!resp_buff)
		{
			OmnAlarm << "error! miss resp!." 
				<< " opr:" << getType()
				<< enderr; 
		}
		
		sendErrResp();	
	}
	
	return true;
}
*/

/*
bool
AosIILTransCommitSnapShot::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);

	bool rslt = false; //AosIILMgrObj::getIILMgr()->commitSnapShot(rdata, iil);
	aos_assert_r(rslt, false);

	//resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs); 
	//resp_buff->setU8(rslt);
	return true;
}
*/

/*
u64
AosIILTransCommitSnapShot::getIILID(const AosRundataPtr &rdata)
{
	return getIILIDByName(mIILName, rdata);
}
*/


