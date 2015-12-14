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
// Created: 04/17/2013 by Ketty 
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/CubeProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "SvrProxyMgr/IpcSvrInfo.h"

#include <unistd.h>
#include "SysMsg/RebootProcMsg.h"	


AosCubeProcess::AosCubeProcess(const bool reg_flag)
:
AosProcess(AosProcessType::eCube, AOSPROCESS_CUBE, reg_flag),
mRebooting(false)
{
}

AosCubeProcess::AosCubeProcess(
		const AosProcessType::E tp,
		const OmnString &name,
		const bool reg_flag)
:
AosProcess(tp, name, reg_flag),
mRebooting(false)
{
}


AosCubeProcess::AosCubeProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const u32 cube_grp_id,
		const AosProcessType::E tp,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
AosProcess(proc_mgr, proxy, logic_pid, tp,
		AosGetProcExeName(AosProcessType::eCube), tmp_dir, auto_start, show_log),
mCubeGrpId(cube_grp_id),
mRebooting(false)
{
}


AosCubeProcess::~AosCubeProcess()
{
}


AosProcessPtr
AosCubeProcess::clone(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
{
	int cube_grp_id = proc_conf.mCubeGrpId;
	aos_assert_r(cube_grp_id >=0, 0);
	return OmnNew AosCubeProcess(proc_mgr, proxy, logic_pid,
			cube_grp_id, AosProcessType::eCube, tmp_dir, auto_start, show_log);
}


bool
AosCubeProcess::setNetworkConf(const AosXmlTagPtr &norm_config)
{
	aos_assert_r(norm_config, false);
	bool rslt = AosProcess::setNetworkConf(norm_config);
	aos_assert_r(rslt, false);

	AosXmlTagPtr net_conf = norm_config->getFirstChild("networkmgr");
	aos_assert_r(net_conf, false);
	
	net_conf->setAttr("cube_grp_id", mCubeGrpId);
	net_conf->setAttr("vids", getVidStr());
	
	int config_master = AosGetConfigMaster(mCubeGrpId);
	aos_assert_r(config_master >=0, false);
	net_conf->setAttr("config_master", config_master);
	return true;
}

//Jozhi 2015-04-20 remove _0,1,2
/*
OmnString
AosCubeProcess::getStrBaseDir()
{
	OmnString ss = AosProcess::getStrBaseDir(); 
	ss << "_" << mCubeGrpId;
	return ss;	
}
*/
	
u32
AosCubeProcess::getArgsNum(const u32 pp_args_num)
{
	return AosProcess::getArgsNum(pp_args_num) + 2;
}

void
AosCubeProcess::addNewArgs()
{
	AosProcess::addNewArgs();
	
	//mArgs[mCrtArgsIdx] = (char*)"-crt_master";
	OmnString tmp = "-crt_master";
	mArgs[mCrtArgsIdx] = OmnNew char[tmp.length() + 1];
	strcpy(mArgs[mCrtArgsIdx], tmp.data());
	mCrtArgsIdx++;

	mArgs[mCrtArgsIdx] = getStrCrtMaster();
	mCrtArgsIdx++;
}
	

bool
AosCubeProcess::resetArgs()
{
	for (u32 i=1; i< mCrtArgsIdx; i++)
	{
		if(strcmp(mArgs[i], "-crt_master") != 0)	continue;
	
		// reset crt_master
		u32 master_idx = i + 1;
		aos_assert_r(master_idx < mCrtArgsIdx && mArgs[master_idx], "");
		mArgs[master_idx] = getStrCrtMaster();
		break;
	}
	
	return true;
}


char *
AosCubeProcess::getStrCrtMaster()
{
	static char str_master[128];
	int crt_master = mProcMgr->getCrtMaster(mCubeGrpId);
	sprintf(str_master, "%d", crt_master);
	
	OmnScreen << "------cube process. get crt master:" << crt_master << endl;
	return str_master;
}


OmnString
AosCubeProcess::getVidStr()
{
	vector<u32> & vids = AosGetCubeIdsByGrpId(mCubeGrpId);
	OmnString vids_str;
	for (u32 i=0; i<vids.size(); i++)
	{
		if (i == vids.size() - 1)
		{
			vids_str << vids[i];
		}
		else
		{
			vids_str << vids[i] << ",";
		}
	}
	return vids_str;
}

bool
AosCubeProcess::reboot()
{
	if(isDeath())                              
	{
		OmnScreen << "====== reboot failed."
			<< " old_master has death." << endl;
		return true;
	}
	OmnScreen << "reboot proc"
			<< "; logic_pid:" << getLogicPid() 
			<< "; child_pid:" << getChildPid() << endl;
	
	mRebooting = true;

	AosAppMsgPtr msg = OmnNew AosRebootProcMsg(AosGetSelfServerId(), getLogicPid()); 
	bool succ;
	bool rslt = sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	if(succ) return true;

	// see Mark AA.
	OmnScreen << "reboot proc . but proc maybe death."
		<< "; proc_id:" << getLogicPid() << endl;
	mRebooting = false;
	setDeath();
	//return restart();
	return true;
}


bool
AosCubeProcess::handProcDeath()
{
	if(mRebooting)
	{
		OmnScreen << "reboot proc. succ"
			<< "; proc_id:" << getLogicPid() << endl;
		mRebooting = false;
		return restart();
	}
	if(mDiskError)
	{
		return handProcDeathPriv();
	}

	if(mProActiveKill)
	{
		setDeath();
		return true;
	}

	OmnScreen << "Cube proc death. "
		<< "will restart proc. "
		<< "logic_pid:" << getLogicPid()
		<< endl;
	return restart();
	
	// Ketty Reliable Test.
	//return handProcDeathPriv();
}


bool
AosCubeProcess::handProcDeathPriv()
{
	bool rslt;
	int crt_master = mProcMgr->getCrtMaster(mCubeGrpId);
	aos_assert_r(crt_master >=0, false);
	
	if(crt_master != AosGetSelfServerId())
	{
		// means this death proc is not master.
		setDeath();
		rslt = mProcMgr->bkpCubeSvrDeath(mCubeGrpId, AosGetSelfServerId(), crt_master);
		aos_assert_r(rslt, false);
		return true;
	}

	bool succ;
	rslt = mProcMgr->tryStartBkpCubeSvr(mCubeGrpId, crt_master, succ);
	aos_assert_r(rslt, false);
	if(succ)
	{
		// this svr's setDeath() until setNewMaster succ. see Mark AA. 
		mSvr->setConnFailed();
		return true;
	}
	
	setDeath();
	OmnString reason = "cube proc death. no more bkp cube. "; 
	reason << "cube_grp_id:" << mCubeGrpId;
	return mProcMgr->procDeathTriggerResend(reason);
}


