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
#include "ReplicMgr/ReplicNoBkp.h"

#include "API/AosApiG.h"
#include "API/AosApiI.h"
#include "Porting/Sleep.h"
#include "StorageMgr/StorageMgr.h"

AosReplicNoBkp::AosReplicNoBkp(const bool regflag)
:
AosReplicPolicy(AOSPOLICY_NOBKP, AosReplicPolicyType::eNoBkp, regflag)
{
}


AosReplicNoBkp::~AosReplicNoBkp()
{
}


