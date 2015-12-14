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
#include "IILTrans/IILTestTrans.h"

#include "Porting/Sleep.h"

AosIILTestTrans::AosIILTestTrans(const bool flag)
:
AosIILTrans(AosTransType::eTestIILTrans, flag AosMemoryCheckerArgs)
{
}


AosIILTestTrans::AosIILTestTrans(
		const OmnString &iilname,
		const i64 req_size,
		const i64 resp_size, 
		const i64 sleep,
		const AosRundataPtr &rdata)
:
AosIILTrans(AosTransType::eTestIILTrans, iilname, false, 0, false, false AosMemoryCheckerArgs),
mReqSize(req_size),
mRespSize(resp_size),
mSleep(sleep)
{
	mBuff = OmnNew AosBuff(mReqSize+10 AosMemoryCheckerArgs);
	mBuff->setDataLen(mReqSize);
}


AosTransPtr 
AosIILTestTrans::clone()
{
	return OmnNew AosIILTestTrans(false);
}


bool
AosIILTestTrans::serializeFrom(const AosBuffPtr &buff)
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
AosIILTestTrans::serializeTo(const AosBuffPtr &buff)
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
AosIILTestTrans::proc(
		const AosIILObjPtr &iilobj,
		AosBuffPtr &resp_buff,
		const AosRundataPtr &rdata)
{
	if (mSleep != 0) OmnSleep(mSleep);
	return true;
}


AosIILType 
AosIILTestTrans::getIILType() const
{
//	OmnShouldNeverComeHere;
	return eAosIILType_Str;
}


int
AosIILTestTrans::getSerializeSize() const
{
	OmnShouldNeverComeHere;
	return -1;
}

bool 
AosIILTestTrans::respCallBack() 
{
	return true;
}


