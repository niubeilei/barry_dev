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
#include "JimoAgentPackage/JimoAgentFileToRemote.h"
#include "JimoAgentPackage/JimoAgentFileToRemoteResp.h"
#include "Util/File.h"
#include <pwd.h>
AosJimoAgentFileToRemote::AosJimoAgentFileToRemote()
:
AosJimoAgentPackage(AosAgentPackageType::eFileToRemote, AOSTAG_AGENTPACKAGE_FILETOREMOTE)
{
}

AosJimoAgentFileToRemote::AosJimoAgentFileToRemote(
								const OmnString &user, 
								const OmnString &remote_dir,
								const OmnString &name,
								const int64_t &offset,
								const int64_t &length,
								const AosBuffPtr &buff)


:
AosJimoAgentPackage(AosAgentPackageType::eFileToRemote, AOSTAG_AGENTPACKAGE_FILETOREMOTE),
mUser(user),
mRemoteDir(remote_dir),
mFileName(name),
mOffset(offset),
mLength(length),
mBuff(buff)
{
}

bool
AosJimoAgentFileToRemote::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mUser != "", false);
	buff->setOmnStr(mUser);
	aos_assert_r(mRemoteDir != "", false);
	buff->setOmnStr(mRemoteDir);
	aos_assert_r(mFileName != "", false);
	buff->setOmnStr(mFileName);
	aos_assert_r(mOffset >= 0, false);
	buff->setI64(mOffset);
	aos_assert_r(mLength >=0, false);
	buff->setI64(mLength);
	aos_assert_r(mBuff, false);
	buff->setU32(mBuff->dataLen());
	buff->setBuff(mBuff);
	return true;
}

bool 
AosJimoAgentFileToRemote::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mUser = buff->getOmnStr("");
	aos_assert_r(mUser != "", false);
	mRemoteDir = buff->getOmnStr("");
	aos_assert_r(mRemoteDir != "", false);
	mFileName = buff->getOmnStr("");
	aos_assert_r(mFileName != "", false);
	mOffset = buff->getI64(-1);
	aos_assert_r(mOffset >= 0, false);
	mLength = buff->getI64(-1);
	aos_assert_r(mLength >= 0, false);
	u32 buff_len = buff->getU32(0);
	mBuff = buff->getBuff(buff_len, true AosMemoryCheckerArgs);
	aos_assert_r(mBuff, false);
	return true;
}

bool
AosJimoAgentFileToRemote::sendResp(const OmnString &resp, const bool rslt)
{
	AosPackagePtr package = OmnNew AosJimoAgentFileToRemoteResp(resp, rslt);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	package->serializeTo(resp_buff);
	resp_buff->reset();
	mClient->smartSend(resp_buff->data(), resp_buff->dataLen());
	return true;
}

bool
AosJimoAgentFileToRemote::proc()
{
	passwd* pa = getpwnam(mUser.data());
	if (!pa)
	{
		OmnString resp;
		resp << "Unknown user: " << mUser.data();
		sendResp(resp, false);
		return true;
	}
	//set user
	OmnString file_name;
	file_name << mRemoteDir << "/"
			  << mFileName;
	OmnScreen << file_name << " : " << mOffset << endl;
	int rslt = access(file_name.data(), 0);
	if (rslt !=0 )
	{
		OmnString touch;
		touch << "su " << mUser << " --command=\"touch " << file_name << "\"";
		system(touch.data());
	}
	OmnFilePtr file = OmnNew OmnFile(file_name, OmnFile::eWriteCreate AosMemoryCheckerArgs);
	if (!file || !file->isGood())
	{
		OmnString resp;
		resp << "No souch file: " << file_name;
		sendResp(resp, false);
		return true;
	}
	file->seek(mOffset);
	file->append(mBuff->data(), mBuff->dataLen(), true);
	OmnString resp;
	sendResp(resp, true);
	return true;
}


