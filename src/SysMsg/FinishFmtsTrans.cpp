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
#if 0
#include "SysMsg/FinishFmtsTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/FmtMgrObj.h"

AosFinishFmtsTrans::AosFinishFmtsTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eFinishFmts, regflag)
{
	setIsSystemTrans();
}


AosFinishFmtsTrans::AosFinishFmtsTrans(
		const int svr_id,
		const u32 cube_grp_id,
		const u64 max_fmt_id,
		const int crt_svr_id)
:
AosTaskTrans(AosTransType::eFinishFmts, svr_id,
		AosProcessType::eCube, cube_grp_id, false, false),
mMaxFmtId(max_fmt_id),
mCrtSvrId(crt_svr_id)
{
	setIsSystemTrans();
}


AosFinishFmtsTrans::~AosFinishFmtsTrans()
{
}


bool
AosFinishFmtsTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mMaxFmtId = buff->getU64(0);
	mCrtSvrId = buff->getInt(-1);
	return true;
}


bool
AosFinishFmtsTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mMaxFmtId);
	buff->setInt(mCrtSvrId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosFinishFmtsTrans::clone()
{
	return OmnNew AosFinishFmtsTrans(false);
}


bool
AosFinishFmtsTrans::proc()
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);
	
	fmt_mgr->recvFinishFmts(mMaxFmtId, mCrtSvrId); 
	return true;
}

#endif
