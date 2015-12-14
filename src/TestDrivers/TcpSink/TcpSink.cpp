////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TcpSink.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestDrivers/TcpSink/TcpSink.h"

#include "Util/OmnNew.h"
#include "UtilComm/TcpServer.h"


AosTcpSink::AosTcpSink(const OmnString &name, 
					   const OmnIpAddr &localAddr, 
					   const int localPort, 
					   const int numLocalPorts)
:
mName(name),
mLocalAddr(localAddr),
mLocalPort(localPort),
mNumLocalPorts(numLocalPorts)
{
	mServer = OmnNew OmnTcpServer(localAddr, localPort,
                 numLocalPorts, name);
	OmnTcpListenerPtr thisPtr(this, false);
	mServer->setListener(thisPtr);
}


bool
AosTcpSink::connect(OmnString &errmsg)
{
	if (!mServer)
	{
		errmsg = "Server not created before connecting";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	return mServer->connect(errmsg);
}


void
AosTcpSink::msgRecved(const OmnConnBuffPtr &buff, const OmnTcpClientPtr &client)
{
	// 
	// We have read something from a remote site. Since this is a sinker, do nothing.
	// 
	return;
}


bool
AosTcpSink::startReading()
{
	if (!mServer)
	{
		OmnAlarm << "Server not created before startReading()" << enderr;
		return false;
	}

	mServer->startReading();
	return true;
}

