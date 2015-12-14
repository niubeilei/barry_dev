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
#ifndef AOS_StorageMgr_SpaceAlloc_h
#define AOS_StorageMgr_SpaceAlloc_h

#include "Rundata/Ptrs.h"
#include "Util/String.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "SEUtil/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "XmlUtil/Ptrs.h"

#include <map>

class AosSpaceAlloc : virtual public OmnRCObject
{
public:
	enum AosSpaceAllocType 
	{
		eInvalid,

		eRoundRoin,
		eAppointRobin,

		eMax
	};

private:
	static AosSpaceAllocPtr  			smSpaceAlloc[eMax];	

public:
	virtual bool allocateSpace(
					AosRundata *rdata,
					const u32 virtual_id,
					const u64 request_space,
					const bool reserve_flag,
					const int filter_deviceid,
					int &device_id,
					OmnString &sub_dir_name) = 0;	

	static AosSpaceAllocPtr getSpaceAlloc(const bool appoint_device_id);

	static bool createSpaceAlloc(
						const AosXmlTagPtr &config,
						const AosStorageMgrPtr &storage_mgr,
						map<u32, AosDiskAllocPtr> &total_device);
};
#endif
