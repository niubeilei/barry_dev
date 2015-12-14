////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: File.h
// Description:
//   
//
// Modification History:
// 09/07/2011	Created by Ketty.Guo
////////////////////////////////////////////////////////////////////////////
#ifndef Aos_StorageMgrUtil_PolicyCapacityRate_h
#define Aos_StorageMgrUtil_PolicyCapacityRate_h


#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "StorageMgr/StoragePolicy.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"

class AosPolicyCapacityRate : public AosStoragePolicy 
{
	OmnDefineRCObject;

public:
	AosPolicyCapacityRate(const AosXmlTagPtr &config);
	~AosPolicyCapacityRate();

	virtual bool canAllocateSpace(
					const u64 &requested_size,
					const u64 &reserved_size,
					const u64 &total_size,
					AosRundata *rdata);

	virtual u64 getAvailableSize(const u64 allocedSize, const u64 totalSize);		
};
#endif

