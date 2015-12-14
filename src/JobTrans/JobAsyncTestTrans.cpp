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
#include "JobTrans/JobAsyncTestTrans.h"

#include "Porting/Sleep.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("AosJobAsyncTestTrans::TestCallbackThrd", __FILE__, __LINE__);

AosJobAsyncTestTrans::AosJobAsyncTestTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAsyncTestJobTrans, regflag)
{
}


AosJobAsyncTestTrans::AosJobAsyncTestTrans(
		const int svr_id,
		const i64 req_size,
		const i64 resp_size, 
		const i64 sleep,
		const AosDataProcObjPtr &resp_caller,
		const AosRundataPtr &rdata)
:
AosTaskTrans(AosTransType::eAsyncTestJobTrans, svr_id, false, true),
mReqSize(req_size),
mRespSize(resp_size),
mSleep(sleep),
mRespCaller(resp_caller)
{
	mBuff = OmnNew AosBuff(mReqSize+10 AosMemoryCheckerArgs);
	mBuff->setDataLen(mReqSize);
}


AosJobAsyncTestTrans::~AosJobAsyncTestTrans()
{
}


bool
AosJobAsyncTestTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	mReqSize = buff->getI64(0);
	mRespSize = buff->getI64(0);
	mSleep = buff->getI64(0);
	return true;
}


bool
AosJobAsyncTestTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setAosBuff(mBuff);
	buff->setI64(mReqSize);
	buff->setI64(mRespSize);
	buff->setI64(mSleep);
	return true;
}


AosTransPtr
AosJobAsyncTestTrans::clone()
{
	return OmnNew AosJobAsyncTestTrans(false);
}


bool
AosJobAsyncTestTrans::proc()
{
	if (mSleep != 0) OmnSleep(mSleep);
	AosBuffPtr resp_buff = OmnNew AosBuff(mReqSize AosMemoryCheckerArgs);
	resp_buff->setDataLen(mReqSize);
	sendResp(resp_buff);
	return true;
}

bool 
AosJobAsyncTestTrans::respCallBack() 
{
	AosBuffPtr resp = getResp();                       
	bool svr_death = isSvrDeath();                     
	mRespCaller->callback(resp, svr_death);
	return true;
}

