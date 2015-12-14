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
// 2013/06/02 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "Jimo/MethodId.h"

#include "Thread/Mutex.h"
#include "Util/HashUtil.h"
#include <hash_map>
using namespace std;


static AosStr2U32_t sgNameMap;
static OmnMutex sgLock;
static bool sgInited = false;
static OmnString sgNames[AosMethodId::eMaxMethodId];

#define AOSMETHODNAME_AosRundata_Jimo				"AosRundata_Jimo"
#define AOSMETHODNAME_AosRundata_Jimo_WorkerDoc		"AosRundata_Jimo_WorkerDoc"

AosMethodId::E 
AosMethodId::toEnum(const OmnString &name)
{
	sgLock.lock();
	if (!sgInited)
	{
		init();
	}

	AosStr2U32Itr_t itr = sgNameMap.find(name);
	u32 id = itr->second;
	AosMethodId::E method_id = (AosMethodId::E)id;
	sgLock.unlock();
	return method_id;
}


OmnString
AosMethodId::toName(const E code)
{
	if (!isValid(code)) return AOSMETHODNAME_INVALID;
	return sgNames[code];
}


bool
AosMethodId::init()
{
	sgNameMap[AOSMETHODNAME_AosRundata_Jimo] = eAosRundata_Jimo;
	sgNameMap[AOSMETHODNAME_AosRundata_Jimo_WorkerDoc] = eAosRundata_Jimo_WorkerDoc;

	sgNames[eAosRundata_Jimo] = AOSMETHODNAME_AosRundata_Jimo;
	sgNames[eAosRundata_Jimo_WorkerDoc] = AOSMETHODNAME_AosRundata_Jimo_WorkerDoc;

	sgInited = true;
	return true;
}

