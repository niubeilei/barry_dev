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
#include "SvrProxyMgr/ProcessMgr.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Porting/Sleep.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "SvrProxyMgr/CubeProcess.h"
#include "Util/OmnNew.h"

#include "SysMsg/SwitchToMasterMsg.h"
#include "SysMsg/ReSwitchToMasterMsg.h"
//#include "SysMsg/SetNewMasterMsg.h"	
#include "SysMsg/TriggerResendMsg.h"

#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>


bool
AosProcessMgr::monitorCubeSvrsDeath(const int death_svr_id)
{
	aos_assert_r(death_svr_id >= 0 && (u32)death_svr_id < eAosMaxServerId, false);

	OmnScreen << "<ProxyMsg> SvrProxy svr death! "
		<< "; death_svr:" << death_svr_id 
		<< "; try to start bkp svr. "
		<< endl;
	bool rslt;
	AosServerInfoPtr svr_info = AosGetSvrInfo(death_svr_id);
	aos_assert_r(svr_info, false);
	vector<ProcInfo> &total_procs = svr_info->getDefProcs();
	for(u32 i=0; i<total_procs.size(); i++)
	{
		ProcInfo proc_conf = total_procs[i]; 
		if(proc_conf.mType != AosProcessType::eCube)	continue;

		u32 cube_grp_id = proc_conf.mCubeGrpId;
		rslt = cubeProcDeath(cube_grp_id, death_svr_id);
		aos_assert_r(rslt, false);
	}

	return true;
}


bool
AosProcessMgr::cubeProcDeath(const u32 cube_grp_id, const int from_svr_id)
{
	bool rslt;
	int crt_master = getCrtMaster(cube_grp_id);
	aos_assert_r(crt_master >=0, false);
	if(crt_master != from_svr_id)
	{
		// means this death proc is not master.
		rslt = bkpCubeSvrDeath(cube_grp_id, from_svr_id, crt_master);
		aos_assert_r(rslt, false);
		return true;
	}
	
	rslt = startBkpCubeSvr(cube_grp_id, crt_master);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosProcessMgr::startBkpCubeSvr(
		const u32 cube_grp_id,
		const int crt_master)
{
	bool succ;
	bool rslt = tryStartBkpCubeSvr(cube_grp_id, crt_master, succ);
	aos_assert_r(rslt, false);
	if(succ)	return true;

	OmnString reason = "cube proc death. no more bkp cube. "; 
	reason << "cube_grp_id:" << cube_grp_id;
	return procDeathTriggerResend(reason);
}

bool
AosProcessMgr::bkpCubeSvrDeath(
		const u32 cube_grp_id,
		const int from_svr_id,
		const int crt_master)
{
	aos_assert_r(crt_master >=0 && crt_master != from_svr_id, false);
	
	OmnScreen << "<ProxyMsg> ProcessMgr bkp cubeProc death"
		<< "; from_svr_id:" << from_svr_id 
		<< "; crt_master:" << crt_master
		<< endl;
	
	OmnString reason = "cube proc death. it's bkp cube. svr_id:";
	reason << from_svr_id << "; cube_grp_id:" << cube_grp_id;
	procDeathTriggerResend(reason);

	if(AosGetConfigMaster(cube_grp_id) != from_svr_id)	return true;

	OmnNotImplementedYet;
	// maybe this death proc has send switchToBkp msg to crt_master.
	// reSwitchToMaster.
	AosAppMsgPtr msg = OmnNew AosReSwitchToMasterMsg(crt_master, cube_grp_id);
	bool succ;
	bool rslt = mSvrProxy->sendAppMsg(msg, succ);
	aos_assert_r(rslt, false);
	return true;
}


bool
AosProcessMgr::tryStartBkpCubeSvr(
		const u32 cube_grp_id,
		const int crt_master,
		bool &succ)
{
	// when detect child proc deathed. will call this func.
	// ready start the bkp svr.

	OmnScreen << "<ProxyMsg> ProcessMgr try to start bkp cube svr."
		<< "cube_grp_id:" << cube_grp_id << "; " 
		<< "crt_master:" << crt_master << "; " 
		<< endl;
	
	succ = true;	
	int next_master = AosGetNextSvrId(cube_grp_id, crt_master);
	if(next_master == -1)
	{
		OmnScreen << "<ProxyMsg> ProcessMgr "
			<< "this cube grp:" << cube_grp_id
			<< "; don't has bkp svr any more!"
			<< "; crt_master:" << crt_master << endl;

		succ = false;
		return true;
	}

	OmnScreen << "<ProxyMsg> ProcessMgr "
		<< "start bkp svr "
		<< "; cube_grp:" << cube_grp_id
		<< "; new_master:" << next_master
		<< endl;
	AosAppMsgPtr msg = OmnNew AosSwitchToMasterMsg(
			next_master, AOSTAG_SVRPROXY_PID,
			cube_grp_id);
	bool send_succ;
	bool rslt = mSvrProxy->sendAppMsg(msg, send_succ);
	aos_assert_r(rslt, false);	
	if(send_succ)	return true;
	
	OmnScreen << "<ProxyMsg> ProcessMgr "
		<< "switch to master failed. try next master"
		<< ". crt_master:" << crt_master
		<< ". crt_try_master:" << next_master
		<< "; cube_grp_id:" << cube_grp_id << endl;
	return tryStartBkpCubeSvr(cube_grp_id, next_master, succ);
}


bool
AosProcessMgr::rebootCubeProc(const u32 cube_grp_id)
{
	OmnScreen << "<ProxyMsg> reboot cube proc"
		<< "; cube_grp_id:" << cube_grp_id
		<< endl;

	AosProcessPtr proc = getCubeProcByCubeGrpId(cube_grp_id);
	aos_assert_r(proc, false);
	
	return proc->reboot();
}


