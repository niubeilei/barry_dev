////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliSession.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliComm/CliSession.h"

#include "AosAlarm/AosAlarm.h"
#include "CliComm/CliRequest.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"



AosCliSession::AosCliSession(const OmnTcpClientPtr &client)
:mClient(client)
{
}


int 
AosCliSession::getSock() const
{
	return mClient->getSock();
}	


bool
AosCliSession::msgRecved(const OmnTcpClientPtr &client,
						 const OmnConnBuffPtr &buff)
{
	// 
	// It receives something from the client. The data should be
	// a CLI request. 
	//
	if (!mRequest)
	{
		mRequest = OmnNew AosCliRequest(client);
		if (!mRequest)
		{
			aos_alarm(eAosAMD_Platform, eAosAlarm_MemErr, 
				("Failed to allocate memory"));
			return false;
		}
	}

	return mRequest->procMsg(buff);
}

