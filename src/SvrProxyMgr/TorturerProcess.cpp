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
#include "SvrProxyMgr/TorturerProcess.h"

#include "alarm_c/alarm.h"
#include "API/AosApi.h"
#include "Alarm/Alarm.h"
#include "Thread/Mutex.h"
#include "Util/OmnNew.h"

#include <unistd.h>

AosTorturerProcess::AosTorturerProcess(const bool reg_flag)
:
AosProcess(AosProcessType::eTorturer, AOSPROCESS_TORTURER, reg_flag)
{
}

AosTorturerProcess::AosTorturerProcess(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
:
AosProcess(proc_mgr, proxy, logic_pid, AosProcessType::eTorturer,
		AosGetProcExeName(AosProcessType::eTorturer), tmp_dir, auto_start, show_log)
{
}


AosTorturerProcess::~AosTorturerProcess()
{
}

	
AosProcessPtr
AosTorturerProcess::clone(
		const AosProcessMgrPtr &proc_mgr,
		const AosSvrProxyPtr &proxy,
		const int logic_pid,
		const ProcInfo &proc_conf,
		const OmnString &tmp_dir,
		const bool auto_start,
		const bool show_log)
{
	return OmnNew AosTorturerProcess(proc_mgr, proxy,
			logic_pid, tmp_dir, auto_start, show_log);
}


