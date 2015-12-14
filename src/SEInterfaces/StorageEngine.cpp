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
// 02/29/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
Moved to StorageEngineObj
#include "SEInterfaces/StorageEngine.h"

#include "alarm_c/alarm.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"


u64 AosStorageEngine::smMaxFixedSizeFileSize = AosStorageEngine::eDftMaxFixedSizeFileSize;

static AosStorageEnginePtr	sgStorageEnginees[AosDocType::eMax];
static OmnMutex				sgLock;
static bool					sgInited = false;

AosStorageEngine::AosStorageEngine(
		const AosDocType::E type, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		AosStorageEnginePtr thisptr(this, false);
		registerStorageEngine(thisptr);
	}
}


bool
AosStorageEngine::registerStorageEngine(const AosStorageEnginePtr &storage_eng)
{
	OmnString errmsg;
    aos_assert_r(AosDocType::isValid(storage_eng->mType), false);

	sgLock.lock();
    if (sgStorageEnginees[storage_eng->mType])
	{
		OmnAlarm << "StorageEngine already registered: " << storage_eng->mType << enderr;
		sgLock.unlock();
		return false;
	}
    sgStorageEnginees[storage_eng->mType] = storage_eng;
	sgLock.unlock();
	return true;
}


AosStorageEnginePtr
AosStorageEngine::getStorageEngine(const AosDocType::E type)
{
	if (!AosDocType::isValid(type)) return 0;
	
	if(!sgInited) init();
	return sgStorageEnginees[type];
}


bool
AosStorageEngine::init()
{
	if(sgInited) return true;
	sgInited = true;
	static AosSengineFixedLength sgAosSengineFixedLength(true);
	return true;
}
#endif

