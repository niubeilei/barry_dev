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
#include "JobTrans/JobTestTrans.h"

#include "Porting/Sleep.h"

AosJobTestTrans::AosJobTestTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eTestJobTrans, regflag)
{
}


AosJobTestTrans::AosJobTestTrans(
		const int svr_id,
		const i64 req_size,
		const i64 resp_size, 
		const i64 sleep,
		const AosRundataPtr &rdata)
:
AosTaskTrans(AosTransType::eTestJobTrans, svr_id, false, false),
mReqSize(req_size),
mRespSize(resp_size),
mSleep(sleep)
{
	mBuff = OmnNew AosBuff(mReqSize+10 AosMemoryCheckerArgs);
	mBuff->setDataLen(mReqSize);
}


AosJobTestTrans::~AosJobTestTrans()
{
}


bool
AosJobTestTrans::serializeFrom(const AosBuffPtr &buff)
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
AosJobTestTrans::serializeTo(const AosBuffPtr &buff)
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
AosJobTestTrans::clone()
{
	return OmnNew AosJobTestTrans(false);
}


bool
AosJobTestTrans::proc()
{
	if (mSleep != 0) OmnSleep(mSleep);
	AosBuffPtr resp_buff = OmnNew AosBuff(mRespSize AosMemoryCheckerArgs);
	resp_buff->setU8(true);
	//sendResp(resp_buff);
	return true;
}

bool 
AosJobTestTrans::respCallBack() 
{
	return true;
}


