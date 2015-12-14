////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/23/2012, Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Groupby/GroupbyOpr.h"

#include "alarm_c/alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgGroupbyOprMap;
static OmnString			sgNames[AosGroupbyOpr::eMax];
static OmnMutex				sgLock;
static bool 				sgInited = false;

bool
AosGroupbyOpr::init()
{
	static OmnMutex lsLocalLock;
	
	lsLocalLock.lock();
	addName(AOSGROUPBYOPR_FIRST, 		eFirst);
	addName(AOSGROUPBYOPR_LAST, 		eLast);
	addName(AOSGROUPBYOPR_FIRSTCOND,	eFirstCond);
	addName(AOSGROUPBYOPR_LASTCOND,		eLastCond);
	addName(AOSGROUPBYOPR_NONE,			eNone);

	sgInited = true;
	for (int i=eInvalid+1; i<eMax; i++)
	{
		OmnString name = toStr((E)i);
		E code = toEnum(name);
		if (code != (E)i)
		{
			OmnAlarm << "Name not registered yet: " << name << enderr;
		}
	}
	lsLocalLock.unlock();
	return true;
}


OmnString
AosGroupbyOpr::toStr(const E id)
{
	if (id <= eInvalid || id >= eMax) return AOSGROUPBYOPR_INVALID;

	if (!sgInited) init();
	return sgNames[id];
}


AosGroupbyOpr::E 
AosGroupbyOpr::toEnum(const OmnString &id)
{
	if (!sgInited) init();

	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgGroupbyOprMap.find(id);
	sgLock.unlock();
	if (itr == sgGroupbyOprMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosGroupbyOpr::addName(const OmnString &name, const E eid)
{
	if (name == "")
	{
		OmnString errmsg = "Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}
/*
	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		OmnString errmsg = "Name already added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}
*/
	sgGroupbyOprMap[name] = eid;
	sgNames[eid] = name;
	return true;
}

