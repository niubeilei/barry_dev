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
#ifndef Aos_StorageMgr_StoragePolicy_h
#define Aos_StorageMgr_StoragePolicy_h


#include "aosUtil/Types.h"
#include "Rundata/Ptrs.h"
#include "StorageMgr/Ptrs.h"
#include "Util/RCObject.h"
#include "Util/RCObjImp.h"
#include "Util/String.h"
#include "Util/Ptrs.h"
#include "UtilComm/Ptrs.h"
#include "XmlUtil/Ptrs.h"

class AosStoragePolicy : public virtual OmnRCObject
{

public:
	virtual bool canAllocateSpace(
					const u64 &requested_size,
					const u64 &reserved_size,
					const u64 &total_size,
					AosRundata *rdata) = 0;
	
	virtual u64 getAvailableSize(const u64 allocedSize, const u64 totalSize) = 0;

	static AosStoragePolicyPtr createStoragePolicy(
					const AosXmlTagPtr &config);
};
#endif

