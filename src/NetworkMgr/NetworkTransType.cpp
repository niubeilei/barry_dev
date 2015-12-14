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
// 03/10/2012 Created by Chen Ding
// 2013/10/25 Commented out by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "NetworkMgr/NetworkTransType.h"

#include "Util/HashUtil.h"
#include "Thread/Mutex.h"
#include "alarm_c/alarm.h"

static OmnMutex 		sgLock;
static AosStr2U32_t     sgNameMap;

AosNetworkTransType::E 
AosNetworkTransType::toEnum(const OmnString &name)
{
	if (name.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (sgNameMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgNameMap.find(name);
	sgLock.unlock();
	if (itr == sgNameMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosNetworkTransType::addName(const OmnString &name, const E eid)
{
	aos_assert_r(name != "", false);
	E ee = toEnum(name);
	aos_assert_r(ee == eInvalid, false);

	sgLock.lock();
	sgNameMap[name] = eid;
	sgLock.unlock();
	return true;
}
#endif
