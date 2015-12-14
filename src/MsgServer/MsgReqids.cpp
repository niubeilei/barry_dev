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
// 06/15/2011	Created by Jozhi Peng
////////////////////////////////////////////////////////////////////////////
#include "MsgServer/MsgReqids.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t 		sgReqidMap;
static OmnMutex				sgLock;

AosMsgReqid::E 
AosMsgReqid::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;

	sgLock.lock();
	if (sgReqidMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	
	AosStr2U32Itr_t itr = sgReqidMap.find(id);
	sgLock.unlock();
	if (itr == sgReqidMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosMsgReqid::addName(const OmnString &name, const E eid)
{
	E id = toEnum(name);
	sgLock.lock();
	if (id != eInvalid)
	{
		sgLock.unlock();
		OmnAlarm << "Name already registered: " << name << enderr;
		return false;
	}

	sgReqidMap[name] = eid;
	sgLock.unlock();
	return true;
}

