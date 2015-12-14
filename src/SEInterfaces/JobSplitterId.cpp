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
// 04/30/2012 Created by Chen Ding
// 2013/06/02 Moved from Job by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "SEInterfaces/JobSplitterId.h"

#include "alarm_c/alarm.h"
#include "SEInterfaces/JobSplitterObj.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgJobSplitterIdMap;
static OmnMutex				sgLock;

AosJobSplitterId::E 
AosJobSplitterId::toEnum(const OmnString &id)
{
	AosJobSplitterObj::initStatic();
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgJobSplitterIdMap.find(id);
	sgLock.unlock();
	if (itr == sgJobSplitterIdMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosJobSplitterId::addName(const OmnString &name, const E eid)
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
		OmnString errmsg = "Splitter has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgJobSplitterIdMap[name] = eid;
	sgLock.unlock();
	return true;
}

#endif
