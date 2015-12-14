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
// 12/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SmartObj/SmartObj.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/String.h"
#include "Util/HashUtil.h"
#include <hash_map>
using namespace std;

typedef hash_map<const OmnString, AosSmartObjPtr, Omn_Str_hash, compare_str> AosSmartObjHash_t;
typedef hash_map<const OmnString, AosSmartObjPtr, Omn_Str_hash, compare_str>::iterator AosSmartObjHashItr_t;
static AosSmartObjHash_t	sgSmartObjs;
static OmnMutex				sgLock;


AosSmartObj::AosSmartObj(const OmnString &name, const bool regflag)
:
mName(name)
{
	aos_assert(name != "");
	if (regflag) 
	{
		AosSmartObjPtr thisptr(this, false);
		OmnString errmsg;
		if (!registerSmartObj(thisptr, name, errmsg))
		{
			OmnThrowException(errmsg);
		}
	}
}


AosSmartObj::~AosSmartObj()
{
}


bool
AosSmartObj::registerSmartObj(
		const AosSmartObjPtr &sobj, 
		const OmnString &name, 
		OmnString &errmsg)
{
	sgLock.lock();
	AosSmartObjHashItr_t itr = sgSmartObjs.find(name);
	if (itr == sgSmartObjs.end())
	{
		sgSmartObjs[name] = sobj;
		sgLock.unlock();
		return true;
	}
	sgLock.unlock();
	errmsg = "Smart doc already registered: ";
	errmsg << name;
	return false;
}


AosSmartObjPtr
AosSmartObj::getSmartObj(const OmnString &name)
{
	sgLock.lock();
	AosSmartObjHashItr_t itr = sgSmartObjs.find(name);
	if (itr == sgSmartObjs.end())
	{
		sgLock.unlock();
		OmnAlarm << "Unrecognized smart objid: " << name << enderr;
		return 0;
	}
	
	sgLock.unlock();
	return itr->second;
}

