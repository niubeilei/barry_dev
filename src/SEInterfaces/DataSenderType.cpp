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
#include "SEInterfaces/DataSenderType.h"

#include "alarm_c/alarm.h"
#include "Debug/ExitHandler.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgDataSenderMap;
static OmnString			sgNames[AosDataSenderType::eMax];
static OmnMutex				sgLock;
bool AosDataSenderType::smInited = false;


bool
AosDataSenderType::check()
{
	bool rslt = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = sgNames[i];
		if (name == "")
		{
			rslt = false;
			OmnAlarm << "DataSender not registered: " << i << enderr;
		}

		E code = toEnum(name);
		if (code != (E)i)
		{
			rslt = false;
			OmnAlarm << "DataSender registered incorrect: " << code << ":" << i << enderr;
		}
	}
	return rslt;
}

		
AosDataSenderType::E 
AosDataSenderType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if (!smInited)
	{
		sgLock.unlock();
		OmnAlarm << "DataSenders were not initialized. Please add the following"
			<< " line in main.cpp:\n"
			<< "      AosDataSender::init();" << enderr;
		OmnExitApp("DataSenders not initialized");
		return eInvalid;
	}

	AosStr2U32Itr_t itr = sgDataSenderMap.find(id);
	sgLock.unlock();
	if (itr == sgDataSenderMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosDataSenderType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "DataSender Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "DataSender has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgDataSenderMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}


OmnString  
AosDataSenderType::toString(const E type)
{
	if (!isValid(type)) return "";
	return sgNames[type];
}


