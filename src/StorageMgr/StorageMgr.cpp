////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 09/07/2011 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/StorageMgr.h"

#include "alarm_c/alarm.h"
#include "Alarm/Alarm.h"
#include "API/AosApiG.h"
#include "Porting/Sleep.h"
#include "Rundata/Rundata.h"
#include "SEUtil/SystemIdNames.h"
#include "SEUtil/SeConfig.h"
#include "StorageMgr/FileId.h"
#include "StorageMgr/IdPrefix.h"
#include "StorageMgr/SpaceAlloc.h"

#include <sys/stat.h>
#include <dirent.h>


AosStorageMgr::AosStorageMgr()
:
mLock(OmnNew OmnMutex())
{
}


AosStorageMgr::~AosStorageMgr()
{
}


bool
AosStorageMgr::config(const AosXmlTagPtr &config)
{
	// The configuration should be in the following format:
	// 	<config ...>
	// 		<storage_mgr>
	// 			<space_allocation />
	// 			<default_dev_policy ../>
	// 			<AOSCONFIG_DEVICE>
	// 				<partition policy .../>
	// 				<partition .../>
	// 				...
	// 			</AOSCONFIG_DEVICE>
	// 		</storage_mgr>
	// 		...
	// 	</config>
	aos_assert_r(config, false);
	AosXmlTagPtr stm_conf = config->getFirstChild(AOSCONFIG_STORAGEMGR);
	aos_assert_r(stm_conf, false);

	AosXmlTagPtr device_conf = stm_conf->getFirstChild(AOSCONFIG_DEVICE);
	aos_assert_r(device_conf, false);
	AosXmlTagPtr part_conf = device_conf->getFirstChild(AOSCONFIG_PARTITION); 
	while(part_conf)
	{
		AosDiskAllocPtr partition;
		try
		{
			partition = OmnNew AosDiskAlloc(part_conf);	
		}
		catch (...)
		{
			OmnAlarm << "error!" << enderr;
			return true;
		}
		
		int part_id = partition->getDeviceId();
		aos_assert_r(part_id >=0, false);
		
		pair<mapitr_d, bool> pr = mDevices.insert(make_pair(part_id, partition));
		aos_assert_r(pr.second, false);
		
		if(partition->isDamageDevice())
		{
			OmnScreen << "partition has damaged!" << part_id<< endl;
			return false;
		}
		if(partition->isNewDevice())	mNewDevices.insert(part_id);

		part_conf = device_conf->getNextChild(AOSCONFIG_PARTITION);
	}
	// Chen Ding, 02/19/2012
	if (!mDevices.size())
	{
		OmnAlarm << "No partition configured! Please fix the configuration!" << enderr;
		exit(-1);
	}

	initDeviceSpaceAlloc();
	
	// Linda, 2013/05/14
	//AosStorageMgrPtr thisPtr(this, false);
	//mSpaceAllocator->setStorageMgr(thisPtr, mDevices, mDeviceNum);
	// init space alloc
	AosStorageMgrPtr thisPtr(this, false);
	AosXmlTagPtr spaceAlloc = stm_conf->getFirstChild(AOSCONFIG_SPACE_ALLOC);
	AosSpaceAlloc::createSpaceAlloc(spaceAlloc, thisPtr, mDevices);
	return true;
}


bool
AosStorageMgr::allocateSpace(
		AosRundata *rdata,
		const u32 virtual_id,
		const u32 site_id,
		const u64 request_space,
		const bool reserve_flag,
		const int filter_deviceid,
		const bool appoint_device_id,
		int &device_id,
		OmnString &dir_name)
{
	// 1. first get the available device_id.
	//aos_assert_rr(mSpaceAllocator, rdata, false);
	// Ketty 2013/01/17
	//bool rslt = mSpaceAllocator->allocateSpace(rdata, virtual_id, 
	//				site_id, request_space, reserve_flag, filter_deviceid, device_id, dir_name);
	//	Linda 2013/05/14
	//bool rslt = mSpaceAllocator->allocateSpace(rdata, virtual_id, 
	//				request_space, reserve_flag, filter_deviceid, device_id, dir_name);

	AosSpaceAllocPtr space_allocator = AosSpaceAlloc::getSpaceAlloc(appoint_device_id);
	aos_assert_r(space_allocator, false);

	//bool rslt = space_allocator->allocateSpace(rdata, virtual_id,
	//				request_space, reserve_flag, filter_deviceid, device_id, dir_name);

	bool rslt = space_allocator->allocateSpace(rdata, virtual_id,
					20, reserve_flag, filter_deviceid, device_id, dir_name);
	
	aos_assert_rr(rslt && device_id >=0 && dir_name != "", rdata, false);

	// 2. save the alloc to the mSpaceAllocfile.
	saveSpaceAlloc(device_id);
	return true;
}


bool
AosStorageMgr::hasNewDevice()
{
	return mNewDevices.size() != 0;
}

bool
AosStorageMgr::isAvailDevice(const int device_id)
{
	aos_assert_r(device_id>=0, false);
	mapitr_d itr = mDevices.find(device_id);
	if(itr == mDevices.end())
	{
		// means this disk is not exist.
		return false;
	}
	
	set<u32>::iterator s_itr = mNewDevices.find(device_id);
	bool avail = (s_itr == mNewDevices.end());
	return avail;
}


bool
AosStorageMgr::getGoodFiles(set<AosStorageFileInfo> &files, const u32 cube_id)
{
	AosDiskAllocPtr disk_alloc;
	mapitr_d itr = mDevices.begin();
	set<u32>::iterator s_itr;
	for(; itr != mDevices.end(); itr++)
	{
		u32 device_id = itr->first;	
		s_itr = mNewDevices.find(device_id);
		if(s_itr != mNewDevices.end())		continue;  // new disk. ignore.
		
		disk_alloc = itr->second;
		aos_assert_r(disk_alloc, false);
		disk_alloc->getTotalFiles(files, cube_id);
	}
	return true;
}


int
AosStorageMgr::getNewDeviceId()
{
	if(mNewDevices.size() == 0)	return -1;
	
	u32 idx = rand() % mNewDevices.size();
	
	set<u32>::iterator s_itr = mNewDevices.begin();
	while(idx--)
	{
		s_itr++;
	}
	
	aos_assert_r(s_itr != mNewDevices.end(), -1);
	return *s_itr;
}


bool
AosStorageMgr::allocSpaceFromNewDisk(
		AosRundata *rdata,
		const u32 virtual_id,
		const u32 site_id,
		const u64 request_space,
		int &device_id,
		OmnString &dir_name)
{
	device_id = getNewDeviceId();
	aos_assert_r(device_id>=0, false);
	bool rslt = allocateSpace(rdata, virtual_id, site_id, request_space,
			true, -1, true, device_id, dir_name);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosStorageMgr::setNewDiskAvail()
{
	mLock->lock();
	
	set<u32>::iterator s_itr = mNewDevices.begin();
	mapitr_d m_itr;
	AosDiskAllocPtr disk_alloc;
	for(; s_itr != mNewDevices.end(); s_itr++)
	{
		u32 device_id = *s_itr;
		m_itr = mDevices.find(device_id);
		aos_assert_rl(m_itr != mDevices.end(), mLock, false);
		
		disk_alloc = m_itr->second;
		disk_alloc->setNewDiskAvail();
	}
		
	mNewDevices.clear();
	mLock->unlock();
	return true;	
}


bool
AosStorageMgr::initDeviceSpaceAlloc()
{
	// init mSpaceAllocFile
	/*
	OmnString sys_dir = getSysDir();
	OmnString fname = sys_dir;
	fname << "space_rem";

	mSpaceAllocFile = OmnNew OmnFile(fname, OmnFile::eReadWrite AosMemoryCheckerArgs);
	if(!mSpaceAllocFile->isGood())
	{
		// The file has not been created yet. Create it.
		mSpaceAllocFile = OmnNew OmnFile(fname, OmnFile::eCreate AosMemoryCheckerArgs);
		if(!mSpaceAllocFile->isGood())
		{
			OmnAlarm << "Failed to open the file: " << fname << enderr;
			return false;
		}
	
		mSpaceAllocFile->setU32(0, mDevices.size(), true);	
	}

	// init the mDevices
	u32 buff_len = eEntryStart + eMaxDeviceNum * eEntrySize;;
	AosBuffPtr file_data = OmnNew AosBuff(buff_len, 0 AosMemoryCheckerArgs);
	int read = mSpaceAllocFile->readToBuff(0, buff_len, file_data.data());
	file_data->setDataLen(read);
	if(read > 4)
	{
		u32 file_device_num = file_data->getU32(0);
		
		for(u32 i=0; i<file_device_num; i++)
		{
			if(i>= mDeviceNum)
			{
				OmnScreen << "Device " << i << "-" << (file_device_num -1)
					<< " has deleted ?"	<< endl;
				return true;
			}

			aos_assert_r(file_data->getU32(0) == i, false);
			u64 alloced_size = file_data->getU64(0); 
			
			aos_assert_r(mDevices[i], false);
			mDevices[i]->setAllocedSize(alloced_size);	
		}
		if(mDeviceNum > file_device_num)
		{
			mSpaceAllocFile->setU32(0, mDeviceNum, true);	
		}
	}
	*/	
	return true;
}


bool
AosStorageMgr::saveSpaceAlloc(const u32 device_id)
{
	/*
	u32 offset = eEntryStart + device_id * eEntrySize;
	u64 alloced_size = mDevices[device_id]->getAllocedSize();
	mSpaceAllocFile->lock();
	mSpaceAllocFile->setU32(offset, device_id, false);
	mSpaceAllocFile->setU64(offset + sizeof(u32), alloced_size, true);	
	mSpaceAllocFile->unlock();
	*/
	return true;	
}


/*
bool
AosStorageMgr::deleteFiles(const vector<AosStorageFileInfo> &total_files)
{
	u64 free_size[mDeviceNum];
	memset(free_size, 0, sizeof(u64) * mDeviceNum);

	for(u32 i=0; i<total_files.size(); i++)
	{
		u32 device_id = total_files[i].deviceId;
		free_size[device_id] += total_files[i].requestSize;

		OmnString full_fname = total_files[i].getFullFname();
		if(full_fname == "")
		{
			OmnAlarm << "fname is empty!" << enderr;
			continue;
		}
		//unlink(full_fname.data());
	}
	
	for(u32 i=0; i<mDeviceNum; i++)
	{
		freeSpace(i, free_size[i]);
	}
	
	return true;    
}


bool
AosStorageMgr::freeSpace(
		const int device_id,
		const u64 free_size)
{
	aos_assert_r(device_id >=0 && (u32)device_id < mDeviceNum, false);
	aos_assert_r(getSpaceAllocFile(), false);

	u64 alloced_size = mDevices[device_id]->getAllocedSize() - free_size;
	mDevices[device_id]->setAllocedSize(alloced_size);	
	
	u32 offset = eEntryStart + device_id * eEntrySize;
	mSpaceAllocFile->lock();
	mSpaceAllocFile->setU32(offset, device_id, false);
	mSpaceAllocFile->setU64(offset + sizeof(u32), alloced_size, true);	
	mSpaceAllocFile->unlock();
	return true;
}


bool
AosStorageMgr::checkSpace(const u64 need_space, bool &enough)
{
	u64 total_available = 0;
	for(u32 i=0; i<mDeviceNum; i++)
	{
		total_available += mDevices[i]->getAvailableSize();	
	}
	
	enough = (total_available > need_space);
	return true;
}
*/

