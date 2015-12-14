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
#include "ReplicMgr/ReplicMgr.h"

#include "API/AosApiG.h"
#include "SEInterfaces/ServerInfo.h"
#include "ReplicMgr/ReplicPolicy.h"

AosReplicMgr::AosReplicMgr()
{
}


AosReplicMgr::~AosReplicMgr()
{
}


bool
AosReplicMgr::initPolicy(const OmnString str_policy)
{
	mPolicy = AosReplicPolicy::getReplicPolicy(str_policy);
	return true;
}


u32
AosReplicMgr::getCubeBkpNum()
{
	aos_assert_r(mPolicy, 0);
	return mPolicy->getCubeBkpNum();
}

AosReplicPolicyType::E
AosReplicMgr::getPolicyType()
{
	aos_assert_r(mPolicy, AosReplicPolicyType::eInvalid);
	return mPolicy->getType();
}

bool
AosReplicMgr::initBkpProc(
		map<u32, AosServerInfoPtr> &svr_infos,
		map<u32, AosCubeGroupPtr> &cube_grps)
{
	map<u32, AosServerInfoPtr>::iterator s_itr = svr_infos.begin();

	bool rslt;
	for(; s_itr != svr_infos.end(); s_itr++)
	{
		AosServerInfoPtr svr_info = s_itr->second;
		vector<ProcInfo> & total_procs = svr_info->getDefProcs();
		for(u32 i=0; i<total_procs.size(); i++)
		{
			ProcInfo proc = total_procs[i]; 
			AosProcessType::E type = proc.mType;
			if(type != AosProcessType::eCube)	continue;

			int cube_grp_id = proc.mCubeGrpId;
			aos_assert_r(cube_grp_id != -1, false);
			map<u32, AosCubeGroupPtr>::iterator c_itr = cube_grps.find(cube_grp_id);
			aos_assert_r(c_itr != cube_grps.end(), false);
			AosCubeGroupPtr cube_group = c_itr->second;
			
			rslt = mPolicy->addBkpProc(svr_infos, s_itr, cube_group);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


