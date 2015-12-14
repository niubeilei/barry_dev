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
// 08/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/FilterType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static OmnMutex				sgLock;
static bool					sgChecked = false;

bool
AosFilterType::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toStr((E)i);
		if (name == "")
		{
			OmnAlarm << "Filter not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Filter not registered correctly: " 
				<< i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


AosFilterType::E 
AosFilterType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	if (id == AOSFILTER_SIMPLE) 		return eSimple;

	return eInvalid;
}


OmnString
AosFilterType::toStr(const E id)
{
	switch (id)
	{
	case eSimple: 	
		 return AOSFILTER_SIMPLE;

	default:			
		 break;
	}

	return AOSFILTER_INVALID;
}

