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
// 07/28/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#if 0
#include "JobTrans/AsyncReadFileCbTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"


AosAsyncReadFileCbTrans::AosAsyncReadFileCbTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAsyncReadFileCb, regflag)
{
}


AosAsyncReadFileCbTrans::AosAsyncReadFileCbTrans(
		const AosAioCaller::Parameter &parm,
		const int callerId,
		const AosBuffPtr &buff,
		const int svr_id,
		const u32 proc_id,
		const bool need_save,
		const bool need_resp)
:
AosTaskTrans(AosTransType::eAsyncReadFileCb,
		svr_id, proc_id,  need_save, need_resp),
mParm(parm),
mCallerId(callerId),
mBuff(buff)
{
}


AosAsyncReadFileCbTrans::~AosAsyncReadFileCbTrans()
{
}


bool
AosAsyncReadFileCbTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mCallerId = buff->getInt(-1);
	u32 len = buff->getU32(0);
	if (len > 0)
	{
		mBuff = buff->getBuff(len, true AosMemoryCheckerArgs);
	}
	else
	{
		mBuff = OmnNew AosBuff(AosMemoryCheckerArgsBegin);
		mBuff->setDataLen(0);
	}
	mParm.serializeFrom(buff);
	return true;
}


bool
AosAsyncReadFileCbTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setInt(mCallerId);
	buff->setU32(mBuff->dataLen());
	if (mBuff->dataLen() > 0)
	{
		buff->setBuff(mBuff);
	}
	mParm.serializeTo(buff);
	return true;
}


AosTransPtr
AosAsyncReadFileCbTrans::clone()
{
	return OmnNew AosAsyncReadFileCbTrans(false);
}


bool
AosAsyncReadFileCbTrans::proc()
{
if (mBuff)
{
OmnScreen << "========== " << mCallerId << " : " << mParm.reqId << " : " << mBuff->dataLen() << endl;
}
	AosAioCallerPtr caller;
	AosNetFileMgr::getSelf()->getCaller(caller, mCallerId, mRdata);
	caller->dataRead(mParm, mBuff);
	return true;
}
#endif 
