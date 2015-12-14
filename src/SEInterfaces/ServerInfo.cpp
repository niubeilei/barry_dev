////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// Description:
//
// Modification History:
// 11/02/2012	Created by Ketty
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/ServerInfo.h"

AosServerInfo::AosServerInfo(const AosXmlTagPtr &conf)
{
	bool rslt = config(conf);
	if(!rslt)
	{
		OmnThrowException("error!");
	}
}


AosServerInfo::~AosServerInfo()
{
}


bool
AosServerInfo::config(const AosXmlTagPtr &svr_conf)
{
	aos_assert_r(svr_conf, false);
	mServerId = svr_conf->getAttrInt("server_id", -1);
	aos_assert_r(mServerId >=0 && (u32)mServerId < eAosMaxServerId, false);

	mAddr = svr_conf->getAttrStr("remote_addr", "");
	mPort = svr_conf->getAttrInt("remote_port", -1);
	//mMountDir = svr_conf->getAttrStr("mount_dir", "");

	return true;
}


/*
bool
AosServerInfo::addDefaultProcs(
		const AosXmlTagPtr procs_conf,
		map<u32, AosCubeGroupPtr> &cube_grps)
{
	aos_assert_r(procs_conf, false);
	
	AosProcessType::E proc_tp;
	int cube_grp_id = -1;
	AosXmlTagPtr proc_conf = procs_conf->getFirstChild("proc");
	map<u32, AosCubeGroupPtr>::iterator c_itr;
	while (proc_conf)
	{
		proc_tp = AosProcessType::toEnum(proc_conf->getAttrStr("proc_type"));
		aos_assert_r(proc_tp != AosProcessType::eInvalid && proc_tp != AosProcessType::eBkpCube, false);
		
		if(proc_tp == AosProcessType::eCube)
		{
			cube_grp_id = proc_conf->getAttrInt("cube_grp_id", -1);
			aos_assert_r(cube_grp_id != -1, false);
			
			c_itr = cube_grps.find(cube_grp_id);
			aos_assert_r(c_itr != mCubeGroups.end(), false);
			AosCubeGroupPtr cube_group = c_itr->second;
			cube_group->addSvrId(svr_id); 
		}
		else
		{
			cube_grp_id = -1;
		}
		
		ProcInfo proc_info;
		proc_info.mType = proc_tp;
		proc_info.mCubeGrpId = cube_grp_id;
		mProcInfos.push_back(proc_info);
		
		proc_conf = procs_conf->getNextChild("proc");
	}
	return true;
}
*/


void
AosServerInfo::addBkpCubeProc(const int cube_grp_id)
{
	ProcInfo proc_info = {AosProcessType::eBkpCube, cube_grp_id};
	mProcInfos.push_back(proc_info);
}


// Ketty 2013/11/29
/*
bool
AosServerInfo::setProcStatus(
		const u32 proc_id, 
		const AosProcInfo::Status status)
{
	mProcStatus[proc_id] = status;
	return true;
}

AosProcInfo::Status 
AosServerInfo::getProcStatus(const AosProcessType::E type)
{
	map<u32, AosProcInfoPtr>::iterator itr = mProcInfos.begin();
	for(; itr != mProcInfos.end(); itr++)
	{
		if ((itr->second)->getType() == type)
		{
			u32 proc_id = (itr->second)->getProcId();
			map<u32, AosProcInfo::Status>::iterator s_itr = mProcStatus.find(proc_id);
			if (s_itr != mProcStatus.end())
			{
				return mProcStatus[proc_id];
			}
		}
	}
	return AosProcInfo::eUnknown;
}

AosProcInfo::Status
AosServerInfo::getProcStatus(const u32 proc_id)
{
	map<u32, AosProcInfo::Status>::iterator s_itr = mProcStatus.find(proc_id);
	if (s_itr != mProcStatus.end())
	{
		return mProcStatus[proc_id];
	}
	return AosProcInfo::eUnknown;
}
*/
