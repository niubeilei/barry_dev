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
#ifndef AOS_StorageMgrUtil_SpaceAllocAppointRobin_h
#define AOS_StorageMgrUtil_SpaceAllocAppointRobin_h

#include "Util/Buff.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEUtil/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "StorageMgr/SpaceAlloc.h"


class AosSpaceAllocAppointRobin :	virtual public AosSpaceAlloc
{
	OmnDefineRCObject; 

private:
	OmnMutexPtr				mLock;
	map<u32, AosDiskAllocPtr> mDevices;

public:
	AosSpaceAllocAppointRobin(map<u32, AosDiskAllocPtr> &devices);
	~AosSpaceAllocAppointRobin();

	virtual bool allocateSpace(
					AosRundata *rdata,
					const u32 virtual_id,
					const u64 request_space,
					const bool reserve_flag,
					const int filter_deviceid,
					int &device_id,
					OmnString &sub_dir_name);	
};
#endif
