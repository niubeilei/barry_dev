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
#ifndef Aos_SysConsole_MsgServerOperation_h
#define Aos_SysConsole_MsgServerOperation_h

#include "Message/BuffMsg.h"


class AosMsgServerOperation : public AosBuffMsg
{
public:
	AosMsgServerOperation(
		const AosRundataPtr &rdata, 
		const int send_physical_id, 
		const int recv_physical_id, 
		const OmnString &server_name, 
		const OmnString &operation, 
		const OmnString &schedule);
	~AosMsgServerOperation(); 

	OmnString getServerName() {return getFieldStr(0, "");}
	OmnString getOperation() {return getFieldStr(1, "");}
	OmnString getSchedule() {return getFieldStr(2, "");}
};
#endif

