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
#include "SEInterfaces/DataCacherType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static OmnMutex				sgLock;
static bool					sgChecked = false;

bool
AosDataCacherType::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toStr((E)i);
		if (name == "")
		{
			OmnAlarm << "Data Cacher not registered: " << i << enderr;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Data cacher not registered correctly: " 
				<< i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


AosDataCacherType::E 
AosDataCacherType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	if (id == AOSDATACACHER_SIMPLE) 		return eSimple;
	if (id == AOSDATACACHER_MULTI) 			return eMulti;
	if (id == AOSDATACACHER_FILE) 			return eCasherFile;
	if (id == AOSDATACACHER_VIRTUAL_FILE) 	return eCasherVirtualFile;
	if (id == AOSDATACACHER_SCANCACHER) 	return eScanCacher;

	return eInvalid;
}


OmnString
AosDataCacherType::toStr(const E id)
{
	switch (id)
	{
	case eMulti: 	
		 return AOSDATACACHER_MULTI;

	case eSimple: 	
		 return AOSDATACACHER_SIMPLE;

	case eCasherFile:
		 return AOSDATACACHER_FILE;

	case eCasherVirtualFile:
		 return AOSDATACACHER_VIRTUAL_FILE;

	case eScanCacher:
		 return AOSDATACACHER_SCANCACHER;
	default:			
		 break;
	}

	return AOSDATACACHER_INVALID;
}

