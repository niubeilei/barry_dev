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
// 2013/03/07 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SysConsole/MsgProcOperation.h"


AosMsgProcOperation::AosMsgProcOperation()
{
}


AosMsgProcOperation::~AosMsgProcOperation()
{
}


bool 
AosMsgProcServerOperation::procMsg(
		const AosRundataPtr &rdata, 
		const AosBuffMsgPtr &the_msg)
{
	OmnMsgId::E msgid = the_msg->getMsgId();
	aos_assert_rr(msgid == OmnMsgId::eServerOperation, rdata, false);

	AosMsgServerOperationPtr msg = (AosMsgServerOperation*)the_msg.getPtr();
	int sender_phyid = msg->getFieldInt(AosMsgFieldId::eSenderPhysicalId, -1);
	OmnString server_name = msg->getServerName();
	OmnString operation = msg->getOperation();
	OmnString schedule = msg->getSchedule();

	// TBD
	OmnNotImplementedYet;
	return false;
}


AosCubeMsgProcPtr 
AosMsgProcServerOperation::clone()
{
	return OmnNew AosMsgProcServerOperation();
}
#endif
