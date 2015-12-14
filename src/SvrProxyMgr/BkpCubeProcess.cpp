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
#include "SvrProxyMgr/BkpCubeProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "Util/File.h"
#include "SvrProxyMgr/ProcessMgr.h"

#include <unistd.h>


AosBkpCubeProcess::AosBkpCubeProcess(const bool reg_flag)
:
AosCubeProcess(AosProcessType::eBkpCube, AOSPROCESS_BKPCUBE, reg_flag)
{
}

AosBkpCubeProcess::AosBkpCubeProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const u32 cube_grp_id,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
AosCubeProcess(proc_mgr, proxy, logic_pid, cube_grp_id,
		AosProcessType::eBkpCube, tmp_dir, auto_start, show_log)
{
}


AosBkpCubeProcess::~AosBkpCubeProcess()
{
}


AosProcessPtr
AosBkpCubeProcess::clone(
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
	
	return OmnNew AosBkpCubeProcess(proc_mgr, proxy, 
			logic_pid, cube_grp_id, tmp_dir, auto_start, show_log);
}


bool
AosBkpCubeProcess::handProcDeath()
{
	setDeath();
	if(mProActiveKill)	return true;
	
	bool rslt = notifyListenerPidDeath();
	aos_assert_r(rslt, false);
	
	OmnString reason = "proc death. svr_id:";
	reason << AosGetSelfServerId() << "; proc_id:" << getLogicPid();
	mProcMgr->procDeathTriggerResend(reason);
	
	OmnAlarm << "Bkp proc death!" << enderr;
	return true;
}


