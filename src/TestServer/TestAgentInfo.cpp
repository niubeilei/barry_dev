////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: TestAgentInfo.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "TestServer/TestAgentInfo.h"

#include "Alarm/Alarm.h"
#include "UtilComm/TcpClient.h"



OmnTestAgentInfo::OmnTestAgentInfo(const OmnString &name, 
								   const OmnTcpClientPtr &conn)
:
mAgentName(name),
mConn(conn)
{
	
}


OmnTestAgentInfo::~OmnTestAgentInfo()
{
}


bool
OmnTestAgentInfo::isConnGood()
{
	return (mConn && mConn->isConnGood());
}


OmnRslt
OmnTestAgentInfo::sendMsg(const OmnMsgPtr &msg)
{
	if (!mConn || !mConn->isConnGood())
	{
		return OmnAlarm << "Agent is not connected yet: " << mAgentName << enderr;
	}

	return mConn->sendMsgTo(msg);
}


bool
OmnTestAgentInfo::setConn(const OmnTcpClientPtr &conn)
{
	mConn = conn;
	return true;
}
