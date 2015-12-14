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
#include "JimoAgentPackage/JimoAgentCommand.h"
#include "JimoAgentPackage/JimoAgentCommandResp.h"
#include <pwd.h> 

AosJimoAgentCommand::AosJimoAgentCommand()
:AosJimoAgentPackage(AosAgentPackageType::eCommand, AOSTAG_AGENTPACKAGE_COMMAND)
{
}

AosJimoAgentCommand::AosJimoAgentCommand(const OmnString &user, const OmnString &command)
:
AosJimoAgentPackage(AosAgentPackageType::eCommand, AOSTAG_AGENTPACKAGE_COMMAND),
mUser(user),
mCommand(command)
{
}

bool
AosJimoAgentCommand::serializeTo(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeTo(buff);
	aos_assert_r(rslt, false);
	aos_assert_r(mUser != "", false);
	buff->setOmnStr(mUser);
	aos_assert_r(mCommand != "", false);
	buff->setOmnStr(mCommand);
	return true;
}

bool 
AosJimoAgentCommand::serializeFrom(const AosBuffPtr &buff)
{
	bool rslt = AosJimoAgentPackage::serializeFrom(buff);
	aos_assert_r(rslt, false);
	mUser = buff->getOmnStr("");
	aos_assert_r(mUser != "", false);
	mCommand = buff->getOmnStr("");
	aos_assert_r(mCommand != "", false);
	return true;
}

bool
AosJimoAgentCommand::proc()
{
	OmnScreen << "jozhi: proc command" << endl;
	passwd* pa = getpwnam(mUser.data());
	if (!pa)
	{
		OmnString resp;
		resp << "Unknown user: " << mUser.data();
		AosPackagePtr package = OmnNew AosJimoAgentCommandResp(resp);
		AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
		package->serializeTo(resp_buff);
		resp_buff->reset();
		mClient->smartSend(resp_buff->data(), resp_buff->dataLen());
		return true;
	}
	OmnString str;
	str << "su " << mUser << " --command=\"" << mCommand << " 2>&1\"";
	FILE* f = popen(str.data(), "r");
	aos_assert_r(f, false);
	OmnString resp;
	char line[1024];
	while(fgets(line, sizeof(line), f))
	{
		resp << line;
	}
	pclose(f);
	printf("%s", resp.data());
	AosPackagePtr package = OmnNew AosJimoAgentCommandResp(resp);
	AosBuffPtr resp_buff = OmnNew AosBuff(10 AosMemoryCheckerArgs);
	package->serializeTo(resp_buff);
	resp_buff->reset();
	mClient->smartSend(resp_buff->data(), resp_buff->dataLen());
	return true;
}


