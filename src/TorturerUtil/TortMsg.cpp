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
#include "TorturerUtil/TortMsg.h"

#include "alarm/Alarm.h"
#include "UtilComm/TcpClient.h"


AosTortMsg::AosTortMsg(const OmnTcpClientPtr &conn)
:
mIsGood(false),
mConn(conn)
{
	aos_assert(mConn)
}


AosTortMsg::~AosTortMsg()
{
}

