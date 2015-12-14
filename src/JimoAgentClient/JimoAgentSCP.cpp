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
#include "JimoAgentClient/JimoAgentSCP.h"
#include "JimoAgentPackage/JimoAgentPackage.h"
#include "JimoAgentPackage/JimoAgentFileToRemote.h"
#include "JimoAgentPackage/JimoAgentFileToLocal.h"
#include "Util/ReadFile.h"
#include "Util/Buff.h"

AosJimoAgentSCP::AosJimoAgentSCP(
		const OmnTcpClientPtr &client,
		const OmnString	&user,
		const bool is_file,
		const bool is_tolocal,
		const OmnString &remote,
		const OmnString &local)
:
mClient(client),
mUser(user),
mIsFile(is_file),
mIsToLocal(is_tolocal),
mRemote(remote),
mLocal(local)
{
}

AosJimoAgentSCP::~AosJimoAgentSCP()
{
}
	
bool 
AosJimoAgentSCP::start()
{
	if (!mIsToLocal)
	{
		return localToRemote();
	}
	else
	{
		return remoteToLocal();
	}
	return true;
}

bool
AosJimoAgentSCP::remoteToLocal()
{
	if (mIsFile)
	{
		return remoteFileToLocal();
	}
	else
	{
		return remoteDirToLocal();
	}
	return true;
}

bool
AosJimoAgentSCP::remoteFileToLocal()
{
	int idx = mRemote.find('/', true);
	OmnString name = mRemote.substr(idx+1);
	int64_t crt_offset = -1;
	int64_t length = 0;
	while(crt_offset < length)
	{
		if (crt_offset < 0)
		{
			crt_offset = 0;
		}
		AosPackagePtr package = OmnNew AosJimoAgentFileToLocal(mUser, mRemote, mLocal, crt_offset);
		AosBuffPtr send_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		package->serializeTo(send_buff);
		send_buff->reset();
		mClient->smartSend(send_buff->data(), send_buff->dataLen());

		OmnConnBuffPtr buff;
		mClient->smartRead(buff);
		AosBuffPtr resp_buff = OmnNew AosBuff(buff AosMemoryCheckerArgs);
		package = AosJimoAgentPackage::serializeFromStatic(resp_buff);
		bool rslt = package->proc();
		if (!rslt)
		{
			exit(-1);
		}
		crt_offset = package->getCrtOffset();
		length = package->getLength();
		//printf("%s%100ld\t%ld\r", name.data(), length, crt_offset);
	}
	return true;
}

bool
AosJimoAgentSCP::remoteDirToLocal()
{
	return true;
}

bool
AosJimoAgentSCP::localToRemote()
{
	if (mIsFile)
	{
		return localFileToRemote();
	}
	else
	{
		return localDirToRemote();
	}
	return true;
}

bool
AosJimoAgentSCP::localFileToRemote()
{
	AosReadFilePtr file = OmnNew AosReadFile(mLocal);
	if (!file || !file->isGood())
	{
		printf("No souch file : %s\n", mLocal.data());
		exit(-1);
	}
	int idx = mLocal.find('/', true);
	OmnString name = mLocal.substr(idx+1);
	int64_t length = file->getLength();
	int64_t offset = 0;
	AosPackagePtr package;
	while(offset < length)
	{
		AosBuffPtr file_buff = OmnNew AosBuff(eBlockSize, 0 AosMemoryCheckerArgs);
		file->seek(offset);
		u32 bytes_read = file->readToBuff(eBlockSize, file_buff->data());
		file_buff->setDataLen(bytes_read);
		package = OmnNew AosJimoAgentFileToRemote(mUser, mRemote, name, offset, length, file_buff);
		AosBuffPtr send_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		package->serializeTo(send_buff);
		send_buff->reset();
		mClient->smartSend(send_buff->data(), send_buff->dataLen());

		OmnConnBuffPtr buff;
		mClient->smartRead(buff);
		AosBuffPtr resp_buff = OmnNew AosBuff(buff AosMemoryCheckerArgs);
		package = AosJimoAgentPackage::serializeFromStatic(resp_buff);
		package->proc();
		offset += bytes_read;
		//printf("%s%100ld\t%ld\r", name.data(), length, offset);
	}
	return true;
}

bool
AosJimoAgentSCP::localDirToRemote()
{
	return true;
}
