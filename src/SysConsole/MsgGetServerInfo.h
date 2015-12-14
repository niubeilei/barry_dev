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
// 2013/03/06 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_SysConsole_MsgGetServerInfo_h
#define Aos_SysConsole_MsgGetServerInfo_h

#include "Message/BuffMsg.h"


class AosMsgGetServerInfo : public AosBuffMsg
{
public:
	AosMsgGetServerInfo(
		const AosRundataPtr &rdat, 
		const int send_physical_id, 
		const int recv_physical_id, 
		const OmnString &msgid);
	~AosMsgGetServerInfo(); 

};
#endif

