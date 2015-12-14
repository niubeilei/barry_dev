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
// 03/18/2011	Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "SEInterfaces/NetworkMgrObj.h"

AosNetworkMgrObjPtr	AosNetworkMgrObj::smNetworkMgr;
u32 AosNetworkMgrObj::smNumVirtuals = 0;
vector<int> AosNetworkMgrObj::smCubeId2PhyId;

#if 0
#include "Debug/ExitHandler.h"
#include "Rundata/Rundata.h"
#include "SEInterfaces/ReplicPolicyType.h"
#include "SEInterfaces/NetListener.h"
#include "SEInterfaces/CubeGroup.h"
#include "SEInterfaces/ProcInfo.h"
#include "Thread/Mutex.h"
#include "XmlUtil/XmlTag.h"


// AosProcessType::E 					AosNetworkMgrObj::smSelfProcType = AosProcessType::eInvalid;
// int	 							AosNetworkMgrObj::smSelfServerId = 0;
// int	 							AosNetworkMgrObj::smSelfClientId = 0;
// u32	 							AosNetworkMgrObj::smSelfProcId = 0;
// int								AosNetworkMgrObj::smSelfCubeGrpId = -1;
// map<u32, AosServerInfoPtr>		AosNetworkMgrObj::smSvrInfos;
// vector<u32>						AosNetworkMgrObj::smSvrIds;
// map<u32, AosCubeGroupPtr>		AosNetworkMgrObj::smCubeGroups;
// vector<u32>						AosNetworkMgrObj::smCubeIds;
// map<u32, u32>			 		AosNetworkMgrObj::smCubeIdToGrpIdMap;
// AosReplicPolicyPtr				AosNetworkMgrObj::smReplicPolicy;
// vector<u32>						AosNetworkMgrObj::smSelfCubeIds;
// int								AosNetworkMgrObj::smMasterId;


bool
AosNetworkMgrObj::config(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr net_conf = app_conf->getFirstChild("networkmgr");
	if (net_conf)
	{
		bool rslt = configSelfSvr(net_conf);
		aos_assert_r(rslt, false);
		AosXmlTagPtr servers_conf = net_conf->getFirstChild("servers");
		if (servers_conf)
		{
			rslt = configSvrInfos(servers_conf);
			aos_assert_r(rslt, false);
		}

		AosXmlTagPtr cubes_conf = net_conf->getFirstChild("cubes");
		if (cubes_conf)
		{
			rslt = configCubeInfos(cubes_conf);
			aos_assert_r(rslt, false);
		}
	}

	return true;
}


bool
AosNetworkMgrObj::updateSvrInfos(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr svr_conf = app_conf->getFirstChild(AOSCONFIG_SERVER);
	while (svr_conf)
	{
		int svr_id = svr_conf->getAttrInt("server_id", -1);
		aos_assert_r(svr_id != -1, false);
		map<u32, AosServerInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
		aos_assert_r(itr != smSvrInfos.end(), false);
		AosServerInfoPtr svr_info = itr->second;
		AosXmlTagPtr proc_conf = svr_conf->getFirstChild("Process");
		while (proc_conf)
		{
			AosProcInfoPtr proc_info;
			try
			{
				proc_info = OmnNew AosProcInfo(proc_conf);
			}
			catch (...)
			{
				OmnAlarm << "error!" << enderr;
				return false;
			}

			svr_info->addProc(proc_info);
			proc_conf = svr_conf->getNextChild();
		}
		svr_conf = app_conf->getNextChild();
	}
	return true;
}

bool
AosNetworkMgrObj::configSvrInfos(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	smSvrIds.clear();
	AosXmlTagPtr svr_conf = app_conf->getFirstChild(AOSCONFIG_SERVER);
	while (svr_conf)
	{
		AosServerInfoPtr svr_info;
		try
		{
			svr_info = OmnNew AosServerInfo(svr_conf);
		}
		catch (...)
		{
			OmnAlarm << "error!" << enderr;
			return false;
		}
		
		int svr_id = svr_info->getServerId();
		aos_assert_r(svr_id >=0 && (u32)svr_id < eAosMaxServerId, false);
		//map<u32, AosServerInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
		//aos_assert_r(itr == smSvrInfos.end(), false);
		smSvrInfos[svr_id] = svr_info; 
		smSvrIds.push_back(svr_id);

		svr_conf = app_conf->getNextChild(AOSCONFIG_SERVER);
	}
	
	return true;
}

bool
AosNetworkMgrObj::addCubeInfo(const AosCubeGroupPtr &cube_grp)
{
	int group_id = cube_grp->getGroupId();
	aos_assert_r(group_id >=0, false);

	map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.find(group_id);
	aos_assert_r(itr == smCubeGroups.end(), false);
	smCubeGroups[group_id] = cube_grp; 

	vector<u32> & cubes = cube_grp->getCubeIds();
	for(u32 i=0; i<cubes.size(); i++)
	{
		u32 cube_id = cubes[i];
		smCubeIds.push_back(cube_id);
		smCubeIdToGrpIdMap.insert(make_pair(cube_id, group_id));
	}
	return true;
}


bool
AosNetworkMgrObj::configCubeInfos(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	AosXmlTagPtr cube_conf = app_conf->getFirstChild("cube_grp");
	while (cube_conf)
	{
		AosCubeGroupPtr cube_grp;
		try
		{
			cube_grp = OmnNew AosCubeGroup(cube_conf);
		}
		catch (...)
		{
			OmnAlarm << "error!" << enderr;
			return false;
		}

		int group_id = cube_grp->getGroupId();
		aos_assert_r(group_id >=0, false);

		map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.find(group_id);
		if (itr == smCubeGroups.end())
		{
			smCubeGroups[group_id] = cube_grp; 

			vector<u32> & cubes = cube_grp->getCubeIds();
			for(u32 i=0; i<cubes.size(); i++)
			{
				u32 cube_id = cubes[i];
				smCubeIds.push_back(cube_id);
				smCubeIdToGrpIdMap.insert(make_pair(cube_id, group_id));
			}
		}
	
		cube_conf = app_conf->getNextChild("cube_grp");
	}
	
	OmnString str_policy = app_conf->getAttrStr(AOSCONFIG_REPLICPOLICY, "");
	if (str_policy != "")
	{
		smReplicPolicy = AosReplicPolicy::getReplicPolicy(str_policy);
		smReplicPolicy->init(smSvrInfos, smCubeGroups);
	}

	return true;
}


bool
AosNetworkMgrObj::configSelfSvr(const AosXmlTagPtr &app_conf)
{
	aos_assert_r(app_conf, false);
	
	smSelfServerId = app_conf->getAttrInt64(AOSTAG_PHYSICAL_SERVER_ID, 0);
	smSelfClientId = app_conf->getAttrInt(AOSTAG_PHYSICAL_CLIENT_ID, 0);
	smSelfProcId = app_conf->getAttrU32("zky_procid", 0);
	smSelfProcType = AosProcessType::toEnum(app_conf->getAttrStr("proc_type", ""));
	smSelfCubeGrpId = app_conf->getAttrInt("cube_grp_id", -1);
	OmnString str_vids = app_conf->getAttrStr("vids", "");
	if(str_vids != "")
	{
		OmnStrParser1 parser(str_vids, ", ");
		OmnString svid;
		while ((svid = parser.nextWord()) != "")
		{
			u32 vid = atoi(svid.data());
			smSelfCubeIds.push_back(vid);
		}
	}
	smMasterId = app_conf->getAttrInt("master_id", -1);

	return true;
}

bool 
AosNetworkMgrObj::isVirtualIdLocal(const int cube_id)
{
	int svr_id = -1;
	u32 proc_id = 0;
	
	int cube_grp_id = getCubeGrpIdByCubeId(cube_id);
	bool rslt = getNextSvrInfo(cube_grp_id, svr_id, proc_id);
	aos_assert_r(rslt && svr_id >=0 && proc_id >1, false);
	if(svr_id == smSelfServerId && proc_id == smSelfProcId)	return true; 
	
	return false;
}


/*
bool
AosNetworkMgrObj::getVirIdsBySvrId(const u32 svr_id, vector<u32> &total_vids)
{
	map<u32, AosSvrInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
	aos_assert_r(itr != smSvrInfos.end(), false);
	AosSvrInfoPtr svr_info = itr->second; 

	OmnNotImplementedYet;
	return true;
}

*/

bool
AosNetworkMgrObj::getCubeIdsByGrpId(const u32 grp_id, vector<u32> &vids)
{
	map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.find(grp_id);
	aos_assert_r(itr != smCubeGroups.end(), false);
	
	AosCubeGroupPtr cube_grp = itr->second;
	vids = cube_grp->getCubeIds();
	return true;
}

bool
AosNetworkMgrObj::getTotalCubeIds(vector<u32> &vids)
{
	vids = smCubeIds;
	return true;
}

bool
AosNetworkMgrObj::getTotalCubeGrps(vector<u32> &cube_grps)
{
	map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.begin();
	for(; itr != smCubeGroups.end(); itr++)
	{
		cube_grps.push_back(itr->first);
	}
	return true;
}

bool 
AosNetworkMgrObj::getCubeSvrIds(const u32 cube_grp_id, vector<u32> &svr_ids)
{
	map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.find(cube_grp_id);
	aos_assert_r(itr != smCubeGroups.end(), false);
	
	AosCubeGroupPtr cube_grp = itr->second;
	return cube_grp->getSvrIds(svr_ids);
}

u32
AosNetworkMgrObj::getSendProcId(const u32 cube_grp_id, const int svr_id)
{
	map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.find(cube_grp_id);
	aos_assert_r(itr != smCubeGroups.end(), false);
	
	AosCubeGroupPtr cube_grp = itr->second;
	return cube_grp->getProcId(svr_id);
}

//bool
//AosNetworkMgrObj::getNextSvrInfo1(const u32 cube_grp_id, int &svr_id, u32 &proc_id)
//{
//	//<svr_map>
//	//	<cube_grp server_id="0" cube_grp_id="0" proc_id="xxxx"/>
//	//	<cube_grp server_id="1" cube_grp_id="1" proc_id="xxxx"/>
//	//	<cube_grp server_id="2" cube_grp_id="2" proc_id="xxxx"/>
//	//	<cube_grp server_id="3" cube_grp_id="3" proc_id="xxxx"/>
//	//</svr_map>
//	map<u32, map<u32, u32> >::iterator itr = smSvrProcMap.find(cube_grp_id);
//	aos_assert_r(itr != smSvrProcMap.end(), false);
//	map<u32, u32> svr_proc = itr->second;
//	map<u32, u32>::iterator s_itr = svr_proc.find(svr_id);
//	return s_itr->second;
//}

bool
AosNetworkMgrObj::getNextSvrInfo(const u32 cube_grp_id, int &svr_id, u32 &proc_id)
{
	map<u32, AosCubeGroupPtr>::iterator itr = smCubeGroups.find(cube_grp_id);
	aos_assert_r(itr != smCubeGroups.end(), false);

	AosCubeGroupPtr cube_grp = itr->second;
	return cube_grp->getNextSvrInfo(svr_id, proc_id);	
}

int
AosNetworkMgrObj::getCubeGrpIdByCubeId(const u32 cube_id)
{
	map<u32, u32>::iterator itr = smCubeIdToGrpIdMap.find(cube_id);
	aos_assert_r(itr != smCubeIdToGrpIdMap.end(), -1);
	return itr->second;
}

u32
AosNetworkMgrObj::getCubeBkpNum()
{
	aos_assert_r(smReplicPolicy, false);
	return smReplicPolicy->getCubeBkpNum();
}

AosServerInfoPtr 
AosNetworkMgrObj::getSvrInfo(const u32 svr_id)
{
	map<u32, AosServerInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
	aos_assert_r(itr != smSvrInfos.end(), 0);
	return itr->second;
}

bool
AosNetworkMgrObj::isValidSvrId(const int svr_id)
{
	map<u32, AosServerInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
	return (itr != smSvrInfos.end());
}

bool
AosNetworkMgrObj::getSvrIds(vector<u32> &svr_ids)
{
	svr_ids.insert(svr_ids.end(), smSvrIds.begin(), smSvrIds.end());
	return true;
}

bool
AosNetworkMgrObj::isPhysicalIdLocal(const int64_t &svr_id)
{
	return smSelfServerId == svr_id;
}


bool
AosNetworkMgrObj::setStatus(
		const u32 svr_id,
		const u32 proc_id, 
		const AosProcInfo::Status status)
{
	//map<svr_id, map<proc_id, status> >
	aos_assert_r(smSelfServerId == 0, false);
	map<u32, AosServerInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
	aos_assert_r(itr != smSvrInfos.end(), false);	
	bool rslt = (itr->second)->setProcStatus(proc_id, status);
	aos_assert_r(rslt, false);
OmnScreen << "server id: " << svr_id << " proc id: " << proc_id << " status: " << status << endl;
	return true;
}


AosServerInfoPtr
AosNetworkMgrObj::getStatus(const u32 svr_id)
{
	aos_assert_r(smSelfServerId == 0, NULL);
	map<u32, AosServerInfoPtr>::iterator itr = smSvrInfos.find(svr_id);
	aos_assert_r(itr != smSvrInfos.end(), NULL);
	return itr->second; 

}
#endif
