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
// 08/07/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "IILSelector/IILSelectorIds.h"

#include "alarm_c/alarm.h"
#include "IILSelector/IILSelectorNames.h"
#include "Thread/Mutex.h"
#include "Util/HashUtil.h"

static AosStr2U32_t 		sgSelectorMap;
static OmnMutex				sgLock;
static bool					sgInited = false;
static AosIILSelectorId		sgSelectorId;



AosIILSelectorId::AosIILSelectorId()
{
	if (!sgInited)
	{
		sgInited = true;
		OmnString errmsg;
		addName(AOSIILSELTYPE_ATTR, 		AosIILSelectorId::eAttr, errmsg);
		addName(AOSIILSELTYPE_CONTAINER, 	AosIILSelectorId::eContainer, errmsg);
		addName(AOSIILSELTYPE_KEYWORDS, 	AosIILSelectorId::eKeyword, errmsg);
		addName(AOSIILSELTYPE_TAGS, 		AosIILSelectorId::eTags, errmsg);
		addName(AOSIILSELTYPE_MICROBLOG, 	AosIILSelectorId::eMicroblog, errmsg);
		addName(AOSIILSELTYPE_COUNTER, 		AosIILSelectorId::eCounter, errmsg);
		addName(AOSIILSELTYPE_LOG, 			AosIILSelectorId::eLog, errmsg);
		addName(AOSIILSELTYPE_IILNAME,		AosIILSelectorId::eIILName, errmsg);
		addName(AOSIILSELTYPE_IILID,        AosIILSelectorId::eIILId, errmsg);
	}
}


AosIILSelectorId::E 
AosIILSelectorId::toEnum(const OmnString &id)
{
	if (id.length() < 1) return eInvalid;
	
	sgLock.lock();
	AosStr2U32Itr_t itr = sgSelectorMap.find(id);
	sgLock.unlock();
	if (itr == sgSelectorMap.end()) return eInvalid;
	return (E) itr->second;
}


bool
AosIILSelectorId::addName(const OmnString &name, const E eid, OmnString &errmsg)
{
	if (name == "")
	{
		errmsg = "IIL Selector Name is empty";
		OmnAlarm << errmsg << enderr;
		return false;
	}

	E ee = toEnum(name);
	if (ee != eInvalid)
	{
		errmsg = "IIL Selector has already been added: ";
		errmsg << name;
		OmnAlarm << errmsg << enderr;
		return false;
	}

	sgLock.lock();
	sgSelectorMap[name] = eid;
	sgLock.unlock();
	return true;
}

