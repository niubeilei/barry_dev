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
// 07/27/2011	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#ifndef AOS_StorageMgr_StorageMgr_h
#define AOS_StorageMgr_StorageMgr_h

#include "DocFileMgr/DocFileMgr.h"
#include "Rundata/Ptrs.h"
#include "ReliableFile/ReliableFile.h"
#include "SEUtil/DocTags.h"
#include "SEInterfaces/Ptrs.h"
#include "StorageMgr/DiskAlloc.h"
#include "StorageMgr/Ptrs.h"
#include "UtilData/ModuleId.h"
#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"

#include <set>

class AosStorageMgr : virtual public OmnRCObject
{
	OmnDefineRCObject;

	enum
	{
		eMaxDeviceNum = 1000,
		eMinFileSize = 10000,
	
		// the following is for the spaceAllocFile
		eEntryStart = 4,	// the file 4 byte, save the crt device num.
		eEntrySize = 4 + 8,
		
	};
	
	typedef map<u32, AosDiskAllocPtr> map_d;
	typedef map<u32, AosDiskAllocPtr>::iterator mapitr_d;

private:
	OmnMutexPtr				mLock;
	map_d					mDevices;
	set<u32>				mNewDevices;
	
	// This file record each device already alloc how many space.
	OmnFilePtr				mSpaceAllocFile;		
	//AosSpaceAllocPtr		mSpaceAllocator;
	

public:
	AosStorageMgr();
	~AosStorageMgr();

	bool 	config(const AosXmlTagPtr &config);

	inline	OmnString	getBaseDir(const u32 device_id)
	{
		// Chen Ding, 2014/08/02
		// aos_assert_r(device_id >=0 && mDevices[device_id], "");
		mapitr_d itr = mDevices.find(device_id);
		if (itr == mDevices.end())
		{
			OmnAlarm << "Device not found: " << device_id << enderr;
			return "";
		}

		return itr->second->getBaseDir();
	}

	inline	OmnString getSysDir()
	{
		if (!mDevices[0])	return "";
		return mDevices[0]->getSysDir();
	}
	
	inline OmnString	getVirtualSysDir(
			const u32 virtual_id, 
			const bool local_backup,  
			int &device_id)
	{
		device_id = -1;
		
		if(!local_backup)
		{
			aos_assert_r(mDevices[0], "");
			device_id = 0;
			return mDevices[0]->getVirtualSysDir(virtual_id);
		}
			
		aos_assert_r(mDevices[1], "");
		device_id = 1;
		return mDevices[1]->getVirtualSysDir(virtual_id);
	}

	bool	allocateSpace(
				AosRundata *rdata,
				const u32 virtual_id,
				const u32 site_id,
				const u64 request_space,
				const bool reserve_flag,
				const int filter_deviceid,
				const bool appoint_device_id,
				int &device_id,
				OmnString &dir_name);
	
	//bool 	checkSpace(const u64 need_space, bool &enough);
	//bool	freeSpace( const int device_id, const u64 free_size);
	//bool 	deleteFiles(const vector<AosStorageFileInfo> &total_files);

	u32 	getDeviceNum() {return mDevices.size();}

	// for disk recover.
	bool 	hasNewDevice();
	bool 	isAvailDevice(const int device_id);
	bool 	getGoodFiles(set<AosStorageFileInfo> &files, const u32 cube_id);
	bool 	allocSpaceFromNewDisk(
				AosRundata *rdata,
				const u32 virtual_id,
				const u32 site_id,
				const u64 request_space,
				int &device_id,
				OmnString &dir_name);

	bool 	setNewDiskAvail();

private:
	bool 	initDeviceSpaceAlloc();
	bool 	saveSpaceAlloc(const u32 device_id);
	int 	getNewDeviceId();
	
};
#endif
