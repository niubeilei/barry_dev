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
// 10/06/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/CreateIILTrans.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApi.h"
#include "Debug/Except.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/IILMgrObj.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"


AosIILTransCreateIIL::AosIILTransCreateIIL(const bool flag)
:
AosIILTrans(AosTransType::eCreateIIL, flag AosMemoryCheckerArgs),
mIILType(eAosIILType_Invalid)
{
}


AosIILTransCreateIIL::AosIILTransCreateIIL(
		const u64 iilid,
		const AosIILType iiltype,
		const bool is_persis,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eCreateIIL, iilid, is_persis,
	snap_id, need_save, need_resp AosMemoryCheckerArgs),
mIILType(iiltype)
{
}


AosIILTransCreateIIL::AosIILTransCreateIIL(
		const OmnString &iilname,
		const AosIILType iiltype,
		const bool is_persis,
		const bool need_save,
		const bool need_resp,
		const u64 snap_id)
:
AosIILTrans(AosTransType::eCreateIIL, iilname, is_persis,
	snap_id, need_save, need_resp AosMemoryCheckerArgs),
mIILType(iiltype)
{
}


AosTransPtr 
AosIILTransCreateIIL::clone()
{
	return OmnNew AosIILTransCreateIIL(false);
}


bool
AosIILTransCreateIIL::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mIILType);
	return true;
}


bool
AosIILTransCreateIIL::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mIILType = (AosIILType)(buff->getU32(0)); 
	aos_assert_r(mIILType != eAosIILType_Invalid, false)
	return true;
}


int
AosIILTransCreateIIL::getSerializeSize() const
{
	OmnNotImplementedYet;
	return 0;
}


bool
AosIILTransCreateIIL::proc()
{
	u64 iilid = getIILID(mRdata);
	if (iilid == 0)
	{
		OmnAlarm << "create iilid error" << enderr;
		return sendErrResp();
	}

	u32 siteid = mRdata->getSiteid();
	u64 snap_id = getSnapshotId();
	AosIILType iiltype = mIILType;
	AosIILObjPtr iilobj = AosIILMgrObj::getIILMgr()->loadIILPublic(
		iilid, siteid, snap_id, iiltype, mRdata);
	if (iilobj)
	{
		OmnAlarm << "iil already exist" << enderr;
		AosIILMgrObj::getIILMgr()->returnIILPublic(iilobj, mRdata);
		return sendErrResp();
	}

	iilobj = AosIILMgrObj::getIILMgr()->createIILPublic(
		iilid, siteid, snap_id, mIILType, isPersis(), mRdata);
	if (!iilobj)
	{
		OmnAlarm << "failed to create iil" << enderr;
		return sendErrResp();
	}
	
	iiltype = mIILType;
	if (iiltype == eAosIILType_NumAlpha) iiltype = eAosIILType_Str;
	if (iilobj->getIILType() != iiltype)
	{
		OmnAlarm << "iil type error:" << iiltype
			<< ":" << iilobj->getIILType() << enderr;
		AosIILMgrObj::getIILMgr()->returnIILPublic(iilobj, mRdata);
		return sendErrResp();
	}
	
	AosIILMgrObj::getIILMgr()->returnIILPublic(iilobj, mRdata);

	AosBuffPtr resp = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp->setU8(true);
	resp->setU64(iilid);

	sendResp(resp);
	return true;
}


AosIILType 
AosIILTransCreateIIL::getIILType() const
{
	return mIILType;
}

