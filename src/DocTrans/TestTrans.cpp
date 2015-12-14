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
#include "DocTrans/TestTrans.h"

#include "Porting/Sleep.h"

AosDocTestTrans::AosDocTestTrans(const bool regflag)
:
AosDocTrans(AosTransType::eTestDocTrans, regflag)
{
}


AosDocTestTrans::AosDocTestTrans(
		const u64 docid,
		const i64 req_size,
		const i64 resp_size, 
		const i64 sleep,
		const AosRundataPtr &rdata)
:
AosDocTrans(AosTransType::eTestDocTrans, docid, false, false, 0), 
mReqSize(req_size),
mRespSize(resp_size),
mSleep(sleep)
{
	mBuff = OmnNew AosBuff(mReqSize+10 AosMemoryCheckerArgs);
	mBuff->setDataLen(mReqSize);
}


AosDocTestTrans::~AosDocTestTrans()
{
}


bool
AosDocTestTrans::serializeFrom(const AosBuffPtr &buff)
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
AosDocTestTrans::respCallBack()
{
	return true;
}

bool
AosDocTestTrans::serializeTo(const AosBuffPtr &buff)
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
AosDocTestTrans::clone()
{
	return OmnNew AosDocTestTrans(false);
}


bool
AosDocTestTrans::proc()
{
	if (mSleep != 0) OmnSleep(mSleep);
	//AosBuffPtr resp_buff = OmnNew AosBuff(mRespSize AosMemoryCheckerArgs);
	//resp_buff->setU8(true);
	//sendResp(resp_buff);
	return true;
}


