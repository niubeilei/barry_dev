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
#include "TorturerUtil/TortMsgCreateRVG.h"

#include "Alarm/Alarm.h"
#include "Debug/Except.h"
#include "Util/SerialFrom.h"


AosTortMsgCreateRVG::AosTortMsgCreateRVG(OmnSerialFrom &s, 
						const OmnTcpClientPtr &conn)
:
AosTortMsg(conn)
{
	try
	{
		s >> mName
		  >> mType;
		mIsGood = true;
		return;
	}

	catch (OmnExcept &e)
	{
		OmnAlarm << "Failed to decode" << enderr;
		mIsGood = false;
	}
}


AosTortMsgCreateRVG::~AosTortMsgCreateRVG()
{
}


bool	
AosTortMsgCreateRVG::proc()
{
	return false;
}


