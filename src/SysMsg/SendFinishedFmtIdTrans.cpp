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
#include "SysMsg/SendFinishedFmtIdTrans.h"

#include "API/AosApi.h"
#include "SEInterfaces/FmtMgrObj.h"

AosSendFinishedFmtIdTrans::AosSendFinishedFmtIdTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eSendFinishedFmtId, regflag)
{
	setIsSystemTrans();
}


AosSendFinishedFmtIdTrans::AosSendFinishedFmtIdTrans(
		const int svr_id,
		const u32 cube_grp_id,
		const u64 finished_fmtid)
:
AosTaskTrans(AosTransType::eSendFinishedFmtId, svr_id,
		AosProcessType::eCube, cube_grp_id, false, false),
mFinishedFmtId(finished_fmtid)
{
	setIsSystemTrans();
}


AosSendFinishedFmtIdTrans::~AosSendFinishedFmtIdTrans()
{
}


bool
AosSendFinishedFmtIdTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	
	mFinishedFmtId = buff->getU64(0);
	return true;
}


bool
AosSendFinishedFmtIdTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU64(mFinishedFmtId);
	return true;
}


//AosConnMsgPtr
AosTransPtr
AosSendFinishedFmtIdTrans::clone()
{
	return OmnNew AosSendFinishedFmtIdTrans(false);
}


bool
AosSendFinishedFmtIdTrans::proc()
{
	AosFmtMgrObjPtr fmt_mgr = AosFmtMgrObj::getFmtMgr();
	aos_assert_r(fmt_mgr, false);

	fmt_mgr->recvFinishedFmtId(mFinishedFmtId); 
	
	return true;
}


