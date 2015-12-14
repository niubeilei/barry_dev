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
// 09/25/2010: Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_MsgUtil_ReqType_h
#define Aos_MsgUtil_ReqType_h



class AosReqType
{
public:
	enum E
	{
		eInvalid,

		eHeartbeatReq,
		eHeartbeatResp,

		eMax
	};

public:
	static OmnString toString(E);
	static E toEnum(const OmnString &);
};
#endif

