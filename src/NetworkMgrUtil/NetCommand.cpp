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
// 05/01/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "NetworkMgrUtil/NetCommand.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgNetCommandMap;
static OmnString			sgNames[AosNetCommand::eMax];
static OmnMutex				sgLock;
static bool					sgInited = false;

AosNetCommand::E 
AosNetCommand::toEnum(const OmnString &id)
{
	if(!sgInited)	init();
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgNetCommandMap.find(id);
	sgLock.unlock();
	if (itr == sgNetCommandMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosNetCommand::init()
{
	if (sgInited)	return true;

	sgInited = true;
	addName(AOSCOMMAND_RESTARTJOB, 				eReStartJob);
	addName(AOSCOMMAND_STARTJOB, 				eStartJob);
	addName(AOSCOMMAND_PAUSEJOB, 				ePauseJob);
	addName(AOSCOMMAND_STOPJOB, 				eStopJob);
	addName(AOSCOMMAND_FAILJOB, 				eFailJob);
	addName(AOSCOMMAND_FINISHJOB, 				eFinishJob);
	addName(AOSCOMMAND_SWITCHJOBMASTER, 		eSwitchJobMaster);

	addName(AOSCOMMAND_STARTTASK, 				eStartTask);
	addName(AOSCOMMAND_PAUSETASK, 				ePauseTask);
	addName(AOSCOMMAND_STOPTASK, 				eStopTask);
	addName(AOSCOMMAND_FAILTASK, 				eFailTask);
	addName(AOSCOMMAND_FINISHTASK, 				eFinishTask);

	
	int i = eReStartJob;
	for (; i<eMax; i++)
	{
		if (sgNames[i] == "")
		{
			OmnAlarm << "NetCommand name not registered yet: " << i << enderr;
		}
	}
	return false;
}


bool
AosNetCommand::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgNetCommandMap.find(name);
	if (itr != sgNetCommandMap.end()) 
	{
		sgLock.unlock();
		OmnString errmsg = "Name already added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgNetCommandMap[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}

