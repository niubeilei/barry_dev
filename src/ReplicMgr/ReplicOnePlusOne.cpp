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
#include "ReplicMgr/ReplicOnePlusOne.h"

#include "API/AosApiG.h"
#include "API/AosApiI.h"
#include "Porting/Sleep.h"
#include "SEInterfaces/VfsMgrObj.h"


AosReplicOnePlusOne::AosReplicOnePlusOne(const bool regflag)
:
AosReplicPolicy(AOSPOLICY_ONEPLUSONE, AosReplicPolicyType::eOnePlusOne, regflag)
{
}

	
AosReplicOnePlusOne::~AosReplicOnePlusOne()
{
}


bool
AosReplicOnePlusOne::addBkpProc(
		map<u32, AosServerInfoPtr> &svr_infos,
		map<u32, AosServerInfoPtr>::iterator &crt_itr,
		AosCubeGroupPtr &cube_group)
{
	aos_assert_r(svr_infos.size() >= 2, false);
	aos_assert_r(crt_itr != svr_infos.end(), false);

	// first bkp.
	map<u32, AosServerInfoPtr>::iterator bkp_s_itr = crt_itr;
	bkp_s_itr++;
	if(bkp_s_itr == svr_infos.end())	bkp_s_itr = svr_infos.begin();

	AosServerInfoPtr bkp_svr_info = bkp_s_itr->second;
	bkp_svr_info->addBkpCubeProc(cube_group->getGroupId());
	cube_group->addSvrId(bkp_svr_info->getServerId()); 
	return true;
}


