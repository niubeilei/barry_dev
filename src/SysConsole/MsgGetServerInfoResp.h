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
#ifndef Aos_SysConsole_MsgGetServerInfoResp_h
#define Aos_SysConsole_MsgGetServerInfoResp_h

#include "Message/BuffMsg.h"


class AosMsgGetServerInfoResp : public AosBuffMsg
{
public:
	AosMsgGetServerInfoResp(
		const AosRundataPtr &rdat, 
		const int send_physical_id, 
		const int recv_physical_id, 
		const OmnString &msgid);
	~AosMsgGetServerInfoResp(); 
};
#endif

