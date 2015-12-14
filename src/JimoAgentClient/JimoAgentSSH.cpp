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
#include "JimoAgentClient/JimoAgentSSH.h"
#include "JimoAgentPackage/JimoAgentPackage.h"
#include "JimoAgentPackage/JimoAgentCommand.h"
#include "Porting/Sleep.h"

AosJimoAgentSSH::AosJimoAgentSSH(
		const OmnTcpClientPtr &client,
		const OmnString &user,
		const OmnString &command)
:
mClient(client),
mUser(user),
mCommand(command)
{
}

AosJimoAgentSSH::~AosJimoAgentSSH()
{
}


bool 
AosJimoAgentSSH::start()
{
	//send command
	AosPackagePtr package = OmnNew AosJimoAgentCommand(mUser, mCommand);
	AosBuffPtr cmd_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	package->serializeTo(cmd_buff);
	cmd_buff->reset();
	mClient->smartSend(cmd_buff->data(), cmd_buff->dataLen());

	//recve response
	OmnConnBuffPtr buff;
	mClient->smartRead(buff);
	AosBuffPtr resp_buff = OmnNew AosBuff(buff AosMemoryCheckerArgs);
	package = AosJimoAgentPackage::serializeFromStatic(resp_buff);
	package->proc();
	return true;
}


