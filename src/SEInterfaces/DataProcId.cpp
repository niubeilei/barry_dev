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
// 2013/07/28 Moved from DataProc by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/DataProcId.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgDataProcIdMap;
static OmnString			sgNames[AosDataProcId::eMax];
static OmnMutex				sgLock;
static bool					sgChecked = false;

bool
AosDataProcId::check()
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


AosDataProcId::E 
AosDataProcId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgDataProcIdMap.find(id);
	sgLock.unlock();
	if (itr == sgDataProcIdMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosDataProcId::addName(const OmnString &name, const E eid)
{
	// Chen Ding, 2013/05/04
	static bool lsInited = false;

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
	// Chen Ding, 2013/05/04
	if (!lsInited)
	{
		lsInited = true;
		sgDataProcIdMap[AOSDATAPROC_DLLPROC] = eDLLProc;
		sgNames[eDLLProc] = AOSDATAPROC_DLLPROC;
	}
	sgDataProcIdMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}

