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
// 02/12/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "RemoteBackupUtil/RemoteBkType.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t         sgMap;
static OmnString			sgNames[AosRemoteBkType::eMax];
static OmnMutex             sgLock;

AosRemoteBkType::E 
AosRemoteBkType::toEnum(const OmnString &name)
{
	if (name.length() < 1) return eInvalid;
	
	if (sgMap.empty()) return eInvalid;
	AosStr2U32Itr_t itr = sgMap.find(name);
	if (itr == sgMap.end()) return eInvalid;
	return (E) itr->second;
}


OmnString 
AosRemoteBkType::toString(const E code)
{
	if (!isValid(code)) return AOSREMOTEBACK_INVALID;
	return sgNames[code];
}


bool
AosRemoteBkType::addName(const OmnString &name, const E eid)
{
OmnCout << "remote bk Type: " << name.data() << " : " << eid << endl;
	aos_assert_r(name != "", false);
	sgLock.lock();
	sgMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}

