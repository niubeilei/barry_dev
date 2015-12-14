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
// 12/05/2011	Created by Linda 
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_SeMessage_MsgId_h
#define AOS_SeMessage_MsgId_h


#define AOSSEMSG_HEARTBEAT_REQ			"heartbeat_req"
#define AOSSEMSG_HEARTBEAT_RESP			"heartbeat_resp"


class AosMsgId
{
public:
	enum E
	{
		eInvalid,

		eHeartbeat,
		eHeartbeatResp,

		eMax
	};

	static E toEnum(const OmnString &id);
};

#endif

