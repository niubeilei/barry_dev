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
// Created: 01/20/2015 by Yazong Ma
////////////////////////////////////////////////////////////////////////////
#include "SvrProxyMgr/IndexEngineProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"
#include "SvrProxyMgr/ProcessMgr.h"
#include "SvrProxyMgr/SvrProxy.h"
#include "Porting/Sleep.h"

#include <unistd.h>


AosIndexEngineProcess::AosIndexEngineProcess(const bool reg_flag)
:
AosProcess(AosProcessType::eIndexEngine, AOSPROCESS_INDEXENGINE, reg_flag)
{
}

AosIndexEngineProcess::AosIndexEngineProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
AosProcess(proc_mgr, proxy, logic_pid, AosProcessType::eIndexEngine, 
		AosGetProcExeName(AosProcessType::eIndexEngine), tmp_dir, auto_start, show_log)
{
}


AosIndexEngineProcess::~AosIndexEngineProcess()
{
}


AosProcessPtr
AosIndexEngineProcess::clone(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
{
	return OmnNew AosIndexEngineProcess(proc_mgr, proxy,
			logic_pid, tmp_dir, auto_start, show_log);
}


bool
AosIndexEngineProcess::handProcDeath()
{
	if(mProActiveKill)
	{
		setDeath();
		return true;
	}

	OmnScreen << "IILPackage proc death. "
		<< "will kill all task, and restart proc. "
		<< "logic_pid:" << getLogicPid()
		<< endl;
	
	mProcMgr->killAllTaskProcess();
	OmnSleep(1);
	
	return restart();
}


//bool
//AosIndexEngineProcess::handProcUp()
//{
//	bool rslt = notifyListenerPidUp();
//	aos_assert_r(rslt, false);
//
//	rslt = procUpTriggerResend(); 
//	aos_assert_r(rslt, false);
//
////	if(mSvrProxy->selfIsJudgerSvr())
////	{
////OmnScreen << "================jozhi job start" << endl;
////		rslt = mProcMgr->startJobSvr();
////		aos_assert_r(rslt, false);
////	}
//	return true;
//}


