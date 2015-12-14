////////////////////////////////////////////////////////////////////////////

// Copyright (C) 2005
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
#ifndef Aos_JimoAgentClient_JimoAgentSSH_h
#define Aos_JimoAgentClient_JimoAgentSSH_h

#include "UtilComm/TcpClient.h"

class AosJimoAgentSSH
{
private:
	OmnTcpClientPtr 		mClient;
	OmnString				mUser;
	OmnString				mCommand;
public:
	AosJimoAgentSSH(
		const OmnTcpClientPtr &client,
		const OmnString	&user,
		const OmnString &command);
	~AosJimoAgentSSH();
	bool start();
};
#endif

