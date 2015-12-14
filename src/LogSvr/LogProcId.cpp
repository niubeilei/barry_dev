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
// 05/21/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "LogSvr/LogProcId.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgReqidMap;
static OmnMutex				sgLock;

AosLogProcId::E 
AosLogProcId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	AosStr2U32Itr_t itr = sgReqidMap.find(id);
	if (itr == sgReqidMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosLogProcId::addName(const OmnString &name, const E eid)
{
	sgLock.lock();
	E id = toEnum(name);
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

