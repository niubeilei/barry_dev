////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: CliCommClt.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "CliComm/CliCommClt.h"

#include "Util/OmnNew.h"
#include "UtilComm/TcpServerGrp.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"


AosCliCommClt::AosCliCommClt(
		const OmnString &name,
		const OmnIpAddr &remoteAddr,
		const int remotePort)
{
	OmnString errmsg;

	mClient = OmnNew OmnTcpClient(name, remoteAddr, remotePort, 1);
	if (!mClient->connect(errmsg))
	{
		OmnAlarm << "Failed to connect: " << errmsg << enderr;
	}
}


bool
AosCliCommClt::read(OmnConnBuffPtr &buff, bool &broken)
{
	// 
	// It reads data from the connection. This is a blocking call.
	// If there is no data, it will block.
	//
	return mClient->readFrom1(buff, broken);
}


