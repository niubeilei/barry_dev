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
// 05/24/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataProcStatus.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgDataProcStatusMap;
static OmnString			sgNames[AosDataProcStatus::eMax];
static OmnMutex				sgLock;
static bool 				sgInited = false;

bool
AosDataProcStatus::init()
{
	sgLock.lock();
	addName(AOSDATAPROCSTATUS_ERROR,			eError);
	addName(AOSDATAPROCSTATUS_EXIT,				eExit);
	addName(AOSDATAPROCSTATUS_CONTINUE,			eContinue);

	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toStr((E)i);
		E code = toEnum(name);
		if (code != (E)i)
		{
			OmnAlarm << "Name not registered yet: " << name << enderr;
		}
	}
	sgInited = true;
	sgLock.unlock();
	return true;
}


OmnString
AosDataProcStatus::toStr(const E id)
{
	if (id <= eInvalid || id >= eMax) return AOSDATAPROCSTATUS_INVALID;

	if (!sgInited) init();
	return sgNames[id];
}


AosDataProcStatus::E 
AosDataProcStatus::toEnum(const OmnString &id)
{
	if (!sgInited) init();

	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgDataProcStatusMap.find(id);
	sgLock.unlock();
	if (itr == sgDataProcStatusMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosDataProcStatus::addName(const OmnString &name, const E eid)
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
		OmnString errmsg = "Name already added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgDataProcStatusMap[name] = eid;
	sgNames[eid] = name;
	return true;
}

