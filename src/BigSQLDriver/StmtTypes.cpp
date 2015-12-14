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
// Modification History:
// 12/25/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "BigSQLDriver/StmtTypes.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include <vector>
using namespace std;

static AosStr2U32_t			sgIdMapParsers;
static OmnString			sgNames[AosSqlStmtType::eMaxInvalid];
static OmnMutex				sgLock;
static OmnMutex				sgCheckLock;
static bool 				sgInited = false;


bool
AosSqlStmtType::check()
{
	if (sgInited) return true;
	sgCheckLock.lock();
	for (int i=eMinInvalid+1; i<eMaxInvalid; i++)
	{
		OmnString name = toString((E)i);
		E code = toEnum(name);
		if (code != i)
		{
			OmnAlarm << "Parser not registered: " << i << enderr;
		}
	}
	sgInited = true;
	sgCheckLock.unlock();
	return true;
}


OmnString
AosSqlStmtType::toString(const E code)
{
	if (code <= eMinInvalid || code >= eMaxInvalid) return "invalid";
	return sgNames[code];
}


AosSqlStmtType::E 
AosSqlStmtType::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eMinInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgIdMapParsers.find(id);
	sgLock.unlock();
	if (itr == sgIdMapParsers.end()) return eMinInvalid;
	return (E) itr->second;
}


bool
AosSqlStmtType::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "SeRequest Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eMinInvalid)
	{
		errmsg = "SeRequest has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgIdMapParsers[name] = eid;
	sgNames[eid] = name;
	sgLock.unlock();
	return true;
}


