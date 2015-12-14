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
// 06/14/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEInterfaces/VirtualFileType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static OmnMutex				sgLock;
static bool					sgChecked = false;

bool
AosVirtualFileType::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toStr((E)i);
		if (name == "")
		{
			OmnAlarm << "Virtual File not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Virtual File not registered correctly: " 
				<< i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


AosVirtualFileType::E 
AosVirtualFileType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	if (id == AOSVIRTUALFILE_SIMPLE) 		return eSimple;

	return eInvalid;
}


OmnString
AosVirtualFileType::toStr(const E id)
{
	switch (id)
	{
	case eSimple: 	
		 return AOSVIRTUALFILE_SIMPLE;

	default:			
		 break;
	}

	return AOSVIRTUALFILE_INVALID;
}
#endif
