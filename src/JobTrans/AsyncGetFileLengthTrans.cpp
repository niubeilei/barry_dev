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
#include "JobTrans/AsyncGetFileLengthTrans.h"

#include "API/AosApi.h"
#include "XmlUtil/XmlTag.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "JobTrans/AsyncGetFileLengthCbTrans.h"


AosAsyncGetFileLengthTrans::AosAsyncGetFileLengthTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eAsyncGetFileLength, regflag)
{
}


AosAsyncGetFileLengthTrans::AosAsyncGetFileLengthTrans(
		const int callerId,
		const u64 &fileId,
		const int svr_id)
:
AosTaskTrans(AosTransType::eAsyncGetFileLength, svr_id, false, false),
mCallerId(callerId),
mFileId(fileId)
{
}


AosAsyncGetFileLengthTrans::~AosAsyncGetFileLengthTrans()
{
}


bool
AosAsyncGetFileLengthTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mFileId = buff->getU64(0);
	mCallerId = buff->getInt(-1);
	return true;
}


bool
AosAsyncGetFileLengthTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU64(mFileId);
	buff->setInt(mCallerId);
	return true;
}


AosTransPtr
AosAsyncGetFileLengthTrans::clone()
{
	return OmnNew AosAsyncGetFileLengthTrans(false);
}


bool
AosAsyncGetFileLengthTrans::proc()
{
	return true;
}
