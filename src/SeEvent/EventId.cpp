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
// 07/31/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SeEvent/EventId.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgEventMap;
static OmnMutex				sgLock;

AosEventId::E 
AosEventId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgEventMap.find(id);
	sgLock.unlock();
	if (itr == sgEventMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosEventId::addName(const OmnString &name, const E eid)
{
	aos_assert_r(name != "", false);
	E ee = toEnum(name);
	aos_assert_r(ee == eInvalid, false);

	sgLock.lock();
	sgEventMap[name] = eid;
	sgLock.unlock();
	return true;
}

