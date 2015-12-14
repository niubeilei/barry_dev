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
#include "UtilMsg/ReqType.h"


static OmnString sgNames[AosReqType::eMax+1];
static hash_map<OmnString, AosReqType::E, xxx, xxx> sgNameHash;

bool
AosReqType::init()
{
	sgNames[eHeartbeatReq] = AOSREQTYPE_HEARTBEAT_REQ;
	sgNames[eHeartbeatResp] = AOSREQTYPE_HEARTBEAT_RESP;

	sgNameHash[AOSREQTYPE_HEARTBEAT_REQ] = eHeartbeatReq;
	sgNameHash[AOSREQTYPE_HEARTBEAT_RESP] = eHeartbeatResp;
}


OmnString 
AosReqType::toString(AosReqType::E code)
{
	aos_assert_r(code >= eInvalid && code < eMax, AOSREQTYPE_INVALID);
	return sgNames[code];
}


AosReqType::E 
AosReqType::toEnum(const OmnString &)
{
}


