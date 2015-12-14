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
#include "TransBasic/AppMsg.h"

#include "API/AosApi.h"
#include "SEInterfaces/TransSvrObj.h"
#include "TransBasic/Trans.h"

AosAppMsg::AosAppMsg(
		const MsgType::E tp,
		const bool reg_flag)
:
AosConnMsg(tp, reg_flag)
{
}


AosAppMsg::AosAppMsg(
		const MsgType::E tp,
		const int to_svrid,
		const u32 to_proc_id)
:
AosConnMsg(tp, false),
mFromSvrId(AosGetSelfServerId()),
mFromProcId(AosGetSelfProcId()),
mToSvrIdOff(-1),
mToSvrId(to_svrid),
mToProcId(to_proc_id),
mToProcType(AosProcessType::eInvalid),
mCubeGrpId(-1),
mRlbSeqOff(-1),
mRlbSeq(0)
{
}


AosAppMsg::AosAppMsg(
		const MsgType::E tp,
		const int to_svrid,
		const AosProcessType::E to_proc_tp,
		const int cube_grp_id)
:
AosConnMsg(tp, false),
mFromSvrId(AosGetSelfServerId()),
mFromProcId(AosGetSelfProcId()),
mToSvrIdOff(-1),
mToSvrId(to_svrid),
mToProcId(0),
mToProcType(to_proc_tp),
mCubeGrpId(cube_grp_id),
mRlbSeqOff(-1),
mRlbSeq(0)
{
}


AosAppMsg::~AosAppMsg()
{
}


bool
AosAppMsg::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);
	u32 crt_idx = buff->getCrtIdx();
	
	buff->setInt(mFromSvrId);
	buff->setU32(mFromProcId);
	
	mToSvrIdOff = buff->getCrtIdx();
	buff->setInt(mToSvrId);
	buff->setU32(mToProcId);
	buff->setU32(mToProcType);
	buff->setInt(mCubeGrpId);
	
	mRlbSeqOff = buff->getCrtIdx();
	buff->setU32(mRlbSeq);
	
	aos_assert_r(buff->getCrtIdx() == crt_idx + eIsTransOff, false);
	buff->setU8(isTrans());		// for Svrproxy serialize
	
	aos_assert_r(buff->getCrtIdx() == crt_idx + eDataLen, false);
	return true;
}


bool
AosAppMsg::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosConnMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);

	mFromSvrId = buff->getInt(-1);
	mFromProcId = buff->getU32(0);
	
	mToSvrIdOff = buff->getCrtIdx();
	mToSvrId = buff->getInt(-1);
	mToProcId = buff->getU32(0);
	mToProcType = (AosProcessType::E)buff->getU32(0);
	mCubeGrpId = buff->getInt(-1);

	mRlbSeqOff = buff->getCrtIdx();
	mRlbSeq = buff->getU32(0);

	bool is_trans = buff->getU8(0);
	aos_assert_r(is_trans == isTrans(), false);
	return true;
}


AosConnMsgPtr
AosAppMsg::serializeStatic(
		const AosBuffPtr &buff, 
		const u32 beg_idx,
		const MsgType::E tp)
{
	// for SvrProxy serialize.
	aos_assert_r(buff, 0);
	
	u32 crt_idx = buff->getCrtIdx();
	buff->setCrtIdx(crt_idx + eIsTransOff);
	bool is_trans = buff->getU8(0);
	
	if(is_trans)
	{
		buff->setCrtIdx(crt_idx + eDataLen);
		return AosTrans::serializeStatic(buff, beg_idx, tp);
	}
	
	AosConnMsgPtr msg = OmnNew AosAppMsg(tp, false);
	buff->setCrtIdx(beg_idx);
	msg->serializeFrom(buff);
	return msg;
}


void
AosAppMsg::setToSvrId(const int svr_id)
{
	// for cube trans.
	mToSvrId = svr_id;
	if(!mConnBuff)	return;
	
	mConnBuff->setCrtIdx(mToSvrIdOff);
	mConnBuff->setInt(svr_id);
}


void
AosAppMsg::setToProcId(const u32 proc_id)
{
	// for cube trans.
	mToProcId = proc_id;
	if(!mConnBuff)	return;
	
	mConnBuff->setCrtIdx(mToSvrIdOff + sizeof(int));
	mConnBuff->setU32(proc_id);
}
	

void
AosAppMsg::setRlbSeq(const u32 rlb_seq)
{ 
	mRlbSeq = rlb_seq; 
	
	if(!mConnBuff)	return;
	
	mConnBuff->setCrtIdx(mRlbSeqOff);
	mConnBuff->setU32(rlb_seq);
}


AosConnMsgPtr
AosAppMsg::clone2()
{ 
	OmnShouldNeverComeHere; 
	return 0; 
}


bool
AosAppMsg::proc()
{ 
	OmnShouldNeverComeHere; 
	return false;
}


bool
AosAppMsg::directProc()
{
	bool rslt = proc();
	AosTransSvrObj::getTransSvr()->resetCrtCacheSize(getSize());
	return rslt;
}


