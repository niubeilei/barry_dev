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

#include "StorageMgrUtil/SpaceAllocRoundRobin.h"

#include "ErrorMgr/ErrMsgMgr.h"
#include "ErrorMgr/ErrmsgId.h"
#include "ErrorMgr/Ptrs.h"
#include "Rundata/Rundata.h"
#include "StorageMgr/DiskAlloc.h"
#include "StorageMgr/StorageMgr.h"
#include "XmlUtil/XmlTag.h"


AosSpaceAllocRoundRobin::AosSpaceAllocRoundRobin(
		const AosStorageMgrPtr &storage_mgr,
		map<u32, AosDiskAllocPtr> &devices)
:
mLock(OmnNew OmnMutex),
mDevices(devices),
mStorageMgr(storage_mgr),
mCrtIdx(0)
{
	u32 device_num = devices.size();
	mAllocationSizes = OmnNew int[device_num];
	mPartitionSizes = OmnNew u32[device_num];
	mMinPartitionSize = 0;

	for(u32 i=0; i<device_num; i++)
	{
		u32 total_size = (mDevices[i]->getTotalSize() / 1024 / 1024 /1024);
		mPartitionSizes[i] = total_size;
		mAllocationSizes[i] = total_size;
		if(mMinPartitionSize == 0 || mMinPartitionSize > total_size)
		{
			mMinPartitionSize = total_size;
		}
	}
}


AosSpaceAllocRoundRobin::~AosSpaceAllocRoundRobin()
{
}


/*
void
AosSpaceAllocRoundRobin::setStorageMgr(
		const AosStorageMgrPtr &storage_mgr,
		AosDiskAllocPtr *devices,
		const u32 deviceNum)
{
	mDevices = devices;	
	mDeviceNum = deviceNum;
	mStorageMgr = storage_mgr;
}
*/

bool
AosSpaceAllocRoundRobin::allocateSpace(
		AosRundata *rdata,
		const u32 virtual_id,
		const u64 request_space,
		const bool reserve_flag,
		const int filter_deviceid,
		int &device_id,
		OmnString &sub_dir_name)	
{
	// This function allocates the space. It first determines the partition.
	/*
	mLock->lock();
	for (u32 i=0; i<mDeviceNum; i++)
	{
		if (mDevices[mCrtDeviceid]->allocateSpace(rdata, virtual_id, site_id, 
						request_space, reserve_flag, device_id, sub_dir_name))
		{
			mLock->unlock();

			mCrtDeviceid++;
			if (mCrtDeviceid >= mDeviceNum) mCrtDeviceid = 0;
			return true;
		}
		mCrtDeviceid++;
		if (mCrtDeviceid >= mDeviceNum) mCrtDeviceid = 0;
	}

	mLock->unlock();
	AosSetError(rdata, AosErrmsgId::eFailedAllocatingSpace);
	OmnAlarm << rdata->getErrmsg() << ". Size: " << request_space << enderr;
	device_id = -1;
	sub_dir_name = "";
	return false;
	*/	

	aos_assert_r(rdata, false);
	bool rslt = false;
	int trys = 500;
	mLock->lock();
	while(trys--)
	{
		device_id = getNextPartition(); 
		aos_assert_rl(device_id != -1, mLock, false);
		// Ketty 2013/01/17
		if(filter_deviceid == device_id)	continue;

		rslt = mDevices[device_id]->allocateSpace(rdata, virtual_id, 
				request_space, reserve_flag, sub_dir_name);
		if(rslt)	break;
	}
	mLock->unlock();
	if(rslt)	return true;
	
	AosSetError(rdata, AosErrmsgId::eFailedAllocatingSpace);
	OmnAlarm << rdata->getErrmsg() << ". Size: " << request_space << enderr;
	device_id = -1;
	sub_dir_name = "";
	return false;
}


int
AosSpaceAllocRoundRobin::getNextPartition()
{
	// This function determines the next partition. There are two
	// arrays:
	// 		mPartitionSizes[];
	// 		mAllocationSizes[];
	//
	// Every time when we want to get the next partition, we increment 
	// mAllocationSizes[mCrtIdx] += mMinPartitionSize;
	// If it is bigger than mPartitionSizes[mCrtIdx], need to 
	// move on to the next one. 

	/*
	mAllocationSizes[mCrtIdx] += mMinPartitionSize;
	if (mAllocationSizes[mCrtIdx] > mPartitionSizes[mCrtIdx])
	{
		mAllocationSizes[mCrtIdx] = 0;
		mCrtIdx++;
		if (mCrtIdx >= mDeviceNum) mCrtIdx = 0;
		mAllocationSizes[mCrtIdx] = mMinPartitionSize;
	}
	return mCrtIdx;
	*/
	
	int trys = 500;
	while(trys--)
	{
		mAllocationSizes[mCrtIdx] -= mMinPartitionSize;
		if(mAllocationSizes[mCrtIdx] >= 0 )
		{
			map<u32, AosDiskAllocPtr>::iterator itr = mDevices.begin();
			for(u32 i=0; i<mCrtIdx; i++)	itr++;
			aos_assert_r(itr != mDevices.end(), -1);
			return itr->first;
		}
	
		mAllocationSizes[mCrtIdx] = mAllocationSizes[mCrtIdx] + mMinPartitionSize + mPartitionSizes[mCrtIdx];	
		
		mCrtIdx++;
		if (mCrtIdx >= mDevices.size()) mCrtIdx = 0;
	}
	
	OmnAlarm << "error!" << enderr;
	return -1;
}

