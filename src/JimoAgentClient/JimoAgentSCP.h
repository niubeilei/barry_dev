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
#ifndef Aos_JimoAgentClient_JimoAgentSCP_h
#define Aos_JimoAgentClient_JimoAgentSCP_h

#include "UtilComm/TcpClient.h"

class AosJimoAgentSCP
{
public:
	enum
	{
		eBlockSize = 1*1024*1024
	};
private:
	OmnTcpClientPtr 		mClient;
	OmnString				mUser;
	bool					mIsFile;
	bool					mIsToLocal;
	OmnString				mRemote;
	OmnString				mLocal;
public:
	AosJimoAgentSCP(
		const OmnTcpClientPtr &client,
		const OmnString	&user,
		const bool is_file,
		const bool is_tolocal,
		const OmnString &remote,
		const OmnString &local);
	~AosJimoAgentSCP();
	bool start();
private:
	bool		localToRemote();
	bool		localFileToRemote();
	bool		localDirToRemote();
	bool		remoteToLocal();
	bool		remoteFileToLocal();
	bool		remoteDirToLocal();
};
#endif

