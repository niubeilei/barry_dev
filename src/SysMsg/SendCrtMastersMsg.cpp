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
#include "SysMsg/SendCrtMastersMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/SvrProxyObj.h"

AosSendCrtMastersMsg::AosSendCrtMastersMsg(const bool regflag)
:
AosAppMsg(MsgType::eSendCrtMastersMsg, regflag)
{
}

AosSendCrtMastersMsg::AosSendCrtMastersMsg(
		const int to_svr_id,
		const bool from_file,
		const AosBuffPtr &masters_buff)
:
AosAppMsg(MsgType::eSendCrtMastersMsg, to_svr_id, AOSTAG_SVRPROXY_PID),
mGetBuffSucc(true),
mFromFile(from_file),
mMastersBuff(masters_buff)
{
}


AosSendCrtMastersMsg::AosSendCrtMastersMsg(const int to_svr_id)
:
AosAppMsg(MsgType::eSendCrtMastersMsg, to_svr_id, AOSTAG_SVRPROXY_PID),
mGetBuffSucc(false),
mFromFile(false)
{
}


AosSendCrtMastersMsg::~AosSendCrtMastersMsg()
{
}

bool
AosSendCrtMastersMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mGetBuffSucc = buff->getU8(0);
	if(!mGetBuffSucc)	return true;

	mFromFile = buff->getU8(0);

	u32 masters_buff_len = buff->getU32(0);
	aos_assert_r(masters_buff_len != 0, false);
	
	mMastersBuff = buff->getBuff(masters_buff_len, true AosMemoryCheckerArgs); 
	return true;
}


bool
AosSendCrtMastersMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff);
	aos_assert_r(rslt, false);
	
	buff->setU8(mGetBuffSucc);
	if(!mGetBuffSucc)	return true;

	buff->setU8(mFromFile);
	buff->setU32(mMastersBuff->dataLen());
	buff->setBuff(mMastersBuff);
	return true;
}


AosConnMsgPtr
AosSendCrtMastersMsg::clone2()
{
	return OmnNew AosSendCrtMastersMsg(false);
}


bool
AosSendCrtMastersMsg::proc()
{
	AosSvrProxyObjPtr svr_proxy = AosSvrProxyObj::getSvrProxy();
	aos_assert_r(svr_proxy, false);
	
	if(mGetBuffSucc)	return svr_proxy->recvMastersBuff(getFromSvrId(),
			mFromFile, mMastersBuff);
	
	return svr_proxy->getMastersFailed(getFromSvrId());
}


