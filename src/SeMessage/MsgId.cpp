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
// 12/05/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeMessage/MsgId.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgMsgidMap;
static OmnMutex				sgLock;
static bool 				sgInited = false;

bool
AosMsgId::initNames()
{
	addName(AOSMSGID_HEARTBEAT_REQ, eHeartbeatReq);
	addName(AOSMSGID_HEARTBEAT_RESP, eHeartbeatResp);
	return true;
}


AosMsgId::E 
AosMsgId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (!sgInited)
	{
		initNames();
	}
	AosStr2U32Itr_t itr = sgMsgidMap.find(id);
	sgLock.unlock();
	if (itr == sgMsgidMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosMsgId::addName(const OmnString &name, const E id)
{
	if (name == "")
	{
		OmnAlarm << "MsgId Name is empty";
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		OmnAlarm << "MsgId has already been added: "; << name;
		return false;
	}

	sgLock.lock();
	sgMsgidMap[name] = id;
	sgLock.unlock();
	return true;
}

