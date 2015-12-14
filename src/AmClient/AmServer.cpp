////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 3/30/2007: Created by cding
////////////////////////////////////////////////////////////////////////////
#include "AmClient/AmServer.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "Util/OmnNew.h"
#include "UtilComm/TcpClient.h"



AosAmServer::AosAmServer()
:
mIsGood(false)
{
}


AosAmServer::AosAmServer(const OmnIpAddr &addr, 
				const u16 port, 
				const u32 numPorts,
				const OmnString &name)
:
mAddr(addr),
mPort(port),
mNumPorts(numPorts),
mName(name),
mIsGood(false)
{
}


AosAmServer::~AosAmServer()
{
}


bool
AosAmServer::sendMsg(char *buff, u32 len)
{
	if (!mConn)
	{
		if (!connect())
		{
			OmnAlarm << "Failed to connect to the server: " 
				<< toString() << enderr;
			return false;
		}
	}

	if (!mConn->writeTo(buff, len))
	{
		OmnAlarm << "Failed to send the message: " 
			<< toString() << enderr;
		return false;
	}

	return true;
}


bool
AosAmServer::connect()
{
	if (mConn && mConn->isConnGood())
	{
		mIsGood = true;
		return true;
	}

	mConn = OmnNew OmnTcpClient(mName, mAddr, mPort, mNumPorts);
	aos_assert_r(mConn, false);

	OmnString errmsg;
	if (!mConn->connect(errmsg))
	{
		OmnAlarm << "Failed to connect: " << errmsg << enderr;
		mIsGood = false;
		return false;
	}

	mIsGood = true;
	return true;
}


bool
AosAmServer::stop()
{
	if (!mConn) return true;

	mConn->closeConn();
	mConn = 0;
	mIsGood = false;
	return true;
}


OmnString
AosAmServer::toString() const
{
	OmnString str;
	str << "AosAmServer: \n"
		<< "    Address:  " << mAddr.toString() << "\n"
		<< "    Port:     " << mPort
		<< "    NumPorts: " << mNumPorts
		<< "    UseCount: " << mUseCount;
	return str;
}


bool		
AosAmServer::receiveResp(OmnConnBuffPtr &buff, const u32 timer)
{
	bool timeout = false, connBroken = false;
	mConn->readFrom(buff, timer, timeout, connBroken, false);
	return (!timeout && !connBroken);
}


// Return Value: 
// 		If recieved message successfully, return true. 
// 		Otherwise, return false.
bool 
AosAmServer::receiveResp(OmnConnBuffPtr &buff)
{
	bool connBroken = false; // this parameter has no use
	bool isEndOfFile;
	return mConn->readFrom1(buff, connBroken, isEndOfFile);
}


bool 
AosAmServer::operator == (const AosAmServer &rhs) const
{
	if(!mConn || !rhs.mConn)
	{
		return false;
	}
	return (mConn->getSock() == rhs.mConn->getSock());
}

int 
AosAmServer::getSock() const 
{
	if(mConn)
	{
		return mConn->getSock();
	}
	else
	{
		return -1;
	}
}
