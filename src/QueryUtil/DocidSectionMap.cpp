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
// 2013/01/13 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#if 0
#include "QueryUtil/DocidSectionMap.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"



OmnSingletonImpl(AosDocidSectionMapSingleton,
                 AosDocidSectionMap,
                 AosDocidSectionMapSelf,
                "AosDocidSectionMap");



AosDocidSectionMap::AosDocidSectionMap()
:
mLock(OmnNew OmnMutex())
{
}


AosDocidSectionMap::~AosDocidSectionMap()
{
}


bool
AosDocidSectionMap::start()
{
	return true;
}


bool
AosDocidSectionMap::config(const AosXmlTagPtr &config)
{
	return true;
}


bool
AosDocidSectionMap::stop()
{
    return true;
}


int
AosDocidSectionMap::getPhysicalId(
		const u64 &section_id,
		const AosRundataPtr &rdata)
{
	// It converts the section ID into its physical ID. There is a map that 
	// maps section IDs to physical IDs. The map is rarely modified. 
	mLock->lock();
	OmnNotImplementedYet;
	mLock->unlock();
	return -1;
}
#endif
