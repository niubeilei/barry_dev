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
//
// Modification History:
// 01/23/2008 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "TorturerServer/TSTrans.h"

#include "alarm/Alarm.h"
#include "Alarm/Alarm.h"
#include "Message/MsgId.h"
#include "TorturerServer/TorturerServer.h"
#include "TorturerUtil/TortMsgId.h"
#include "TorturerUtil/TortMsgCreateRVG.h"
#include "UtilComm/TcpClient.h"
#include "Util/OmnNew.h"

AosTSTrans::AosTSTrans(const AosTorturerServerPtr server, 
		   			   const OmnTcpClientPtr &conn)
:
mServer(server),
mConn(conn)
{
	aos_assert(mServer);
	aos_assert(mConn);
}


AosTSTrans::~AosTSTrans()
{
}


bool	
AosTSTrans::msgReceived(const OmnConnBuffPtr &buff, 
						const OmnTcpClientPtr &conn)
{
	aos_assert_r(buff, false);
	aos_assert_r(conn, false);

	// 
	// Append the buffer to mBuffer. If it returns true, 
	// it means it has received enough contents for the 
	// message. If it returns false, it means that the
	// contents it has received so far are not sufficient
	// for a whole message. Need to wait for more contents
	// to come in.
	//
	if (!mBuffer.appendBuffer(buff)) return false;

	// 
	// Received a complete message. Reconstruct the 
	// object. To do so, first, we retrieve the object ID.
	//
	AosTortMsgId objid  = (AosTortMsgId)mBuffer.popMsgId();
	switch (objid)
	{
	case eAosTortMsgId_CreateRVG:
		 mMsg = OmnNew AosTortMsgCreateRVG(mBuffer, conn);
		 break;

	default:
		 OmnAlarm << "Unrecognized messge ID: " << objid << enderr;
		 return false;
	}

	aos_assert_r(mMsg, false);
	if (!mMsg->isGood())
	{
		OmnAlarm << "Message not good: " << mMsg->toString() << enderr;
		return false;
	}

	if (!mMsg->proc())
	{
		OmnAlarm <<"Failed to process the message" << enderr;
		return false;
	}

	return true;
}


int		
AosTSTrans::getSock() const
{
	aos_assert_r(mConn, -1);
	return mConn->getSock();
}

