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
#include "TransUtil/IdRobin.h"

#include "API/AosApiG.h"

AosIdRobin::AosIdRobin()
//AosIdRobin::AosIdRobin(const AosServerInfo::Type type)
:
mCrtVirServerNum(0)
//mSvrType(type)
{
	mNumVirtuals = AosGetNumCubes();
	aos_assert(mNumVirtuals >0);
}


AosIdRobin::~AosIdRobin()
{
}


int
AosIdRobin::routeReq(const u64 &distid)
{
	// This Func is just get the virtual_id, not the svr_id.
	if(mNumVirtuals == 0) return -1;

	u32 vid = 0;
	if (distid == 0)
	{
		vid = mCrtVirServerNum++;
		if(mCrtVirServerNum >= mNumVirtuals)
		{
			mCrtVirServerNum = 0;
		}
	}
	else
	{
		vid = distid % mNumVirtuals;
	}
	
	return vid;
}

