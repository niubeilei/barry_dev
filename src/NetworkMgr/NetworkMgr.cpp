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
//
// Modification History:
// 03/09/2012 Created by Chen Ding
////////////////////////////////////////////////////////////////////////////
#include "NetworkMgr/NetworkMgr.h"

#include "API/AosApi.h"
#include "Debug/ExitHandler.h"
#include "ErrorMgr/ErrorCode.h"
#include "NetworkMgr/ServerWrap.h"
#include "NetworkMgr/ServerComm.h"
#include "NetworkMgr/NetworkTransType.h"
#include "NetworkMgr/NetworkTrans.h"
#include "NetworkMgrUtil/NetMgrUtil.h"
#include "SEInterfaces/CommandRunnerObj.h"
#include "SEInterfaces/NetListener.h"
#include "SEInterfaces/ReplicMgrObj.h"
#include "SEInterfaces/CubeGroup.h"
#include "SingletonClass/SingletonImpl.cpp"
#include "Thread/ThreadMgr.h"
#include "Util/Buff.h"
#include "XmlUtil/XmlTag.h"
#include "XmlUtil/SeXmlParser.h"

OmnSingletonImpl(AosNetworkMgrSingleton,
				 AosNetworkMgr,
				 AosNetworkMgrSelf,
				 "AosNetworkMgr");

static vector<u32> sInvalidVt;

AosNetworkMgr::AosNetworkMgr()
:
mLock(OmnNew OmnMutex()),
mCondVar(OmnNew OmnCondVar())
{
	mSelfConf.mServerId = -1;
	mSelfConf.mClientId = -1;
	mSelfConf.mProcId = 0;
}


AosNetworkMgr::~AosNetworkMgr()
{
}


bool
AosNetworkMgr::start()
{
	return true;
}


bool
AosNetworkMgr::stop()
{
	return true;
}


bool 
AosNetworkMgr::config(const AosXmlTagPtr &conf)
{
	return true;
}
	
bool
AosNetworkMgr::configNetwork(const AosXmlTagPtr &app_conf)
{
	// It's for child process.
	
	aos_assert_r(app_conf, false);
	AosXmlTagPtr net_conf = app_conf->getFirstChild("networkmgr");
	aos_assert_r(net_conf, false);

	bool rslt = configSelfSvr(net_conf);
	aos_assert_r(rslt, false);

	AosXmlTagPtr svrs_conf = net_conf->getFirstChild("servers");
	if (svrs_conf)
	{
		rslt = configServers(net_conf);
		aos_assert_r(rslt, false);
	}

	AosXmlTagPtr cubes_conf = net_conf->getFirstChild("cubes");
	if (cubes_conf)
	{
		rslt = configCubeInfos(cubes_conf);
		aos_assert_r(rslt, false);
	
		rslt = addProcsToSvrInfo(svrs_conf);	
		aos_assert_r(rslt, false);
	
		rslt = tryAddBkpCubeToSvrInfo();	// cube proc need know bkp svr info.
		aos_assert_r(rslt, false);
	
		//rslt = initCubeGrpToSvrMap();
		//aos_assert_r(rslt, false);
	}
	return true;
}


bool
AosNetworkMgr::configSelfSvr(const AosXmlTagPtr &self_conf)
{
	aos_assert_r(self_conf, false);
	
	mSelfConf.mServerId = self_conf->getAttrInt64(AOSTAG_PHYSICAL_SERVER_ID, 0);
	mSelfConf.mClientId = self_conf->getAttrInt(AOSTAG_PHYSICAL_CLIENT_ID, 0);
	mSelfConf.mProcId = self_conf->getAttrU32("zky_procid", 0);
	mSelfConf.mProcType = AosProcessType::toEnum(self_conf->getAttrStr("proc_type", ""));
	mSelfConf.mCubeGrpId = self_conf->getAttrInt("cube_grp_id", -1);
	OmnString str_vids = self_conf->getAttrStr("vids", "");
	if(str_vids != "")
	{
		OmnStrParser1 parser(str_vids, ", ");
		OmnString svid;
		while ((svid = parser.nextWord()) != "")
		{
			u32 vid = atoi(svid.data());
			mSelfConf.mCubeIds.push_back(vid);
		}
	}
	mSelfConf.mConfigMasterId = self_conf->getAttrInt("config_master", -1);
	mSelfConf.mCrtMasterId = self_conf->getAttrInt("crt_master", -1);
	return true;
}


bool
AosNetworkMgr::configServers(const AosXmlTagPtr &svrs_config)
{
	aos_assert_r(svrs_config, false);
	AosXmlTagPtr svrs_conf = svrs_config->getFirstChild("servers");
	aos_assert_r(svrs_conf, false);
	
	mSvrIds.clear();
	AosXmlTagPtr svr_conf = svrs_conf->getFirstChild(AOSCONFIG_SERVER);
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
	
		// the admin svr will init self svr first. and then addServer.
		//map<u32, AosServerInfoPtr>::iterator itr = mSvrInfos.find(svr_id);
		//aos_assert_r(itr == mSvrInfos.end(), false);
		//mSvrInfos.insert(make_pair(svr_id, svr_info));
		mSvrInfos[svr_id] = svr_info;
		mSvrIds.push_back(svr_id);

		svr_conf = svrs_conf->getNextChild(AOSCONFIG_SERVER);
	}
	
	mBroadcastPort = svrs_conf->getAttrInt("broadcast_port", -1);
	return true;
}


bool
AosNetworkMgr::configCluster(const AosXmlTagPtr &cluster_conf)
{
	aos_assert_r(cluster_conf, false);	
	bool rslt;

	AosXmlTagPtr procs_conf = cluster_conf->getFirstChild("processes");
	aos_assert_r(procs_conf, false);
	rslt = configProcesses(procs_conf);
	aos_assert_r(rslt, false);
	
	AosXmlTagPtr cubes_conf = cluster_conf->getFirstChild("cubes");
	aos_assert_r(cubes_conf, false);
	rslt = configCubeInfos(cubes_conf);
	aos_assert_r(rslt, false);

	AosXmlTagPtr svrs_conf = cluster_conf->getFirstChild("servers");
	aos_assert_r(svrs_conf, false);	
	rslt = addProcsToSvrInfo(svrs_conf);	
	aos_assert_r(rslt, false);
	
	rslt = tryAddBkpCubeToSvrInfo();
	aos_assert_r(rslt, false);

	//rslt = initCubeGrpToSvrMap();
	//aos_assert_r(rslt, false);
	return true;
}


bool
AosNetworkMgr::configProcesses(const AosXmlTagPtr &procs_conf)
{
	aos_assert_r(procs_conf, false);

	AosProcessType::E proc_tp;
	OmnString exe_name;
	AosXmlTagPtr proc_conf = procs_conf->getFirstChild("proc");
	while (proc_conf)
	{
		proc_tp = AosProcessType::toEnum(proc_conf->getAttrStr("proc_type"));
		aos_assert_r(proc_tp != AosProcessType::eInvalid, false);
	
		exe_name = proc_conf->getAttrStr("exe");
		aos_assert_r(exe_name != "", false);

		map<AosProcessType::E, OmnString>::iterator itr = mTotalProcess.find(proc_tp);
		aos_assert_r(itr == mTotalProcess.end(), false);
		mTotalProcess.insert(make_pair(proc_tp, exe_name));
	
		proc_conf = procs_conf->getNextChild("proc");
	}
	return true;
}


bool
AosNetworkMgr::configCubeInfos(const AosXmlTagPtr &cubes_conf)
{
	aos_assert_r(cubes_conf, false);
	bool rslt;

	AosXmlTagPtr cube_conf = cubes_conf->getFirstChild("cube_grp");
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

		map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.find(group_id);
		aos_assert_r(itr == mCubeGroups.end(), false);
		mCubeGroups.insert(make_pair(group_id, cube_grp));

		cube_conf = cubes_conf->getNextChild("cube_grp");
	}

	rslt = initCubeIdToGrpIdMap();
	aos_assert_r(rslt, false);
	
	AosReplicMgrObjPtr replic_mgr = AosReplicMgrObj::getReplicMgr();
	aos_assert_r(replic_mgr, false);
	
	OmnString str_policy = cubes_conf->getAttrStr(AOSCONFIG_REPLICPOLICY, "nobkp");
	rslt = replic_mgr->initPolicy(str_policy);
	aos_assert_r(rslt, false);
	return true;
}

bool
AosNetworkMgr::tryAddBkpCubeToSvrInfo()
{
	AosReplicMgrObjPtr replic_mgr = AosReplicMgrObj::getReplicMgr();
	aos_assert_r(replic_mgr, false);
	
	bool rslt = replic_mgr->initBkpProc(mSvrInfos, mCubeGroups);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosNetworkMgr::initCubeIdToGrpIdMap()
{
	map<u32, AosCubeGroupPtr>::iterator c_itr;
	for(c_itr = mCubeGroups.begin(); c_itr != mCubeGroups.end(); c_itr++)
	{
		AosCubeGroupPtr cube_grp = c_itr->second;
		int cube_grp_id = cube_grp->getGroupId();
		
		vector<u32> & cubes = cube_grp->getCubeIds();
		for(u32 i=0; i<cubes.size(); i++)
		{
			u32 cube_id = cubes[i];
			
			mCubeIds.push_back(cube_id);
			mCubeIdToGrpIdMap.insert(make_pair(cube_id, cube_grp_id));
		}
	}

	// Chen Ding, 2014/08/12
	smNumVirtuals = mCubeIds.size();
	return true;
}	

/*
bool
AosNetworkMgr::initCubeGrpToSvrMap()
{
	map<u32, AosServerInfoPtr>::iterator s_itr = mSvrInfos.begin();
	map<u32, AosCubeGroupPtr>::iterator c_itr;

	for(; s_itr != mSvrInfos.end(); s_itr++)
	{
		AosServerInfoPtr svr_info = s_itr->second;
		u32 svr_id = s_itr->first;
	
		vector<ProcInfo> & total_procs = svr_info->getDefProcs();
		for(u32 i=0; i<total_procs.size(); i++)
		{
			ProcInfo proc = total_procs[i]; 
			AosProcessType::E type = proc.mType;
			if(type != AosProcessType::eCube && type != AosProcessType::eBkpCube)	continue;
			
			int cube_grp_id = proc.mCubeGrpId;
			aos_assert_r(cube_grp_id != -1, false);
			
			c_itr = mCubeGroups.find(cube_grp_id);
			aos_assert_r(c_itr != mCubeGroups.end(), false);
			AosCubeGroupPtr cube_group = c_itr->second;
			//cube_group->addSvrProc(svr_id, proc_info->getProcId());
			cube_group->addSvrId(svr_id); 
		}
	}

	// Chen Ding, 2014/08/12
	map<u32, u32>::iterator itr = mCubeIdToGrpIdMap.begin();
	u32 cube_id, cube_grp_id;
	for(; itr != mCubeIdToGrpIdMap.end(); ++itr)
	{
		cube_id = itr->first;
		cube_grp_id = itr->second;
		aos_assert_r(cube_id < eMaxCubeId, false);
		if (cube_id >= smCubeId2PhyId.size()) 
		{
			for (u32 i=smCubeId2PhyId.size(); i<=cube_id; i++)
			{
				smCubeId2PhyId.push_back(0);
			}
		}
		int phyid = getNextSvrId(cube_grp_id, -1);
		if(phyid == -1) continue;
		//aos_assert_r(phyid >= 0, false);
		smCubeId2PhyId[cube_id] = phyid;
	}
	// End of Chen Ding, 2014/08/12
	return true;
}
*/

bool
AosNetworkMgr::addProcsToSvrInfo(const AosXmlTagPtr &svrs_conf)
{
	aos_assert_r(svrs_conf, false);

	bool rslt;
	AosXmlTagPtr svr_conf = svrs_conf->getFirstChild(AOSCONFIG_SERVER);
	while (svr_conf)
	{
		int svr_id = svr_conf->getAttrInt("server_id", -1);
		aos_assert_r(svr_id != -1, false);
		
		map<u32, AosServerInfoPtr>::iterator itr = mSvrInfos.find(svr_id);
		aos_assert_r(itr != mSvrInfos.end(), false);
		AosServerInfoPtr svr_info = itr->second;
	
		rslt = addDefaultProcs(svr_info, svr_conf);
		aos_assert_r(rslt, false);
		//svr_info->addDefaultProcs(svr_conf, mCubeGroups);

		svr_conf = svrs_conf->getNextChild();
	}
	return true;
}


bool
AosNetworkMgr::addDefaultProcs(
		const AosServerInfoPtr &svr_info,
		const AosXmlTagPtr procs_conf)
{
	aos_assert_r(procs_conf, false);
	
	AosProcessType::E proc_tp;
	int cube_grp_id = -1;
	AosCubeGroupPtr cube_group;
	
	AosXmlTagPtr proc_conf = procs_conf->getFirstChild("proc");
	map<u32, AosCubeGroupPtr>::iterator c_itr;
	while (proc_conf)
	{
		proc_tp = AosProcessType::toEnum(proc_conf->getAttrStr("proc_type"));
		aos_assert_r(proc_tp != AosProcessType::eInvalid && proc_tp != AosProcessType::eBkpCube, false);
		cube_grp_id = -1;
		
		if(proc_tp == AosProcessType::eCube)
		{
			cube_grp_id = proc_conf->getAttrInt("cube_grp_id", -1);
			aos_assert_r(cube_grp_id != -1, false);
			
			c_itr = mCubeGroups.find(cube_grp_id);
			aos_assert_r(c_itr != mCubeGroups.end(), false);
			cube_group = c_itr->second;
			cube_group->addSvrId(svr_info->getServerId()); 
		}
		
		ProcInfo proc_info;
		proc_info.mType = proc_tp;
		proc_info.mCubeGrpId = cube_grp_id;
		svr_info->addProc(proc_info);
		//mProcInfos.push_back(proc_info);
		
		proc_conf = procs_conf->getNextChild("proc");
	}

	// Chen Ding, 2014/08/12
	map<u32, u32>::iterator itr = mCubeIdToGrpIdMap.begin();
	u32 cube_id;
	for(; itr != mCubeIdToGrpIdMap.end(); ++itr)
	{
		cube_id = itr->first;
		cube_grp_id = itr->second;
		aos_assert_r(cube_id < eMaxCubeId, false);
		if (cube_id >= smCubeId2PhyId.size()) 
		{
			for (u32 i=smCubeId2PhyId.size(); i<=cube_id; i++)
			{
				smCubeId2PhyId.push_back(0);
			}
		}
		int phyid = getNextSvrId(cube_grp_id, -1);
		if(phyid == -1) continue;
		//aos_assert_r(phyid >= 0, false);
		smCubeId2PhyId[cube_id] = phyid;
	}
	// End of Chen Ding, 2014/08/12
	return true;
}


vector<u32> &
AosNetworkMgr::getCubeIdsByGrpId(const u32 grp_id)
{
	map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.find(grp_id);
	aos_assert_r(itr != mCubeGroups.end(), sInvalidVt);
	
	AosCubeGroupPtr cube_grp = itr->second;
	return cube_grp->getCubeIds();
}

bool
AosNetworkMgr::getTotalCubeGrps(vector<u32> &cube_grps)
{
	map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.begin();
	for(; itr != mCubeGroups.end(); itr++)
	{
		cube_grps.push_back(itr->first);
	}
	return true;
}


int
AosNetworkMgr::getNextSvrId(const u32 cube_grp_id, const int crt_sid)
{
	map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.find(cube_grp_id);
	aos_assert_r(itr != mCubeGroups.end(), false);

	AosCubeGroupPtr cube_grp = itr->second;
	return cube_grp->getNextSvrId(crt_sid);
}

int
AosNetworkMgr::getCubeGrpIdByCubeId(const u32 cube_id)
{
	map<u32, u32>::iterator itr = mCubeIdToGrpIdMap.find(cube_id);
	aos_assert_r(itr != mCubeIdToGrpIdMap.end(), -1);
	return itr->second;
}


vector<u32> &
AosNetworkMgr::getCubeSendSvrIds(const u32 cube_grp_id)
{
	map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.find(cube_grp_id);
	aos_assert_r(itr != mCubeGroups.end(), sInvalidVt);

	AosCubeGroupPtr cube_grp = itr->second;
	return cube_grp->getSvrIds();
}


AosServerInfoPtr 
AosNetworkMgr::getSvrInfo(const u32 svr_id)
{
	map<u32, AosServerInfoPtr>::iterator itr = mSvrInfos.find(svr_id);
	aos_assert_r(itr != mSvrInfos.end(), 0);
	return itr->second;
}

bool
AosNetworkMgr::isValidSvrId(const int svr_id)
{
	map<u32, AosServerInfoPtr>::iterator itr = mSvrInfos.find(svr_id);
	return (itr != mSvrInfos.end());
}

bool 
AosNetworkMgr::isVirtualIdLocal(const u32 cube_id)
{
	for(u32 i=0; i<mSelfConf.mCubeIds.size(); i++)
	{
		u32 self_cube_id = (mSelfConf.mCubeIds)[i];
		if(cube_id == self_cube_id)	return true;
	}
	return false;
}
	
OmnString
AosNetworkMgr::getProcExeName(const AosProcessType::E tp)
{
	map<AosProcessType::E, OmnString>::iterator itr = mTotalProcess.find(tp);
	aos_assert_r(itr != mTotalProcess.end(), "");
	
	return itr->second;
}


/*
bool
AosNetworkMgr::setStatus(
		const u32 svr_id,
		const u32 proc_id, 
		const AosProcInfo::Status status)
{
	//map<svr_id, map<proc_id, status> >
	aos_assert_r(mSelfServerId == 0, false);
	map<u32, AosServerInfoPtr>::iterator itr = mSvrInfos.find(svr_id);
	aos_assert_r(itr != mSvrInfos.end(), false);	
	bool rslt = (itr->second)->setProcStatus(proc_id, status);
	aos_assert_r(rslt, false);
OmnScreen << "server id: " << svr_id << " proc id: " << proc_id << " status: " << status << endl;
	return true;
}


AosServerInfoPtr
AosNetworkMgr::getStatus(const u32 svr_id)
{
	aos_assert_r(mSelfServerId == 0, NULL);
	map<u32, AosServerInfoPtr>::iterator itr = mSvrInfos.find(svr_id);
	aos_assert_r(itr != mSvrInfos.end(), NULL);
	return itr->second; 

}
*/

/*
bool
AosNetworkMgr::addCubeInfo(const AosCubeGroupPtr &cube_grp)
{
	int group_id = cube_grp->getGroupId();
	aos_assert_r(group_id >=0, false);

	map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.find(group_id);
	aos_assert_r(itr == mCubeGroups.end(), false);
	mCubeGroups[group_id] = cube_grp; 

	vector<u32> & cubes = cube_grp->getCubeIds();
	for(u32 i=0; i<cubes.size(); i++)
	{
		u32 cube_id = cubes[i];
		mCubeIds.push_back(cube_id);
		mCubeIdToGrpIdMap.insert(make_pair(cube_id, group_id));
	}
	return true;
}
*/


//bool
//AosNetworkMgr::getTotalCubeIds(vector<u32> &vids)
//{
//	vids = mCubeIds;
//	return true;
//}

//u32
//AosNetworkMgr::getSendProcId(const u32 cube_grp_id, const int svr_id)
//{
//	map<u32, AosCubeGroupPtr>::iterator itr = mCubeGroups.find(cube_grp_id);
//	aos_assert_r(itr != mCubeGroups.end(), false);
	
//	AosCubeGroupPtr cube_grp = itr->second;
//	return cube_grp->getProcId(svr_id);
//}



//bool
//AosNetworkMgr::initReplicPolicy()
//{
//	if (mReplicPolicy)
//	{
//		mReplicPolicy->init(mSvrInfos, mCubeGroups);
//	}
//	return true;
//}
