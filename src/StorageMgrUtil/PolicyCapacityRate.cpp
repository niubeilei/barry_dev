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

#include "StorageMgrUtil/PolicyCapacityRate.h"

#include "XmlUtil/XmlTag.h"

AosPolicyCapacityRate::AosPolicyCapacityRate(const AosXmlTagPtr &config)
{
	//aos_assert(config);
	//mRate = config->getAttrStr("capacity_rate", 0.8);			
}


AosPolicyCapacityRate::~AosPolicyCapacityRate()
{
}


bool
AosPolicyCapacityRate::canAllocateSpace(
		const u64 &requested_size,
		const u64 &reserved_size,
		const u64 &total_size,
		AosRundata *rdata)
{
	if(reserved_size + requested_size < total_size)
	{
		return true;	
	}
	return false;
}

u64 
AosPolicyCapacityRate::getAvailableSize(const u64 allocedSize, const u64 totalSize)
{
	if(totalSize <= allocedSize)	return 0;
	return totalSize - allocedSize;
}


