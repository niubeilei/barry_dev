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
// 09/23/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/InmemCounterType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static OmnMutex		sgLock;
static bool			sgChecked = false;
static OmnString	sgNames[AosInmemCounterType::eMax];

bool
AosInmemCounterType::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toStr((E)i);
		if (name == "")
		{
			OmnAlarm << "Inmem Counter not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Inmem Counter not registered correctly: " 
				<< i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


bool
AosInmemCounterType::addName(const OmnString &name, const E code)
{
	aos_assert_r(isValid(code), false);
	if (sgNames[code] == "") 
	{
		sgNames[code] = name;
		return true;
	}

	aos_assert_r(sgNames[code] == name, false);
	return true;
}


AosInmemCounterType::E 
AosInmemCounterType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	for (int i=eInvalid+1; i<eMax; i++)
	{
		if (sgNames[i] == id) return (E)i;
	}

	return eInvalid;
}


OmnString
AosInmemCounterType::toStr(const E id)
{
	if (!isValid(id)) return AOS_INMEMCOUNTER_INVALID;
	return sgNames[id];
}

