////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
//
// Modification History:
// 05/31/2011 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/IILIdRobin.h"

#include "API/AosApiG.h"

#if 0
AosIILIdRobin::AosIILIdRobin()
:
mCrtServerNum(0)
{
	mNumServers = AosGetNumPhysicals();
	mNumVirtuals = AosGetNumCubes();
	aos_assert(mNumServers >0 && mNumVirtuals >0);
}


AosIILIdRobin::~AosIILIdRobin()
{
}


int
AosIILIdRobin::routeReq(const u64 &distid)
{
	if (mNumServers == 0 || mNumVirtuals== 0) return -1;	

	if (distid == 0)
	{
		u32 idx = mCrtServerNum;
		mCrtServerNum = (mCrtServerNum + 1) % mNumServers;
		return idx;
	}
	
	u32 vid = distid % mNumVirtuals;
	u32 idx = AosGetPhysicalIdByVirtualId(vid); 	
	return idx;
}
#endif
