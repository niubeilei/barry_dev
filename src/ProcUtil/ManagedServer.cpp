////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Modification History:
// 07/04/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "ProcUtil/ManagedServer.h"
#include "UtilComm/CommTypes.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "Util/StrParser.h"
#include <deque>



AosManagedServer::AosManagedServer(const AosXmlTagPtr &config, const AosRundataPtr &rdata)
:
mLock(OmnNew OmnMutex())
{
	mAddr = config->getAttrStr(AOSCONFIG_REMOTE_ADDR);
	mPort = config->getAttrInt(AOSCONFIG_REMOTE_PORT, -1);
	if (mAddr == "")
	{
		OmnString errmsg = "Missing address";
		OmnExcept except(__FILE__, __LINE__, errmsg);
		throw except;
	}

	if (mPort <= 0)
	{
		OmnString errmsg = "Port incorrect";
		OmnExcept except(__FILE__, __LINE__, errmsg);
		throw except;
	}

}

AosManagedServer::~AosManagedServer()
{
	mConn->closeConn();
}

bool
AosManagedServer::connect(OmnString &errmsg)
{
	if(!mConn)
	{
		mConn = OmnNew OmnTcpClient("nn", mAddr, mPort, 1, eAosTLT_FirstFourHigh);
	}
	if (!mConn->connect(errmsg))
	{
		mConn = 0;
		return false;
	}
	return true;
}

bool AosManagedServer::sendRequest(
						const OmnString &req,
						OmnString &resp,
						OmnString &errmsg)
{
	aos_assert_r(mConn, false);
	aos_assert_rl(mConn->smartSend(req.data(), req.length()), mLock, false);
	OmnConnBuffPtr buff;
	mConn->smartRead(buff);
	if (!buff)
	{
		errmsg = "Failed to read response!";
		return false;
	}
	resp << "<server " << "ip=" << mAddr << " port=" << mPort << ">";
	resp << buff->getData() << "</server>";
	return true;
}
void AosManagedServer::closeConn()
{
	if(mConn)
	{
		mConn->closeConn();
	}
	return;
}
