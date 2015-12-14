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
#include "SysConsole/MsgServerOperation.h"


AosMsgServerOperation::AosMsgServerOperation(
		const AosRundataPtr &rdata, 
		const int send_phyid, 
		const int recv_phyid, 
		const OmnString &server_name, 
		const OmnString &operation, 
		const OmnString &schedule)
AosBuffMsg(rdata, send_phyid, recv_phyid, OmnMsgId::eServerOperation)
{
	setFieldStr(0, server_name);
	setFieldStr(1, operation);
	setFieldStr(2, schedule);
}


AosMsgServerOperation::~AosMsgServerOperation()
{
}
#endif
