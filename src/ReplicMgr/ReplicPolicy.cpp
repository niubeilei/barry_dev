////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//	
// Modification History:
// 09/21/2012 Created by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "ReplicMgr/ReplicPolicy.h"

#include "API/AosApiG.h"
#include "SEInterfaces/ServerInfo.h"

extern AosReplicPolicyPtr 	sgReplicPolicy[AosReplicPolicyType::eMax];	
static OmnMutex             sgLock;

AosReplicPolicy::AosReplicPolicy(
		const OmnString &name,
		const AosReplicPolicyType::E tp,
		const bool reg)
:
mName(name),
mType(tp)
{
	if(reg)
	{
		AosReplicPolicyPtr thisptr(this, false);
		registerReplicPolicy(thisptr);
	}
}


AosReplicPolicy::~AosReplicPolicy()
{
}


bool
AosReplicPolicy::registerReplicPolicy(AosReplicPolicyPtr &rp)
{
	sgLock.lock();
	bool valid = AosReplicPolicyType::isValid(rp->mType);
	if(!valid)
	{
		OmnAlarm << "Invalid type: " << rp->mType << enderr;
		sgLock.unlock();
		return false;
	}

	bool rslt = AosReplicPolicyType::addName(rp->mName, rp->mType);
	if(!rslt)
	{
		OmnAlarm << "error!: " << enderr;
		sgLock.unlock();
		return false;
	}
	
	if(!sgReplicPolicy[rp->mType])
	{
		sgReplicPolicy[rp->mType] = rp;
	}
	sgLock.unlock();
	return true;
}


AosReplicPolicyPtr
AosReplicPolicy::getReplicPolicy(const OmnString &str_policy)
{
	AosReplicPolicyType::E type = AosReplicPolicyType::toEnum(str_policy);
	aos_assert_r(AosReplicPolicyType::isValid(type), 0);	
	
	sgLock.lock();
	AosReplicPolicyPtr policy = sgReplicPolicy[type];	
	sgLock.unlock();
	
	aos_assert_r(policy, 0);
	return policy;
}


