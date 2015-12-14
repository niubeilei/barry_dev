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
#include "TransUtil/RoundRobin.h"

#include "API/AosApi.h"
#include "Rundata/Rundata.h"
#include "SEUtil/Ptrs.h"
#include "TransBasic/Trans.h"

AosRoundRobin::AosRoundRobin()
{
	mSvrIds = AosGetServerIds();
	mCrtItr = mSvrIds.begin();
}


AosRoundRobin::~AosRoundRobin()
{
}


int
AosRoundRobin::routeReq(const u64 &dist_id)
{
	u32 svr_num = mSvrIds.size();
	if(svr_num == 0) return -1;	

	u32 svr_id = *mCrtItr;
	mCrtItr++;
	if(mCrtItr == mSvrIds.end())	mCrtItr == mSvrIds.begin();
	return svr_id;

}

