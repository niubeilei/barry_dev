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
#include "SysMsg/DiskBadMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosDiskBadMsg::AosDiskBadMsg(const bool regflag)
:
AosAppMsg(MsgType::eDiskBadMsg, regflag)
{
}


AosDiskBadMsg::AosDiskBadMsg(
		const int to_svr_id,
		const u32 to_proc_id)
:
AosAppMsg(MsgType::eDiskBadMsg, to_svr_id, to_proc_id),
mLogicProcId(AosGetSelfProcId()),
mActualProcId(getpid())
{
}


AosDiskBadMsg::~AosDiskBadMsg()
{
}


bool
AosDiskBadMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mLogicProcId = buff->getU32(0);
	mActualProcId = buff->getU32(0);
	return true;
}


bool
AosDiskBadMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setU32(mLogicProcId);
	buff->setU32(mActualProcId);
	return true;
}


AosConnMsgPtr
AosDiskBadMsg::clone2()
{
	return OmnNew AosDiskBadMsg(false);
}


bool
AosDiskBadMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	OmnScreen << "Child check disk error! logic_pid:" << mLogicProcId
		<< "; actual_pid:" << mActualProcId << endl;	

	svr_proxy->diskDamaged();
	return true;
}


