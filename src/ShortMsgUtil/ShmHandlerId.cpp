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
// 06/15/2011	Created by Brian Zhang
////////////////////////////////////////////////////////////////////////////
#include "ShortMsgUtil/ShmHandlerId.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgReqidMap;
static OmnMutex				sgLock;


	
AosShmHandlerId::E 
AosShmHandlerId::toEnum(const OmnString &id)
{
	AosStr2U32Itr_t itr = sgReqidMap.find(id);
	if (itr == sgReqidMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosShmHandlerId::addName(const OmnString &name, const E eid)
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

