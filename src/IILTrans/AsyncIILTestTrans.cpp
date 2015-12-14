////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 11/19/2015	Created by Andy 
////////////////////////////////////////////////////////////////////////////
#include "IILTrans/AsyncIILTestTrans.h"

#include "Porting/Sleep.h"

static OmnThreadPoolPtr sgThreadPool = OmnNew OmnThreadPool("AsyncIILTestTrans::TestCallbackThrd", __FILE__, __LINE__);

AosAsyncIILTestTrans::AosAsyncIILTestTrans(const bool flag)
:
AosIILTrans(AosTransType::eAsyncTestIILTrans, flag AosMemoryCheckerArgs)
{
}


AosAsyncIILTestTrans::AosAsyncIILTestTrans(
		const OmnString &iilname,
		const i64 req_size,
		const i64 resp_size, 
		const i64 sleep,
		const AosDataProcObjPtr &resp_caller,
		const AosRundataPtr &rdata)
:
AosIILTrans(AosTransType::eAsyncTestIILTrans, iilname, false, 0, false, true AosMemoryCheckerArgs),
mReqSize(req_size),
mRespSize(resp_size),
mSleep(sleep),
mRespCaller(resp_caller)
{
	mBuff = OmnNew AosBuff(mReqSize+10 AosMemoryCheckerArgs);
	mBuff->setDataLen(mReqSize);
}


AosTransPtr 
AosAsyncIILTestTrans::clone()
{
	return OmnNew AosAsyncIILTestTrans(false);
}


bool
AosAsyncIILTestTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mBuff = buff->getAosBuff(true AosMemoryCheckerArgs);
	mReqSize = buff->getI64(0);
	mRespSize = buff->getI64(0);
	mSleep = buff->getI64(0);
	return true;
}


bool
AosAsyncIILTestTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosIILTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setAosBuff(mBuff);
	buff->setI64(mReqSize);
	buff->setI64(mRespSize);
	buff->setI64(mSleep);

	return true;
}


bool
AosAsyncIILTestTrans::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
//	aos_assert_r(iilobj, false);
	if (mSleep != 0) OmnSleep(mSleep);
	resp_buff = OmnNew AosBuff(mReqSize AosMemoryCheckerArgs);
	resp_buff->setDataLen(mReqSize);
	sendResp(resp_buff);
	return true;
}

bool 
AosAsyncIILTestTrans::respCallBack() 
{
	AosBuffPtr resp = getResp();                       
	bool svr_death = isSvrDeath();                     
	mRespCaller->callback(resp, svr_death);
	return true;
}


AosIILType 
AosAsyncIILTestTrans::getIILType() const
{
//	OmnShouldNeverComeHere;
	return eAosIILType_Str;
}

int
AosAsyncIILTestTrans::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

