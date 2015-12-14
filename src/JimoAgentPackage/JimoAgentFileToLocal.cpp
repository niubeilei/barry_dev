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
#include "JimoAgentPackage/JimoAgentFileToLocal.h"
#include "JimoAgentPackage/JimoAgentFileToLocalResp.h"
#include "Util/ReadFile.h"
#include <pwd.h>

AosJimoAgentFileToLocal::AosJimoAgentFileToLocal()
:
AosJimoAgentPackage(AosAgentPackageType::eFileToLocal, AOSTAG_AGENTPACKAGE_FILETOLOCAL)
{
}

AosJimoAgentFileToLocal::AosJimoAgentFileToLocal(
								const OmnString &user, 
								const OmnString &remote_file,
								const OmnString &local_dir,
								const int64_t &offset)


:
AosJimoAgentPackage(AosAgentPackageType::eFileToLocal, AOSTAG_AGENTPACKAGE_FILETOLOCAL),
mUser(user),
mRemoteFile(remote_file),
mLocalDir(local_dir),
mOffset(offset)
{
}

bool
AosJimoAgentFileToLocal::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mUser != "", false);
	buff->setOmnStr(mUser);
	aos_assert_r(mRemoteFile != "", false);
	buff->setOmnStr(mRemoteFile);
	aos_assert_r(mLocalDir != "", false);
	buff->setOmnStr(mLocalDir);
	aos_assert_r(mOffset >= 0, false);
	buff->setI64(mOffset);
	return true;
}

bool 
AosJimoAgentFileToLocal::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mUser = buff->getOmnStr("");
	aos_assert_r(mUser != "", false);
	mRemoteFile = buff->getOmnStr("");
	aos_assert_r(mRemoteFile != "", false);
	mLocalDir = buff->getOmnStr("");
	aos_assert_r(mLocalDir != "", false);
	mOffset = buff->getI64(-1);
	aos_assert_r(mOffset >= 0, false);
	return true;
}

bool
AosJimoAgentFileToLocal::proc()
{
	passwd* pa = getpwnam(mUser.data());
	if (!pa)
	{
		OmnString resp;
		resp << "Unknown user: " << mUser.data();
		return sendResp(resp, false);
	}
	//set user
	AosReadFilePtr file = OmnNew AosReadFile(mRemoteFile);
	if (!file || !file->isGood())
	{
		OmnString resp;
		resp << "No souch file : " << mRemoteFile;
		return sendResp(resp, false);
	}
	int idx = mRemoteFile.find('/', true);     
	OmnString name = mRemoteFile.substr(idx+1);
	int64_t length = file->getLength();
	AosBuffPtr file_buff = OmnNew AosBuff(eBlockSize, 0 AosMemoryCheckerArgs);
	file->seek(mOffset);
	u32 bytes_read = file->readToBuff(eBlockSize, file_buff->data());
	file_buff->setDataLen(bytes_read);

	AosPackagePtr package = OmnNew AosJimoAgentFileToLocalResp(
			mUser, mLocalDir, name, length, mOffset, file_buff);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	package->serializeTo(resp_buff);
	resp_buff->reset();
	mClient->smartSend(resp_buff->data(), resp_buff->dataLen());
	//set user
	return true;
}

bool
AosJimoAgentFileToLocal::sendResp(const OmnString &resp, const bool rslt)
{
	AosPackagePtr package = OmnNew AosJimoAgentFileToLocalResp(resp, rslt);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	package->serializeTo(resp_buff);
	resp_buff->reset();
	mClient->smartSend(resp_buff->data(), resp_buff->dataLen());
	return true;
}
