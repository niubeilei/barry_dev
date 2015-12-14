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
// 07/27/2012 	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "TransUtil/ServerFix.h"

AosServerFix::AosServerFix(const int numphy)
:
mNumServers(numphy)
{
}


AosServerFix::~AosServerFix()
{
}


int
AosServerFix::routeReq(const u64 &physical_server_id)
{
	// This distributor distributes traffic based on physical 
	// server ID.
	if(mNumServers <= 0 || physical_server_id >= (u64)mNumServers) return -1;	
	
	return physical_server_id;
}

