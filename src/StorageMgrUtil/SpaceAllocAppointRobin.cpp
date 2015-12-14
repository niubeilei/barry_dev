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
// 09/07/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////

#include "StorageMgrUtil/SpaceAllocAppointRobin.h"

#include "ErrorMgr/ErrMsgMgr.h"
#include "ErrorMgr/ErrmsgId.h"
#include "ErrorMgr/Ptrs.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/DiskAlloc.h"
#include "StorageMgr/StorageMgr.h"
#include "XmlUtil/XmlTag.h"


AosSpaceAllocAppointRobin::AosSpaceAllocAppointRobin(
		map<u32, AosDiskAllocPtr> &devices)
:
mLock(OmnNew OmnMutex),
mDevices(devices)
{
}


AosSpaceAllocAppointRobin::~AosSpaceAllocAppointRobin()
{
}



bool
AosSpaceAllocAppointRobin::allocateSpace(
		AosRundata *rdata,
		const u32 virtual_id,
		const u64 request_space,
		const bool reserve_flag,
		const int filter_deviceid,
		int &device_id,
		OmnString &sub_dir_name)	
{
	// This function allocates the space. It first determines the partition.
	aos_assert_r(rdata, false);

	aos_assert_r(filter_deviceid != device_id, false);
	aos_assert_r(device_id != -1 && mDevices[device_id], false);

	mLock->lock();
	bool rslt = mDevices[device_id]->allocateSpace(rdata, virtual_id, 
			request_space, reserve_flag, sub_dir_name);

	mLock->unlock();
	if(rslt)	return true;
	
	AosSetError(rdata, AosErrmsgId::eFailedAllocatingSpace);
	OmnAlarm << rdata->getErrmsg() << ". Size: " << request_space << enderr;
	device_id = -1;
	sub_dir_name = "";
	return false;
}


