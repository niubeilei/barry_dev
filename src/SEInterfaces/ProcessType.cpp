////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// Created: 04/26/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ProcessType.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include <map>
using namespace std;

static map<OmnString, u32>  sgProcessTpMap;
static OmnMutex      sgLock;
static bool			 sgInit = false;

AosProcessType::E
AosProcessType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	if(!sgInit)		init();

	map<OmnString, u32>::iterator itr = sgProcessTpMap.find(id);
	sgLock.unlock();
	if (itr == sgProcessTpMap.end()) return eInvalid;
	return (E) itr->second;
}

OmnString
AosProcessType::toStr(const E code)
{
	aos_assert_r(isValid(code), "");

	sgLock.lock();
	if(!sgInit)		init();

	OmnString name;
	map<OmnString, u32>::iterator itr = sgProcessTpMap.begin();
	for(; itr != sgProcessTpMap.end(); itr++)
	{
		if(itr->second == code)
		{
			name = itr->first;
			break;
		}
	}
	sgLock.unlock();

	aos_assert_r(name != "", "");
	return name;
}


void
AosProcessType::init()
{
	addName(AOSPROCESS_CUBE, eCube);
	addName(AOSPROCESS_BKPCUBE, eBkpCube);
	addName(AOSPROCESS_TASK, eTask);
	addName(AOSPROCESS_SERVICE, eService);
	addName(AOSPROCESS_FRONTEND, eFrontEnd);
	addName(AOSPROCESS_ADMIN, eAdmin);
	addName(AOSPROCESS_TORTURER, eTorturer);
	addName(AOSPROCESS_MSG, eMsg);
	addName(AOSPROCESS_INDEXENGINE, eIndexEngine);
	addName(AOSPROCESS_DOCENGINE, eDocEngine);
	addName(AOSPROCESS_DOCSTORE, eDocStore);
	sgInit = true;
}


bool
AosProcessType::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	map<OmnString, u32>::iterator itr = sgProcessTpMap.find(name);
	if (itr != sgProcessTpMap.end())
	{
		OmnAlarm << "this name has added." << enderr;
		return false;
	}

	sgProcessTpMap.insert(make_pair(name, eid));
	return true;
}

