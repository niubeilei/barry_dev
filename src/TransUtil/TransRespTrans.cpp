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
// 10/10/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/TransRespTrans.h"

#include "SEInterfaces/TransCltObj.h"
#include "SEInterfaces/TransSvrObj.h"
#include "API/AosApi.h"


AosTransRespTrans::AosTransRespTrans(const bool reg_flag)
:
AosTaskTrans(AosTransType::eTransResp, reg_flag)
{
	setIsSystemTrans();
}


AosTransRespTrans::AosTransRespTrans(
		const int to_svr_id,
		const u32 to_proc_id,
		const AosTransId req_id,
		const bool req_is_sync_resp,
		const AosBuffPtr &resp,
		const bool need_save)
:
AosTaskTrans(AosTransType::eTransResp, to_svr_id, to_proc_id, false, false),
mReqId(req_id),
mReqIsSyncResp(req_is_sync_resp),
mResp(resp)
{
	setIsSystemTrans();
}


AosTransRespTrans::~AosTransRespTrans()
{
}


bool
AosTransRespTrans::serializeFrom(const AosBuffPtr &buff)
{
	AosTaskTrans::serializeFrom(buff);
	
	mReqId = AosTransId::serializeFrom(buff);
	mReqIsSyncResp = buff->getU8(0);
	u32 resp_len = buff->getU32(0);
	aos_assert_r(resp_len, false);
	mResp = buff->getBuff(resp_len, true AosMemoryCheckerArgs);
	return true;
}


bool
AosTransRespTrans::serializeTo(const AosBuffPtr &buff)
{
	AosTaskTrans::serializeTo(buff);

	aos_assert_r(mResp, false);
	mReqId.serializeTo(buff);
	buff->setU8(mReqIsSyncResp);
	buff->setU32(mResp->dataLen());
	buff->setBuff(mResp);

	return true;
}


//AosConnMsgPtr
AosTransPtr
AosTransRespTrans::clone()
{
	return OmnNew AosTransRespTrans(false);
}


bool
AosTransRespTrans::proc()
{
	AosTransCltObjPtr trans_clt = AosTransCltObj::getTransClt();
	aos_assert_r(trans_clt, false);
	
	//OmnScreen << "ttttt TransClient; recv resp:"
	//	<< "; trans_id:" << mReqId.toString()
	//	<< "; resp_trans_id:" << getTransId().toString()
	//	<< "; from_sid:" << getFromSvrId() 
	//	<< endl;
	
	trans_clt->recvResp(mReqId, mResp, getFromSvrId());
	return true;
}

bool	
AosTransRespTrans::directProc()
{
	if (mReqIsSyncResp)
	{
		proc();	
		AosTransSvrObj::getTransSvr()->resetCrtCacheSize(getSize());
	}
	else
	{
		AosTaskTrans::directProc();
	}
	return true;
}

