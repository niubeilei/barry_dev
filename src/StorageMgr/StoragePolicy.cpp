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
// 09/13/2011	Created by Ketty.Guo
////////////////////////////////////////////////////////////////////////////
#include "StorageMgr/StoragePolicy.h"

#include "StorageMgrUtil/PolicyCapacityRate.h"
#include "Util/OmnNew.h"
#include "Util/Ptrs.h"

AosStoragePolicyPtr
AosStoragePolicy::createStoragePolicy(
	const AosXmlTagPtr &config)
{
	AosStoragePolicyPtr storagePolicy = OmnNew AosPolicyCapacityRate(config);
	return storagePolicy;
}

