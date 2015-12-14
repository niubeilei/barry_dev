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
#if 0
#include "SysConsole/ProcGetServerInfo.h"


AosMsgProcGetServerInfo::AosMsgProcGetServerInfo()
{
}


AosMsgProcGetServerInfo::~AosMsgProcGetServerInfo()
{
}


bool 
AosMsgProcGetServerInfo::procMsg(
		const AosRundataPtr &rundata,
		const AosBuffMsgPtr &the_msg)
{
	// Convert the message to 'AosMsgGetServerInfo' message. 
	OmnMsgId::E msgid = the_msg->getMsgId();
	aos_assert_rr(msgid == OmnMsgId::eGetServerInfo, rundata, false);
	AosMsgGetServerInfoPtr msg = (AosMsgGetServerInfo*)the_msg.getPtr();
	AosRundataPtr rdata = msg->getRundata();
	aos_assert_rr(rdata, rundata, false);

	AosPhyServerPtr server = AosGetSysConsole()->getLocalServer(rdata);
	aos_assert_rr(server, data, false);

	return server->getServerInfo(rdata, msg);
}
#endif
