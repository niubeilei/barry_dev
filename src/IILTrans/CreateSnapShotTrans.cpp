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
#include "IILTrans/CreateSnapShotTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "SEInterfaces/IILMgrObj.h"
#include "TransUtil/TaskTransChecker.h"
#include "Util/Buff.h"
#include "Util/StrU64Array.h"
#include "XmlUtil/XmlTag.h"


AosIILTransCreateSnapShot::AosIILTransCreateSnapShot(const bool flag)
:
AosIILTrans(AosTransType::eCreateSnapShot, flag AosMemoryCheckerArgs)
{
}


AosIILTransCreateSnapShot::AosIILTransCreateSnapShot(
		const u64 &taskdocid,
		const u32 &virtual_id,
		const u64 snap_id,
		const bool need_save,
		const bool need_resp)
:
AosIILTrans(AosTransType::eCreateSnapShot, virtual_id, 
	true, snap_id, need_save, need_resp AosMemoryCheckerArgs),
mTaskDocid(taskdocid),
mVirtualId(virtual_id)
{
}


AosTransPtr 
AosIILTransCreateSnapShot::clone()
{
	return OmnNew AosIILTransCreateSnapShot(false);
}


bool
AosIILTransCreateSnapShot::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mTaskDocid = buff->getU64(0); 
	aos_assert_r(mTaskDocid, false);

	mVirtualId = buff->getU32(0); 
	return true;
}


bool
AosIILTransCreateSnapShot::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU64(mTaskDocid);
	buff->setU32(mVirtualId);
	return true;
}


bool
AosIILTransCreateSnapShot::proc()
{
	u64 snap_id = AosIILMgrObj::getIILMgr()->createSnapshot(mVirtualId, mSnapshotId, getTransId(), mRdata);

	AosTaskTransChecker::getSelf()->addEntry(mTaskDocid, mVirtualId, snap_id); 

	AosBuffPtr resp_buff = OmnNew AosBuff(15 AosMemoryCheckerArgs);
	resp_buff->setU64(snap_id);
	sendResp(resp_buff);
	return true;
}

AosIILType 
AosIILTransCreateSnapShot::getIILType() const
{
	OmnNotImplementedYet;
	return eAosIILType_Invalid; 
}


int
AosIILTransCreateSnapShot::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}


/*
bool
AosIILTransCreateSnapShot::proc()
{
	u32 snap_id;
	bool rslt = AosIILMgrObj::getIILMgr()->createSnapShot(snap_id, mVirtualId, mNeedRemove, mRdata);

	AosBuffPtr resp_buff = OmnNew AosBuff(20 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);
	resp_buff->setU32(snap_id);

	if(isNeedResp())
	{
		if(resp_buff && rslt)
		{
			sendResp(resp_buff);
			return true;
		}

		if(!resp_buff)
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
AosIILTransCreateSnapShot::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	aos_assert_r(iilobj, false);
	
	bool rslt = false; //AosIILMgrObj::getIILMgr()->createSnapShot(rdata, iil);
	aos_assert_r(rslt, false);

	//int64_t bufflen = sizeof(u32) + sizeof(u8);
	//resp_buff = OmnNew AosBuff(bufflen AosMemoryCheckerArgs);
	//resp_buff->setU8(rslt);
	return true;
}
*/



