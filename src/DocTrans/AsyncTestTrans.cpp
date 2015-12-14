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
// 11/19/2015	Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "DocTrans/AsyncTestTrans.h"

#include "Util/MemoryChecker.h"
#include "Porting/Sleep.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("AsyncTestTrans::TestCallbackThrd", __FILE__, __LINE__);

AosAsyncTestTrans::AosAsyncTestTrans(const bool regflag)
:
AosDocTrans(AosTransType::eAsyncTestDocTrans, regflag)
{
}

AosAsyncTestTrans::AosAsyncTestTrans(
		const u64 docid,
		const i64 req_size,
		const i64 resp_size, 
		const i64 sleep,
		const AosDataProcObjPtr &resp_caller,
		const AosRundataPtr &rdata)
:
AosDocTrans(AosTransType::eAsyncTestDocTrans, docid, false, true, 0), 
mReqSize(req_size),
mRespSize(resp_size),
mSleep(sleep),
mRespCaller(resp_caller)
{
	mBuff = OmnNew AosBuff(mReqSize AosMemoryCheckerArgs);
	mBuff->setDataLen(mReqSize);
}


AosAsyncTestTrans::~AosAsyncTestTrans()
{
}


bool
AosAsyncTestTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	mReqSize = buff->getI64(0);
	mRespSize = buff->getI64(0);
	mSleep = buff->getI64(0);
	return true;
}


bool
AosAsyncTestTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosDocTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setAosBuff(mBuff);
	buff->setI64(mReqSize);
	buff->setI64(mRespSize);
	buff->setI64(mSleep);
	return true;
}


AosTransPtr
AosAsyncTestTrans::clone()
{
	return OmnNew AosAsyncTestTrans(false);
}


bool
AosAsyncTestTrans::proc()
{
	if (mSleep != 0) OmnSleep(mSleep);
	AosBuffPtr resp_buff = OmnNew AosBuff(mRespSize AosMemoryCheckerArgs);
	resp_buff->setDataLen(mRespSize);
	sendResp(resp_buff);
	return true;
}


bool 
AosAsyncTestTrans::respCallBack() 
{
	AosBuffPtr resp = getResp();                       
	bool svr_death = isSvrDeath();                     
	mRespCaller->callback(resp, svr_death);
	return true;
}

