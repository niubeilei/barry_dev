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
#include "JobTrans/CreateRaidFileTrans.h"

#include "DocFileMgr/RaidFile.h"
#include "SEInterfaces/NetFileMgrObj.h"
#include "XmlUtil/XmlTag.h"


AosCreateRaidFileTrans::AosCreateRaidFileTrans(const bool regflag)
:
AosTaskTrans(AosTransType::eCreateRaidFile, regflag)
{
}


AosCreateRaidFileTrans::AosCreateRaidFileTrans(
		const int svr_id,
		const OmnString &fname_prefix,
		const u64 &requested_space,
		const bool reserve_flag)
:
AosTaskTrans(AosTransType::eCreateRaidFile, svr_id, true, true),
mFnamePrefix(fname_prefix),
mRequestedSpace(requested_space),
mReserveFlag(reserve_flag)
{
	if (fname_prefix.length() > 200)
	{
		mFnamePrefix.setLength(200);
	}
}


AosCreateRaidFileTrans::~AosCreateRaidFileTrans()
{
}


bool
AosCreateRaidFileTrans::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeFrom(buff);
	aos_assert_r(rslt, false);

	mFnamePrefix = buff->getOmnStr("");
	mRequestedSpace = buff->getU64(0);
	mReserveFlag = buff->getU8(0);

	return true;
}


bool
AosCreateRaidFileTrans::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosTaskTrans::serializeTo(buff);
	aos_assert_r(rslt, false);

	buff->setOmnStr(mFnamePrefix);
	buff->setU64(mRequestedSpace);
	buff->setU8(mReserveFlag);

	return true;
}


AosTransPtr
AosCreateRaidFileTrans::clone()
{
	return OmnNew AosCreateRaidFileTrans(false);
}


bool
AosCreateRaidFileTrans::proc()
{
	u64 fileId = 0;
	OmnLocalFilePtr file = AosNetFileMgrObj::createRaidFileStatic(
		mRdata.getPtr(), fileId, mFnamePrefix, mRequestedSpace, mReserveFlag);

	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);

	if (file && file->isGood())
	{
		resp_buff->setU8(true);
		resp_buff->setU64(fileId);

		AosRaidFile * rfile = dynamic_cast<AosRaidFile *>(file.getPtr());
		bool rslt = rfile->serializeTo(resp_buff);
		if (!rslt) 
		{
			resp_buff->reset();
			resp_buff->setU8(false);
			resp_buff->setDataLen(sizeof(u8));
		}
	}
	else
	{
		resp_buff->setU8(false);
	}

	sendResp(resp_buff);
	return true;
}

