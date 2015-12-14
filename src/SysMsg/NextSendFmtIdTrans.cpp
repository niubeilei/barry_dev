////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// Created: 05/08/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/NextSendFmtIdTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/FmtMgrObj.h"

AosNextSendFmtIdTrans::AosNextSendFmtIdTrans(const bool reg_flag)
:
AosTaskTrans(AosTransType::eNextSendFmtId, reg_flag)
{
	setIsSystemTrans(); 
}


AosNextSendFmtIdTrans::AosNextSendFmtIdTrans(
		const int to_svrid,
		const u32 cube_grp_id)
:
AosTaskTrans(AosTransType::eNextSendFmtId, to_svrid,
		AosProcessType::eCube, cube_grp_id, false, true),
mCubeGrpId(cube_grp_id)
{
	setIsSystemTrans(); 
}


AosNextSendFmtIdTrans::~AosNextSendFmtIdTrans()
{
}


bool
AosNextSendFmtIdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mCubeGrpId = buff->getU32(0);
	return true;
}


bool
AosNextSendFmtIdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mCubeGrpId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosNextSendFmtIdTrans::clone()
{
	return OmnNew AosNextSendFmtIdTrans(false);
}


bool
AosNextSendFmtIdTrans::proc()
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);

	u64 next_fmt_id;
	bool rslt = fmt_mgr->getNextSendFmtId(
		mRdata.getPtrNoLock(), mCubeGrpId, getFromSvrId(), next_fmt_id);

	AosBuffPtr resp = OmnNew AosBuff(10, 0 AosMemoryCheckerArgs);
	resp->setU8(rslt);
	resp->setU64(next_fmt_id);

	sendResp(resp);
	return true;
}


