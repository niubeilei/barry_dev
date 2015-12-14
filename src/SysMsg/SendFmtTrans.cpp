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
// 03/24/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SysMsg/SendFmtTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/FmtMgrObj.h"

AosSendFmtTrans::AosSendFmtTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSendFmt, regflag)
{
	setIsSystemTrans(); 
}


AosSendFmtTrans::AosSendFmtTrans(
		const int svr_id,
		const u32 cube_grp_id,
		const AosBuffPtr &fmt_buff)
:
AosTaskTrans(AosTransType::eSendFmt, svr_id, 
		AosProcessType::eCube, cube_grp_id, false, true),
mCubeGrpId(cube_grp_id),
mFmtBuff(fmt_buff)
{
	setIsSystemTrans(); 
}


AosSendFmtTrans::~AosSendFmtTrans()
{
}


bool
AosSendFmtTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mCubeGrpId = buff->getU32(0);
	u32 log_len = buff->getU32(0);
	mFmtBuff = buff->getBuff(log_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosSendFmtTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU32(mCubeGrpId );
	buff->setU32(mFmtBuff->dataLen());
	buff->setBuff(mFmtBuff);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosSendFmtTrans::clone()
{
	return OmnNew AosSendFmtTrans(false);
}


bool
AosSendFmtTrans::proc()
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);
	
	bool rslt = fmt_mgr->recvFmt(
		mRdata.getPtrNoLock(), mCubeGrpId, getFromSvrId(), mFmtBuff);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	resp_buff->setU8(rslt);

	sendResp(resp_buff);
	return true;
}


