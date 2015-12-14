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
// Modification History:
// 2013/07/23	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SysMsg/TransRespMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransCltObj.h"

AosTransRespMsg::AosTransRespMsg(const bool reg_flag)
:
AosAppMsg(MsgType::eTransRespMsg, reg_flag)
{
}


AosTransRespMsg::AosTransRespMsg(
		const int to_svr_id,
		const u32 to_proc_id,
		const AosTransId req_id,
		const AosBuffPtr &resp)
:
AosAppMsg(MsgType::eTransRespMsg, to_svr_id, to_proc_id),
mReqId(req_id),
mResp(resp)
{
}


AosTransRespMsg::~AosTransRespMsg()
{
}


bool
AosTransRespMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);
	
	mReqId = AosTransId::serializeFrom(buff);
	u32 resp_len = buff->getU32(0);
	aos_assert_r(resp_len, false);
	mResp = buff->getBuff(resp_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosTransRespMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);

	aos_assert_r(mResp, false);
	mReqId.serializeTo(buff);
	buff->setU32(mResp->dataLen());
	buff->setBuff(mResp);

	return true;
}


AosConnMsgPtr
AosTransRespMsg::clone2()
{
	return OmnNew AosTransRespMsg(false);
}


bool
AosTransRespMsg::proc()
{
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	trans_clt->recvResp(mReqId, mResp, getFromSvrId());
	return true;
}
#endif
