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
// 07/28/2011	Created by Linda
////////////////////////////////////////////////////////////////////////////
#include "DocServer/ProcTypes.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t 		sgProcMap;
static OmnMutex				sgLock;

AosDocSvrProcId::E 
AosDocSvrProcId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (sgProcMap.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgProcMap.find(id);
	sgLock.unlock();
	if (itr == sgProcMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosDocSvrProcId::addName(const OmnString &name, const E eid)
{
	aos_assert_r(name != "", false);
	E ee = toEnum(name);
	aos_assert_r(ee == eInvalid, false);

	sgLock.lock();
	sgProcMap[name] = eid;
	sgLock.unlock();
	return true;
}

