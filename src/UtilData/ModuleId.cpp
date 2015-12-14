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
// 10/10/2011	Created by Ketty
// 2015/03/31   Moved from TransUtil by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "UtilData/ModuleId.h"

#include "Rundata/Rundata.h"

static AosStr2U32_t         sgModuleIdMap;   
static AosU322Str_t         sgModuleNameMap;   
static OmnMutex             sgLock;
static bool                 sgInited = false;

AosModuleId::AosModuleId(const E id):
mModuleId(id)
{
}



AosModuleId::~AosModuleId()
{
}


AosModuleId::E
AosModuleId::toEnum(const OmnString &id)
{
	if (!sgInited)   init();
	if (id.length() < 1) return eInvalid;

	sgLock.lock();
	AosStr2U32Itr_t itr = sgModuleIdMap.find(id);
	sgLock.unlock();
	if (itr == sgModuleIdMap.end()) return eInvalid;
	return (E) itr->second;
}


OmnString
AosModuleId::toStr(const E code)
{
	if (!sgInited)   init();
	if (!isValid(code)) return AOSMODULEID_INVALID;

	sgLock.lock();
	AosU322StrItr_t itr = sgModuleNameMap.find(code);
	sgLock.unlock();
	if (itr == sgModuleNameMap.end()) return AOSMODULEID_INVALID;
	return itr->second;
}


bool
AosModuleId::init()
{
	if (sgInited)   return true;

	sgInited = true;

	addName(AOSMODULEID_DOC,		eDoc);
	addName(AOSMODULEID_IIL,		eIIL);
	addName(AOSMODULEID_JOBMGR,		eJobMgr);
	addName(AOSMODULEID_TASKMGR,	eTaskMgr);
	
	return true;
}


bool
AosModuleId::addName(const OmnString &name, const E eid)
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

	sgLock.lock();
	sgModuleIdMap[name] = eid;
	sgModuleNameMap[eid] = name;
	sgLock.unlock();
	return true;
}


