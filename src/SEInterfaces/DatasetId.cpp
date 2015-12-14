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
// 04/29/2012 Created by Chen Ding
// 2013/07/28 Moved from Dataset by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DatasetId.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgDatasetIdMap;
static OmnString			sgNames[AosDatasetId::eMax];
static OmnMutex				sgLock;
static bool					sgChecked = false;

bool
AosDatasetId::check()
{
	if (sgChecked) return true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = sgNames[i];
		if (name == "")
		{
			//OmnAlarm << "Data Proc not registered: " << i << enderr;
			OmnScreen << "Data Proc not registered: " << i << endl;
			continue;
		}

		if (toEnum(name) != (E)i)
		{
			OmnAlarm << "Data proc not registered correctly: " << i << ":" << name << enderr;
			continue;
		}
	}

	sgChecked = true;
	return true;
}


AosDatasetId::E 
AosDatasetId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgDatasetIdMap.find(id);
	sgLock.unlock();
	if (itr == sgDatasetIdMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosDatasetId::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		OmnString errmsg = "Name has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgDatasetIdMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}

