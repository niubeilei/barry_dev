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
// 05/29/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SengTorUtil/TesterId.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

extern AosStr2U32_t        sgNameMapE;
static OmnMutex			sgLock;

AosTesterId::E 
AosTesterId::toEnum(const OmnString &name)
{
	aos_assert_r(name != "", eInvalid);
	sgLock.lock();
	if (sgNameMapE.empty())
	{
		sgLock.unlock();
		return eInvalid;
	}
	AosStr2U32Itr_t itr = sgNameMapE.find(name);
	sgLock.unlock();
	if (itr == sgNameMapE.end()) return eInvalid;
	return (E)itr->second;
}


bool
AosTesterId::addName(const OmnString &name, const E code)
{
	OmnScreen << "============ name: " << name << ":" << code << endl;
	aos_assert_r(name != "", false);
	sgLock.lock();
	sgNameMapE[name] = code;
	sgLock.unlock();
	return true;
}

