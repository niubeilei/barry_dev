//////////////////////////////////////////////////////////////////////////// //
// Copyright (C) 2010
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
// Modification History:
// Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "JimoAgentPackage/JimoAgentFileToLocalResp.h"
#include "Util/File.h"
#include <pwd.h>

AosJimoAgentFileToLocalResp::AosJimoAgentFileToLocalResp()
:
AosJimoAgentPackage(AosAgentPackageType::eFileToLocalResp, AOSTAG_AGENTPACKAGE_FILETOLOCALRESP)
{
}

AosJimoAgentFileToLocalResp::AosJimoAgentFileToLocalResp(const OmnString &resp, const bool rslt)
:
AosJimoAgentPackage(AosAgentPackageType::eFileToLocalResp, AOSTAG_AGENTPACKAGE_FILETOLOCALRESP),
mRslt(rslt),
mResp(resp)
{
}

AosJimoAgentFileToLocalResp::AosJimoAgentFileToLocalResp(
										const OmnString &user,
										const OmnString &local_dir,
										const OmnString &name,
										const int64_t &length,
										const int64_t &offset,
										const AosBuffPtr &buff)
:
AosJimoAgentPackage(AosAgentPackageType::eFileToLocalResp, AOSTAG_AGENTPACKAGE_FILETOLOCALRESP),
mRslt(true),
mResp(""),
mUser(user),
mLocalDir(local_dir),
mFileName(name),
mLength(length),
mOffset(offset),
mBuff(buff)
{
}

bool
AosJimoAgentFileToLocalResp::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeTo(buff);
	aos_assert_r(rslt, false);
	buff->setU8(mRslt);
	buff->setOmnStr(mResp);
	aos_assert_r(mUser != "", false);
	buff->setOmnStr(mUser);
	aos_assert_r(mLocalDir != "", false);
	buff->setOmnStr(mLocalDir);
	aos_assert_r(mFileName != "", false);
	buff->setOmnStr(mFileName);
	aos_assert_r(mLength >= 0, false);
	buff->setI64(mLength);
	aos_assert_r(mOffset >= 0, false);
	buff->setI64(mOffset);
	aos_assert_r(mBuff, false);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);
	return true;
}

bool 
AosJimoAgentFileToLocalResp::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mRslt = buff->getU8(0);
	mResp = buff->getOmnStr("");
	mUser = buff->getOmnStr("");
	aos_assert_r(mUser != "", false);
	mLocalDir = buff->getOmnStr("");
	aos_assert_r(mLocalDir != "", false);
	mFileName = buff->getOmnStr("");
	aos_assert_r(mFileName != "", false);
	mLength = buff->getI64(-1);
	aos_assert_r(mLength >= 0, false);
	mOffset = buff->getI64(-1);
	aos_assert_r(mOffset >= 0, false);
	u32 buff_len = buff->getU32(0);
	mBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(mBuff, false);
	return true;
}

bool
AosJimoAgentFileToLocalResp::proc()
{
	passwd* pa = getpwnam(mUser.data());
	if (!pa)
	{
		printf("Unknown user: %s\n", mUser.data());
		return false;
	}
	if (!mRslt)
	{
		printf("%s\n", mResp.data());
		return false;
	}
	OmnString file_name;
	file_name << mLocalDir << "/" << mFileName;
	int rslt = access(file_name.data(), 0);
	if (rslt !=0 )
	{
		OmnString touch;
		touch << "su " << mUser << " --command=\"touch " << file_name << "\"";
		system(touch.data());
	}
	OmnFilePtr file = OmnNew OmnFile(file_name, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	AosBuffPtr file_buff = OmnNew AosBuff(eBlockSize, 0 AosMemoryCheckerArgs);
	file->seek(mOffset);
	file->append(mBuff->data(), mBuff->dataLen(), true);
	mCrtOffset = mOffset + mBuff->dataLen();
	return true;
}
