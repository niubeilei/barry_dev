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
// 06/04/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/CombinerType.h"

#include "alarm_c/alarm.h"
#include "Debug/ExitHandler.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgCombinerMap;
static OmnString			sgNames[AosCombinerType::eMax];
static OmnMutex				sgLock;
bool AosCombinerType::smInited = false;


bool
AosCombinerType::check()
{
	bool rslt = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = sgNames[i];
		if (name == "")
		{
			rslt = false;
			OmnAlarm << "Combiner not registered: " << i << enderr;
		}

		E code = toEnum(name);
		if (code != (E)i)
		{
			rslt = false;
			OmnAlarm << "Combiner registered incorrect: " << code << ":" << i << enderr;
		}
	}
	return rslt;
}

		
AosCombinerType::E 
AosCombinerType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (!smInited)
	{
		sgLock.unlock();
		OmnAlarm << "Combiners were not initialized. Please add the following"
			<< " line in main.cpp:\n"
			<< "      AosAllCombiners.init();" << enderr;
		OmnExitApp("Combiners not initialized");
		return eInvalid;
	}

	AosStr2U32Itr_t itr = sgCombinerMap.find(id);
	sgLock.unlock();
	if (itr == sgCombinerMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosCombinerType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "Value Selector Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "Value Selector has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgCombinerMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}


OmnString  
AosCombinerType::toString(const E type)
{
	if (!isValid(type)) return "";
	return sgNames[type];
}


