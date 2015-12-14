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
//	This class is used to manage a group of files, used to store 
//	either IILs or Documents. There are a number of files in the
//  group, each with a unique sequence number. Each file can 
//  store up to a given amount of data. 
//	
// Modification History:
// 09/13/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/SpaceAlloc.h"

#include "StorageMgrUtil/SpaceAllocRoundRobin.h"
#include "StorageMgrUtil/SpaceAllocAppointRobin.h"
#include "StorageMgr/DiskAlloc.h"
#include "Util/OmnNew.h"


AosSpaceAllocPtr 	AosSpaceAlloc::smSpaceAlloc[eMax];

bool
AosSpaceAlloc::createSpaceAlloc(
	const AosXmlTagPtr &config,
	const AosStorageMgrPtr &storage_mgr,
	map<u32, AosDiskAllocPtr> &total_device)
{
	smSpaceAlloc[eRoundRoin] = OmnNew AosSpaceAllocRoundRobin(storage_mgr, total_device);
	aos_assert_r(smSpaceAlloc[eRoundRoin], false);

	smSpaceAlloc[eAppointRobin] = OmnNew AosSpaceAllocAppointRobin(total_device);
	aos_assert_r(smSpaceAlloc[eAppointRobin], false);
	return true;
}


AosSpaceAllocPtr
AosSpaceAlloc::getSpaceAlloc(const bool appoint_device_id)
{
	if (appoint_device_id)
	{
		aos_assert_r(smSpaceAlloc[eAppointRobin], 0);
		return smSpaceAlloc[eAppointRobin];
	}
	aos_assert_r(smSpaceAlloc[eRoundRoin], 0);
	return smSpaceAlloc[eRoundRoin]; 
}
