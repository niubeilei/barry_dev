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
// 2014/08/14	Created by Ken Lee
////////////////////////////////////////////////////////////////////////////
#include "JobTrans/CreateTempFileTrans.h"

#include "SEInterfaces/NetFileMgrObj.h"
#include "Util/File.h"
#include "XmlUtil/XmlTag.h"


AosCreateTempFileTrans::AosCreateTempFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eCreateTempFile, regflag)
{
}


AosCreateTempFileTrans::AosCreateTempFileTrans(
		const int svr_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
:
AosTaskTrans(AosTransType::eCreateTempFile, svr_id, false, true),
mFnamePrefix(fname_prefix),
mRequestedSpace(requested_space),
mReserveFlag(reserve_flag)
{
	if (fname_prefix.length() > 200)
	{
		mFnamePrefix.setLength(200);
	}
}


AosCreateTempFileTrans::~AosCreateTempFileTrans()
{
}


bool
AosCreateTempFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mFnamePrefix = buff->getOmnStr("");
	mRequestedSpace = buff->getU64(0);
	mReserveFlag = buff->getU8(0);

	return true;
}


bool
AosCreateTempFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mFnamePrefix);
	buff->setU64(mRequestedSpace);
	buff->setU8(mReserveFlag);

	return true;
}


AosTransPtr
AosCreateTempFileTrans::clone()
{
	return OmnNew AosCreateTempFileTrans(false);
}


bool
AosCreateTempFileTrans::proc()
{
	u64 fileId = 0;
	OmnFilePtr file = AosNetFileMgrObj::createTempFileStatic(
		mRdata.getPtr(), fileId, mFnamePrefix, mRequestedSpace, mReserveFlag);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	if (file && file->isGood())
	{
		resp_buff->setU8(true);
		resp_buff->setU64(fileId);

		OmnString filename = file->getFileName();
		resp_buff->setOmnStr(filename);
	}
	else
	{
		resp_buff->setU8(false);
	}

	sendResp(resp_buff);
	return true;
}

