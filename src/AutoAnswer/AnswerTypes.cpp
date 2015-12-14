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
// 10/28/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "AutoAnswer/AnswerTypes.h"

#include "alarm_c/alarm.h"
#include "SEServer/SeReqProc.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgAnswerMap;
static OmnMutex				sgLock;

AosAnswerType::E 
AosAnswerType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgAnswerMap.find(id);
	sgLock.unlock();
	if (itr == sgAnswerMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosAnswerType::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnAlarm << "Auto Answer Name is empty" << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		OmnAlarm << "Answer has already been added: " << enderr;
		return false;
	}

	sgLock.lock();
	sgAnswerMap[name] = eid;
	sgLock.unlock();
	return true;
}

