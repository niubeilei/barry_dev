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
// 03/25/2013	Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "ReplicMgr/AllReplicPolicy.h"

#include "ReplicMgr/ReplicOnePlusOne.h"
#include "ReplicMgr/ReplicOnePlusTwo.h"
#include "ReplicMgr/ReplicNoBkp.h"
#include <map>


map<OmnString, u32>  sgReplicPolicTpMap;
AosReplicPolicyPtr 	 sgReplicPolicy[AosReplicPolicyType::eMax];	
AosAllReplicPolicy   gAosAllReplicPolicy;

AosAllReplicPolicy::AosAllReplicPolicy()
{
	static AosReplicOnePlusOne  sgAosReplicOnePlusOne(true);
	static AosReplicOnePlusTwo  sgAosReplicOnePlusTwo(true);
	static AosReplicNoBkp		sgAosReplicNoBkp(true);
}


