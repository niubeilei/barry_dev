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
#include "JobTrans/AsyncGetFileLengthCbTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"


AosAsyncGetFileLengthCbTrans::AosAsyncGetFileLengthCbTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAsyncGetFileLengthCb, regflag)
{
}


AosAsyncGetFileLengthCbTrans::AosAsyncGetFileLengthCbTrans(
		const int callerId,
		const int64_t &fileLen,
		const int svr_id)
:
AosTaskTrans(AosTransType::eAsyncGetFileLengthCb, svr_id, false, false),
mCallerId(callerId),
mLength(fileLen)
{
}


AosAsyncGetFileLengthCbTrans::~AosAsyncGetFileLengthCbTrans()
{
}


bool
AosAsyncGetFileLengthCbTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mCallerId = buff->getInt(-1);
	mLength = buff->getI64(-1);
	return true;
}


bool
AosAsyncGetFileLengthCbTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setInt(mCallerId);
	buff->setI64(mLength);
	return true;
}


AosTransPtr
AosAsyncGetFileLengthCbTrans::clone()
{
	return OmnNew AosAsyncGetFileLengthCbTrans(false);
}


bool
AosAsyncGetFileLengthCbTrans::proc()
{
	return true;
}
