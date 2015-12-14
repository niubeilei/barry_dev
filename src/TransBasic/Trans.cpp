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
#include "TransBasic/Trans.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "Thread/ThreadPool.h"
#include "TransUtil/TransRespTrans.h"
#include "SEInterfaces/TransSvrObj.h"
#include "TransUtil/TransAckMsg.h"
#include "TransUtil/CubicTrans.h"
#include "TransUtil/TaskTrans.h"

//AosTransPtr  sgTrans[AosTransType::eOprMax];
static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("AsyncReqTransRespThrd", __FILE__, __LINE__);

AosTrans::AosTrans(
		const AosTransType::E type,
		const bool reg_flag)
:
AosAppMsg(type, reg_flag),
mIsRecoverd(false)
{
	OmnString str;
	str << type << ":" << __FILE__;
	const char *memory_checker_fname = str.data();
	const int memory_checker_lineno = __LINE__;
	AosMemoryCheckerObjCreated(AosClassName::eAosTrans);
}


AosTrans::AosTrans(
		const AosTransType::E type,
		const int to_svrid,
		const u32 to_proc_id,
		const bool need_save,
		const bool need_resp)
:
AosAppMsg(type, to_svrid, to_proc_id), 
mAttrFlag(0),
mLevelFlag(0),
mSvrDeath(false),
mIsRecoverd(false)
{
	OmnString str;
	str << type << ":" << __FILE__;
	const char *memory_checker_fname = str.data();
	const int memory_checker_lineno = __LINE__;
	AosMemoryCheckerObjCreated(AosClassName::eAosTrans);
	
	if(need_save)	mAttrFlag |= eNeedSaveFlag;
	if(need_resp)	mAttrFlag |= eNeedRespFlag;
	
	mTransId.mFromSid = 0;
	mTransId.mFromPid = 0;
	mTransId.mSeqno = 0;
}

AosTrans::AosTrans(
		const AosTransType::E type,
		const int to_svrid,
		const AosProcessType::E to_proc_tp,
		const int cube_grp_id,
		const bool need_save,
		const bool need_resp)
:
AosAppMsg(type, to_svrid, to_proc_tp, cube_grp_id), 
mAttrFlag(0),
mLevelFlag(0),
mSvrDeath(false),
mIsRecoverd(false)
{
	OmnString str;
	str << type << ":" << __FILE__;
	const char *memory_checker_fname = str.data();
	const int memory_checker_lineno = __LINE__;
	AosMemoryCheckerObjCreated(AosClassName::eAosTrans);
	
	if(need_save)	mAttrFlag |= eNeedSaveFlag;
	if(need_resp)	mAttrFlag |= eNeedRespFlag;
	
	mTransId.mFromSid = 0;
	mTransId.mFromPid = 0;
	mTransId.mSeqno = 0;
}



AosTrans::~AosTrans()
{
	//if(mIsRecoverd)
	//{
	//OmnScreen << "destruct recoverd trans:" << mTransId.toString()
	//	<< endl;
	//}
	AosMemoryCheckerObjDeleted(AosClassName::eAosTrans);
}
	

AosTransPtr
AosTrans::getNextTrans(const AosBuffPtr &cont)
{
	// call this func is the TransServer.
	// read from file. format data_len - trans_data
	
	AosBuffPtr trans_buff = getNextTransBuff(cont);
	if(!trans_buff)	return 0;
	
	return serializeFromStatic(trans_buff);
}

	
AosBuffPtr
AosTrans::getNextTransBuff(const AosBuffPtr &cont)
{
	u32 data_size = getNextTransSize(cont);
	if(data_size == 0)	return 0;

	AosBuffPtr trans_buff = cont->getBuff(data_size, false AosMemoryCheckerArgs);
	return trans_buff;	
}

u32
AosTrans::getNextTransSize(const AosBuffPtr &cont)
{
	aos_assert_r(cont, 0);
	
	//if(cont->getCrtIdx() >= cont->dataLen())	return 0;
	u32 data_size = cont->getU32(0);
	if(!data_size)	return 0;
	
	int remain_size = cont->dataLen() - cont->getCrtIdx(); 
	if(remain_size < (int)data_size)
	{
		OmnScreen << "read trans failed. remain_size:" << remain_size
			<< "; data_size:" << data_size << endl;
		return 0;
	}
	
	return data_size;
}

AosTransId
AosTrans::getTransIdFromBuff(const AosBuffPtr &buff)
{
	aos_assert_r(buff, AosTransId::Invalid);
	int crt_idx = buff->getCrtIdx();
	
	buff->setCrtIdx(crt_idx + AosAppMsg::eAppMsgLen);	
	
	AosTransId tid = AosTransId::serializeFrom(buff);

	buff->setCrtIdx(crt_idx);
	return tid;
}


AosTransPtr
AosTrans::serializeFromStatic(const AosBuffPtr &buff)
{
	AosConnMsgPtr msg = AosConnMsg::serializeFromStatic(
			buff, false);
	aos_assert_r(msg && msg->isAppMsg(), 0);	
	
	AosAppMsgPtr app_msg = (AosAppMsg *)msg.getPtr();
	aos_assert_r(app_msg && app_msg->isTrans(), 0);	
	
	AosTransPtr trans = (AosTrans *)app_msg.getPtr();
	return trans;
}


AosConnMsgPtr
AosTrans::clone2()
{
	AosTransPtr new_trans = clone();
	AosConnMsgPtr msg = (AosConnMsg *)new_trans.getPtr();
	return msg;
}

bool
AosTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeFrom(buff); 
	aos_assert_r(rslt, false);
	
	mTransId = AosTransId::serializeFrom(buff);
	mAttrFlag = buff->getU8(0);
	mLevelFlag  = buff->getU8(0);

	// Test
	mRdata = AosRundata::getRdataByBuff(buff AosMemoryCheckerArgs);
	
	aos_assert_r(mTransId != AosTransId::Invalid, false);
	return true;
}


bool 
AosTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosAppMsg::serializeTo(buff); 
	aos_assert_r(rslt, false);
	aos_assert_r(buff->getCrtIdx() == AosAppMsg::eAppMsgLen, false);

	mTransId.serializeTo(buff);
	buff->setU8(mAttrFlag);
	buff->setU8(mLevelFlag);

	// Test
	mRdata->addToBuff(buff);
	return true;
}



AosConnMsgPtr
AosTrans::serializeStatic(
		const AosBuffPtr &buff,
		const u32 beg_idx,
		const AosTransType::E tp)
{
	// for SvrProxy serialize.	
	aos_assert_r(buff, 0);
	
	u32 crt_idx = buff->getCrtIdx();
	buff->setCrtIdx(crt_idx + eLevelFlagOff);
	bool level_flag = buff->getU8(0);
	bool is_cube_trans = ((level_flag & eIsCubeTransFlag) == eIsCubeTransFlag); 

	AosConnMsgPtr msg;
	if(is_cube_trans)
	{
		msg = OmnNew AosCubicTrans(tp, false);
	}
	else
	{
		msg = OmnNew AosTaskTrans(tp, false);
	}
	
	buff->setCrtIdx(beg_idx);
	msg->serializeFrom(buff);
	return msg;
}


void
AosTrans::setRundata(const AosRundataPtr &rdata)
{ 
	aos_assert(rdata);
	mRdata = rdata;
}


void
AosTrans::setIsResend()
{ 
	mAttrFlag |= eIsResendFlag; 
	//saveAttrFlagToConnBuff();		// tmp. remove to TransClient.
}

void
AosTrans::setGetResp()
{
	mAttrFlag |= eIsGetRespFlag;
	//saveAttrFlagToConnBuff();		not need. just cube trans svr need this.
}

void
AosTrans::saveAttrFlagToConnBuff()
{
	if(!mConnBuff)	return;
	int idx = AosAppMsg::eAppMsgLen + eAttrFlagOff; 
	mConnBuff->setCrtIdx(idx);
	mConnBuff->setU8(mAttrFlag);
}
	
bool
AosTrans::sendResp(const AosBuffPtr &resp)
{
	// Ketty 2014/06/24
	//if(mIsRecoverd)	 return true;
	
	aos_assert_r(resp, false);

	int to_svr_id = getFromSvrId();
	u32 to_proc_id = getFromProcId();
	AosTransId req_id = getTransId();

	AosTransPtr trans = OmnNew AosTransRespTrans(
			to_svr_id, to_proc_id, req_id, isSyncResp(), resp, isNeedSave());

	//OmnScreen << "TransServer:: sendResp; orig_trans_id:" << mTransId.toString()
	//	<< "; to_svr_id:" << to_svr_id
	//	<< "; to_proc_id:" << to_proc_id
	//	<< endl;

	OmnThrdShellProcPtr runner = OmnNew SendRespThrd(mRdata, trans);
	return sgThreadPool->proc(runner);
}

bool
AosTrans::sendAck()
{
	int to_svr_id = getFromSvrId();
	u32 to_proc_id = getFromProcId();
	AosTransId trans_id = getTransId();
	aos_assert_r(trans_id != AosTransId::Invalid && to_svr_id >=0 && to_proc_id >=1, false);

	AosAppMsgPtr ack_msg = OmnNew AosTransAckMsg(
		to_svr_id, to_proc_id, trans_id);
	return AosSendMsg(ack_msg);
}


bool
AosTrans::SendRespThrd::run()
{
	bool svr_death;
	bool rslt = AosSendTrans(mRdata, mRespTrans, svr_death);
	aos_assert_r(rslt, false);
	if(svr_death)
	{
		OmnScreen << "send resp error! svr_death: " 
			<< mRespTrans->getToSvrId() << endl;
	}
	return true;
}


bool
AosTrans::directProc()
{
	OmnShouldNeverComeHere;
OmnAlarm << "MMMMMMMMMMMMMMMMMM" << enderr;

	bool rslt = proc();
	AosTransSvrObj::getTransSvr()->resetCrtCacheSize(getSize());
	return rslt;
}

void
AosTrans::setNeedSyncResp()
{
	mAttrFlag |= eNeedRespFlag;
	mAttrFlag |= eIsSyncRespFlag;
}
	

void
AosTrans::setNeedAsyncResp()
{
	mAttrFlag |= eNeedRespFlag;
	mAttrFlag &= ~eIsSyncRespFlag;
}

bool
AosTrans::ackRecved()
{
	if(isNeedResp())	return true;
	
	if(mSem)
	{
		mSem->post();
	}
	return true;
}

bool
AosTrans::waitAck()
{
	aos_assert_r(mSem, false);
	if (isNeedResp())
	{
		bool timeout = false;
		mSem->timedWait(eAckWaitTime, timeout);
		if (timeout)
		{
			OmnAlarm << "ack wait time out, transId: " << getTransId().toString() << enderr;
		}
	}
	else
	{
		mSem->wait();
	}
	return true;
}
	

void
AosTrans::setIsRespRecved()
{
	mAttrFlag |= eIsRespRecvedFlag;
}

bool
AosTrans::respRecved(const AosBuffPtr &resp)
{
	mResp = resp;
	setIsRespRecved();

	if(!isNeedResp())
	{
		OmnScreen << "This trans don't need resp." << endl;
		return true;
	}
	if(isSyncResp())
	{
		aos_assert_r(mSem, false);
		mSem->post();
		return true;
	}
	
	return respCallBack();
}

AosBuffPtr
AosTrans::waitResp()
{
	if(isSyncResp())
	{
		aos_assert_r(mSem, 0);
		mSem->wait();
	}
	return mResp;
}

bool
AosTrans::setSvrDeath()
{
	mSvrDeath = true;
	
	if(!isNeedResp())
	{
		if(mSem)
		{
			mSem->post();
		}
		return true;
	}
	
	if(isSyncResp())
	{
		aos_assert_r(mSem, false);
		mSem->post();
		return true;
	}

	return respCallBack();
}


