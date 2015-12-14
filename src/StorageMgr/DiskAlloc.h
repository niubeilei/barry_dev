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
#ifndef AOS_StorageMgr_DiskAlloc_h
#define AOS_StorageMgr_DiskAlloc_h

#include "Rundata/Ptrs.h"
#include "SEUtil/Ptrs.h"
#include "StorageMgrUtil/Ptrs.h"
#include "StorageMgrUtil/DevLocation.h"
#include "StorageMgrUtil/FileInfo.h"
#include "StorageMgr/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/Ptrs.h"
#include "Util/File.h"
#include <vector>
#include <map>
#include <set>
using namespace std;

class AosDiskAlloc : virtual public OmnRCObject
{
	OmnDefineRCObject;
	enum
	{
		eTotalReservedOffset = 0,
		ePoison = 0x12345,
	};

private:
	OmnMutexPtr		mLock;
	int				mDeviceId;
	bool			mIsDamageDevice;
	bool			mIsNewDevice;
	u64				mTotalSize;
	u64				mAllocedSize;

	AosStorageDirInfoPtr			mDirInfo;
	map<u32, AosVirtualDirInfoPtr>	mVirtualDirMap;

	AosStoragePolicyPtr	mPolicy;
	AosDevLocationPtr	mDevLocation;

public:
	AosDiskAlloc(const AosXmlTagPtr &config);
	~AosDiskAlloc();
	
	bool	isDamageDevice(){ return mIsDamageDevice; };
	bool	isNewDevice(){ return mIsNewDevice; };
	u32		getDeviceId(){ return mDeviceId; };
	bool 	setNewDiskAvail();

	OmnString	getBaseDir(){ return mDevLocation->getUserDir();	}
	OmnString 	getSysDir();
	OmnString	getVirtualSysDir(const u32 virtual_id);

	u64			getAllocedSize(){ return mAllocedSize; };
	void        setAllocedSize(const u64 alloced_size){ mAllocedSize = alloced_size ;};
	u64			getAvailableSize();
	u64			getTotalSize(){ return mTotalSize;};

	bool		allocateSpace(
				    AosRundata *rdata,
					const u32 virtual_id,
					const u64 request_space,
					const bool reserve_flag,
					OmnString &sub_dir_name);

	bool		removeDir(const u32 virtual_id);
	u64 		getTotalSize() const {return mTotalSize;}
	
	OmnString	getDiskInfo();

	// for disk recover.
	bool 		getTotalFiles(set<AosStorageFileInfo> &files, const u32 cube_id);
	
private:
	void		init();
	bool		checkDevice();
	OmnString 	getNextSubDirname(const u32 virtual_id, const u32 site_id);
	AosVirtualDirInfoPtr	getVirtualDirInfo(const u32 virtual_id, bool create = true);

};
#endif
