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
// 06/03/2012 Moved from StorageEngine.cpp
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/StorageEngineObj.h"

#include "alarm_c/alarm.h"
#include "Debug/ExitHandler.h"
#include "Rundata/Rundata.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/DocTypes.h"


u64 AosStorageEngineObj::smMaxGroupedDocFileSize = AosStorageEngineObj::eDftMaxGroupedDocFileSize;
bool AosStorageEngineObj::smInited = false;

static AosStorageEngineObjPtr	sgStorageEngineObjs[AosDocType::eMax];
static OmnMutex				sgLock;

AosStorageEngineObj::AosStorageEngineObj(
		const AosDocType::E type, 
		const bool regflag)
:
mType(type)
{
	if (regflag)
	{
		AosStorageEngineObjPtr thisptr(this, false);
		registerStorageEngineObj(thisptr);
	}
}


bool
AosStorageEngineObj::registerStorageEngineObj(const AosStorageEngineObjPtr &storage_eng)
{
	OmnString errmsg;
    aos_assert_r(AosDocType::isValid(storage_eng->mType), false);

	sgLock.lock();
    if (sgStorageEngineObjs[storage_eng->mType])
	{
		OmnAlarm << "StorageEngineObj already registered: " << storage_eng->mType << enderr;
		sgLock.unlock();
		return false;
	}
    sgStorageEngineObjs[storage_eng->mType] = storage_eng;
	sgLock.unlock();
	return true;
}


AosStorageEngineObjPtr
AosStorageEngineObj::getStorageEngine(const AosDocType::E type)
{
	if (!AosDocType::isValid(type)) return 0;
	
	if (!smInited)
	{
		OmnAlarm << "StorageEngine not inited yet. Please add the following line in main.cpp:\n"
			<< " AosAllStorageEngine::init();" << enderr;
		OmnExitApp("StorageEngine not initialized");
		return 0;
	}
	return sgStorageEngineObjs[type];
}

void 
AosStorageEngineObj::storageEngineInited(const AosXmlTagPtr &config)
{
	smInited = true;
	aos_assert(config);
	AosXmlTagPtr conf = config->getFirstChild("storageengine");
	if (conf)
	{
		u64 max_file_size = conf->getAttrU64("max_file_size", 0);	
		if (max_file_size > 0)
		{
			smMaxGroupedDocFileSize = max_file_size; 
		}
	}
}


// bool
// AosStorageEngineObj::init()
// {
// 	if(sgInited) return true;
// 	sgInited = true;
// 	static AosSengineGroupedDoc sgAosSengineGroupedDoc(true);
// 	return true;
// }

