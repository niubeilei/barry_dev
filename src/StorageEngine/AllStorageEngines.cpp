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
// 06/03/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "StorageEngine/AllStorageEngines.h"

#include "SEInterfaces/StorageEngineObj.h"
#include "StorageEngine/SengineGroupedDoc.h"

bool
AosAllStorageEngines::init(const AosXmlTagPtr &config)
{
	static AosSengineGroupedDoc	sgAosSengineGroupedDoc(true);

	// if (!sgInited)
	// {
	// 	sgLock.lock();
	// 	AosActionType::check();
	// 	sgInited = true;
	// 	sgLock.unlock();
	// }
	AosStorageEngineObj::storageEngineInited(config);
	return true;
}

